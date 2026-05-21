#include "process.hpp"

#include <array>
#include <cstdio>
#include <vector>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

namespace
{
#ifdef _WIN32
struct handle_guard
{
    HANDLE handle = nullptr;

    explicit handle_guard(HANDLE value) : handle(value) {}

    handle_guard(const handle_guard &) = delete;
    handle_guard &operator=(const handle_guard &) = delete;
    handle_guard(handle_guard &&other) noexcept : handle(other.handle) { other.handle = nullptr; }
    handle_guard &operator=(handle_guard &&other) noexcept
    {
        if (this != &other)
        {
            reset(other.release());
        }
        return *this;
    }

    ~handle_guard()
    {
        if (handle)
            CloseHandle(handle);
    }

    HANDLE get() const { return handle; }
    HANDLE release()
    {
        HANDLE value = handle;
        handle = nullptr;
        return value;
    }
    void reset(HANDLE value = nullptr)
    {
        if (handle)
            CloseHandle(handle);
        handle = value;
    }
};

std::wstring utf8_to_wide(const std::string &value)
{
    if (value.empty())
        return {};

    const int size = MultiByteToWideChar(CP_UTF8, 0, value.c_str(), -1, nullptr, 0);
    if (size <= 0)
        return {};

    std::wstring result(static_cast<size_t>(size), L'\0');
    MultiByteToWideChar(CP_UTF8, 0, value.c_str(), -1, result.data(), size);
    result.pop_back();
    return result;
}

std::wstring quote_windows_arg(const std::wstring &arg)
{
    if (arg.empty())
        return L"\"\"";

    if (arg.find_first_of(L" \t\n\v\"") == std::wstring::npos)
        return arg;

    std::wstring quoted;
    quoted.reserve(arg.size() + 2);
    quoted.push_back(L'"');

    size_t backslashes = 0;
    for (wchar_t ch : arg)
    {
        if (ch == L'\\')
        {
            backslashes++;
            continue;
        }

        if (ch == L'"')
        {
            quoted.append(backslashes * 2 + 1, L'\\');
            quoted.push_back(L'"');
            backslashes = 0;
            continue;
        }

        if (backslashes > 0)
        {
            quoted.append(backslashes, L'\\');
            backslashes = 0;
        }

        quoted.push_back(ch);
    }

    if (backslashes > 0)
        quoted.append(backslashes * 2, L'\\');

    quoted.push_back(L'"');
    return quoted;
}

std::wstring build_windows_command_line(const std::vector<std::string> &args)
{
    std::wstring command_line;
    for (size_t i = 0; i < args.size(); i++)
    {
        if (i > 0)
            command_line.push_back(L' ');
        command_line += quote_windows_arg(utf8_to_wide(args[i]));
    }
    return command_line;
}

std::string run_windows_process(const std::vector<std::string> &args, int *exit_code)
{
    SECURITY_ATTRIBUTES sa{};
    sa.nLength = sizeof(sa);
    sa.bInheritHandle = TRUE;

    HANDLE read_pipe_raw = nullptr;
    HANDLE write_pipe_raw = nullptr;
    if (!CreatePipe(&read_pipe_raw, &write_pipe_raw, &sa, 0))
        return {};

    handle_guard read_pipe(read_pipe_raw);
    handle_guard write_pipe(write_pipe_raw);

    if (!SetHandleInformation(read_pipe.get(), HANDLE_FLAG_INHERIT, 0))
        return {};

    STARTUPINFOW startup{};
    startup.cb = sizeof(startup);
    startup.dwFlags = STARTF_USESTDHANDLES;
    startup.hStdOutput = write_pipe.get();
    startup.hStdError = write_pipe.get();
    startup.hStdInput = GetStdHandle(STD_INPUT_HANDLE);

    PROCESS_INFORMATION process_info{};
    std::wstring command_line = build_windows_command_line(args);
    std::vector<wchar_t> mutable_command_line(command_line.begin(), command_line.end());
    mutable_command_line.push_back(L'\0');

    if (!CreateProcessW(nullptr, mutable_command_line.data(), nullptr, nullptr, TRUE,
                        CREATE_NO_WINDOW, nullptr, nullptr, &startup, &process_info))
    {
        if (exit_code)
            *exit_code = -1;
        return {};
    }

    handle_guard process_handle(process_info.hProcess);
    handle_guard thread_handle(process_info.hThread);
    write_pipe.reset();

    std::string output;
    std::array<char, 4096> buffer{};
    DWORD bytes_read = 0;
    while (ReadFile(read_pipe.get(), buffer.data(), static_cast<DWORD>(buffer.size()),
                    &bytes_read, nullptr) &&
           bytes_read > 0)
    {
        output.append(buffer.data(), buffer.data() + bytes_read);
    }

    WaitForSingleObject(process_handle.get(), INFINITE);
    if (exit_code)
    {
        DWORD code = STILL_ACTIVE;
        if (GetExitCodeProcess(process_handle.get(), &code))
            *exit_code = static_cast<int>(code);
        else
            *exit_code = -1;
    }
    return output;
}
#else
std::string run_posix_process(const std::vector<std::string> &args, int *exit_code)
{
    int pipe_fds[2];
    if (pipe(pipe_fds) != 0)
    {
        if (exit_code)
            *exit_code = -1;
        return {};
    }

    pid_t pid = fork();
    if (pid < 0)
    {
        close(pipe_fds[0]);
        close(pipe_fds[1]);
        if (exit_code)
            *exit_code = -1;
        return {};
    }

    if (pid == 0)
    {
        dup2(pipe_fds[1], STDOUT_FILENO);
        dup2(pipe_fds[1], STDERR_FILENO);

        close(pipe_fds[0]);
        close(pipe_fds[1]);

        std::vector<char *> argv;
        argv.reserve(args.size() + 1);
        for (const auto &arg : args)
            argv.push_back(const_cast<char *>(arg.c_str()));
        argv.push_back(nullptr);

        execvp(argv[0], argv.data());
        _exit(127);
    }

    close(pipe_fds[1]);

    std::string output;
    std::array<char, 4096> buffer{};
    ssize_t bytes_read = 0;
    while ((bytes_read = read(pipe_fds[0], buffer.data(), buffer.size())) > 0)
    {
        output.append(buffer.data(), static_cast<size_t>(bytes_read));
    }

    close(pipe_fds[0]);
    int status = 0;
    (void)waitpid(pid, &status, 0);
    if (exit_code)
    {
        if (WIFEXITED(status))
            *exit_code = WEXITSTATUS(status);
        else if (WIFSIGNALED(status))
            *exit_code = 128 + WTERMSIG(status);
        else
            *exit_code = -1;
    }
    return output;
}
#endif
} // namespace

