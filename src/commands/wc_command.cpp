#include "cli/commands/wc_command.hpp"
#include <fstream>
#include <cctype>

namespace cli {

int WcCommand::execute(const std::vector<std::string>& args,
                       std::istream& /*in*/,
                       std::ostream& out,
                       std::ostream& err,
                       const Environment& /*env*/) {
    if (args.size() < 2) {
        err << "wc: missing file operand\n";
        return 1;
    }
    for (std::size_t i = 1; i < args.size(); ++i) {
        std::ifstream f(args[i], std::ios::binary);
        if (!f) {
            err << "wc: cannot open '" << args[i] << "'\n";
            return 1;
        }
        unsigned long lines = 0, words = 0, bytes = 0;
        bool in_word = false;
        char c;
        while (f.get(c)) {
            ++bytes;
            if (c == '\n')
                ++lines;
            if (std::isspace(static_cast<unsigned char>(c)))
                in_word = false;
            else {
                if (!in_word)
                    ++words;
                in_word = true;
            }
        }
        out << " " << lines << " " << words << " " << bytes << " " << args[i] << "\n";
    }
    return 0;
}

}  // namespace cli
