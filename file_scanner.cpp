#include "file_scanner.h"
#include "types.h"

#include <filesystem>
#include <system_error>
#include <string>

namespace {
    namespace fs = std::filesystem;

    static std::string PathToUtf8(const std::filesystem::path& p)
    {
        return p.u8string();
    }


    bool safe_prob_file(const fs::path& path, FileInfo& out_info, std::string& warning_out) {
        std::error_code ec;

        if (!fs::exists(path, ec) || ec) {
            warning_out = "Не найден файл: " + path.string();
            return false;
        }

        if (!fs::is_regular_file(path, ec)) {
            return false;
        }

        auto size = fs::file_size(path, ec);
        if (ec) {
            warning_out = "Ошибка размера: " + path.string();
            return false;
        }

        fs::file_time_type modificationTime = fs::last_write_time(path, ec);
        if (ec) {
            warning_out = "Ошибка времени: " + path.string();
            return false;
        }

        out_info.full_path = PathToUtf8(path);
        out_info.name = PathToUtf8(path.filename());
        out_info.extension = normalize_extension(PathToUtf8(path.extension()));
        out_info.size_b = static_cast<unsigned long long>(size);
        out_info.modified = modificationTime;
        return true;
    }

    void ScanDirectory(const fs::path& path, ScanResult& out_info, bool recursive) {
        std::error_code ec;

        if (!fs::exists(path, ec) || ec) {
            out_info.warnings.push_back("Директория не найдена: " + path.string());
            return;
        }

        if (!fs::is_directory(path, ec) || ec) {
            out_info.warnings.push_back("Это не директория: " + path.string());
            return;
        }

        try {
            if (recursive) {
                std::error_code it_ec;

                fs::recursive_directory_iterator it(
                    path,
                    fs::directory_options::skip_permission_denied,
                    it_ec
                    );
                fs::recursive_directory_iterator end;

                if (it_ec) {
                    out_info.warnings.push_back(
                        "Ошибка создания итератора: " + path.string() +
                        " (" + it_ec.message() + ")"
                        );
                    return;
                }

                for (; it != end; it.increment(it_ec)) {
                    if (it_ec) {
                        out_info.warnings.push_back(
                            "Предупреждение обхода (пропуск): " +
                            it->path().string() + " (" + it_ec.message() + ")"
                            );
                        it_ec.clear();
                        continue;
                    }

                    const fs::directory_entry &entry = *it;
                    const fs::path currentPath = entry.path();

                    std::error_code ecType;
                    if (entry.is_regular_file(ecType) && !ecType) {
                        FileInfo temp_file_info;
                        std::string warning;
                        bool res = safe_prob_file(currentPath, temp_file_info, warning);
                        if (res) {
                            out_info.files.push_back(temp_file_info);
                        } else if (!warning.empty()) {
                            out_info.warnings.push_back(warning);
                        }
                    }
                    else if (entry.is_directory(ecType) && !ecType) {
                        out_info.dirs.push_back(PathToUtf8(currentPath));
                    }
                    else if (ecType) {
                        out_info.warnings.push_back(
                            "Ошибка определения типа: " + currentPath.string() +
                            " (" + ecType.message() + ")"
                            );
                    }
                }
            }
            else {
                std::error_code it_ec;

                fs::directory_iterator it(
                    path,
                    fs::directory_options::skip_permission_denied,
                    it_ec
                    );
                fs::directory_iterator end;

                if (it_ec) {
                    out_info.warnings.push_back(
                        "Ошибка создания итератора: " + path.string() +
                        " (" + it_ec.message() + ")"
                        );
                    return;
                }

                for (; it != end; it.increment(it_ec)) {
                    if (it_ec) {
                        out_info.warnings.push_back(
                            "Предупреждение обхода (пропуск): " +
                            it->path().string() + " (" + it_ec.message() + ")"
                            );
                        it_ec.clear();
                        continue;
                    }

                    const fs::directory_entry &entry = *it;
                    const fs::path currentPath = entry.path();

                    std::error_code ecType;
                    if (entry.is_regular_file(ecType) && !ecType) {
                        FileInfo temp_file_info;
                        std::string warning;
                        bool res = safe_prob_file(currentPath, temp_file_info, warning);
                        if (res) {
                            out_info.files.push_back(temp_file_info);
                        } else if (!warning.empty()) {
                            out_info.warnings.push_back(warning);
                        }
                    }
                    else if (entry.is_directory(ecType) && !ecType) {
                        out_info.dirs.push_back(PathToUtf8(currentPath));
                    }
                    else if (ecType) {
                        out_info.warnings.push_back(
                            "Ошибка определения типа: " + currentPath.string() +
                            " (" + ecType.message() + ")"
                            );
                    }
                }
            }
        }
        catch (const fs::filesystem_error& e) {
            out_info.warnings.push_back("Критическая ошибка обхода: " + std::string(e.what()));
        }
        catch (const std::exception& e) {
            out_info.warnings.push_back("Критическая системная ошибка: " + std::string(e.what()));
        }
        catch (...) {
            out_info.warnings.push_back("Неизвестная ошибка");
        }
    }

} // namespace

ScanResult FileScanner::Scan(const std::vector<std::string>& roots, bool recursive) {
    ScanResult res;
    for (const auto& root : roots) {
        ScanDirectory(std::filesystem::path(root), res, recursive);
    }
    return res;
}
