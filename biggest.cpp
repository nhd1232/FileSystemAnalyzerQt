#include "biggest.h"

#include <algorithm> 

ReportChunk BiggestAnalysis::Run(const ScanResult& data, const AppConfig& cfg) {
    std::vector<const FileInfo*> base;
    base.reserve(data.files.size());

    if (cfg.has_size_threshold) {
        for (const auto& f : data.files) {
            if (f.size_b >= cfg.size_threshold) {
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
            return lhs->size_b > rhs->size_b; 
        }
    );

    ReportChunk out;
    out.title = "Самые большие файлы";

    const std::size_t k = cfg.has_top_k
        ? static_cast<std::size_t>(cfg.top_k)
        : 10u;

    const std::size_t limit = std::min<std::size_t>(base.size(), k);
    for (std::size_t i = 0; i < limit; ++i) {
        const auto* f = base[i];
        out.lines.push_back(
            make_line(std::to_string(f->size_b) + " | " + f->full_path)
        );
    }

    return out;
}
