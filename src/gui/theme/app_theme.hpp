#ifndef CLRSYNC_GUI_THEME_APP_THEME_HPP
#define CLRSYNC_GUI_THEME_APP_THEME_HPP

#include "core/palette/palette.hpp"
#include "imgui.h"
#include <string>

namespace clrsync::gui::theme
{

struct color_utils
{
    static ImVec4 from_hex(uint32_t hex);
    static uint32_t to_imgui_u32(uint32_t hex);
    static ImVec4 with_alpha(const ImVec4 &color, float alpha);
    static ImVec4 lighten(const ImVec4 &color, float amount);
    static ImVec4 darken(const ImVec4 &color, float amount);
    static ImVec4 blend(const ImVec4 &a, const ImVec4 &b, float t);
};

class app_theme
{
  public:
    app_theme() = default;
    explicit app_theme(const core::palette &palette);

    void apply_palette(const core::palette &palette);

    ImVec4 window_bg() const { return m_window_bg; }
    ImVec4 child_bg() const { return m_child_bg; }
    ImVec4 popup_bg() const { return m_popup_bg; }
    ImVec4 modal_dim_bg() const { return m_modal_dim_bg; }

    ImVec4 text() const { return m_text; }
    ImVec4 text_disabled() const { return m_text_disabled; }
    ImVec4 text_selected_bg() const { return m_text_selected_bg; }

    ImVec4 border() const { return m_border; }
    ImVec4 border_focused() const { return m_border_focused; }
    ImVec4 separator() const { return m_separator; }

    ImVec4 button() const { return m_button; }
    ImVec4 button_hovered() const { return m_button_hovered; }
    ImVec4 button_active() const { return m_button_active; }

    ImVec4 header() const { return m_header; }
    ImVec4 header_hovered() const { return m_header_hovered; }
    ImVec4 header_active() const { return m_header_active; }

    ImVec4 frame_bg() const { return m_frame_bg; }
    ImVec4 frame_bg_hovered() const { return m_frame_bg_hovered; }
    ImVec4 frame_bg_active() const { return m_frame_bg_active; }

    ImVec4 tab() const { return m_tab; }
    ImVec4 tab_hovered() const { return m_tab_hovered; }
    ImVec4 tab_active() const { return m_tab_active; }
    ImVec4 tab_unfocused() const { return m_tab_unfocused; }
    ImVec4 tab_unfocused_active() const { return m_tab_unfocused_active; }

    ImVec4 scrollbar_bg() const { return m_scrollbar_bg; }
    ImVec4 scrollbar_grab() const { return m_scrollbar_grab; }
    ImVec4 scrollbar_grab_hovered() const { return m_scrollbar_grab_hovered; }
    ImVec4 scrollbar_grab_active() const { return m_scrollbar_grab_active; }

    ImVec4 slider_grab() const { return m_slider_grab; }
    ImVec4 slider_grab_active() const { return m_slider_grab_active; }

    ImVec4 table_header_bg() const { return m_table_header_bg; }
    ImVec4 table_border_strong() const { return m_table_border_strong; }
    ImVec4 table_border_light() const { return m_table_border_light; }
    ImVec4 table_row_bg() const { return m_table_row_bg; }
    ImVec4 table_row_bg_alt() const { return m_table_row_bg_alt; }

    ImVec4 title_bg() const { return m_title_bg; }
    ImVec4 title_bg_active() const { return m_title_bg_active; }
    ImVec4 title_bg_collapsed() const { return m_title_bg_collapsed; }

    ImVec4 docking_preview() const { return m_docking_preview; }
    ImVec4 docking_empty_bg() const { return m_docking_empty_bg; }

    ImVec4 accent() const { return m_accent; }
    ImVec4 accent_secondary() const { return m_accent_secondary; }

    ImVec4 success() const { return m_success; }
    ImVec4 success_hovered() const { return m_success_hovered; }
    ImVec4 success_active() const { return m_success_active; }
    ImVec4 on_success() const { return m_on_success; }

    ImVec4 warning() const { return m_warning; }
    ImVec4 warning_hovered() const { return m_warning_hovered; }
    ImVec4 warning_active() const { return m_warning_active; }
    ImVec4 on_warning() const { return m_on_warning; }

    ImVec4 error() const { return m_error; }
    ImVec4 error_hovered() const { return m_error_hovered; }
    ImVec4 error_active() const { return m_error_active; }
    ImVec4 on_error() const { return m_on_error; }

