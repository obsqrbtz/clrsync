#ifndef CLRSYNC_GUI_PALETTE_CONTROLLER_HPP
#define CLRSYNC_GUI_PALETTE_CONTROLLER_HPP

#include "core/io/toml_file.hpp"
#include "core/palette/palette_manager.hpp"
#include <string>
#include <unordered_map>

class palette_controller
{
  public:
    palette_controller();

    const clrsync::core::palette &current_palette() const
    {
        return m_current_palette;
    }
    const std::unordered_map<std::string, clrsync::core::palette> &palettes() const
    {
        return m_palettes;
    }

    void select_palette(const std::string &name);
    void create_palette(const std::string &name);
    void save_current_palette();
    void delete_current_palette();
    void apply_current_theme() const;
    void set_color(const std::string &key, const clrsync::core::color &color);

  private:
    void reload_palettes();

    clrsync::core::palette_manager<clrsync::core::io::toml_file> m_palette_manager;
    std::unordered_map<std::string, clrsync::core::palette> m_palettes;
    clrsync::core::palette m_current_palette;
};

#endif // CLRSYNC_GUI_PALETTE_CONTROLLER_HPP