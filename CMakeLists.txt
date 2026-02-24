cmake_minimum_required(VERSION 3.24)
project(cli VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(ProjectOptions)
include(CompilerWarnings)
include(Sanitizers)
include(Coverage)

option(CLI_ENABLE_MEMCHECK "Enable CTest memcheck (valgrind)" OFF)
if(CLI_ENABLE_MEMCHECK)
    find_program(MEMORYCHECK_COMMAND NAMES valgrind)
    if(NOT MEMORYCHECK_COMMAND)
        message(FATAL_ERROR "CLI_ENABLE_MEMCHECK=ON but valgrind not found")
    endif()

    set(MEMORYCHECK_COMMAND_OPTIONS
            "--leak-check=full --track-origins=yes --error-exitcode=1 --quiet")
endif()

include(CTest)
include(FetchContent)

FetchContent_Declare(CLI11
    GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
    GIT_TAG v2.4.1
)
FetchContent_MakeAvailable(CLI11)

add_subdirectory(src)
add_subdirectory(app)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

if(CLI_ENABLE_COVERAGE AND BUILD_TESTING AND LCOV AND GENHTML)
    cli_add_coverage_target()
endif()
