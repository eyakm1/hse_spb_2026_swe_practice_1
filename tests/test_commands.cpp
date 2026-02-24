#include "cli/commands/cat_command.hpp"
#include "cli/commands/echo_command.hpp"
#include "cli/commands/exit_command.hpp"
#include "cli/commands/grep_command.hpp"
#include "cli/commands/pwd_command.hpp"
#include "cli/commands/wc_command.hpp"
#include "cli/environment.hpp"
#include <cstdio>
#include <doctest/doctest.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

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

TEST_CASE("ExitCommand returns -1 for exit with no arg") {
  ExitCommand cmd;
  Environment env;
  std::stringstream in, out, err;
  int code = cmd.execute({"exit"}, in, out, err, env);
  CHECK(code == -1);
}

TEST_CASE("ExitCommand returns -(n+1) for exit n") {
  ExitCommand cmd;
  Environment env;
  std::stringstream in, out, err;
  int code = cmd.execute({"exit", "5"}, in, out, err, env);
  CHECK(code == -6);
}

TEST_CASE("ExitCommand invalid argument prints error and returns -256") {
  ExitCommand cmd;
  Environment env;
  std::stringstream in, out, err;
  int code = cmd.execute({"exit", "x"}, in, out, err, env);
  CHECK(code == -256);
  CHECK(err.str().find("numeric argument required") != std::string::npos);
}

TEST_CASE("CatCommand with no args reads from stdin") {
  CatCommand cmd;
  Environment env;
  std::stringstream in("hello from stdin\n"), out, err;
  int code = cmd.execute({"cat"}, in, out, err, env);
  CHECK(code == 0);
  CHECK(out.str() == "hello from stdin\n");
}

