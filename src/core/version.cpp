#include "version.hpp"

namespace clrsync::core
{
const std::string version_string()
{
    return GIT_SEMVER;
}
} // namespace clrsync::core
