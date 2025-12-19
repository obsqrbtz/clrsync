#ifndef CLRSYNC_GUI_COLOR_SCHEME_EDITOR_HPP
#define CLRSYNC_GUI_COLOR_SCHEME_EDITOR_HPP

#include "gui/controllers/palette_controller.hpp"
#include "gui/views/color_table_renderer.hpp"
#include "gui/views/preview_renderer.hpp"
#include "gui/widgets/palette_selector.hpp"
#include "gui/widgets/input_dialog.hpp"
#include "gui/widgets/action_buttons.hpp"

class template_editor;
class settings_window;

class color_scheme_editor
{
  public:
    color_scheme_editor();

    void render_controls_and_colors();
    void render_preview();
    void set_template_editor(template_editor *editor)
    {
        m_template_editor = editor;
    }
    void set_settings_window(settings_window *window)
    {
        m_settings_window = window;
    }
    const palette_controller &controller() const
    {
        return m_controller;
    }

  private:
    void render_controls();
    void apply_themes();
    void notify_palette_changed();
    void setup_widgets();

    palette_controller m_controller;
    color_table_renderer m_color_table;
    preview_renderer m_preview;
    template_editor *m_template_editor{nullptr};
    settings_window *m_settings_window{nullptr};
    bool m_show_delete_confirmation{false};
    
    clrsync::gui::widgets::palette_selector m_palette_selector;
    clrsync::gui::widgets::input_dialog m_new_palette_dialog;
    clrsync::gui::widgets::action_buttons m_action_buttons;
};

#endif // CLRSYNC_GUI_COLOR_SCHEME_EDITOR_HPP