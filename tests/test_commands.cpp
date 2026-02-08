#include <doctest/doctest.h>
#include "cli/commands/echo_command.hpp"
#include "cli/commands/cat_command.hpp"
#include "cli/commands/wc_command.hpp"
#include "cli/commands/pwd_command.hpp"
#include "cli/commands/exit_command.hpp"
#include "cli/environment.hpp"
#include <sstream>
#include <fstream>
#include <cstdio>
#include <string>

using namespace cli;

TEST_CASE("EchoCommand prints args space-separated with newline") {
    EchoCommand cmd;
    Environment env;
    std::stringstream in, out, err;
    std::vector<std::string> args = {"echo", "a", "b", "c"};
    int code = cmd.execute(args, in, out, err, env);
    CHECK(code == 0);
    CHECK(out.str() == "a b c\n");
}

TEST_CASE("EchoCommand single arg") {
    EchoCommand cmd;
    Environment env;
    std::stringstream in, out, err;
    int code = cmd.execute({"echo", "only"}, in, out, err, env);
    CHECK(code == 0);
    CHECK(out.str() == "only\n");
}

TEST_CASE("EchoCommand only command name") {
    EchoCommand cmd;
    Environment env;
    std::stringstream in, out, err;
    int code = cmd.execute({"echo"}, in, out, err, env);
    CHECK(code == 0);
    CHECK(out.str() == "\n");
}

TEST_CASE("ExitCommand returns -1") {
    ExitCommand cmd;
    Environment env;
    std::stringstream in, out, err;
    int code = cmd.execute({"exit"}, in, out, err, env);
    CHECK(code == -1);
}

TEST_CASE("CatCommand missing file operand") {
    CatCommand cmd;
    Environment env;
    std::stringstream in, out, err;
    int code = cmd.execute({"cat"}, in, out, err, env);
    CHECK(code == 1);
    CHECK(err.str().find("missing file operand") != std::string::npos);
}

TEST_CASE("CatCommand cannot open file") {
    CatCommand cmd;
    Environment env;
    std::stringstream in, out, err;
    int code = cmd.execute({"cat", "/nonexistent/path/xyz123"}, in, out, err, env);
    CHECK(code == 1);
    CHECK(err.str().find("cannot open") != std::string::npos);
}

TEST_CASE("CatCommand reads and outputs file") {
    std::string path = "cli_test_cat_fixture.txt";
    {
        std::ofstream f(path, std::ios::binary);
        REQUIRE(f);
        f << "line1\nline2\n";
    }
    CatCommand cmd;
    Environment env;
    std::stringstream in, out, err;
    int code = cmd.execute({"cat", path}, in, out, err, env);
    std::remove(path.c_str());
    CHECK(code == 0);
    CHECK(out.str() == "line1\nline2\n");
}

TEST_CASE("WcCommand missing file operand") {
    WcCommand cmd;
    Environment env;
    std::stringstream in, out, err;
    int code = cmd.execute({"wc"}, in, out, err, env);
    CHECK(code == 1);
    CHECK(err.str().find("missing file operand") != std::string::npos);
}

TEST_CASE("WcCommand cannot open file") {
    WcCommand cmd;
    Environment env;
    std::stringstream in, out, err;
    int code = cmd.execute({"wc", "/nonexistent/xyz123"}, in, out, err, env);
    CHECK(code == 1);
    CHECK(err.str().find("cannot open") != std::string::npos);
}

TEST_CASE("WcCommand counts lines words bytes") {
    std::string path = "cli_test_wc_fixture.txt";
    {
        std::ofstream f(path);
        REQUIRE(f);
        f << "hello world\nsecond line\n";
    }
    WcCommand cmd;
    Environment env;
    std::stringstream in, out, err;
    int code = cmd.execute({"wc", path}, in, out, err, env);
    std::remove(path.c_str());
    CHECK(code == 0);
    // "hello world\nsecond line\n" = 2 lines, 4 words, 24 bytes (or 23 if no trailing newline)
    CHECK(out.str().find(" 2 ") != std::string::npos);
    CHECK(out.str().find(" 4 ") != std::string::npos);
    CHECK(out.str().find(path) != std::string::npos);
}

TEST_CASE("PwdCommand returns 0 and prints something") {
    PwdCommand cmd;
    Environment env;
    std::stringstream in, out, err;
    int code = cmd.execute({"pwd"}, in, out, err, env);
    CHECK(code == 0);
    CHECK(!out.str().empty());
    CHECK(out.str().back() == '\n');
    // Should look like a path (contain slash or backslash)
    bool has_sep = out.str().find('/') != std::string::npos || out.str().find('\\') != std::string::npos;
    CHECK(has_sep);
}
