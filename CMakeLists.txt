project(simple_cli)
cmake_minimum_required(VERSION 3.22)


add_library(simple_cli 
            src/tokenizer.cpp)

target_include_directories(simple_cli PUBLIC include/)