#ifndef CLRSYNC_CORE_COMMON_PROCESS_HPP
#define CLRSYNC_CORE_COMMON_PROCESS_HPP

#include <string>
#include <vector>

namespace clrsync::core
{
std::string run_process_capture_output(const std::vector<std::string> &args,
                                        int *exit_code = nullptr);
std::string strip_ansi_escapes(const std::string &text);
std::string process_failure_message(const std::string &output,
                                    const char *fallback_message = "command failed");
} // namespace clrsync::core

#endif // CLRSYNC_CORE_COMMON_PROCESS_HPP
