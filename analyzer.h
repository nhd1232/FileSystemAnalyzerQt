#pragma once

#include <memory>
#include <vector>
#include "types.h"
#include "config.h"

class IAnalysis;
class FileScanner;

class Analyzer {
public:
    explicit Analyzer(const AppConfig& cfg);
    ~Analyzer();

    std::vector<ReportChunk> RunAnalysis();

    const AppConfig& GetConfig() const { return config_; }
    std::vector<ReportChunk> RunSelected(std::vector<int> indices);

private:
    AppConfig config_;
    std::unique_ptr<FileScanner> scanner_;
    std::vector<std::unique_ptr<IAnalysis>> analyses_;

    void InitializeAnalyses();
};
