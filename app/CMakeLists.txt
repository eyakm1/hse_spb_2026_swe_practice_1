add_executable(cli_app
        main.cpp
)
target_link_libraries(cli_app PRIVATE cli)

cli_apply_warnings(cli_app)
cli_apply_sanitizers(cli_app)
if(CLI_ENABLE_COVERAGE)
    cli_apply_coverage(cli_app)
endif()
