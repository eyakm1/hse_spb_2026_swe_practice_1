#include "cli/external_command.hpp"
#include "cli/environment.hpp"
#include <algorithm>
#include <array>
#include <list>
#include <cstring>
#include <sstream>
#include <thread>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <io.h>
#include <process.h>
#else
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#endif

namespace cli {

namespace {

#ifdef _WIN32
/** Builds command line string for CreateProcess: "prog" "arg1" "arg2"... */
std::string build_cmdline(const std::vector<std::string>& args) {
    if (args.empty()) return "";
    std::string s;
    for (const auto& a : args) {
        if (!s.empty()) s += ' ';
        s += '"';
        for (char c : a) {
            if (c == '"') s += "\\\"";
            else s += c;
        }
        s += '"';
    }
    return s;
}
#else
#include <sys/stat.h>

/** Builds argv for execve: vector of char* ending with nullptr. */
std::vector<char> str_to_vec(const std::string& x) {
    std::vector<char> v(x.begin(), x.end());
    v.push_back('\0');
    return v;
}

/** Resolves executable path using PATH when name has no '/'. */
std::string resolve_executable(const Environment& env, const std::string& name) {
    if (name.find('/') != std::string::npos)
        return name;
    std::string path_var = env.get("PATH");
    if (path_var.empty())
        return name;
    std::string dir;
    for (char c : path_var) {
        if (c == ':' || c == ';') {
            if (!dir.empty()) {
                std::string candidate = dir + "/" + name;
                struct stat st = {};
                if (stat(candidate.c_str(), &st) == 0 && (st.st_mode & S_IXUSR))
                    return candidate;
            }
            dir.clear();
        } else {
            dir += c;
        }
    }
    if (!dir.empty()) {
        std::string candidate = dir + "/" + name;
        struct stat st = {};
        if (stat(candidate.c_str(), &st) == 0 && (st.st_mode & S_IXUSR))
            return candidate;
    }
    return name;
}
#endif

}  // namespace

int ExternalCommand::execute(const std::vector<std::string>& args,
                             std::istream& in,
                             std::ostream& out,
                             std::ostream& err,
                             const Environment& env) {
    if (args.empty())
        return 127;

#ifdef _WIN32
    std::string cmdline = build_cmdline(args);
    std::string prog = args[0];

    HANDLE hStdinR = nullptr, hStdinW = nullptr;
    HANDLE hStdoutR = nullptr, hStdoutW = nullptr;
    HANDLE hStderrR = nullptr, hStderrW = nullptr;
    SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), nullptr, TRUE };

    if (!CreatePipe(&hStdinR, &hStdinW, &sa, 0) ||
        !CreatePipe(&hStdoutR, &hStdoutW, &sa, 0) ||
        !CreatePipe(&hStderrR, &hStderrW, &sa, 0)) {
        err << "Failed to create pipes\n";
        return 1;
    }
    SetHandleInformation(hStdinW, HANDLE_FLAG_INHERIT, 0);
    SetHandleInformation(hStdoutR, HANDLE_FLAG_INHERIT, 0);
    SetHandleInformation(hStderrR, HANDLE_FLAG_INHERIT, 0);

    STARTUPINFOA si = {};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdInput = hStdinR;
    si.hStdOutput = hStdoutW;
    si.hStdError = hStderrW;

    PROCESS_INFORMATION pi = {};

    std::vector<std::string> env_vec = env.to_env_vector();
    std::string env_block;
    for (const auto& e : env_vec) {
        env_block += e;
        env_block += '\0';
    }
    env_block += '\0';

    std::vector<char> cmdline_buf(cmdline.begin(), cmdline.end());
    cmdline_buf.push_back('\0');

    BOOL ok = CreateProcessA(
        nullptr,
        cmdline_buf.data(),
        nullptr,
        nullptr,
        TRUE,
        CREATE_NO_WINDOW,
        env_block.empty() ? nullptr : const_cast<char*>(env_block.c_str()),
        nullptr,
        &si,
        &pi
    );

    CloseHandle(hStdinR);
    CloseHandle(hStdoutW);
    CloseHandle(hStderrW);

    if (!ok) {
        DWORD e = GetLastError();
        if (e == 2)  // file not found
            return 127;
        err << "CreateProcess failed: " << e << "\n";
        CloseHandle(hStdinW);
        CloseHandle(hStdoutR);
        CloseHandle(hStderrR);
        return 1;
    }

    std::thread writer([&in, hStdinW]() {
        std::array<char, 4096> buf;
        while (in.read(buf.data(), buf.size()) || in.gcount() > 0) {
            DWORD written = 0;
            WriteFile(hStdinW, buf.data(), static_cast<DWORD>(in.gcount()), &written, nullptr);
        }
        CloseHandle(hStdinW);
    });

    auto read_handle = [](HANDLE h, std::ostream& o) {
        std::array<char, 4096> buf;
        DWORD read_len = 0;
        while (ReadFile(h, buf.data(), static_cast<DWORD>(buf.size()), &read_len, nullptr) && read_len > 0)
            o.write(buf.data(), read_len);
        CloseHandle(h);
    };

    read_handle(hStdoutR, out);
    read_handle(hStderrR, err);
    writer.join();

    WaitForSingleObject(pi.hProcess, INFINITE);
    DWORD exit_code = 0;
    GetExitCodeProcess(pi.hProcess, &exit_code);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return static_cast<int>(exit_code);
