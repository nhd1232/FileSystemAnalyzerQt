#pragma once

#include <string>
#include <algorithm>

#include "ianalysis.h"
#include "config.h"

class AnalysisBase : public IAnalysis {
public:
    explicit AnalysisBase(std::string display_name);
    ~AnalysisBase() override = default;

    std::string Name() const override;  

private:
    std::string name_;
};

class TopKAnalysisBase : public AnalysisBase {
public:
    using AnalysisBase::AnalysisBase;   

protected:
    template <class Container, class Compare>
    void ApplyTopK(Container& cont, const AppConfig& cfg, Compare comp) const {
        std::sort(cont.begin(), cont.end(), comp);
        if (cfg.has_top_k && cont.size() > cfg.top_k) {
            cont.resize(cfg.top_k);
        }
    }
};
