#pragma once

#include <string>
#include <vector>
#include <chrono>

std::vector<std::string> GetPathsFromUser();

bool GetYesOrNo(const std::string& question);

int GetNumber(const std::string& query_text, int min, int max);

std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>
GetDateFromUser(const std::string& prompt);