TEST_CASE("CatCommand cannot open file") {
  CatCommand cmd;
  Environment env;
  std::stringstream in, out, err;
  int code =
      cmd.execute({"cat", "/nonexistent/path/xyz123"}, in, out, err, env);
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

TEST_CASE("WcCommand with no args reads from stdin") {
  WcCommand cmd;
  Environment env;
  std::stringstream in("hello world\n"), out, err;
  int code = cmd.execute({"wc"}, in, out, err, env);
  CHECK(code == 0);
  CHECK(out.str().find(" 1 ") != std::string::npos); // 1 line
  CHECK(out.str().find(" 2 ") != std::string::npos); // 2 words
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
  // "hello world\nsecond line\n" = 2 lines, 4 words, 24 bytes (or 23 if no
  // trailing newline)
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
  bool has_sep = out.str().find('/') != std::string::npos ||
                 out.str().find('\\') != std::string::npos;
  CHECK(has_sep);
}

// --- GrepCommand tests ---

TEST_CASE("GrepCommand missing pattern returns 2") {
  GrepCommand cmd;
  Environment env;
  std::stringstream in, out, err;
  int code = cmd.execute({"grep"}, in, out, err, env);
  CHECK(code == 2);
  CHECK(err.str().find("missing pattern") != std::string::npos);
}

TEST_CASE("GrepCommand invalid regex returns 2") {
  GrepCommand cmd;
  Environment env;
  std::stringstream in, out, err;
  int code = cmd.execute({"grep", "["}, in, out, err, env);
  CHECK(code == 2);
  CHECK(err.str().find("invalid regular expression") != std::string::npos);
}

TEST_CASE("GrepCommand basic regex match in file") {
  std::string path = "cli_test_grep_fixture.txt";
  {
    std::ofstream f(path);
    REQUIRE(f);
    f << "Minimal syntax grep\nline two\nMinimal again\n";
  }
  GrepCommand cmd;
  Environment env;
  std::stringstream in, out, err;
  int code = cmd.execute({"grep", "Minimal", path}, in, out, err, env);
  std::remove(path.c_str());
  CHECK(code == 0);
  CHECK(out.str().find("Minimal") != std::string::npos);
}

TEST_CASE("GrepCommand regex anchor start") {
  std::string path = "cli_test_grep_anchor.txt";
  {
    std::ofstream f(path);
    REQUIRE(f);
    f << "Minimal syntax grep\nline two\nMinimal again\n";
  }
  GrepCommand cmd;
  Environment env;
  std::stringstream in, out, err;
  int code = cmd.execute({"grep", "^Minimal", path}, in, out, err, env);
  std::remove(path.c_str());
  CHECK(code == 0);
  CHECK(out.str() == "Minimal syntax grep\nMinimal again\n");
}

TEST_CASE("GrepCommand -i case insensitive") {
  std::string path = "cli_test_grep_i.txt";
  {
    std::ofstream f(path);
    REQUIRE(f);
    f << "Minimal syntax grep\n";
  }
  GrepCommand cmd;
  Environment env;
  std::stringstream in, out, err;
  int code = cmd.execute({"grep", "-i", "minimal", path}, in, out, err, env);
  std::remove(path.c_str());
  CHECK(code == 0);
  CHECK(out.str().find("Minimal") != std::string::npos);
}

TEST_CASE("GrepCommand -w whole word no partial match") {
  std::string path = "cli_test_grep_w.txt";
  {
    std::ofstream f(path);
    REQUIRE(f);
    f << "Minimal syntax\n";
  }
  GrepCommand cmd;
  Environment env;
  std::stringstream in, out, err;
  int code = cmd.execute({"grep", "-w", "Minima", path}, in, out, err, env);
  std::remove(path.c_str());
  CHECK(code == 1);
  CHECK(out.str().empty());
}

TEST_CASE("GrepCommand -w whole word match") {
  std::string path = "cli_test_grep_w2.txt";
  {
    std::ofstream f(path);
    REQUIRE(f);
    f << "foo bar baz\n";
  }
  GrepCommand cmd;
  Environment env;
  std::stringstream in, out, err;
  int code = cmd.execute({"grep", "-w", "bar", path}, in, out, err, env);
  std::remove(path.c_str());
  CHECK(code == 0);
  CHECK(out.str() == "foo bar baz\n");
}

TEST_CASE("GrepCommand -A after context") {
  std::string path = "cli_test_grep_A.txt";
  {
    std::ofstream f(path);
    REQUIRE(f);
    f << "line0\nII\nline2\nline3\nII\nline5\n";
  }
  GrepCommand cmd;
  Environment env;
  std::stringstream in, out, err;
  int code = cmd.execute({"grep", "-A", "1", "II", path}, in, out, err, env);
  std::remove(path.c_str());
  CHECK(code == 0);
  CHECK(out.str() == "II\nline2\nII\nline5\n");
}

TEST_CASE("GrepCommand -A overlapping context merged") {
  std::string path = "cli_test_grep_A_overlap.txt";
  {
    std::ofstream f(path);
    REQUIRE(f);
    f << "a\nX\nb\nX\nc\n";
  }
  GrepCommand cmd;
  Environment env;
  std::stringstream in, out, err;
  int code = cmd.execute({"grep", "-A", "1", "X", path}, in, out, err, env);
  std::remove(path.c_str());
  CHECK(code == 0);
  // Overlapping: match at line 1 (a\nX\nb) and line 3 (b\nX\nc). Merged: 1,2,3,4
  CHECK(out.str() == "X\nb\nX\nc\n");
}

TEST_CASE("GrepCommand no match returns 1") {
  std::string path = "cli_test_grep_nomatch.txt";
  {
    std::ofstream f(path);
    REQUIRE(f);
    f << "hello\nworld\n";
  }
  GrepCommand cmd;
  Environment env;
  std::stringstream in, out, err;
  int code = cmd.execute({"grep", "xyz", path}, in, out, err, env);
  std::remove(path.c_str());
  CHECK(code == 1);
  CHECK(out.str().empty());
}

TEST_CASE("GrepCommand reads from stdin when no file") {
  GrepCommand cmd;
  Environment env;
  std::stringstream in("alpha\nbeta\nalpha\n"), out, err;
  int code = cmd.execute({"grep", "alpha"}, in, out, err, env);
  CHECK(code == 0);
  CHECK(out.str() == "alpha\nalpha\n");
}
