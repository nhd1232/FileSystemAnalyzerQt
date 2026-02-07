#include "newest.h"

#include <algorithm> 

ReportChunk NewestSinceAnalysis::Run(const ScanResult& data, const AppConfig& cfg) {
    std::vector<const FileInfo*> base;
    base.reserve(data.files.size());

    if (cfg.has_date_threshold) {
        for (const auto& f : data.files) {
            if (file_time_to_system_time(f.modified) > cfg.date_threshold) {
                base.push_back(&f);
            }
        }
    }
    else {
        for (const auto& f : data.files) {
            base.push_back(&f);
        }
    }

    ApplyTopK(
        base,
        cfg,
        [](const FileInfo* lhs, const FileInfo* rhs) {
            return lhs->modified > rhs->modified;
        }
    );

    ReportChunk out;
    out.title = "Самые новые файлы";

    const std::size_t k = cfg.has_top_k
        ? static_cast<std::size_t>(cfg.top_k)
        : 10u;

    const std::size_t limit = std::min<std::size_t>(base.size(), k);
    for (std::size_t i = 0; i < limit; ++i) {
        const auto* f = base[i];

        auto modified_as_sys = file_time_to_system_time(f->modified);

        out.lines.push_back(
            make_line(format_time(modified_as_sys) + " | " + f->full_path)
        );
    }

    return out;
}
