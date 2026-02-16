#include "cli/command_registry.hpp"
#include "cli/commands/echo_command.hpp"
#include "cli/commands/exit_command.hpp"
#include "cli/commands/wc_command.hpp"
#include "cli/executor.hpp"
#include <doctest/doctest.h>
#include <memory>
#include <sstream>

using namespace cli;

TEST_CASE("Executor runs registered built-in command") {
  CommandRegistry registry;
  registry.register_command("echo", std::make_unique<EchoCommand>());
  Executor exec(registry);
  Environment env;

  Pipeline pl;
  pl.push_back(CommandNode{"echo", {"hello", "world"}});

  std::stringstream in_dummy;
  std::stringstream out, err;

  ExecutorResult result = exec.execute(pl, in_dummy, out, err, env);

  CHECK(result.should_exit == false);
  CHECK(result.exit_code == 0);
  CHECK(out.str() == "hello world\n");
}

TEST_CASE("Executor exit command sets should_exit") {
  CommandRegistry registry;
  registry.register_command("exit", std::make_unique<ExitCommand>());
  Executor exec(registry);
  Environment env;

  Pipeline pl;
  pl.push_back(CommandNode{"exit", {}});

  std::stringstream in, out, err;
  ExecutorResult result = exec.execute(pl, in, out, err, env);

  CHECK(result.should_exit == true);
  CHECK(result.exit_code == 0);
}

TEST_CASE("Executor unknown command runs external and returns non-zero when "
          "not found") {
  CommandRegistry registry;
  Executor exec(registry);
  Environment env;

  Pipeline pl;
  pl.push_back(CommandNode{"nonexistent_command_xyz_12345",
                           {"nonexistent_command_xyz_12345"}});

  std::stringstream in, out, err;
  ExecutorResult result = exec.execute(pl, in, out, err, env);

  CHECK(result.should_exit == false);
  CHECK(result.exit_code == 127);
}

TEST_CASE("Executor passes args to built-in") {
  CommandRegistry registry;
  registry.register_command("echo", std::make_unique<EchoCommand>());
  Executor exec(registry);
  Environment env;

  Pipeline pl;
  pl.push_back(CommandNode{"echo", {"one", "two", "three"}});

  std::stringstream in, out, err;
  exec.execute(pl, in, out, err, env);

  CHECK(out.str() == "one two three\n");
}

TEST_CASE("Executor substitutes $VAR in args when Substitute::Yes") {
  CommandRegistry registry;
  registry.register_command("echo", std::make_unique<EchoCommand>());
  Executor exec(registry);
  Environment env;
  env.set("X", "expanded");

  CommandNode node;
  node.name = "echo";
  node.args = {"$X"};
  node.substitute_name = Substitute::Yes;
  node.substitute_arg = {Substitute::Yes};
  Pipeline pl;
  pl.push_back(std::move(node));

  std::stringstream in, out, err;
  exec.execute(pl, in, out, err, env);
  CHECK(out.str() == "expanded\n");
}

TEST_CASE("Executor no substitution when Substitute::No") {
  CommandRegistry registry;
  registry.register_command("echo", std::make_unique<EchoCommand>());
  Executor exec(registry);
  Environment env;
  env.set("LIT", "not_used");

  CommandNode node;
  node.name = "echo";
  node.args = {"$LIT"};
  node.substitute_name = Substitute::Yes;
  node.substitute_arg = {Substitute::No};
  Pipeline pl;
  pl.push_back(std::move(node));

  std::stringstream in, out, err;
  exec.execute(pl, in, out, err, env);
  CHECK(out.str() == "$LIT\n");
}

TEST_CASE("Executor pipeline passes stdout to next stdin") {
  CommandRegistry registry;
  registry.register_command("echo", std::make_unique<EchoCommand>());
  registry.register_command("wc", std::make_unique<WcCommand>());
  Executor exec(registry);
  Environment env;

  Pipeline pl;
  pl.push_back(CommandNode{"echo", {"one", "two", "three"}});
  pl.push_back(CommandNode{"wc", {}});

  std::stringstream in, out, err;
  ExecutorResult result = exec.execute(pl, in, out, err, env);
  CHECK(result.exit_code == 0);
  // wc on "one two three\n" -> 1 line, 3 words
  CHECK(out.str().find("1") != std::string::npos);
  CHECK(out.str().find("3") != std::string::npos);
}

TEST_CASE("Executor empty command in pipeline returns 127 and message to err") {
  CommandRegistry registry;
  registry.register_command("echo", std::make_unique<EchoCommand>());
  Executor exec(registry);
  Environment env;

  CommandNode empty_node;
  empty_node.name = "";
  empty_node.args = {};
  Pipeline pl;
  pl.push_back(std::move(empty_node));
  pl.push_back(CommandNode{"echo", {"ok"}});

  std::stringstream in, out, err;
  ExecutorResult result = exec.execute(pl, in, out, err, env);
  CHECK(result.exit_code == 127);
  CHECK(err.str().find("empty command") != std::string::npos);
}

TEST_CASE("Executor empty pipeline returns zero exit code") {
  CommandRegistry registry;
  Executor exec(registry);
  Environment env;
  Pipeline pl;
  std::stringstream in, out, err;
  ExecutorResult result = exec.execute(pl, in, out, err, env);
  CHECK(result.should_exit == false);
  CHECK(result.exit_code == 0);
}

TEST_CASE("Executor exit in pipeline sets should_exit") {
  CommandRegistry registry;
  registry.register_command("echo", std::make_unique<EchoCommand>());
  registry.register_command("exit", std::make_unique<ExitCommand>());
  Executor exec(registry);
  Environment env;
  Pipeline pl;
  pl.push_back(CommandNode{"echo", {"x"}});
  pl.push_back(CommandNode{"exit", {}});
  pl.push_back(CommandNode{"echo", {"y"}});
  std::stringstream in, out, err;
  ExecutorResult result = exec.execute(pl, in, out, err, env);
  CHECK(result.should_exit == true);
  CHECK(out.str().find("y") == std::string::npos);
}

TEST_CASE("Executor substitution in command name") {
  CommandRegistry registry;
  registry.register_command("echo", std::make_unique<EchoCommand>());
  Executor exec(registry);
  Environment env;
  env.set("CMD", "echo");
  CommandNode node;
  node.name = "$CMD";
  node.args = {"hello"};
  node.substitute_name = Substitute::Yes;
  node.substitute_arg = {Substitute::Yes};
  Pipeline pl;
  pl.push_back(std::move(node));
  std::stringstream in, out, err;
  exec.execute(pl, in, out, err, env);
  CHECK(out.str() == "hello\n");
}

TEST_CASE("Executor pipeline exit code is last command exit code") {
  CommandRegistry registry;
  registry.register_command("echo", std::make_unique<EchoCommand>());
  Executor exec(registry);
  Environment env;
  Pipeline pl;
  pl.push_back(CommandNode{"echo", {"a"}});
  pl.push_back(CommandNode{"nonexistent_xyz_999", {"nonexistent_xyz_999"}});
  std::stringstream in, out, err;
  ExecutorResult result = exec.execute(pl, in, out, err, env);
  CHECK(result.exit_code == 127);
}
