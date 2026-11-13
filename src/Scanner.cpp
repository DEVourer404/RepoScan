#include "include/Scanner.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>

namespace scanner {
    bool is_skippable(const fs::path &filename, const std::vector<std::string>& excluded_directories) {
        return std::ranges::any_of(excluded_directories, [&](std::string_view excluded) {
            return filename == excluded;
        });
    }

    std::vector<FileInfo> list_directory(const config::Config& config) {
        std::vector<FileInfo> file_infos{};
        std::error_code ec;

        if (!fs::is_directory(config.dir_path, ec) || ec) {
            std::cerr << "Invalid directory or insufficient permissions: " << config.dir_path << "\n";
            return {};
        }

        auto options = fs::directory_options::skip_permission_denied;
        fs::recursive_directory_iterator it(config.dir_path, options, ec);
        fs::recursive_directory_iterator end;

        if (ec) {
            std::cerr << "Failed to open the root directory: " << ec.message() << "\n";
            return {};
        }

        while (it != end) {
            const auto& path = it->path();
            std::error_code st_ec;
            const auto st = it->status(st_ec);

            if (!st_ec) {
                if (fs::is_directory(st)) {
                    if (is_skippable(path.filename(), config.excluded_directories)) {
                        it.disable_recursion_pending();
                    }
                } else if (fs::is_regular_file(st)) {
                    std::error_code size_ec;
                    const auto size = it->file_size(size_ec);
                    if (!size_ec) {
                        file_infos.push_back({
                            .path = path,
                            .file_name = path.filename().string(),
                            .extension = path.extension().string(),
                            .size = size,
                            .lines = 0,
                        });
                    }
                }
            }

            it.increment(ec);
            if (ec) {
                std::cerr << "Failed to advance: " << ec.message() << "\n";
                ec.clear();
            }
        }

        for (auto& info : file_infos) {
            if (info.size == 0) continue;
            info.lines = count_lines(info.path);
        }
        return file_infos;
    }


    std::size_t count_lines(const fs::path &file_path) {
        // if (!fs::is_regular_file(file_path)) {
        //     std::cerr << "Path does not exist or is not a regular file: " << file_path << '\n';
        //     return 0;
        // }

        const auto ext = file_path.extension().string();
        if (std::find(excluded_extensions.begin(), excluded_extensions.end(), ext) != excluded_extensions.end()) {
                return 0;
        }

        std::ifstream file(file_path);
        if (!file.is_open()) {
            std::cerr << "Failed to open the file: " << file_path << '\n';
            return 0;
        }

        std::size_t count = 0;


        while (file.read(buffer.data(), buffer.size()) || file.gcount() > 0) {
            for (std::streamsize i = 0; i < file.gcount(); i++) {
                if (buffer[i] == '\n') count++;
            }
        }

        // std::string current_line;
        //
        //  while (std::getline(file, current_line)) {
        //      ++count;
        //  }

        return count;
    }


    std::unordered_map<std::string, ExtStats> list_extensions(const std::vector<FileInfo>& file_infos) {
        std::unordered_map<std::string, ExtStats> ext_stats;

        for (const auto& entry: file_infos) {
            ext_stats[entry.extension] += {
                .files = 1,
                .lines = entry.lines,
                .bytes = entry.size,
            };
        }

        return ext_stats;
    }
}
