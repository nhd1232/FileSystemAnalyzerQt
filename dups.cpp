#include "dups.h"

#include <map>
#include <vector>

namespace {
    std::string make_key(const FileInfo& f, DupMode mode) {
        switch (mode) {
        case DupMode::ByName:
            return f.name;
        case DupMode::BySize:
            return std::to_string(f.size_b);
        case DupMode::ByNameAndSize:
            return f.name + "|" + std::to_string(f.size_b);
        }
        return f.name;
    }
}

ReportChunk DupsAnalysis::Run(const ScanResult& data, const AppConfig& cfg) {
    std::map<std::string, std::vector<const FileInfo*>> groups;

    for (const auto& file : data.files) {
        std::string key = make_key(file, cfg.dup_mode);
        groups[key].push_back(&file);
    }

    ReportChunk out;
    out.title = "Возможные дубликаты";

    for (const auto& element : groups) {
        if (element.second.size() >= 2) {
            out.lines.push_back(make_line("Ключ: " + element.first));
            for (const FileInfo* p : element.second) {
                out.lines.push_back(make_line("  - " + p->full_path));
            }
        }
    }

    return out;
}

