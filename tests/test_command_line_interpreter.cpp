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

TEST_CASE("CommandLineInterpreter variable substitution") {
    CommandLineInterpreter cli;
    std::stringstream in("X=foo\necho $X\n exit\n");
    std::stringstream out, err;
    cli.run(in, out, err);
    CHECK(out.str().find("foo") != std::string::npos);
}

TEST_CASE("CommandLineInterpreter pipe") {
    CommandLineInterpreter cli;
    std::stringstream in("echo a b c | wc\nexit\n");
    std::stringstream out, err;
    cli.run(in, out, err);
    CHECK(out.str().find("1") != std::string::npos);
    CHECK(out.str().find("3") != std::string::npos);
}

TEST_CASE("CommandLineInterpreter assignment then command") {
    CommandLineInterpreter cli;
    std::stringstream in("A=1 B=2 echo $A $B\nexit\n");
    std::stringstream out, err;
    cli.run(in, out, err);
    CHECK(out.str().find("1") != std::string::npos);
    CHECK(out.str().find("2") != std::string::npos);
}

TEST_CASE("CommandLineInterpreter single-quoted no substitution") {
    CommandLineInterpreter cli;
    std::stringstream in("V=expanded\necho '$V'\nexit\n");
    std::stringstream out, err;
    cli.run(in, out, err);
    CHECK(out.str().find("$V") != std::string::npos);
    CHECK(out.str().find("expanded") == std::string::npos);
}

TEST_CASE("CommandLineInterpreter only assignments no command continues") {
    CommandLineInterpreter cli;
    std::stringstream in("A=1 B=2\necho $A$B\nexit\n");
    std::stringstream out, err;
    cli.run(in, out, err);
    CHECK(out.str().find("12") != std::string::npos);
}

TEST_CASE("CommandLineInterpreter assignment with empty value") {
    CommandLineInterpreter cli;
    std::stringstream in("X=\necho a${X}b\nexit\n");
    std::stringstream out, err;
    cli.run(in, out, err);
    CHECK(out.str().find("ab") != std::string::npos);
}

TEST_CASE("CommandLineInterpreter leading pipe drops empty and runs rest") {
    CommandLineInterpreter cli;
    std::stringstream in("| echo ok\nexit\n");
    std::stringstream out, err;
    cli.run(in, out, err);
    CHECK(out.str().find("ok") != std::string::npos);
}

TEST_CASE("CommandLineInterpreter empty command in middle of pipeline reports error") {
    CommandLineInterpreter cli;
    std::stringstream in("echo a | | wc\nexit\n");
    std::stringstream out, err;
    cli.run(in, out, err);
    CHECK(err.str().find("empty command") != std::string::npos);
}

TEST_CASE("CommandLineInterpreter substitution in assignment value") {
    CommandLineInterpreter cli;
    std::stringstream in("B=world\nA=hello_$B\necho $A\nexit\n");
    std::stringstream out, err;
    cli.run(in, out, err);
    CHECK(out.str().find("hello_world") != std::string::npos);
}

TEST_CASE("CommandLineInterpreter pipe then exit in pipeline stops REPL") {
    CommandLineInterpreter cli;
    std::stringstream in("echo hi | exit\n");
    std::stringstream out, err;
    cli.run(in, out, err);
    CHECK(out.str().empty());
}

TEST_CASE("CommandLineInterpreter three-way pipe") {
    CommandLineInterpreter cli;
    std::stringstream in("echo one two three | wc | wc\nexit\n");
    std::stringstream out, err;
    cli.run(in, out, err);
    CHECK(err.str().empty());
    CHECK(out.str().find("1") != std::string::npos);
}
