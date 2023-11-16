cmake_minimum_required(VERSION 3.25)
project(ddmgr2)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(libs/yaml-cpp)

add_executable(ddmgr2
        src/main.cpp)

target_link_libraries(ddmgr2
        yaml-cpp)