#else
    std::string program_path = resolve_executable(env, args[0]);

    std::list<std::vector<char>> arg_storage;
    std::vector<char*> argv_ptrs;
    arg_storage.push_back(str_to_vec(program_path));
    argv_ptrs.push_back(arg_storage.back().data());
    for (std::size_t i = 1; i < args.size(); ++i) {
        arg_storage.push_back(str_to_vec(args[i]));
        argv_ptrs.push_back(arg_storage.back().data());
    }
    argv_ptrs.push_back(nullptr);

    std::vector<std::vector<char>> env_storage;
    std::vector<char*> env_ptrs;
    for (const auto& e : env.to_env_vector()) {
        env_storage.push_back(str_to_vec(e));
        env_ptrs.push_back(env_storage.back().data());
    }
    env_ptrs.push_back(nullptr);

    int stdin_pipe[2], stdout_pipe[2], stderr_pipe[2];
    if (pipe(stdin_pipe) != 0 || pipe(stdout_pipe) != 0 || pipe(stderr_pipe) != 0) {
        err << "pipe() failed\n";
        return 1;
    }

    pid_t pid = fork();
    if (pid < 0) {
        err << "fork() failed\n";
        return 1;
    }
    if (pid == 0) {
        close(stdin_pipe[1]);
        close(stdout_pipe[0]);
        close(stderr_pipe[0]);
        dup2(stdin_pipe[0], STDIN_FILENO);
        dup2(stdout_pipe[1], STDOUT_FILENO);
        dup2(stderr_pipe[1], STDERR_FILENO);
        close(stdin_pipe[0]);
        close(stdout_pipe[1]);
        close(stderr_pipe[1]);
        execve(program_path.c_str(), argv_ptrs.data(), env_ptrs.data());
        _exit(127);
    }

    close(stdin_pipe[0]);
    close(stdout_pipe[1]);
    close(stderr_pipe[1]);

    std::thread writer([&in, stdin_pipe]() {
        std::array<char, 4096> buf;
        while (in.read(buf.data(), buf.size()) || in.gcount() > 0) {
            ssize_t n = in.gcount();
            if (n > 0)
                write(stdin_pipe[1], buf.data(), static_cast<size_t>(n));
        }
        close(stdin_pipe[1]);
    });

    auto read_pipe = [](int fd, std::ostream& o) {
        std::array<char, 4096> buf;
        ssize_t n;
        while ((n = read(fd, buf.data(), buf.size())) > 0)
            o.write(buf.data(), n);
        close(fd);
    };
    read_pipe(stdout_pipe[0], out);
    read_pipe(stderr_pipe[0], err);
    writer.join();

    int status = 0;
    if (waitpid(pid, &status, 0) == -1)
        return 1;
    if (WIFEXITED(status))
        return WEXITSTATUS(status);
    return 1;
#endif
}

}  // namespace cli
