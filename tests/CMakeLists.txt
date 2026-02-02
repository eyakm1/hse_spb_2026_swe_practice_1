include(FetchContent)

FetchContent_Declare(doctest
        GIT_REPOSITORY https://github.com/doctest/doctest.git
        GIT_TAG v2.4.12
)

FetchContent_MakeAvailable(doctest)

add_executable(cli_tests
        doctest_main.cpp
        test_foo.cpp
)

target_link_libraries(cli_tests
    PRIVATE
        cli
        doctest::doctest
)

cli_apply_warnings(cli_tests)
cli_apply_sanitizers(cli_tests)

include(${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake)
doctest_discover_tests(cli_tests)
