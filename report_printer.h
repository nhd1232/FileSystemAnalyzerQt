#pragma once

#include <vector>
#include <ostream>

#include "types.h"

class ReportPrinter {
protected:
    void PrintBody(const std::vector<ReportChunk>& report,
        std::ostream& out) const;
};
