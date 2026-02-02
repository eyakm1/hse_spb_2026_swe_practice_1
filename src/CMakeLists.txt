add_library(cli STATIC
        foo.cpp
)

target_include_directories(cli
        PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
)

cli_apply_warnings(cli)
cli_apply_sanitizers(cli)
