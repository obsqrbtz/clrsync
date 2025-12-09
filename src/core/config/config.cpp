#include "config.hpp"
#include "core/utils.hpp"

#include <core/palette/color.hpp>
#include <filesystem>
#include <fstream>
#include <stdexcept>

#ifdef _WIN32
#include "windows.h"
#endif

namespace clrsync::core
{
config &config::instance()
{
    static config inst;
    return inst;
}

void config::initialize(std::unique_ptr<clrsync::core::io::file> file)
{
    copy_default_configs();
    m_file = std::move(file);
    if (m_file)
        if (!m_file->parse())
            throw std::runtime_error{"Could not parse config file"};
}

std::filesystem::path config::get_user_config_dir()
{
    auto home = expand_user("~");
#ifdef _WIN32
    return home + "\\.config\\clrsync";

#else
    return home + "/.config/clrsync";
#endif
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

    std::ifstream in(src, std::ios::binary);
    std::ofstream out(dst, std::ios::binary);
    out << in.rdbuf();
}

void config::copy_dir(const std::filesystem::path &src, const std::filesystem::path &dst)
{
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

void config::set_default_theme(const std::string &theme)
{
    if (m_file)
    {
        m_file->set_value("general", "default_theme", theme);
        m_file->save_file();
    }
}

void config::set_palettes_path(const std::string &path)
{
    if (m_file)
    {
        m_file->set_value("general", "palettes_path", path);
        m_file->save_file();
    }
}

void config::set_font(const std::string &font)
{
    if (m_file)
    {
        m_file->set_value("general", "font", font);
        m_file->save_file();
    }
}
void config::set_font_size(int font_size)
{
    if (m_file)
    {
        m_file->set_value("general", "font_size", font_size);
        m_file->save_file();
    }
}

void config::update_template(const std::string &key,
                             const clrsync::core::theme_template &theme_template)
{
    m_themes[key] = theme_template;
    m_file->set_value("templates." + key, "input_path", theme_template.template_path());
    m_file->set_value("templates." + key, "output_path", theme_template.output_path());
    m_file->set_value("templates." + key, "enabled", theme_template.enabled());
    m_file->set_value("templates." + key, "reload_cmd", theme_template.reload_command());
    m_file->save_file();
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
            theme.load_template();
            m_themes.insert({theme.name(), theme});
        }
    }
    return m_themes;
}

const clrsync::core::theme_template &config::template_by_name(const std::string &name) const
{
    auto it = m_themes.find(name);
    if (it != m_themes.end())
    {
        return it->second;
    }
    throw std::runtime_error("Template not found: " + name);
}

} // namespace clrsync::core
