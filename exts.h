#pragma once

#include "analysis_base.h"

class ExtsAnalysis : public AnalysisBase {
public:
    ExtsAnalysis()
        : AnalysisBase("Extensions")
    {
    }

    ReportChunk Run(const ScanResult& data, const AppConfig& cfg) override;
};
