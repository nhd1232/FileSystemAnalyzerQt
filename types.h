#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <filesystem>

struct FileInfo {
    std::string full_path;
    std::string name;
    std::string extension;
    unsigned long long size_b{};
    std::filesystem::file_time_type modified{};
};

struct ScanResult {
    std::vector<FileInfo> files;
    std::vector<std::string> dirs;
    std::vector<std::string> warnings;
};

struct ReportLine {
    std::string text;
};

struct ReportChunk {
    std::string title;
    std::vector<ReportLine> lines;
};

ReportLine make_line(std::string x);

std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds> file_time_to_system_time(std::filesystem::file_time_type ft);

std::string format_time(
    std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds> m);

std::string normalize_extension(std::string s);
