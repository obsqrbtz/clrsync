#ifndef CLRSYNC_GUI_SETTINGS_WINDOW_HPP
#define CLRSYNC_GUI_SETTINGS_WINDOW_HPP

#include "core/palette/palette.hpp"
#include "gui/widgets/error_message.hpp"
#include "gui/widgets/form_field.hpp"
#include "gui/widgets/settings_buttons.hpp"
#include <string>
#include <vector>

namespace clrsync::gui
{
class ui_manager;
}

class settings_window
{
  public:
    settings_window(clrsync::gui::ui_manager* ui_mgr);
    void render();
    void show() { m_visible = true; }
    void hide() { m_visible = false; }
    bool is_visible() const { return m_visible; }

    void set_palette(const clrsync::core::palette& palette) { m_current_palette = palette; }

  private:
    void load_settings();
    void apply_settings();
    void render_general_tab();
    void render_appearance_tab();
    void reset_to_defaults();
    void setup_widgets();

    bool m_visible{false};
    bool m_settings_changed{false};

    std::string m_default_theme;
    std::string m_palettes_path;
    std::string m_font;
    int m_font_size{14};
    int m_selected_font_idx{0};

    std::vector<std::string> m_available_fonts;

    clrsync::core::palette m_current_palette;
    clrsync::gui::ui_manager* m_ui_manager;

    clrsync::gui::widgets::form_field m_form;
    clrsync::gui::widgets::error_message m_error;
    clrsync::gui::widgets::settings_buttons m_buttons;
};

#endif // CLRSYNC_GUI_SETTINGS_WINDOW_HPP