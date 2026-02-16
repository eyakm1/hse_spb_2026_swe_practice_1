#include "cli/ast.hpp"
#include "cli/parser.hpp"
#include <doctest/doctest.h>

using namespace cli;

static const CommandNode &first_command(const std::optional<Pipeline> &pl) {
  REQUIRE(pl.has_value());
  REQUIRE(!pl->empty());
  return (*pl)[0];
}

TEST_CASE("Parser returns nullopt for empty or whitespace-only line") {
  Parser p;
  CHECK(p.parse("") == std::nullopt);
  CHECK(p.parse("   ") == std::nullopt);
  CHECK(p.parse("\t\n ") == std::nullopt);
}

TEST_CASE("Parser parses single token as command name with no args") {
  Parser p;
  auto pl = p.parse("echo");
  const auto &node = first_command(pl);
  CHECK(node.name == "echo");
  CHECK(node.args.empty());
}

TEST_CASE("Parser parses multiple unquoted tokens") {
  Parser p;
  auto pl = p.parse("cat file1.txt file2.txt");
  const auto &node = first_command(pl);
  CHECK(node.name == "cat");
  REQUIRE(node.args.size() == 2);
  CHECK(node.args[0] == "file1.txt");
  CHECK(node.args[1] == "file2.txt");
}

TEST_CASE("Parser collapses multiple spaces between tokens") {
  Parser p;
  auto pl = p.parse("echo   hello   world");
  const auto &node = first_command(pl);
  CHECK(node.name == "echo");
  REQUIRE(node.args.size() == 2);
  CHECK(node.args[0] == "hello");
  CHECK(node.args[1] == "world");
}

TEST_CASE("Single quote is weak quoting with backslash escapes") {
  Parser p;
  // \' = literal single quote
  auto pl = p.parse("echo 'don\\'t'");
  const auto &node = first_command(pl);
  REQUIRE(node.args.size() == 1);
  CHECK(node.args[0] == "don't");

  // \\ = literal backslash
  pl = p.parse("x 'a\\\\b'");
  CHECK(first_command(pl).args[0] == "a\\b");

  // \n = newline
  pl = p.parse("x 'line1\\nline2'");
  CHECK(first_command(pl).args[0] == "line1\nline2");

  // \t = tab
  pl = p.parse("x 'a\\tb'");
  CHECK(first_command(pl).args[0] == "a\tb");

  // \r = carriage return
  pl = p.parse("x 'a\\rb'");
  CHECK(first_command(pl).args[0] == "a\rb");

  // Unknown escape \X → backslash + X
  pl = p.parse("x 'a\\xb'");
  CHECK(first_command(pl).args[0] == "a\\xb");
}

TEST_CASE("Single-quoted token can contain spaces") {
  Parser p;
  auto pl = p.parse("echo 'hello world'");
  const auto &node = first_command(pl);
  REQUIRE(node.args.size() == 1);
  CHECK(node.args[0] == "hello world");
}

TEST_CASE("Double quote is full quoting with no escapes") {
  Parser p;
  // Backslash is literal inside double quotes
  auto pl = p.parse("echo \"path\\to\\file\"");
  CHECK(first_command(pl).args[0] == "path\\to\\file");

  // Double quote inside must end the token (no escape), so we get empty + rest
  pl = p.parse("echo \"say\"hello");
  REQUIRE(first_command(pl).args.size() >= 1);
  CHECK(first_command(pl).args[0] == "say");
  CHECK(first_command(pl).args[1] == "hello");
}

TEST_CASE("Double-quoted token can contain spaces and backslashes literally") {
  Parser p;
  auto pl = p.parse("x \"a b c\"");
  CHECK(first_command(pl).args[0] == "a b c");

  pl = p.parse("x \"\\n is not newline\"");
  CHECK(first_command(pl).args[0] == "\\n is not newline");
}

TEST_CASE("Mixed quoted and unquoted tokens") {
  Parser p;
  auto pl = p.parse("cmd unquoted 'single quoted' \"double quoted\"");
  const auto &node = first_command(pl);
  CHECK(node.name == "cmd");
  REQUIRE(node.args.size() == 3);
  CHECK(node.args[0] == "unquoted");
  CHECK(node.args[1] == "single quoted");
  CHECK(node.args[2] == "double quoted");
}

TEST_CASE("Adjacent quoted segments form separate tokens") {
  Parser p;
  auto pl = p.parse("echo 'a' 'b'");
  const auto &node = first_command(pl);
  REQUIRE(node.args.size() == 2);
  CHECK(node.args[0] == "a");
  CHECK(node.args[1] == "b");
}

