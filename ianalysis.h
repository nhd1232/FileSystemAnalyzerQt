#pragma once

#include <string>

#include "types.h"
#include "config.h"

class IAnalysis {
public:
    virtual ~IAnalysis();

    virtual ReportChunk Run(const ScanResult& data, const AppConfig& cfg) = 0;
    virtual std::string Name() const = 0;
};
