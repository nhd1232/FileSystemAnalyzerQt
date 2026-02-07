#include <string>
#include "analysis_base.h"

AnalysisBase::AnalysisBase(std::string display_name)
    : name_(std::move(display_name))
{
}

std::string AnalysisBase::Name() const {
    return name_;
}
