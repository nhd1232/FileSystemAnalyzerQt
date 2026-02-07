#include "analyzer.h"
#include "ianalysis.h"
#include "types.h"
#include "config.h"

#include "file_scanner.h"

#include "count_size.h"
#include "biggest.h"
#include "newest.h"
#include "exts.h"
#include "dups.h"
#include "category_analysis.h"

#include <QDebug>
#include <QString>

#include <memory>

static inline QString S(const std::string& s)
{
    return QString::fromUtf8(s.c_str());
}

static inline void LOGI(const QString& msg)
{
    qInfo().noquote() << msg;
}

static inline void LOGW(const QString& msg)
{
    qWarning().noquote() << msg;
}

static inline void LOGE(const QString& msg)
{
    qCritical().noquote() << msg;
}

Analyzer::Analyzer(const AppConfig& cfg)
    : config_(cfg),
    scanner_(std::make_unique<FileScanner>())
{
    InitializeAnalyses();
}

Analyzer::~Analyzer() = default;

void Analyzer::InitializeAnalyses()
{
    analyses_.clear();
    analyses_.push_back(std::make_unique<CountSizeAnalysis>());
    analyses_.push_back(std::make_unique<BiggestAnalysis>());
    analyses_.push_back(std::make_unique<NewestSinceAnalysis>());
    analyses_.push_back(std::make_unique<ExtsAnalysis>());
    analyses_.push_back(std::make_unique<DupsAnalysis>());
    analyses_.push_back(std::make_unique<CategoryAnalysis>());
}

std::vector<ReportChunk> Analyzer::RunAnalysis()
{
    LOGI(QStringLiteral("Начинаем сканирование..."));

    ScanResult scan_result = scanner_->Scan(config_.roots, config_.recursive);

    LOGI(QString("Найдено файлов: %1").arg(scan_result.files.size()));
    LOGI(QString("Найдено директорий: %1").arg(scan_result.dirs.size()));

    if (!scan_result.warnings.empty()) {
        LOGW(QStringLiteral("Предупреждения:"));
        for (const auto& warning : scan_result.warnings) {
            LOGW(QString("  - %1").arg(S(warning)));
        }
    }

    std::vector<ReportChunk> full_report;
    full_report.reserve(analyses_.size());

    LOGI(QStringLiteral("Запуск анализаторов..."));
    for (const auto& analysis : analyses_) {
        LOGI(QString("Запуск: %1").arg(S(analysis->Name())));
        full_report.push_back(analysis->Run(scan_result, config_));
    }

    return full_report;
}

std::vector<ReportChunk> Analyzer::RunSelected(std::vector<int> indices)
{
    ScanResult scan = scanner_->Scan(config_.roots, config_.recursive);
    std::vector<ReportChunk> results;

    for (int idx : indices) {
        if (idx < 0 || idx >= static_cast<int>(analyses_.size())) {
            LOGW(QString("Предупреждение: индекс анализатора %1 вне диапазона [0..%2]")
                     .arg(idx)
                     .arg(static_cast<int>(analyses_.size()) - 1));
            continue;
        }
        auto& analysis = analyses_[idx];
        results.push_back(analysis->Run(scan, config_));
    }
    return results;
}
