#ifndef SCANNER_HPP
#define SCANNER_HPP

#include <array>
#include <vector>
#include <filesystem>
#include <unordered_map>
#include <map>
#include <cstdint>
#include <atomic>

#include "include/Config.hpp"

namespace fs = std::filesystem;

namespace scanner {
    struct FileInfo {
        fs::path path;
        std::string file_name;
        std::string extension;
        uintmax_t size = 0;
        std::size_t lines =0;
    };

    struct ExtStats {
        size_t files;
        size_t lines;
        uintmax_t bytes;

        ExtStats& operator+=(const ExtStats& other) {
            files += other.files;
            lines += other.lines;
            bytes += other.bytes;
            return *this;
        }
    };

    bool is_skippable(const fs::path &filename, const std::vector<std::string>& excluded_directories);

    std::vector<FileInfo> list_directory(const config::Config& config);
    std::unordered_map<std::string, ExtStats> list_extensions(const std::vector<FileInfo>& file_infos);
    size_t count_file_lines(const fs::path& file_path);
    void concurrent_count_lines(std::vector<FileInfo>& files_infos);
    void worker_task(std::vector<FileInfo>& files_infos, std::atomic<size_t>& next_index);

    inline const std::vector<std::string> excluded_extensions = {
        ".exe", ".dll", ".so", ".dylib", ".bin",
        ".sys", ".drv", ".ocx", ".class", ".jar",
        ".war", ".ear", ".pyc", ".pyo",

        ".png", ".jpg", ".jpeg", ".gif", ".bmp",
        ".ico", ".webp", ".tiff", ".tif", ".heic",
        ".psd", ".ai", ".xcf",

        ".mp3", ".wav", ".ogg", ".flac", ".aac",
        ".m4a", ".wma", ".mid",

        ".mp4", ".avi", ".mkv", ".mov", ".wmv",
        ".webm", ".mpeg", ".mpg", ".flv",

        ".zip", ".rar", ".7z", ".tar", ".gz",
        ".bz2", ".xz", ".zst", ".iso",

        ".pdf", ".doc", ".docx", ".xls", ".xlsx",
        ".ppt", ".pptx",

        ".db", ".sqlite", ".sqlite3", ".mdb",

        ".ttf", ".otf", ".woff", ".woff2",

        ".dat", ".cache", ".pak", ".bundle",
        ".wasm", ".o", ".obj", ".a", ".lib",
        ".pdb", ".ilk", ".pch"
    };
}



#endif //SCANNER_HPP
