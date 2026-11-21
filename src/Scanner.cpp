#include "include/Scanner.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <thread>

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

            if (it->is_directory(st_ec)) {
                if (is_skippable(path.filename(), config.excluded_directories)) {
                    it.disable_recursion_pending();
                }
            } else if (!st_ec && it->is_regular_file(st_ec)) {
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

            it.increment(ec);
            if (ec) {
                std::cerr << "Failed to advance: " << ec.message() << "\n";
                ec.clear();
            }
        }

        return file_infos;
    }

    void concurrent_count_lines(std::vector<FileInfo>& files_infos) {
        std::atomic<size_t> next_index{0};
        unsigned int n = std::thread::hardware_concurrency();
        std::vector<std::thread> threads;
        threads.reserve(n);

        for(unsigned int i = 0; i < n; i++) {
            threads.emplace_back(worker_task, std::ref(files_infos), std::ref(next_index));
        }

        for (auto& t : threads) {
            t.join();
        }
    }

    std::size_t count_file_lines(const fs::path &file_path) {
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
        return count;
    }

    void worker_task(std::vector<FileInfo>& files_infos, std::atomic<size_t>& next_index) {
        while (true) {
            std::size_t i = next_index.fetch_add(1);

            if(i >= files_infos.size())
                break;

            if(files_infos[i].size > 0) {
                files_infos[i].lines = count_file_lines(files_infos[i].path);
            }
        }
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
