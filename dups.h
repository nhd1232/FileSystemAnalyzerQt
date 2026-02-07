#pragma once

#include "analysis_base.h"

class DupsAnalysis : public AnalysisBase {
public:
    DupsAnalysis()
        : AnalysisBase("Duplicates")
    {
    }

    ReportChunk Run(const ScanResult& data, const AppConfig& cfg) override;
};