    ImVec4 info() const { return m_info; }
    ImVec4 info_hovered() const { return m_info_hovered; }
    ImVec4 info_active() const { return m_info_active; }
    ImVec4 on_info() const { return m_on_info; }

    ImVec4 checkmark() const { return m_checkmark; }
    ImVec4 resize_grip() const { return m_resize_grip; }
    ImVec4 resize_grip_hovered() const { return m_resize_grip_hovered; }
    ImVec4 resize_grip_active() const { return m_resize_grip_active; }

    ImVec4 autocomplete_bg() const { return m_autocomplete_bg; }
    ImVec4 autocomplete_border() const { return m_autocomplete_border; }
    ImVec4 autocomplete_selected() const { return m_autocomplete_selected; }
    ImVec4 autocomplete_text() const { return m_autocomplete_text; }
    ImVec4 autocomplete_selected_text() const { return m_autocomplete_selected_text; }
    ImVec4 autocomplete_dim_text() const { return m_autocomplete_dim_text; }

    static constexpr float WINDOW_ROUNDING = 6.0f;
    static constexpr float FRAME_ROUNDING = 4.0f;
    static constexpr float POPUP_ROUNDING = 6.0f;
    static constexpr float SCROLLBAR_ROUNDING = 4.0f;
    static constexpr float GRAB_ROUNDING = 4.0f;
    static constexpr float TAB_ROUNDING = 4.0f;

    static constexpr float FRAME_BORDER_SIZE = 1.0f;
    static constexpr float WINDOW_BORDER_SIZE = 1.0f;
    static constexpr float POPUP_BORDER_SIZE = 1.0f;

    static constexpr ImVec2 WINDOW_PADDING{10.0f, 10.0f};
    static constexpr ImVec2 FRAME_PADDING{8.0f, 5.0f};
    static constexpr ImVec2 ITEM_SPACING{8.0f, 6.0f};
    static constexpr ImVec2 ITEM_INNER_SPACING{6.0f, 4.0f};

    void apply_to_imgui() const;
    void apply_style_vars() const;

  private:
    ImVec4 get_palette_color(const std::string &key,
                             const std::string &fallback = "") const;

    core::palette m_palette;

    ImVec4 m_window_bg{0.067f, 0.067f, 0.067f, 1.0f};
    ImVec4 m_child_bg{0.067f, 0.067f, 0.067f, 1.0f};
    ImVec4 m_popup_bg{0.098f, 0.098f, 0.098f, 0.98f};
    ImVec4 m_modal_dim_bg{0.0f, 0.0f, 0.0f, 0.5f};

    ImVec4 m_text{0.83f, 0.83f, 0.83f, 1.0f};
    ImVec4 m_text_disabled{0.52f, 0.60f, 0.60f, 1.0f};
    ImVec4 m_text_selected_bg{0.60f, 0.52f, 0.32f, 0.5f};

    ImVec4 m_border{0.14f, 0.14f, 0.14f, 1.0f};
    ImVec4 m_border_focused{0.18f, 0.18f, 0.18f, 1.0f};
    ImVec4 m_separator{0.14f, 0.14f, 0.14f, 1.0f};

    ImVec4 m_button{0.098f, 0.098f, 0.098f, 1.0f};
    ImVec4 m_button_hovered{0.60f, 0.52f, 0.32f, 0.7f};
    ImVec4 m_button_active{0.60f, 0.52f, 0.32f, 1.0f};

    ImVec4 m_header{0.098f, 0.098f, 0.098f, 1.0f};
    ImVec4 m_header_hovered{0.60f, 0.52f, 0.32f, 0.7f};
    ImVec4 m_header_active{0.60f, 0.52f, 0.32f, 1.0f};

    ImVec4 m_frame_bg{0.098f, 0.098f, 0.098f, 1.0f};
    ImVec4 m_frame_bg_hovered{0.12f, 0.12f, 0.12f, 1.0f};
    ImVec4 m_frame_bg_active{0.14f, 0.14f, 0.14f, 1.0f};

    ImVec4 m_tab{0.067f, 0.067f, 0.067f, 1.0f};
    ImVec4 m_tab_hovered{0.60f, 0.52f, 0.32f, 0.7f};
    ImVec4 m_tab_active{0.098f, 0.098f, 0.098f, 1.0f};
    ImVec4 m_tab_unfocused{0.067f, 0.067f, 0.067f, 1.0f};
    ImVec4 m_tab_unfocused_active{0.098f, 0.098f, 0.098f, 1.0f};

