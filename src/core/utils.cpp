#include "utils.hpp"
#include <iostream>
#include <filesystem>

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
    const char* env_path = std::getenv("CLRSYNC_CONFIG_PATH");
    if (env_path && env_path[0] != '\0')
        return normalize_path(env_path).string();
    
    std::filesystem::path home = normalize_path("~");
    std::filesystem::path config_path = home / ".config" / "clrsync" / "config.toml";
    return config_path.string();
}

std::string expand_user(const std::string &path)
{
    if (path.empty() || path[0] != '~')
        return path;
    
    if (path.length() == 1 || path[1] == '/' || path[1] == '\\')
    {
#ifdef _WIN32
        const char *home = std::getenv("USERPROFILE");
#else
        const char *home = std::getenv("HOME");
#endif
        if (!home)
            return path;
        
        if (path.length() == 1)
            return std::string(home);
        
        return std::string(home) + path.substr(1);
    }
    return path;
}

std::filesystem::path normalize_path(const std::string &path)
{
    std::string expanded = expand_user(path);
    std::filesystem::path fs_path(expanded);
    return fs_path.lexically_normal();
}

} // namespace clrsync::core
