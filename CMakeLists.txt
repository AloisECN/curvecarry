cmake_minimum_required(VERSION 3.20)
project(curvecarry LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(
            curvecarry src/main.cpp
            src/csv_loader.cpp
            )
target_include_directories(curvecarry PRIVATE include)