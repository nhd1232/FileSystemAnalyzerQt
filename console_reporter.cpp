#include "console_reporter.h"

#include <iostream>

void ConsoleReporter::Report(const std::vector<ReportChunk>& report,
    const AppConfig&) {
    std::cout << std::endl;
    std::cout << "========== РЕЗУЛЬТАТЫ АНАЛИЗА ==========" << std::endl;
    std::cout << std::endl;

    PrintBody(report, std::cout);

    std::cout << "========================================" << std::endl;
}
