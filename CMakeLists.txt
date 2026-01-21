cmake_minimum_required(VERSION 3.20)
project(nuke)

find_package(CLI11 CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(yaml-cpp CONFIG REQUIRED)

file(GLOB_RECURSE SOURCES "src/*.cpp" "include/*.hpp")

add_executable(nuke ${SOURCES})

target_include_directories(nuke PRIVATE include)

target_link_libraries(nuke PRIVATE 
    CLI11::CLI11 
    fmt::fmt 
    yaml-cpp::yaml-cpp
)

set_property(TARGET nuke PROPERTY CXX_STANDARD 20)