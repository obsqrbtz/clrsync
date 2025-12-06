#ifndef CLRSYNC_CORE_PALETTE_PALETTE_FILE_HPP
#define CLRSYNC_CORE_PALETTE_PALETTE_FILE_HPP

#include "core/palette/color.hpp"
#include <cstdint>
#include <string>

#include <core/io/file.hpp>
#include <core/palette/color_keys.hpp>
#include <core/palette/palette.hpp>

#include <memory>

namespace clrsync::core
{
template <typename FileType> class palette_file
{
  public:
    palette_file() = default;
    palette_file(std::string file_path) : m_file(std::make_unique<FileType>(file_path))
    {
        m_palette.set_file_path(file_path);
    }
    bool parse()
    {
        if (!m_file->parse())
            return false;
        m_palette.set_name(m_file->get_string_value("general", "name"));
        for (const auto &color_key : COLOR_KEYS)
        {
            auto color_str = m_file->get_string_value("colors", color_key);
            core::color color{0x000000FF};
            if (!color_str.empty())
                color.from_hex_string(color_str);
            m_palette.set_color(color_key, color);
        }
        return true;
    }
    core::palette palette() const
    {
        return m_palette;
    }
    void save_palette(const core::palette &pal)
    {
        set_palette(pal);
        save();
    }
    void set_color(const std::string &color_key, uint32_t color)
    {
        core::color col(color);
        m_file->insert_or_update_value("colors", color_key, col.to_hex_string());
    }
    void save()
    {
        m_file->save_file();
    }

  private:
    core::palette m_palette{};
    std::unique_ptr<io::file> m_file;

    void set_palette(const core::palette &pal)
    {
        m_palette = pal;
        m_file->insert_or_update_value("general", "name", pal.name());
        for (const auto &color_key : COLOR_KEYS)
        {
            const auto &col = pal.get_color(color_key);
            m_file->insert_or_update_value("colors", color_key, col.to_hex_string_with_alpha());
        }
    }
};
} // namespace clrsync::core

#endif