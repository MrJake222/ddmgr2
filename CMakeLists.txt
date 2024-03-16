cmake_minimum_required(VERSION 3.8.2)
# add_executable

project(ddmgr2)

set(CMAKE_CXX_STANDARD 17)

add_executable(ddmgr2
        src/main.cpp
        src/flags.cpp
        src/parse.cpp
        src/datatypes/subnet.cpp
        src/datatypes/host.cpp
        src/datatypes/ipv4.cpp
        src/datatypes/ipv6.cpp
        src/datatypes/mac.cpp
        src/datatypes/portmap.cpp
        src/datatypes/ns.cpp
        src/datatypes/mx.cpp
        src/output.cpp
        )

target_include_directories(ddmgr2 PRIVATE
        src
        src/datatypes
        )

target_link_libraries(ddmgr2
        yaml-cpp
        gflags
        )
