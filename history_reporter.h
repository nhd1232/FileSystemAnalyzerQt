#pragma once

#include <string>

#include "ireporter.h"
#include "report_printer.h"

class HistoryReporter : public IReporter, private ReportPrinter {
public:
    explicit HistoryReporter(std::string path);

    void Report(const std::vector<ReportChunk>& chunks,
        const AppConfig& cfg) override;

private:
    std::string path_;
};
