#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <vector>
#include <filesystem>
#include <optional>

namespace fs = std::filesystem;

namespace config {
    struct Config {
        fs::path dir_path;
        std::vector<std::string> excluded_directories;
    };

    //const std::vector<std::string> default_excluded_directories{".git", "build", ".idea", ".vscode", ".github", "cmake-build-debug", "cmake-build-release", "node_modules"};
    const std::vector<std::string> default_excluded_directories{".gi"};

    std::optional<Config> load_config(int argc, char* argv[]);
    std::vector<std::string> get_excluded_directories(int argc, char* argv[]);
    std::optional<fs::path> get_path(int argc, char* argv[]);
}



#endif //CONFIG_HPP
