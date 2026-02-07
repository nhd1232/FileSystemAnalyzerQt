#include "history_reporter.h"

#include <QDebug>
#include <QString>

#include <ctime>
#include <exception>
#include <filesystem>
#include <fstream>
#include <system_error>

namespace fs = std::filesystem;

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

HistoryReporter::HistoryReporter(std::string path)
    : path_(std::move(path))
{
}

void HistoryReporter::Report(const std::vector<ReportChunk>& report,
                             const AppConfig&)
{
    if (path_.empty()) {
        return;
    }

    try {
        const fs::path p = fs::u8path(path_);

        bool need_bom = false;
        {
            std::error_code ec;
            if (!fs::exists(p, ec)) {
                need_bom = true;
            } else {
                auto sz = fs::file_size(p, ec);
                if (!ec && sz == 0) {
                    need_bom = true;
                }
            }
        }

        std::ofstream out(p, std::ios::binary | std::ios::app);
        if (!out) {
            LOGE(QString("Невозможно открыть файл для записи: %1").arg(S(path_)));
            return;
        }

        if (need_bom) {
            out.put(char(0xEF));
            out.put(char(0xBB));
            out.put(char(0xBF));
        }

        std::time_t now = std::time(nullptr);
        std::tm local_time{};
#if defined(_WIN32)
        localtime_s(&local_time, &now);
#else
        localtime_r(&now, &local_time);
#endif

        char time_buffer[80];
        std::strftime(time_buffer, sizeof(time_buffer),
                      "%d-%m-%Y %H:%M:%S", &local_time);

        out << "=== ОТЧЕТ ОТ " << time_buffer << " ===\n\n";

        PrintBody(report, out);

        out << "================================\n";
        LOGI(QString("Отчет сохранен в: %1").arg(S(path_)));
    }
    catch (const std::ios_base::failure& e) {
        LOGE(QString("Ошибка сохранения отчета: %1").arg(QString::fromLocal8Bit(e.what())));
    }
    catch (const std::exception& e) {
        LOGE(QString("Неожиданная ошибка: %1").arg(QString::fromLocal8Bit(e.what())));
    }
    catch (...) {
        LOGE(QStringLiteral("Неизвестная ошибка"));
    }
}
