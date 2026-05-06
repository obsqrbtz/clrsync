#ifndef CLRSYNC_CORE_COMMON_PROCESS_HPP
#define CLRSYNC_CORE_COMMON_PROCESS_HPP

#include <string>
#include <vector>

namespace clrsync::core
{
std::string run_process_capture_output(const std::vector<std::string> &args);
} // namespace clrsync::core

#endif // CLRSYNC_CORE_COMMON_PROCESS_HPP