TEST_CASE("Empty quoted between two quoted tokens") {
  Parser p;
  // 'a' '' 'b' => need four quotes between a and b: close a, open empty, close
  // empty, open b
  auto pl = p.parse("echo 'a''''b'");
  const auto &node = first_command(pl);
  REQUIRE(node.args.size() == 3);
  CHECK(node.args[0] == "a");
  CHECK(node.args[1] == "");
  CHECK(node.args[2] == "b");
}

TEST_CASE("Empty single-quoted string is valid token") {
  Parser p;
  auto pl = p.parse("echo ''");
  const auto &node = first_command(pl);
  REQUIRE(node.args.size() == 1);
  CHECK(node.args[0] == "");
}

TEST_CASE("Empty double-quoted string is valid token") {
  Parser p;
  auto pl = p.parse("echo \"\"");
  const auto &node = first_command(pl);
  REQUIRE(node.args.size() == 1);
  CHECK(node.args[0] == "");
}

TEST_CASE("Parser splits by pipe outside quotes") {
  Parser p;
  auto pl = p.parse("echo hello | wc");
  REQUIRE(pl.has_value());
  REQUIRE(pl->size() == 2);
  CHECK((*pl)[0].name == "echo");
  REQUIRE((*pl)[0].args.size() == 1);
  CHECK((*pl)[0].args[0] == "hello");
  CHECK((*pl)[1].name == "wc");
  CHECK((*pl)[1].args.empty());
}

TEST_CASE("Parser pipe inside double quotes is literal") {
  Parser p;
  auto pl = p.parse("echo \"a|b\"");
  REQUIRE(pl->size() == 1);
  REQUIRE(first_command(pl).args.size() == 1);
  CHECK(first_command(pl).args[0] == "a|b");
}

TEST_CASE("Parser pipe inside single quotes is literal") {
  Parser p;
  auto pl = p.parse("echo 'x|y'");
  REQUIRE(pl->size() == 1);
  CHECK(first_command(pl).args[0] == "x|y");
}

TEST_CASE("Parser three-way pipeline") {
  Parser p;
  auto pl = p.parse("cat | wc | echo");
  REQUIRE(pl.has_value());
  REQUIRE(pl->size() == 3);
  CHECK((*pl)[0].name == "cat");
  CHECK((*pl)[1].name == "wc");
  CHECK((*pl)[2].name == "echo");
}

TEST_CASE("Parser leading pipe yields empty first command") {
  Parser p;
  auto pl = p.parse("| echo x");
  REQUIRE(pl.has_value());
  REQUIRE(pl->size() == 2);
  CHECK((*pl)[0].name == "");
  CHECK((*pl)[1].name == "echo");
  REQUIRE((*pl)[1].args.size() == 1);
  CHECK((*pl)[1].args[0] == "x");
}

TEST_CASE("Parser trailing pipe yields empty second command") {
  Parser p;
  auto pl = p.parse("echo x |");
  REQUIRE(pl.has_value());
  REQUIRE(pl->size() == 2);
  CHECK((*pl)[0].name == "echo");
  CHECK((*pl)[0].args[0] == "x");
  CHECK((*pl)[1].name == "");
}

TEST_CASE("Parser consecutive pipes yield empty middle segment") {
  Parser p;
  auto pl = p.parse("echo | | wc");
  REQUIRE(pl.has_value());
  REQUIRE(pl->size() == 3);
  CHECK((*pl)[0].name == "echo");
  CHECK((*pl)[1].name == "");
  CHECK((*pl)[2].name == "wc");
}

TEST_CASE("Parser single pipe only") {
  Parser p;
  auto pl = p.parse("|");
  REQUIRE(pl.has_value());
  REQUIRE(pl->size() == 2);
  CHECK((*pl)[0].name == "");
  CHECK((*pl)[1].name == "");
}

TEST_CASE("Parser substitute flag single-quoted is No") {
  Parser p;
  auto pl = p.parse("echo '$x'");
  REQUIRE(pl->size() == 1);
  const auto &node = first_command(pl);
  REQUIRE(node.args.size() == 1);
  CHECK(node.args[0] == "$x");
  REQUIRE(node.substitute_arg.size() == 1);
  CHECK(node.substitute_arg[0] == Substitute::No);
}

TEST_CASE("Parser substitute flag double-quoted is Yes") {
  Parser p;
  auto pl = p.parse("echo \"$x\"");
  REQUIRE(pl->size() == 1);
  const auto &node = first_command(pl);
  REQUIRE(node.args.size() == 1);
  CHECK(node.args[0] == "$x");
  REQUIRE(node.substitute_arg.size() == 1);
  CHECK(node.substitute_arg[0] == Substitute::Yes);
}
