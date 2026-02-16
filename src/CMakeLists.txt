add_library(cli STATIC
        parser.cpp
        environment.cpp
        command_registry.cpp
        executor.cpp
        external_command.cpp
        command_line_interpreter.cpp
        commands/cat_command.cpp
        commands/echo_command.cpp
        commands/wc_command.cpp
        commands/pwd_command.cpp
        commands/exit_command.cpp
)

target_include_directories(cli
        PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
)

cli_apply_warnings(cli)
cli_apply_sanitizers(cli)
if(CLI_ENABLE_COVERAGE)
    cli_apply_coverage(cli)
endif()