namespace
{
bool is_ansi_bracket_param_char(unsigned char c)
{
    return (c >= '0' && c <= '9') || c == ';';
}

void skip_ansi_brackets(const std::string &text, size_t &index)
{
    while (index < text.size() &&
           is_ansi_bracket_param_char(static_cast<unsigned char>(text[index])))
        ++index;
    if (index < text.size() && text[index] >= 0x40 && text[index] <= 0x7E)
        ++index;
}
} // namespace

namespace clrsync::core
{
std::string strip_ansi_escapes(const std::string &text)
{
    std::string result;
    result.reserve(text.size());
    for (size_t i = 0; i < text.size();)
    {
        const unsigned char c = static_cast<unsigned char>(text[i]);
        if (c == 0x1B || c == 0x9B)
        {
            if (c == 0x1B)
                ++i;
            if (i < text.size() && text[i] == '[')
            {
                ++i;
                skip_ansi_brackets(text, i);
            }
            continue;
        }
        if (i + 1 < text.size() && text[i] == '?' && text[i + 1] == '[')
        {
            i += 2;
            skip_ansi_brackets(text, i);
            continue;
        }
        result.push_back(text[i]);
        ++i;
    }
    return result;
}

std::string process_failure_message(const std::string &output, const char *fallback_message)
{
    const std::string cleaned = strip_ansi_escapes(output);
    if (!cleaned.empty())
        return cleaned;
    return fallback_message ? fallback_message : "command failed";
}

std::string run_process_capture_output(const std::vector<std::string> &args, int *exit_code)
{
    if (args.empty())
    {
        if (exit_code)
            *exit_code = -1;
        return {};
    }

#ifdef _WIN32
    return run_windows_process(args, exit_code);
#else
    return run_posix_process(args, exit_code);
#endif
}
} // namespace clrsync::core
