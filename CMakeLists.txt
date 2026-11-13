cmake_minimum_required(VERSION 3.20)
project(RepoScan)

set(CMAKE_CXX_STANDARD 20)

add_library(RepoScan_Lib STATIC
        src/Config.cpp
        src/Scanner.cpp
)

target_include_directories(RepoScan_Lib PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(RepoScan main.cpp)
target_link_libraries(RepoScan PRIVATE RepoScan_Lib)

# Compiler warnings
if(MSVC)
    set(PROJECT_WARNINGS /W4)
else()
    set(PROJECT_WARNINGS -Wall -Wextra -Wpedantic)
endif()

target_compile_options(RepoScan_Lib PRIVATE ${PROJECT_WARNINGS})

