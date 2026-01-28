#ifndef CLRSYNC_GUI_COLOR_SCHEME_EDITOR_HPP
#define CLRSYNC_GUI_COLOR_SCHEME_EDITOR_HPP

#include "gui/controllers/palette_controller.hpp"
#include "gui/views/color_table_renderer.hpp"
#include "gui/views/preview_renderer.hpp"
#include "gui/widgets/action_buttons.hpp"
#include "gui/widgets/input_dialog.hpp"
#include "gui/widgets/palette_selector.hpp"

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
    clrsync::gui::widgets::input_dialog m_generate_dialog;
    int m_generator_idx{0};
    bool m_show_generate_modal{false};
    // hellwal
    std::string m_gen_image_path;
    bool m_gen_neon{false};
    bool m_gen_dark{true};
    bool m_gen_light{false};
    bool m_gen_color{false};
    float m_gen_dark_offset{0.0f};
    float m_gen_bright_offset{0.0f};
    bool m_gen_invert{false};
    float m_gen_gray_scale{0.0f};
    // matugen
    std::string m_matugen_mode{"dark"};
    std::string m_matugen_type{"scheme-tonal-spot"};
    float m_matugen_contrast{0.0f};
    // matugen color option
    bool m_matugen_use_color{false};
    float m_matugen_color_vec[3]{1.0f, 0.0f, 0.0f};
    std::string m_matugen_color_hex{"FF0000"};
    clrsync::gui::widgets::action_buttons m_action_buttons;
};

#endif // CLRSYNC_GUI_COLOR_SCHEME_EDITOR_HPP