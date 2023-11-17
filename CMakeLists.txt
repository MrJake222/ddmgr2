cmake_minimum_required(VERSION 3.25)
project(ddmgr2)

set(CMAKE_CXX_STANDARD 17)

add_executable(ddmgr2
        src/main.cpp
        src/subnet.cpp
        src/host.cpp
        src/parse.cpp
        src/datatypes/ipv4.cpp
        src/datatypes/ipv6.cpp
        src/datatypes/mac.cpp
        src/datatypes/portmap.cpp
        )

target_include_directories(ddmgr2 PRIVATE
        src
        src/datatypes
        )

target_link_libraries(ddmgr2
        yaml-cpp
        )
