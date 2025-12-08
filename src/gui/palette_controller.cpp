#include "palette_controller.hpp"
#include "core/config/config.hpp"
#include "core/theme/theme_renderer.hpp"

palette_controller::palette_controller()
{
    m_palette_manager.load_palettes_from_directory(
        clrsync::core::config::instance().palettes_path());
    m_palettes = m_palette_manager.palettes();
    
    if (m_palettes.empty())
        return;
        
    try {
        m_current_palette = m_palettes[clrsync::core::config::instance().default_theme()];
    } catch (...) {
        m_current_palette = m_palettes.begin()->second;
    }
}

void palette_controller::select_palette(const std::string& name)
{
    auto it = m_palettes.find(name);
    if (it != m_palettes.end()) {
        m_current_palette = it->second;
    }
}

void palette_controller::create_palette(const std::string& name)
{
    clrsync::core::palette new_palette = m_palette_manager.load_palette_from_file(std::string(CLRSYNC_DATADIR) + "/palettes/cursed.toml");
    new_palette.set_name(name);
        
    auto dir = clrsync::core::config::instance().palettes_path();
    m_palette_manager.save_palette_to_file(new_palette, dir);
    
    reload_palettes();
    m_current_palette = new_palette;
}

void palette_controller::save_current_palette()
{
    auto dir = clrsync::core::config::instance().palettes_path();
    m_palette_manager.save_palette_to_file(m_current_palette, dir);
    reload_palettes();
}

void palette_controller::delete_current_palette()
{
    m_palette_manager.delete_palette(m_current_palette.file_path(), m_current_palette.name());
    reload_palettes();
}

void palette_controller::apply_current_theme() const
{
    clrsync::core::theme_renderer<clrsync::core::io::toml_file> theme_renderer;
    theme_renderer.apply_theme(m_current_palette.name());
}

void palette_controller::set_color(const std::string& key, const clrsync::core::color& color)
{
    m_current_palette.set_color(key, color);
}

void palette_controller::reload_palettes()
{
    m_palette_manager.load_palettes_from_directory(
        clrsync::core::config::instance().palettes_path());
    m_palettes = m_palette_manager.palettes();
}
