#include "utils.hpp"
#include <iostream>

namespace clrsync::core
{
void print_color_keys()
{
    for (const auto &key : clrsync::core::COLOR_KEYS)
    {
        std::cout << key << std::endl;
    }
}

std::string get_default_config_path()
{
#ifdef _WIN32
    const char *appdata = std::getenv("APPDATA"); // "C:\Users\<User>\AppData\Roaming"
    if (!appdata)
        throw std::runtime_error("APPDATA environment variable not set");
    return std::string(appdata) + "\\clrsync\\config.toml";
#else
    const char *home = std::getenv("HOME");
    if (!home)
        throw std::runtime_error("HOME environment variable not set");
    return std::string(home) + "/.config/clrsync/config.toml";
#endif
}

std::string expand_user(const std::string &path)
{
    if (!path.empty() && path[0] == '~')
    {
#ifdef _WIN32
        const char *home = std::getenv("USERPROFILE");
#else
        const char *home = std::getenv("HOME");
#endif
        if (!home)
            return path;
        return std::string(home) + path.substr(1);
    }
    return path;
}

} // namespace clrsync::core
