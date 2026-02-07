#pragma once

#include <string>
#include <vector>
#include "types.h"

class FileScanner {
public:
    ScanResult Scan(const std::vector<std::string>& roots, bool recursive);
};
