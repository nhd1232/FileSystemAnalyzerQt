#pragma once

#include <vector>

#include "types.h"
#include "config.h"

class IReporter {
public:
    virtual ~IReporter();

    virtual void Report(const std::vector<ReportChunk>& chunks,
        const AppConfig& cfg) = 0;
};
