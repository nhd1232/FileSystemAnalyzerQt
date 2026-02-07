#include <string>
#include <unordered_set>
#include "types.h"
#include "categories.h"
using namespace std;

bool is_document_ext(const string& ext)
{
    static const unordered_set<string> kDocExt = { "txt", "pdf", "doc", "docx", "xls", "xlsx", "ppt", "pptx" };
    return kDocExt.count(ext) > 0;
}

bool is_image_ext(const string& ext) {
    static const unordered_set<string> kImExt = { "jpg", "jpeg", "png", "bmp", "gif", "tiff", "webp" };
    return kImExt.count(ext) > 0;
}

bool is_video_ext(const string& ext) {
    static const unordered_set<string> kVidExt = { "mp4", "avi", "mkv", "mov", "wmv" };
    return kVidExt.count(ext) > 0;
}

bool is_audio_ext(const string& ext) {
    static const unordered_set<string> kAudExt = { "mp3", "wav", "flac", "ogg", "aac" };
    return kAudExt.count(ext) > 0;
}

bool is_archive_ext(const string& ext) {
    static const unordered_set<string> kArchExt = { "zip", "rar", "7z", "tar", "gz" };
    return kArchExt.count(ext) > 0;
}

bool is_code_ext(const string& ext) {
    static const unordered_set<string> kCodExt = { "cpp", "h", "hpp", "c", "hxx", "py", "cs", "java", "js", "ts", "php", "rb" };
    return kCodExt.count(ext) > 0;
}

FileCategory DefineCategory(string extension) {
    string ext = normalize_extension(extension);
    if (is_document_ext(ext)) {
        return FileCategory::Document;
    }
    else if (is_image_ext(ext)) {
        return FileCategory::Image;
    }
    else if (is_video_ext(ext)) {
        return FileCategory::Video;
    }
    else if (is_audio_ext(ext)) {
        return FileCategory::Audio;
    }
    else if (is_archive_ext(ext)) {
        return FileCategory::Archive;
    }
    else if (is_code_ext(ext)) {
        return FileCategory::Code;
    }
    else {
        return FileCategory::Other;
    }
}

string CategoryToString(FileCategory cat)
{
    switch (cat) {
    case FileCategory::Document:
        return "Документы";
    case FileCategory::Image:
        return "Изображения";
    case FileCategory::Video:
        return "Видео";
    case FileCategory::Audio:
        return "Аудио";
    case FileCategory::Archive:
        return "Архивы";
    case FileCategory::Code:
        return "Файлы кода";
    case FileCategory::Other:
    default:
        return "Прочее";
    }
}
