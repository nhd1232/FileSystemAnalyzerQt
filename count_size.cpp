#include "count_size.h"
#include <numeric>

ReportChunk CountSizeAnalysis::Run(const ScanResult& data, const AppConfig&) {
    const auto files_n = static_cast<unsigned long long>(data.files.size());
    const auto dirs_n = static_cast<unsigned long long>(data.dirs.size());

    unsigned long long sum_b = std::accumulate(
        data.files.begin(), data.files.end(),
        0ull,
        [](unsigned long long acc, const FileInfo& f) {
            return acc + f.size_b;
        }
    );

    ReportChunk out;
    out.title = "Общие подсчеты";
    out.lines = {
        make_line("Файлы: " + std::to_string(files_n)),
        make_line("Директории: " + std::to_string(dirs_n)),
        make_line("Общий размер (байт): " + std::to_string(sum_b))
    };

    return out;
}
