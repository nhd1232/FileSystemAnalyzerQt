#pragma once

#include "analysis_base.h"

class NewestSinceAnalysis : public TopKAnalysisBase {
public:
    NewestSinceAnalysis()
        : TopKAnalysisBase("NewestSince")
    {
    }

    ReportChunk Run(const ScanResult& data, const AppConfig& cfg) override;
};
