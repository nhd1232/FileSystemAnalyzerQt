#pragma once
#include <string>
using namespace std;

enum class FileCategory { Document, Image, Video, Audio, Archive, Code, Other };

bool is_document_ext(const string& ext);

bool is_image_ext(const string& ext);

bool is_video_ext(const string& ext);

bool is_audio_ext(const string& ext);

bool is_archive_ext(const string& ext);

bool is_code_ext(const string& ext);

FileCategory DefineCategory(string extension);

string CategoryToString(FileCategory cat);
