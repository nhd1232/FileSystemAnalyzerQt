#include "report_printer.h"

void ReportPrinter::PrintBody(const std::vector<ReportChunk>& report,
    std::ostream& out) const {
    for (const auto& block : report) {
        if (!block.title.empty()) {
            out << "--- " << block.title << " ---" << std::endl;
        }
        for (const auto& line : block.lines) {
            out << line.text << std::endl;
        }
        out << std::endl;
    }
}
