#include "version.hpp"

namespace clrsync::core
{
const std::string version_string()
{
    return "v" + std::to_string(VERSION_MAJOR) + "." + std::to_string(VERSION_MINOR) + "." +
           std::to_string(VERSION_PATCH);
}
} // namespace clrsync::core
