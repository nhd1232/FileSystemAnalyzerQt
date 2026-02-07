#include "tests.h"

// Подключи твои заголовки ядра (оставь как у тебя в проекте)
#include "config.h"
#include "analyzer.h"
#include "history_reporter.h"

#include <filesystem>
#include <fstream>
#include <chrono>
#include <system_error>
#include <algorithm>
#include <string>
#include <vector>

namespace fs = std::filesystem;

// -------------------- helpers --------------------

static std::string PathToUtf8(const fs::path &p)
{
#if defined(__cpp_lib_char8_t) && (__cpp_lib_char8_t >= 201811L)
    auto u8 = p.u8string(); // std::u8string
    return std::string(u8.begin(), u8.end());
#else
    return p.u8string();    // std::string (C++17)
#endif
}

struct TempDir {
    fs::path root;

    ~TempDir() {
        std::error_code ec;
        if (!root.empty()) {
            fs::remove_all(root, ec); // тихо чистим за собой
        }
    }
};

static void WriteFile(const fs::path &p, std::size_t size)
{
    std::ofstream out(p, std::ios::binary);
    for (std::size_t i = 0; i < size; ++i) {
        out.put(static_cast<char>('A' + (i % 26)));
    }
}

static fs::path MakeUniqueTempPath()
{
    // уникальное имя папки в %TEMP%
    auto stamp = std::chrono::steady_clock::now().time_since_epoch().count();
    return fs::temp_directory_path() / ("fs_analyzer_tests_" + std::to_string(stamp));
}

static TempDir CreateTestTree()
{
    TempDir env;
    env.root = MakeUniqueTempPath();

    std::error_code ec;
    fs::remove_all(env.root, ec);
    fs::create_directories(env.root / "sub1", ec);
    fs::create_directories(env.root / "sub2", ec);

    // Файлы разных размеров
    WriteFile(env.root / "small.txt", 10);
    WriteFile(env.root / "sub1" / "medium.bin", 100);
    WriteFile(env.root / "sub2" / "big.bin", 1000);

    // Даты модификации: small старый, big свежий
    // ВАЖНО: last_write_time ждёт file_time_type!
    auto ft_now = fs::file_time_type::clock::now();

    fs::last_write_time(env.root / "small.txt", ft_now - std::chrono::hours(48), ec);
    ec.clear();
    fs::last_write_time(env.root / "sub2" / "big.bin", ft_now - std::chrono::hours(1), ec);

    return env;
}

static AppConfig MakeBaseConfig(const fs::path &root)
{
    AppConfig cfg;

    // В идеале roots в проекте должны быть UTF-8
    cfg.roots = { PathToUtf8(root) };
    cfg.recursive = true;

    cfg.has_top_k = false;
    cfg.top_k = 0;

    cfg.has_size_threshold = false;
    cfg.size_threshold = 0;

    cfg.has_date_threshold = false;

    cfg.dup_mode = DupMode::ByNameAndSize; // если у тебя другое значение по умолчанию — поправь

    cfg.history_path.clear();
    return cfg;
}

// -------------------- tests --------------------

static TestCaseResult Test_AnalyzerBasic()
{
    TestCaseResult r;
    r.name = u8"Базовый запуск Analyzer на тестовой директории";

    try {
        TempDir env = CreateTestTree();
        AppConfig cfg = MakeBaseConfig(env.root);

        Analyzer analyzer(cfg);
        auto report = analyzer.RunAnalysis();

        if (report.empty()) {
            r.passed = false;
            r.details = u8"RunAnalysis вернул пустой отчёт.";
        } else {
            r.passed = true;
            r.details = u8"Блоков отчёта: " + std::to_string(report.size());
        }
    } catch (const std::exception &ex) {
        r.passed = false;
        r.details = std::string(u8"Исключение: ") + ex.what();
    } catch (...) {
        r.passed = false;
        r.details = u8"Неизвестное исключение.";
    }

    return r;
}

static TestCaseResult Test_NonExistingDirectory()
{
    TestCaseResult r;
    r.name = u8"Анализ несуществующей директории";

    try {
        AppConfig cfg;
        cfg.roots = { "Z:/__definitely_does_not_exist__/fs_analyzer_test" };
        cfg.recursive = true;

        cfg.has_top_k = false;
        cfg.top_k = 0;

        cfg.has_size_threshold = false;
        cfg.size_threshold = 0;

        cfg.has_date_threshold = false;

        cfg.history_path.clear();
        cfg.dup_mode = DupMode::ByNameAndSize;

        Analyzer analyzer(cfg);
        auto report = analyzer.RunAnalysis();

        r.passed = true;
        r.details = u8"Analyzer не упал. Блоков отчёта: " + std::to_string(report.size());
    } catch (const std::exception &ex) {
        r.passed = false;
        r.details = std::string(u8"Исключение: ") + ex.what();
    } catch (...) {
        r.passed = false;
        r.details = u8"Неизвестное исключение.";
    }

    return r;
}

static TestCaseResult Test_HistoryReporter()
{
    TestCaseResult r;
    r.name = u8"Запись отчёта в файл истории";

    try {
        TempDir env = CreateTestTree();
        AppConfig cfg = MakeBaseConfig(env.root);

        Analyzer analyzer(cfg);
        auto report = analyzer.RunAnalysis();

        fs::path histPath = fs::temp_directory_path() / "fs_analyzer_history_test.txt";

        // пишем историю
        {
            HistoryReporter history(PathToUtf8(histPath));
            history.Report(report, cfg);
        }

        std::error_code ec;
        if (!fs::exists(histPath, ec) || ec) {
            r.passed = false;
            r.details = u8"Файл истории не был создан.";
        } else {
            auto sz = fs::file_size(histPath, ec);
            if (ec || sz == 0) {
                r.passed = false;
                r.details = u8"Файл истории пустой или не удалось получить размер.";
            } else {
                r.passed = true;
                r.details = u8"Файл истории создан, размер " + std::to_string(sz) + u8" байт.";
            }
        }

        fs::remove(histPath, ec);
    } catch (const std::exception &ex) {
        r.passed = false;
        r.details = std::string(u8"Исключение: ") + ex.what();
    } catch (...) {
        r.passed = false;
        r.details = u8"Неизвестное исключение.";
    }

    return r;
}

// -------------------- runner --------------------

std::vector<TestCaseResult> RunAllTests()
{
    std::vector<TestCaseResult> out;
    out.push_back(Test_AnalyzerBasic());
    out.push_back(Test_NonExistingDirectory());
    out.push_back(Test_HistoryReporter());
    return out;
}

bool AllTestsPassed(const std::vector<TestCaseResult> &results)
{
    return std::all_of(results.begin(), results.end(),
                       [](const TestCaseResult &r) { return r.passed; });
}
