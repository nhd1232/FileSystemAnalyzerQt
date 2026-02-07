#include "input_guard.h"

#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <sstream>
#include <limits>
#include <chrono>


std::vector<std::string> GetPathsFromUser() {
    std::vector<std::string> paths_list;
    std::cout << "Введите пути для сканирования (по одному на строку)" << std::endl;
    std::cout << "Введите пустую строку для завершения:" << std::endl;

    while (true) {
        std::string path_string;
        std::cout << "Путь: " << std::endl;
        std::getline(std::cin, path_string);

        if (path_string.empty())
            break;

        std::filesystem::path p(path_string);
        if (std::filesystem::exists(p)) {
            paths_list.push_back(path_string);
            std::cout << "Добавлен: " << path_string << std::endl;
        }

        else {
            std::cout << "Предупреждение: путь не существует или недоступен" << std::endl;
            bool ans = GetYesOrNo("Добавить всё равно?");
            if (ans) {
                paths_list.push_back(path_string);
            }
        }
    }

    return paths_list;
}


bool GetYesOrNo(const std::string& question) {
    std::cout << question << " (y/n): " << std::endl;
    while (true) {
        char answer;
        std::cin >> answer;
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Ошибка ввода. Попробуйте снова (y/n): " << std::endl;
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (answer == 'y' || answer == 'Y') {
            return true;
        }
        else if (answer == 'n' || answer == 'N') {
            return false;
        }
        else {
            std::cout << "Некорректный ввод. Введите 'y' или 'n': " << std::endl;
        }
    }
}

int GetNumber(const std::string& query_text, int min, int max) {
    int n;
    while (true) {
        std::cout << query_text << " [" << min << "-" << max << "]: " << std::endl;
        std::cin >> n;
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Ошибка: введите число!" << std::endl;
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (n >= min && n <= max)
            return n;
        else
            std::cout << "Число должно быть от " << min << " до " << max << std::endl;
    }
}

std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>
GetDateFromUser(const std::string& prompt) {
    std::cout << prompt << std::endl;
    std::cout << "Формат: ДД-ММ-ГГГГ (например, 01-09-2025): " << std::endl;

    std::string date_string;
    std::getline(std::cin, date_string);

    try {
        int day, month, year;
        char delimiter;
        std::stringstream s(date_string);
        s >> day >> delimiter >> month >> delimiter >> year;

        if (year < 1900 || year > 2100) throw "Некорректный год";
        if (month < 1 || month > 12) throw "Некорректный месяц";
        if (day < 1 || day > 31) throw "Некорректный день";

        std::tm time_struct{};
        time_struct.tm_year = year - 1900;
        time_struct.tm_mon = month - 1;
        time_struct.tm_mday = day;
        time_struct.tm_hour = 0;
        time_struct.tm_min = 0;
        time_struct.tm_sec = 0;
        time_struct.tm_isdst = -1;

        std::time_t timestamp = std::mktime(&time_struct);
        auto tp = std::chrono::system_clock::from_time_t(timestamp);
        return std::chrono::time_point_cast<std::chrono::seconds>(tp);
    }
    catch (...) {
        std::cout << "Некорректная дата, использую текущую дату" << std::endl;
        auto now = std::chrono::system_clock::now();
        return std::chrono::time_point_cast<std::chrono::seconds>(now);
    }
}


