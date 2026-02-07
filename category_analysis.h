#pragma once

#include "analysis_base.h"
#include "types.h"

class CategoryAnalysis : public AnalysisBase {
public:
    CategoryAnalysis()
        : AnalysisBase("Категории файлов по типу")
    {
    }

    ReportChunk Run(const ScanResult& data, const AppConfig& cfg) override;
};
