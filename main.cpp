#include <algorithm>
#include <iostream>

#include "include/Scanner.hpp"
#include "include/Config.hpp"

using namespace config; using namespace scanner;


int main(int argc, char* argv[]) {
    auto conf = load_config(argc, argv);

    if(!conf) {return 1;}

    Config config = std::move(*conf);

    auto start = std::chrono::steady_clock::now();

    auto files_info = list_directory(config);
    concurrent_count_lines(files_info);

    auto stop = std::chrono::steady_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);


    auto extension_stats = list_extensions(files_info);
    auto number_of_files = files_info.size();


    //----------

    std::cout << "Number of files: " << number_of_files << "\n";

    std::cout << "Extensions: \n";
    std::cout << "name | count | total lines \n";
    for (const auto& extension: extension_stats) {
        std::string name = extension.first;
        if (extension.first.empty())
            name = "[no name]";
        std::cout << name << " " << extension.second.files << " " << extension.second.lines << "\n";
    }
    std::cout << "Scan time: " << time1.count() << " ms" << "\n";

    return 0;
}