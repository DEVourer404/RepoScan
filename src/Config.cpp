#include "include/Config.hpp"

#include <iostream>
#include <algorithm>

namespace config {
    std::vector<std::string> get_excluded_directories(int argc, char* argv[]) {
        if(argc < 3)
            return default_excluded_directories;

        std::vector<std::string> excluded_directories{};

        for (int i = 2; i < argc; i++) {
            excluded_directories.emplace_back(argv[i]);
        }

        for (const auto& str : default_excluded_directories) {
            if (std::find(excluded_directories.begin(), excluded_directories.end(), str) == excluded_directories.end()) {
                excluded_directories.push_back(str);
            }
        }
        return excluded_directories;
    }

    std::optional<fs::path> get_path(int argc, char* argv[]) {
        if(argc < 2) {
            std::cerr << "No path to directory! \n";
            return {};
        }

        fs::path dir_path = argv[1];

        if(!fs::exists(dir_path) || !fs::is_directory(dir_path)) {
            std::cerr << "Directory does not exist! \n";
            return {};
        }

        return dir_path;
    }

    std::optional<Config> load_config(int argc, char* argv[]) {
        Config config;

        auto path = get_path(argc, argv);

        if(!path) {
            return {};
        }

        config.dir_path = path.value();
        config.excluded_directories = get_excluded_directories(argc, argv);

        return config;
    }


}