    ImVec4 m_scrollbar_bg{0.067f, 0.067f, 0.067f, 0.5f};
    ImVec4 m_scrollbar_grab{0.14f, 0.14f, 0.14f, 1.0f};
    ImVec4 m_scrollbar_grab_hovered{0.60f, 0.52f, 0.32f, 0.7f};
    ImVec4 m_scrollbar_grab_active{0.60f, 0.52f, 0.32f, 1.0f};

    ImVec4 m_slider_grab{0.60f, 0.52f, 0.32f, 1.0f};
    ImVec4 m_slider_grab_active{0.72f, 0.62f, 0.38f, 1.0f};

    ImVec4 m_table_header_bg{0.098f, 0.098f, 0.098f, 1.0f};
    ImVec4 m_table_border_strong{0.14f, 0.14f, 0.14f, 1.0f};
    ImVec4 m_table_border_light{0.10f, 0.10f, 0.10f, 1.0f};
    ImVec4 m_table_row_bg{0.0f, 0.0f, 0.0f, 0.0f};
    ImVec4 m_table_row_bg_alt{0.83f, 0.83f, 0.83f, 0.04f};

    ImVec4 m_title_bg{0.067f, 0.067f, 0.067f, 1.0f};
    ImVec4 m_title_bg_active{0.098f, 0.098f, 0.098f, 1.0f};
    ImVec4 m_title_bg_collapsed{0.067f, 0.067f, 0.067f, 0.75f};

    ImVec4 m_docking_preview{0.60f, 0.52f, 0.32f, 0.7f};
    ImVec4 m_docking_empty_bg{0.067f, 0.067f, 0.067f, 1.0f};

    ImVec4 m_accent{0.60f, 0.52f, 0.32f, 1.0f};
    ImVec4 m_accent_secondary{0.60f, 0.52f, 0.32f, 1.0f};

    ImVec4 m_success{0.40f, 0.54f, 0.32f, 1.0f};
    ImVec4 m_success_hovered{0.48f, 0.65f, 0.38f, 1.0f};
    ImVec4 m_success_active{0.32f, 0.43f, 0.26f, 1.0f};
    ImVec4 m_on_success{0.82f, 0.82f, 0.82f, 1.0f};

    ImVec4 m_warning{0.71f, 0.47f, 0.22f, 1.0f};
    ImVec4 m_warning_hovered{0.85f, 0.56f, 0.26f, 1.0f};
    ImVec4 m_warning_active{0.57f, 0.38f, 0.17f, 1.0f};
    ImVec4 m_on_warning{0.82f, 0.82f, 0.82f, 1.0f};

    ImVec4 m_error{0.67f, 0.31f, 0.29f, 1.0f};
    ImVec4 m_error_hovered{0.80f, 0.37f, 0.35f, 1.0f};
    ImVec4 m_error_active{0.53f, 0.25f, 0.23f, 1.0f};
    ImVec4 m_on_error{0.82f, 0.82f, 0.82f, 1.0f};

    ImVec4 m_info{0.23f, 0.54f, 0.55f, 1.0f};
    ImVec4 m_info_hovered{0.27f, 0.65f, 0.66f, 1.0f};
    ImVec4 m_info_active{0.18f, 0.43f, 0.44f, 1.0f};
    ImVec4 m_on_info{0.82f, 0.82f, 0.82f, 1.0f};

    ImVec4 m_checkmark{0.60f, 0.52f, 0.32f, 1.0f};
    ImVec4 m_resize_grip{0.14f, 0.14f, 0.14f, 0.5f};
    ImVec4 m_resize_grip_hovered{0.60f, 0.52f, 0.32f, 0.7f};
    ImVec4 m_resize_grip_active{0.60f, 0.52f, 0.32f, 1.0f};

    ImVec4 m_autocomplete_bg{0.098f, 0.098f, 0.098f, 0.98f};
    ImVec4 m_autocomplete_border{0.25f, 0.25f, 0.28f, 1.0f};
    ImVec4 m_autocomplete_selected{0.60f, 0.52f, 0.32f, 0.9f};
    ImVec4 m_autocomplete_text{0.85f, 0.85f, 0.9f, 1.0f};
    ImVec4 m_autocomplete_selected_text{1.0f, 1.0f, 1.0f, 1.0f};
    ImVec4 m_autocomplete_dim_text{0.52f, 0.60f, 0.60f, 1.0f};
};

app_theme &current_theme();

void set_theme(const core::palette &palette);

} // namespace clrsync::gui::theme

#endif // CLRSYNC_GUI_THEME_APP_THEME_HPP
