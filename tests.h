#pragma once

#include <string>
#include <vector>

// Результат одного теста
struct TestCaseResult {
    std::string name;     // имя теста (UTF-8)
    bool passed = false;  // true, если прошёл
    std::string details;  // подробности (UTF-8)
};

// Запустить все тесты проекта
std::vector<TestCaseResult> RunAllTests();

// Удобная функция "все прошли?"
bool AllTestsPassed(const std::vector<TestCaseResult> &results);
