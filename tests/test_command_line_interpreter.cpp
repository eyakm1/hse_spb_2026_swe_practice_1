#include <doctest/doctest.h>
#include "cli/command_line_interpreter.hpp"
#include <sstream>
#include <string>

using namespace cli;

TEST_CASE("CommandLineInterpreter echo then exit") {
    CommandLineInterpreter cli;
    std::stringstream in("echo hello\nexit\n");
    std::stringstream out, err;
    cli.run(in, out, err);
    // No prompt when in != std::cin
    CHECK(out.str().find("hello") != std::string::npos);
    CHECK(err.str().empty());
}

TEST_CASE("CommandLineInterpreter empty lines are ignored") {
    CommandLineInterpreter cli;
    std::stringstream in("\n\necho ok\n\n exit\n");
    std::stringstream out, err;
    cli.run(in, out, err);
    CHECK(out.str().find("ok") != std::string::npos);
}

TEST_CASE("CommandLineInterpreter exit stops the loop") {
    CommandLineInterpreter cli;
    std::stringstream in("exit\n");
    std::stringstream out, err;
    cli.run(in, out, err);
    // Should not hang; exit is first command
    CHECK(true);
}

TEST_CASE("CommandLineInterpreter multiple commands until exit") {
    CommandLineInterpreter cli;
    std::stringstream in("echo first\necho second\nexit\n");
    std::stringstream out, err;
    cli.run(in, out, err);
    CHECK(out.str().find("first") != std::string::npos);
    CHECK(out.str().find("second") != std::string::npos);
}

TEST_CASE("CommandLineInterpreter pwd runs without error") {
    CommandLineInterpreter cli;
    std::stringstream in("pwd\nexit\n");
    std::stringstream out, err;
    cli.run(in, out, err);
    std::string err_str = err.str();
    bool no_pwd_error = err_str.empty() || err_str.find("pwd") == std::string::npos;
    CHECK(no_pwd_error);
    std::string out_str = out.str();
    bool has_path_sep = out_str.find('/') != std::string::npos || out_str.find('\\') != std::string::npos;
    CHECK(has_path_sep);
}

TEST_CASE("CommandLineInterpreter quoted echo") {
    CommandLineInterpreter cli;
    std::stringstream in("echo 'hello world'\nexit\n");
    std::stringstream out, err;
    cli.run(in, out, err);
    CHECK(out.str().find("hello world") != std::string::npos);
}
