#include "cli/command_line_interpreter.hpp"
#include "cli/ast.hpp"
#include "cli/commands/cat_command.hpp"
#include "cli/commands/echo_command.hpp"
#include "cli/commands/exit_command.hpp"
#include "cli/commands/pwd_command.hpp"
#include "cli/commands/wc_command.hpp"
#include "cli/commands/grep_command.hpp"
#include <cctype>

namespace cli {

namespace {

/** Returns true if s looks like VAR=value (valid identifier before =). */
bool is_assignment(const std::string &s) {
  if (s.empty())
    return false;
  std::size_t eq = s.find('=');
  if (eq == std::string::npos || eq == 0)
    return false;
  for (std::size_t i = 0; i < eq; ++i) {
    char c = s[i];
    bool ok = (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_' ||
              (i > 0 && (c >= '0' && c <= '9'));
    if (!ok)
      return false;
  }
  return true;
}

/** Strip leading VAR=value from first command and apply to env. Mutates
 * pipeline. */
void apply_assignments(Pipeline &pipeline, Environment &env) {
  if (pipeline.empty())
    return;
  CommandNode &first = pipeline[0];
  std::vector<std::string> new_args;
  std::vector<Substitute> new_sub;
  bool name_consumed = false;
  auto apply = [&](const std::string &token, Substitute sub) {
    if (!name_consumed) {
      if (is_assignment(token)) {
        std::size_t eq = token.find('=');
        std::string key = token.substr(0, eq);
        std::string val = (eq + 1 < token.size()) ? token.substr(eq + 1) : "";
        if (sub == Substitute::Yes)
          val = env.substitute(val);
        env.set(key, val);
        return;
      }
      name_consumed = true;
      first.name = token;
      first.substitute_name = sub;
      return;
    }
    new_args.push_back(token);
    new_sub.push_back(sub);
  };
  apply(first.name, first.substitute_name);
  for (std::size_t i = 0; i < first.args.size(); ++i) {
    Substitute sub = (i < first.substitute_arg.size()) ? first.substitute_arg[i]
                                                       : Substitute::Yes;
    apply(first.args[i], sub);
  }
  first.args = std::move(new_args);
  first.substitute_arg = std::move(new_sub);
  if (!name_consumed)
    first.name.clear();
}

/** Remove leading commands that have empty name (after assignment stripping).
 */
void drop_empty_leading_commands(Pipeline &pipeline) {
  while (!pipeline.empty() && pipeline.front().name.empty() &&
         pipeline.front().args.empty()) {
    pipeline.erase(pipeline.begin());
  }
}

} // namespace

CommandLineInterpreter::CommandLineInterpreter() : executor_(registry_) {
  env_.init_from_current();
  register_builtins();
}

void CommandLineInterpreter::register_builtins() {
  registry_.register_command("cat", std::make_unique<CatCommand>());
  registry_.register_command("echo", std::make_unique<EchoCommand>());
  registry_.register_command("wc", std::make_unique<WcCommand>());
  registry_.register_command("pwd", std::make_unique<PwdCommand>());
  registry_.register_command("exit", std::make_unique<ExitCommand>());
  registry_.register_command("grep", std::make_unique<GrepCommand>());
}

int CommandLineInterpreter::run(std::istream &in, std::ostream &out,
                                std::ostream &err) {
  std::string line;
  int exit_code = 0;
  while (true) {
    try {
      if (&in == &std::cin) {
        out << "> " << std::flush;
      }
      if (!std::getline(in, line))
        break;
      auto pipeline = parser_.parse(line);
      if (!pipeline) // empty line
        continue;
      apply_assignments(*pipeline, env_);
      drop_empty_leading_commands(*pipeline);
      if (pipeline->empty()) {
        continue;
      }
      ExecutorResult result = executor_.execute(*pipeline, in, out, err, env_);
      if (result.should_exit) {
        exit_code = result.exit_code;
        break;
      }
    } catch (const std::exception &e) {
      err << "cli: " << e.what() << "\n";
    } catch (...) {
      err << "cli: unknown error\n";
    }
  }
  return exit_code;
}

} // namespace cli
