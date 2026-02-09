#include <doctest/doctest.h>
#include "cli/parser.hpp"

using namespace cli;

TEST_CASE("Parser returns nullopt for empty or whitespace-only line") {
    Parser p;
    CHECK(p.parse("") == std::nullopt);
    CHECK(p.parse("   ") == std::nullopt);
    CHECK(p.parse("\t\n ") == std::nullopt);
}

TEST_CASE("Parser parses single token as command name with no args") {
    Parser p;
    auto node = p.parse("echo");
    REQUIRE(node.has_value());
    CHECK(node->name == "echo");
    CHECK(node->args.empty());
}

TEST_CASE("Parser parses multiple unquoted tokens") {
    Parser p;
    auto node = p.parse("cat file1.txt file2.txt");
    REQUIRE(node.has_value());
    CHECK(node->name == "cat");
    REQUIRE(node->args.size() == 2);
    CHECK(node->args[0] == "file1.txt");
    CHECK(node->args[1] == "file2.txt");
}

TEST_CASE("Parser collapses multiple spaces between tokens") {
    Parser p;
    auto node = p.parse("echo   hello   world");
    REQUIRE(node.has_value());
    CHECK(node->name == "echo");
    REQUIRE(node->args.size() == 2);
    CHECK(node->args[0] == "hello");
    CHECK(node->args[1] == "world");
}

TEST_CASE("Single quote is weak quoting with backslash escapes") {
    Parser p;
    // \' = literal single quote
    auto node = p.parse("echo 'don\\'t'");
    REQUIRE(node.has_value());
    REQUIRE(node->args.size() == 1);
    CHECK(node->args[0] == "don't");

    // \\ = literal backslash
    node = p.parse("x 'a\\\\b'");
    REQUIRE(node.has_value());
    CHECK(node->args[0] == "a\\b");

    // \n = newline
    node = p.parse("x 'line1\\nline2'");
    REQUIRE(node.has_value());
    CHECK(node->args[0] == "line1\nline2");

    // \t = tab
    node = p.parse("x 'a\\tb'");
    REQUIRE(node.has_value());
    CHECK(node->args[0] == "a\tb");

    // \r = carriage return
    node = p.parse("x 'a\\rb'");
    REQUIRE(node.has_value());
    CHECK(node->args[0] == "a\rb");

    // Unknown escape \X → backslash + X
    node = p.parse("x 'a\\xb'");
    REQUIRE(node.has_value());
    CHECK(node->args[0] == "a\\xb");
}

TEST_CASE("Single-quoted token can contain spaces") {
    Parser p;
    auto node = p.parse("echo 'hello world'");
    REQUIRE(node.has_value());
    REQUIRE(node->args.size() == 1);
    CHECK(node->args[0] == "hello world");
}

TEST_CASE("Double quote is full quoting with no escapes") {
    Parser p;
    // Backslash is literal inside double quotes
    auto node = p.parse("echo \"path\\to\\file\"");
    REQUIRE(node.has_value());
    CHECK(node->args[0] == "path\\to\\file");

    // Double quote inside must end the token (no escape), so we get empty + rest
    node = p.parse("echo \"say\"hello");
    REQUIRE(node.has_value());
    REQUIRE(node->args.size() >= 1);
    CHECK(node->args[0] == "say");
    CHECK(node->args[1] == "hello");
}

TEST_CASE("Double-quoted token can contain spaces and backslashes literally") {
    Parser p;
    auto node = p.parse("x \"a b c\"");
    REQUIRE(node.has_value());
    CHECK(node->args[0] == "a b c");

    node = p.parse("x \"\\n is not newline\"");
    REQUIRE(node.has_value());
    CHECK(node->args[0] == "\\n is not newline");
}

TEST_CASE("Mixed quoted and unquoted tokens") {
    Parser p;
    auto node = p.parse("cmd unquoted 'single quoted' \"double quoted\"");
    REQUIRE(node.has_value());
    CHECK(node->name == "cmd");
    REQUIRE(node->args.size() == 3);
    CHECK(node->args[0] == "unquoted");
    CHECK(node->args[1] == "single quoted");
    CHECK(node->args[2] == "double quoted");
}

TEST_CASE("Adjacent quoted segments form separate tokens") {
    Parser p;
    auto node = p.parse("echo 'a' 'b'");
    REQUIRE(node.has_value());
    REQUIRE(node->args.size() == 2);
    CHECK(node->args[0] == "a");
    CHECK(node->args[1] == "b");
}

TEST_CASE("Empty quoted between two quoted tokens") {
    Parser p;
    // 'a' '' 'b' => need four quotes between a and b: close a, open empty, close empty, open b
    auto node = p.parse("echo 'a''''b'");
    REQUIRE(node.has_value());
    REQUIRE(node->args.size() == 3);
    CHECK(node->args[0] == "a");
    CHECK(node->args[1] == "");
    CHECK(node->args[2] == "b");
}

TEST_CASE("Empty single-quoted string is valid token") {
    Parser p;
    auto node = p.parse("echo ''");
    REQUIRE(node.has_value());
    REQUIRE(node->args.size() == 1);
    CHECK(node->args[0] == "");
}

TEST_CASE("Empty double-quoted string is valid token") {
    Parser p;
    auto node = p.parse("echo \"\"");
    REQUIRE(node.has_value());
    REQUIRE(node->args.size() == 1);
    CHECK(node->args[0] == "");
}
