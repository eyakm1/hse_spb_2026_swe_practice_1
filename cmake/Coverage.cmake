# Code coverage (gcov + lcov) for C++.

option(CLI_ENABLE_COVERAGE "Enable code coverage (gcov/lcov)" OFF)

if(NOT CLI_ENABLE_COVERAGE)
    return()
endif()

if(MSVC)
    message(WARNING "Code coverage is not supported for MSVC in this setup")
    return()
endif()

# Apply coverage flags to a target: -O0 for reliable coverage, --coverage for instrumentation
function(cli_apply_coverage target)
    target_compile_options(${target} PRIVATE -O0 --coverage)
    target_link_options(${target} PRIVATE --coverage)
endfunction()

# Tools for HTML report
find_program(LCOV lcov)
find_program(GENHTML genhtml)

if(NOT LCOV OR NOT GENHTML)
    message(WARNING "CLI_ENABLE_COVERAGE=ON but lcov or genhtml not found. Install with: apt-get install lcov (Linux) or brew install lcov (macOS)")
    return()
endif()

# Custom target: run tests then generate coverage report
# Must be added after tests are defined (cli_tests exists)
macro(cli_add_coverage_target)
    # Disable strict consistency checks (Clang/LLVM gcov can produce quirks)
    file(WRITE "${CMAKE_BINARY_DIR}/.lcovrc" "check_data_consistency = 0\n")
    add_custom_target(coverage
        # Run tests to produce .gcda files
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
        # Capture coverage data (ignore Clang/LLVM gcov quirks in third-party code)
        COMMAND ${LCOV} --directory . --capture --output-file coverage.info
            --ignore-errors source,gcov,unsupported,inconsistent,format
        # Keep only project source files
        COMMAND ${LCOV} --remove coverage.info
            '*/_deps/*' '*/usr/*' '*/opt/*' '*/doctest/*'
            --output-file coverage.info
            --ignore-errors inconsistent,empty,format,unused
        # Generate HTML report
        COMMAND ${GENHTML} --demangle-cpp -o coverage coverage.info
            --ignore-errors inconsistent,empty,format,category
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Running tests and generating coverage report in ${CMAKE_BINARY_DIR}/coverage/"
    )
endmacro()
