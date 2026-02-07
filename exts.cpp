#include "exts.h"

#include <map>

struct ExtAgg {
    std::size_t count = 0;
    unsigned long long size = 0;
};

ReportChunk ExtsAnalysis::Run(const ScanResult& data, const AppConfig&) {
    std::map<std::string, ExtAgg> bucket;

    for (const auto& f : data.files) {
        std::string ext = normalize_extension(f.extension);
        auto& agg = bucket[ext];
        ++agg.count;
        agg.size += f.size_b;
    }

    ReportChunk out;
    out.title = "Группировка по расширениям";

    for (const auto& element : bucket) {
        out.lines.push_back(
            make_line(
                element.first + ": " +
                std::to_string(element.second.count) + " шт, " +
                std::to_string(element.second.size) + " байт"
            )
        );
    }

    return out;
}

