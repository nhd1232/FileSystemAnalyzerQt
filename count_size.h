#pragma once

#include "analysis_base.h"

class CountSizeAnalysis : public AnalysisBase {
public:
    CountSizeAnalysis()
        : AnalysisBase("Stats")   
    {
    }

    ReportChunk Run(const ScanResult& data, const AppConfig& cfg) override;
};
