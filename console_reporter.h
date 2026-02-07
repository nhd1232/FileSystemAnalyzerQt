#pragma once

#include "ireporter.h"
#include "report_printer.h"

class ConsoleReporter : public IReporter, private ReportPrinter {
public:
    void Report(const std::vector<ReportChunk>& chunks,
        const AppConfig& cfg) override;
};
