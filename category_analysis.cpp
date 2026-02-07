#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include "category_analysis.h"
#include "categories.h"
#include "types.h"
using namespace std;

struct CategoryAgg {
	int number_of_files;
	long long total_size;
	vector<const FileInfo*> files_list;
};

ReportChunk CategoryAnalysis::Run(const ScanResult& data, const AppConfig& cfg) {
	map<FileCategory, CategoryAgg> stats;
	for (const auto& f : data.files) {
		if (cfg.has_size_threshold && f.size_b < cfg.size_threshold) continue;
		if (cfg.has_date_threshold) {
			auto modified_sys = file_time_to_system_time(f.modified);
			if (modified_sys < cfg.date_threshold) {
				continue;
			}
		}
		FileCategory cat = DefineCategory(f.extension);
		CategoryAgg& agg = stats[cat];
		agg.number_of_files++;
		agg.total_size += f.size_b;
		agg.files_list.push_back(&f);
	}
	vector < pair<FileCategory, CategoryAgg> > entries;
	for (const auto& kv : stats) {
		entries.emplace_back(kv.first, kv.second);
	}
	sort(
		entries.begin(),
		entries.end(),
		[](const auto& a, const auto& b) {
			return a.second.total_size > b.second.total_size;
		}
	);
	size_t k = entries.size(); 
	if (cfg.has_top_k) {
		k = std::min(entries.size(),
			static_cast<std::size_t>(cfg.top_k));
	}
	ReportChunk report;
	report.title = "Категории файлов по типу";
	for (int i = 0; i < k; i++) {
		FileCategory cat = entries[i].first;
		CategoryAgg agg = entries[i].second;
		string category_name = CategoryToString(cat);
		string text = category_name + ": " + to_string(agg.number_of_files) + " файлов, " + to_string(agg.total_size) + " байт";
		report.lines.push_back(make_line(text));
		string file_text;
		for (auto* pf : agg.files_list) {
			file_text = "    " + to_string(pf->size_b) + " байт | " + pf->full_path;
			report.lines.push_back(make_line(file_text));
		}
		report.lines.push_back(make_line(""));
	}
	return report;
}
