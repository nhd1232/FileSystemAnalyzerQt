#pragma once

#include "analysis_base.h"

class BiggestAnalysis : public TopKAnalysisBase {
public:
    BiggestAnalysis()
        : TopKAnalysisBase("TopBySize")
    {
    }

    ReportChunk Run(const ScanResult& data, const AppConfig& cfg) override;
};
