#include "config.hpp"
#include "core/utils.hpp"
#include "core/error.hpp"

#include <core/palette/color.hpp>
#include <filesystem>
#include <fstream>

#ifdef _WIN32
#include "windows.h"
#endif
#include <iostream>

namespace clrsync::core
{
config &config::instance()
{
    static config inst;
    return inst;
}

Result<void> config::initialize(std::unique_ptr<clrsync::core::io::file> file)
{
    copy_default_configs();
    m_file = std::move(file);
    if (!m_file)
        return Err<void>(error_code::config_missing, "Config file is missing");
    
    auto parse_result = m_file->parse();
    if (!parse_result)
        return Err<void>(error_code::config_invalid, parse_result.error().message, parse_result.error().context);
    
    return Ok();
}

std::filesystem::path config::get_user_config_dir()
{
    std::filesystem::path home = normalize_path("~");
    return home / ".config" / "clrsync";
}

std::filesystem::path config::get_data_dir()
{
    if (std::filesystem::exists(CLRSYNC_DATADIR))
        return {CLRSYNC_DATADIR};
#ifdef _WIN32
    char buffer[MAX_PATH];
    GetModuleFileNameA(nullptr, buffer, MAX_PATH);
    std::filesystem::path exe_path(buffer);
    std::filesystem::path data_dir = exe_path.parent_path().parent_path() / "share" / "clrsync";
    return data_dir;
#else
    if (std::filesystem::exists("/usr/share/clrsync"))
        return {"/usr/share/clrsync"};
    if (std::filesystem::exists("/usr/local/share/clrsync"))
        return {"/usr/local/share/clrsync"};
    return {};
#endif
}

void config::copy_file(const std::filesystem::path &src, const std::filesystem::path &dst)
{
    if (std::filesystem::exists(dst))
        return;

    if (!std::filesystem::exists(src))
        return;

    std::ifstream in(src, std::ios::binary);
    std::ofstream out(dst, std::ios::binary);
    
    if (!in || !out)
        return;
    
    out << in.rdbuf();
}

void config::copy_dir(const std::filesystem::path &src, const std::filesystem::path &dst)
{
    if (!std::filesystem::exists(src))
        return;
    
    for (auto const &entry : std::filesystem::recursive_directory_iterator(src))
    {
        auto rel = std::filesystem::relative(entry.path(), src);
        auto out = dst / rel;

        if (entry.is_directory())
        {
            std::filesystem::create_directories(out);
        }
        else if (entry.is_regular_file())
        {
            copy_file(entry.path(), out);
        }
    }
}

void config::copy_default_configs()
{
    std::filesystem::path user_dir = get_user_config_dir();
    std::filesystem::path system_dir = get_data_dir();

    std::filesystem::create_directories(user_dir);

    if (system_dir.empty())
        return;

    {
        auto src = system_dir / "config.toml";
        auto dst = user_dir / "config.toml";
        if (!std::filesystem::exists(dst))
            copy_file(src, dst);
    }

    {
        auto src = system_dir / "templates";
        auto dst = user_dir / "templates";

        if (!std::filesystem::exists(dst))
            std::filesystem::create_directories(dst);

        copy_dir(src, dst);
    }

    {
        auto src = system_dir / "palettes";
        auto dst = user_dir / "palettes";

        if (!std::filesystem::exists(dst))
            std::filesystem::create_directories(dst);

        copy_dir(src, dst);
    }
}

const std::string &config::palettes_path()
{
    if (m_palettes_dir.empty() && m_file)
        m_palettes_dir = m_file->get_string_value("general", "palettes_path");
    return m_palettes_dir;
}

const std::string config::default_theme() const
{
    if (m_file)
        return m_file->get_string_value("general", "default_theme");
    return {};
}

const std::string config::font() const
{
    if (m_file)
        return m_file->get_string_value("general", "font");
    return {};
}

const uint32_t config::font_size() const
{
    if (m_file)
        return m_file->get_uint_value("general", "font_size");
    return 14;
}

Result<void> config::set_default_theme(const std::string &theme)
{
    if (!m_file)
        return Err<void>(error_code::config_missing, "Configuration not initialized");
    
    m_file->set_value("general", "default_theme", theme);
    return m_file->save_file();
}

Result<void> config::set_palettes_path(const std::string &path)
{
    if (!m_file)
        return Err<void>(error_code::config_missing, "Configuration not initialized");
    
    m_file->set_value("general", "palettes_path", path);
    return m_file->save_file();
}

Result<void> config::set_font(const std::string &font)
{
    if (!m_file)
        return Err<void>(error_code::config_missing, "Configuration not initialized");
    
    m_file->set_value("general", "font", font);
    return m_file->save_file();
}
Result<void> config::set_font_size(int font_size)
{
    if (!m_file)
        return Err<void>(error_code::config_missing, "Configuration not initialized");
    
    m_file->set_value("general", "font_size", font_size);
    return m_file->save_file();
}

Result<void> config::update_template(const std::string &key,
                             const clrsync::core::theme_template &theme_template)
{
    if (!m_file)
        return Err<void>(error_code::config_missing, "Configuration not initialized");
    
    m_themes[key] = theme_template;
    m_file->set_value("templates." + key, "input_path", theme_template.template_path());
    m_file->set_value("templates." + key, "output_path", theme_template.output_path());
    m_file->set_value("templates." + key, "enabled", theme_template.enabled());
    m_file->set_value("templates." + key, "reload_cmd", theme_template.reload_command());
    return m_file->save_file();
}

Result<void> config::remove_template(const std::string &key)
{
    if (!m_file)
        return Err<void>(error_code::config_missing, "Configuration not initialized");
    
    auto it = m_themes.find(key);
    if (it == m_themes.end())
        return Err<void>(error_code::template_not_found, "Template not found", key);
    
    std::filesystem::path template_file = it->second.template_path();
    if (std::filesystem::exists(template_file))
    {
        try {
            std::filesystem::remove(template_file);
        } catch (const std::exception& e) {
            return Err<void>(error_code::file_write_failed, "Failed to delete template file", e.what());
        }
    }
    
    m_themes.erase(it);
    
    m_file->remove_section("templates." + key);
    
    return m_file->save_file();
}

const std::unordered_map<std::string, clrsync::core::theme_template> config::templates()
{
    if (m_themes.empty() && m_file)
    {
        auto themes = m_file->get_table("templates");
        for (const auto &t : themes)
        {
            auto current = m_file->get_table("templates." + t.first);
            clrsync::core::theme_template theme(t.first,
                                                std::get<std::string>(current["input_path"]),
                                                std::get<std::string>(current["output_path"]));
            if (std::holds_alternative<bool>(current["enabled"]))
            {
                theme.set_enabled(std::get<bool>(current["enabled"]));
            }
            else
            {
                theme.set_enabled(false);
            }
            theme.set_reload_command(std::get<std::string>(current["reload_cmd"]));
            (void)theme.load_template();
            m_themes.insert({theme.name(), theme});
        }
    }
    return m_themes;
}

Result<const clrsync::core::theme_template*> config::template_by_name(const std::string &name) const
{
    auto it = m_themes.find(name);
    if (it != m_themes.end())
    {
        return Ok(&it->second);
    }
    return Err<const clrsync::core::theme_template*>(error_code::template_not_found, "Template not found", name);
}

} // namespace clrsync::core
