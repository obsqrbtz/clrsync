#ifndef CLRSYNC_CORE_VERSION_HPP
#define CLRSYNC_CORE_VERSION_HPP

#include <cstdint>
#include <string>

namespace clrsync::core
{

constexpr uint8_t VERSION_MAJOR = 0;
constexpr uint8_t VERSION_MINOR = 1;
constexpr uint8_t VERSION_PATCH = 3;

const std::string version_string();
} // namespace clrsync::core

#endif // CLRSYNC_CORE_VERSION_HPP
