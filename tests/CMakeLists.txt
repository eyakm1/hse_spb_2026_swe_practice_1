include(FetchContent)

FetchContent_Declare(doctest
        GIT_REPOSITORY https://github.com/doctest/doctest.git
        GIT_TAG v2.4.12
)

FetchContent_MakeAvailable(doctest)

add_executable(cli_tests
        doctest_main.cpp
        test_parser.cpp
        test_environment.cpp
        test_command_registry.cpp
        test_executor.cpp
        test_commands.cpp
        test_command_line_interpreter.cpp
)

target_link_libraries(cli_tests
    PRIVATE
        cli
        doctest::doctest
)

cli_apply_warnings(cli_tests)
cli_apply_sanitizers(cli_tests)
if(CLI_ENABLE_COVERAGE)
    cli_apply_coverage(cli_tests)
endif()

include(${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake)
doctest_discover_tests(cli_tests)
