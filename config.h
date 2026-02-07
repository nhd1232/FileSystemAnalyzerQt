#pragma once

#include <string>
#include <vector>
#include <chrono>


enum class DupMode { ByName, BySize, ByNameAndSize };

struct AppConfig {
    std::vector<std::string> roots;
    bool recursive = false;

    bool has_top_k = false;
    unsigned int top_k = 10;

    bool has_size_threshold = false;
    unsigned long long size_threshold = 0;

    bool has_date_threshold = false;
    std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds> date_threshold{};

    DupMode dup_mode = DupMode::ByNameAndSize;

    std::string history_path;
};
