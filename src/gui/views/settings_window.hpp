#ifndef CLRSYNC_GUI_SETTINGS_WINDOW_HPP
#define CLRSYNC_GUI_SETTINGS_WINDOW_HPP

#include "core/palette/palette.hpp"
#include <string>
#include <vector>

class settings_window
{
  public:
    settings_window();
    void render();
    void show()
    {
        m_visible = true;
    }
    void hide()
    {
        m_visible = false;
    }
    bool is_visible() const
    {
        return m_visible;
    }

  private:
    void load_settings();
    void save_settings();
    void apply_settings();
    void render_general_tab();
    void render_appearance_tab();
    void render_status_messages();
    void render_action_buttons();
    void show_help_marker(const char *desc);
    void reset_to_defaults();

  public:
    void set_palette(const clrsync::core::palette &palette)
    {
        m_current_palette = palette;
    }

    bool m_visible{false};

    char m_default_theme[128];
    char m_palettes_path[512];
    char m_font[128];
    int m_font_size;

    std::vector<std::string> m_available_fonts;
    int m_selected_font_idx;

    std::string m_error_message;
    bool m_settings_changed;
    int m_current_tab;

    clrsync::core::palette m_current_palette;
};

#endif // CLRSYNC_GUI_SETTINGS_WINDOW_HPP