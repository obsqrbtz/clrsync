#ifndef CLRSYNC_CORE_UTILS_HPP
#define CLRSYNC_CORE_UTILS_HPP

#include <string>

#include <core/palette/color_keys.hpp>

namespace clrsync::core
{
void print_color_keys();
std::string get_default_config_path();
std::string expand_user(const std::string &path);
} // namespace clrsync::core
#endif // CLRSYNC_CORE_UTILS_HPP