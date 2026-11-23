#include <algorithm>
#include <iostream>
#include <chrono>

#include "include/Scanner.hpp"
#include "include/Config.hpp"

using namespace config; using namespace scanner;


int main(int argc, char* argv[]) {
    auto conf = load_config(argc, argv);

    if (!conf) {
        return 1;
    }

    Config config = std::move(*conf);


    auto scan_start = std::chrono::steady_clock::now();
    auto files_info = list_directory(config);
    auto scan_stop = std::chrono::steady_clock::now();

    auto scan_time = std::chrono::duration_cast<std::chrono::milliseconds>(scan_stop - scan_start);

    auto count_start = std::chrono::steady_clock::now();
    concurrent_count_lines(files_info);
    auto count_stop = std::chrono::steady_clock::now();

    auto count_time = std::chrono::duration_cast<std::chrono::milliseconds>(count_stop - count_start);

    auto extension_stats = list_extensions(files_info);
    auto number_of_files = files_info.size();

    std::cout << "Number of files: " << number_of_files << "\n";

    std::cout << "Extensions: \n";
    std::cout << "name | count | total lines \n";

    for (const auto& extension : extension_stats) {
        std::string name = extension.first;

        if (extension.first.empty()) {
            name = "[no name]";
        }

        std::cout << name << " "
                  << extension.second.files << " "
                  << extension.second.lines << "\n";
    }

    std::cout << "Scan time: " << scan_time.count() << " ms\n";
    std::cout << "Counting time: " << count_time.count() << " ms\n";

    return 0;
}