cmake_minimum_required(VERSION 3.20)
project(RepoScan)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
    set(PROJECT_OPTIMIZATION /O2)
else()
    set(PROJECT_WARNINGS -Wall -Wextra -Wpedantic)
    set(PROJECT_OPTIMIZATION -O3)
endif()

# Warnings in all configurations
target_compile_options(RepoScan_Lib PRIVATE
        ${PROJECT_WARNINGS}
)

target_compile_options(RepoScan PRIVATE
        ${PROJECT_WARNINGS}
)

# Optimization only in Release
target_compile_options(RepoScan_Lib PRIVATE
        $<$<CONFIG:Release>:${PROJECT_OPTIMIZATION}>
)

target_compile_options(RepoScan PRIVATE
        $<$<CONFIG:Release>:${PROJECT_OPTIMIZATION}>
)
