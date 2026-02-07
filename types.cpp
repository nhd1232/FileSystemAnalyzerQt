#include "types.h"

#include <algorithm>
#include <cctype>
#include <chrono>
#include <ctime>
#include <string>

namespace {
    std::string strip_leading_dot(std::string s) {
        if (!s.empty() && s.front() == '.') {
            s.erase(0, 1);
        }
        return s;
    }
}

ReportLine make_line(std::string x) {
    return ReportLine{ std::move(x) };
}

std::string normalize_extension(std::string s) {
    if (s.empty()) return {};

    std::transform(
        s.begin(), s.end(), s.begin(),
        [](unsigned char c) { return static_cast<char>(std::tolower(c)); }
    );

    return strip_leading_dot(std::move(s));
}

std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds> file_time_to_system_time(std::filesystem::file_time_type ft)
{
    using namespace std::chrono;

    const auto now_sys = system_clock::now();
    const auto now_file = std::filesystem::file_time_type::clock::now();

    auto diff = ft - now_file;

    auto sys_time = now_sys + diff;

    return time_point_cast<seconds>(sys_time);
}


std::string format_time(
    std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds> m
) {
    using namespace std::chrono;

    const auto tp = time_point_cast<system_clock::duration>(m);
    const std::time_t tt = system_clock::to_time_t(tp);

    std::tm tm_utc{};
#if defined(_WIN32)
    gmtime_s(&tm_utc, &tt);
#else
    gmtime_r(&tt, &tm_utc);
#endif

    char buf[32];
    const std::size_t n = std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%SZ", &tm_utc);
    return std::string(buf, n);
}
