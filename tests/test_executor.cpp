#include <doctest/doctest.h>
#include "cli/executor.hpp"
#include "cli/command_registry.hpp"
#include "cli/commands/echo_command.hpp"
#include "cli/commands/exit_command.hpp"
#include <sstream>
#include <memory>

using namespace cli;

TEST_CASE("Executor runs registered built-in command") {
    CommandRegistry registry;
    registry.register_command("echo", std::make_unique<EchoCommand>());
    Executor exec(registry);
    Environment env;

    CommandNode node;
    node.name = "echo";
    node.args = {"hello", "world"};

    std::stringstream in_dummy;
    std::stringstream out, err;

    ExecutorResult result = exec.execute(node, in_dummy, out, err, env);

    CHECK(result.should_exit == false);
    CHECK(result.exit_code == 0);
    CHECK(out.str() == "hello world\n");
}

TEST_CASE("Executor exit command sets should_exit") {
    CommandRegistry registry;
    registry.register_command("exit", std::make_unique<ExitCommand>());
    Executor exec(registry);
    Environment env;

    CommandNode node;
    node.name = "exit";
    node.args = {};

    std::stringstream in, out, err;
    ExecutorResult result = exec.execute(node, in, out, err, env);

    CHECK(result.should_exit == true);
    CHECK(result.exit_code == 0);
}

TEST_CASE("Executor unknown command runs external and returns non-zero when not found") {
    CommandRegistry registry;
    Executor exec(registry);
    Environment env;

    CommandNode node;
    node.name = "nonexistent_command_xyz_12345";
    node.args = {"nonexistent_command_xyz_12345"};

    std::stringstream in, out, err;
    ExecutorResult result = exec.execute(node, in, out, err, env);

    CHECK(result.should_exit == false);
    CHECK(result.exit_code == 127);
}

TEST_CASE("Executor passes args to built-in") {
    CommandRegistry registry;
    registry.register_command("echo", std::make_unique<EchoCommand>());
    Executor exec(registry);
    Environment env;

    CommandNode node;
    node.name = "echo";
    node.args = {"one", "two", "three"};

    std::stringstream in, out, err;
    exec.execute(node, in, out, err, env);

    CHECK(out.str() == "one two three\n");
}
