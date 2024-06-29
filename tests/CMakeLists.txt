cmake_minimum_required(VERSION 3.14)
project(simple_cli_tests)
include(../cmake/build_flags.cmake)

string(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}"
    is_top_level)


if(is_top_level)
add_library(simple_cli 
            ../src/tokenizer.cpp
            ../src/parameter.cpp
            ../src/program_parameters.cpp
            ../src/errors.cpp
            )

target_include_directories(simple_cli PUBLIC ../include/)
endif()

add_subdirectory(lib/Catch2)

add_executable(tests
                test_tokenizer.cpp
                test_parameter.cpp
                test_program_parameters.cpp
                )

target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
target_link_libraries(tests PRIVATE simple_cli)