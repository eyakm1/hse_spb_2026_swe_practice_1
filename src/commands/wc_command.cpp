#include "cli/commands/wc_command.hpp"
#include <fstream>
#include <cctype>

namespace cli {

namespace {

void count_stream(std::istream& in, unsigned long& lines, unsigned long& words, unsigned long& bytes) {
    lines = words = bytes = 0;
    bool in_word = false;
    char c;
    while (in.get(c)) {
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
}

}  // namespace

int WcCommand::execute(const std::vector<std::string>& args,
                       std::istream& in,
                       std::ostream& out,
                       std::ostream& err,
                       const Environment& /*env*/) {
    if (args.size() < 2) {
        unsigned long lines = 0, words = 0, bytes = 0;
        count_stream(in, lines, words, bytes);
        out << " " << lines << " " << words << " " << bytes << "\n";
        return 0;
    }
    for (std::size_t i = 1; i < args.size(); ++i) {
        std::ifstream f(args[i], std::ios::binary);
        if (!f) {
            err << "wc: cannot open '" << args[i] << "'\n";
            return 1;
        }
        unsigned long lines = 0, words = 0, bytes = 0;
        count_stream(f, lines, words, bytes);
        out << " " << lines << " " << words << " " << bytes << " " << args[i] << "\n";
    }
    return 0;
}

}  // namespace cli
