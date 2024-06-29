cmake_minimum_required(VERSION 3.14)

project(simple_cli
        VERSION 0.1.0
        DESCRIPTION "Simple argument parser for C++"
        LANGUAGES CXX)
        
option(SMPLCLI_BUILD_TESTS "Build simple_cli tests" OFF)

add_library(simple_cli 
            src/tokenizer.cpp
            src/parameter.cpp
            src/program_parameters.cpp
            src/errors.cpp)

target_include_directories(simple_cli PUBLIC include/)

if(SMPLCLI_BUILD_TESTS)
add_subdirectory(tests)
endif()
