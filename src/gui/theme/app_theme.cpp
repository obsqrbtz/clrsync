#include "app_theme.hpp"
#include <algorithm>

namespace clrsync::gui::theme
{
ImVec4 color_utils::from_hex(uint32_t hex)
{
    return {
        ((hex >> 24) & 0xFF) / 255.0f,
        ((hex >> 16) & 0xFF) / 255.0f,
        ((hex >> 8) & 0xFF) / 255.0f,
        (hex & 0xFF) / 255.0f
    };
}

uint32_t color_utils::to_imgui_u32(uint32_t hex)
{
    const uint32_t r = (hex >> 24) & 0xFF;
    const uint32_t g = (hex >> 16) & 0xFF;
    const uint32_t b = (hex >> 8) & 0xFF;
    const uint32_t a = hex & 0xFF;
    return (a << 24) | (b << 16) | (g << 8) | r;
}

ImVec4 color_utils::with_alpha(const ImVec4 &color, float alpha)
{
    return {color.x, color.y, color.z, std::clamp(alpha, 0.0f, 1.0f)};
}

ImVec4 color_utils::lighten(const ImVec4 &color, float amount)
{
    return {
        std::clamp(color.x + (1.0f - color.x) * amount, 0.0f, 1.0f),
        std::clamp(color.y + (1.0f - color.y) * amount, 0.0f, 1.0f),
        std::clamp(color.z + (1.0f - color.z) * amount, 0.0f, 1.0f),
        color.w
    };
}

ImVec4 color_utils::darken(const ImVec4 &color, float amount)
{
    return {
        std::clamp(color.x * (1.0f - amount), 0.0f, 1.0f),
        std::clamp(color.y * (1.0f - amount), 0.0f, 1.0f),
        std::clamp(color.z * (1.0f - amount), 0.0f, 1.0f),
        color.w
    };
}

ImVec4 color_utils::blend(const ImVec4 &a, const ImVec4 &b, float t)
{
    t = std::clamp(t, 0.0f, 1.0f);
    return {
        a.x + (b.x - a.x) * t,
        a.y + (b.y - a.y) * t,
        a.z + (b.z - a.z) * t,
        a.w + (b.w - a.w) * t
    };
}

app_theme::app_theme(const core::palette &palette)
{
    apply_palette(palette);
}

ImVec4 app_theme::get_palette_color(const std::string &key,
                                     const std::string &fallback) const
{
    auto colors = m_palette.colors();
    if (colors.empty())
        return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

    auto it = colors.find(key);
    if (it == colors.end() && !fallback.empty())
        it = colors.find(fallback);

    if (it != colors.end())
        return color_utils::from_hex(it->second.hex());

    return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
}

void app_theme::apply_palette(const core::palette &palette)
{
    m_palette = palette;

    if (palette.colors().empty())
        return;

    const ImVec4 background = get_palette_color("background");
    const ImVec4 on_background = get_palette_color("on_background");
    const ImVec4 surface = get_palette_color("surface");
    const ImVec4 on_surface = get_palette_color("on_surface");
    const ImVec4 surface_variant = get_palette_color("surface_variant");
    const ImVec4 on_surface_variant = get_palette_color("on_surface_variant");
    const ImVec4 border_base = get_palette_color("border");
    const ImVec4 border_focused_base = get_palette_color("border_focused");
    const ImVec4 accent_base = get_palette_color("accent");
    const ImVec4 accent_secondary_base = get_palette_color("accent_secondary");

    const ImVec4 success_base = get_palette_color("success");
    const ImVec4 on_success_base = get_palette_color("on_success");
    const ImVec4 warning_base = get_palette_color("warning");
    const ImVec4 on_warning_base = get_palette_color("on_warning");
    const ImVec4 error_base = get_palette_color("error");
    const ImVec4 on_error_base = get_palette_color("on_error");
    const ImVec4 info_base = get_palette_color("info");
    const ImVec4 on_info_base = get_palette_color("on_info");

    m_window_bg = background;
    m_child_bg = color_utils::with_alpha(surface, 0.0f); // Transparent child bg
    m_popup_bg = color_utils::with_alpha(surface_variant, 0.98f);
    m_modal_dim_bg = ImVec4(0.0f, 0.0f, 0.0f, 0.5f);

    m_text = on_surface;
    m_text_disabled = on_surface_variant;
    m_text_selected_bg = color_utils::with_alpha(accent_base, 0.4f);

    m_border = border_base;
    m_border_focused = border_focused_base;
    m_separator = border_base;

    m_button = surface_variant;
    m_button_hovered = color_utils::with_alpha(accent_base, 0.7f);
    m_button_active = accent_base;

    m_header = surface_variant;
    m_header_hovered = color_utils::with_alpha(accent_base, 0.7f);
    m_header_active = accent_base;

    m_frame_bg = surface_variant;
    m_frame_bg_hovered = color_utils::lighten(surface_variant, 0.08f);
    m_frame_bg_active = color_utils::lighten(surface_variant, 0.15f);

    m_tab = surface;
    m_tab_hovered = color_utils::with_alpha(accent_base, 0.7f);
    m_tab_active = surface_variant;
    m_tab_unfocused = surface;
    m_tab_unfocused_active = surface_variant;

    m_scrollbar_bg = color_utils::with_alpha(surface, 0.5f);
    m_scrollbar_grab = surface_variant;
    m_scrollbar_grab_hovered = color_utils::with_alpha(accent_base, 0.7f);
    m_scrollbar_grab_active = accent_base;

    m_slider_grab = accent_base;
    m_slider_grab_active = color_utils::lighten(accent_base, 0.2f);

    m_table_header_bg = surface_variant;
    m_table_border_strong = border_base;
    m_table_border_light = color_utils::darken(border_base, 0.3f);
    m_table_row_bg = ImVec4(0, 0, 0, 0);
    m_table_row_bg_alt = color_utils::with_alpha(on_surface_variant, 0.04f);

    m_title_bg = surface;
    m_title_bg_active = surface_variant;
    m_title_bg_collapsed = color_utils::with_alpha(surface, 0.75f);

    m_docking_preview = color_utils::with_alpha(accent_base, 0.7f);
    m_docking_empty_bg = background;

    m_accent = accent_base;
    m_accent_secondary = accent_secondary_base;

    m_success = success_base;
    m_success_hovered = color_utils::lighten(success_base, 0.2f);
    m_success_active = color_utils::darken(success_base, 0.2f);
    m_on_success = on_success_base;

    m_warning = warning_base;
    m_warning_hovered = color_utils::lighten(warning_base, 0.2f);
    m_warning_active = color_utils::darken(warning_base, 0.2f);
    m_on_warning = on_warning_base;

    m_error = error_base;
    m_error_hovered = color_utils::lighten(error_base, 0.2f);
    m_error_active = color_utils::darken(error_base, 0.2f);
    m_on_error = on_error_base;

    m_info = info_base;
    m_info_hovered = color_utils::lighten(info_base, 0.2f);
    m_info_active = color_utils::darken(info_base, 0.2f);
    m_on_info = on_info_base;

    m_checkmark = accent_base;
    m_resize_grip = color_utils::with_alpha(surface_variant, 0.5f);
    m_resize_grip_hovered = color_utils::with_alpha(accent_base, 0.7f);
    m_resize_grip_active = accent_base;

    m_autocomplete_bg = color_utils::with_alpha(surface_variant, 0.98f);
    m_autocomplete_border = border_focused_base;
    m_autocomplete_selected = color_utils::with_alpha(accent_base, 0.9f);
    m_autocomplete_text = on_surface;
    m_autocomplete_selected_text = on_surface;
    m_autocomplete_dim_text = on_surface_variant;
}

void app_theme::apply_style_vars() const
{
    ImGuiStyle &style = ImGui::GetStyle();

    style.WindowRounding = WINDOW_ROUNDING;
    style.ChildRounding = FRAME_ROUNDING;
    style.FrameRounding = FRAME_ROUNDING;
    style.PopupRounding = POPUP_ROUNDING;
    style.ScrollbarRounding = SCROLLBAR_ROUNDING;
    style.GrabRounding = GRAB_ROUNDING;
    style.TabRounding = TAB_ROUNDING;

    style.FrameBorderSize = FRAME_BORDER_SIZE;
    style.WindowBorderSize = WINDOW_BORDER_SIZE;
    style.PopupBorderSize = POPUP_BORDER_SIZE;

    style.WindowPadding = WINDOW_PADDING;
    style.FramePadding = FRAME_PADDING;
    style.ItemSpacing = ITEM_SPACING;
    style.ItemInnerSpacing = ITEM_INNER_SPACING;

    style.ScrollbarSize = 12.0f;
    style.GrabMinSize = 10.0f;
    style.SeparatorTextBorderSize = 1.0f;
    style.IndentSpacing = 20.0f;
}

void app_theme::apply_to_imgui() const
{
    apply_style_vars();

    ImGuiStyle &style = ImGui::GetStyle();

    style.Colors[ImGuiCol_WindowBg] = m_window_bg;
    style.Colors[ImGuiCol_ChildBg] = m_child_bg;
    style.Colors[ImGuiCol_PopupBg] = m_popup_bg;
    style.Colors[ImGuiCol_ModalWindowDimBg] = m_modal_dim_bg;

    style.Colors[ImGuiCol_Text] = m_text;
    style.Colors[ImGuiCol_TextDisabled] = m_text_disabled;
    style.Colors[ImGuiCol_TextSelectedBg] = m_text_selected_bg;

    style.Colors[ImGuiCol_Border] = m_border;
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0, 0, 0, 0);
    style.Colors[ImGuiCol_Separator] = m_separator;
    style.Colors[ImGuiCol_SeparatorHovered] = m_accent;
    style.Colors[ImGuiCol_SeparatorActive] = m_accent;

    style.Colors[ImGuiCol_Button] = m_button;
    style.Colors[ImGuiCol_ButtonHovered] = m_button_hovered;
    style.Colors[ImGuiCol_ButtonActive] = m_button_active;

    style.Colors[ImGuiCol_Header] = m_header;
    style.Colors[ImGuiCol_HeaderHovered] = m_header_hovered;
    style.Colors[ImGuiCol_HeaderActive] = m_header_active;

    style.Colors[ImGuiCol_FrameBg] = m_frame_bg;
    style.Colors[ImGuiCol_FrameBgHovered] = m_frame_bg_hovered;
    style.Colors[ImGuiCol_FrameBgActive] = m_frame_bg_active;

    style.Colors[ImGuiCol_Tab] = m_tab;
    style.Colors[ImGuiCol_TabHovered] = m_tab_hovered;
    style.Colors[ImGuiCol_TabActive] = m_tab_active;
    style.Colors[ImGuiCol_TabUnfocused] = m_tab_unfocused;
    style.Colors[ImGuiCol_TabUnfocusedActive] = m_tab_unfocused_active;
    style.Colors[ImGuiCol_TabSelectedOverline] = m_accent;

    style.Colors[ImGuiCol_ScrollbarBg] = m_scrollbar_bg;
    style.Colors[ImGuiCol_ScrollbarGrab] = m_scrollbar_grab;
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = m_scrollbar_grab_hovered;
    style.Colors[ImGuiCol_ScrollbarGrabActive] = m_scrollbar_grab_active;

    style.Colors[ImGuiCol_SliderGrab] = m_slider_grab;
    style.Colors[ImGuiCol_SliderGrabActive] = m_slider_grab_active;

    style.Colors[ImGuiCol_TableHeaderBg] = m_table_header_bg;
    style.Colors[ImGuiCol_TableBorderStrong] = m_table_border_strong;
    style.Colors[ImGuiCol_TableBorderLight] = m_table_border_light;
    style.Colors[ImGuiCol_TableRowBg] = m_table_row_bg;
    style.Colors[ImGuiCol_TableRowBgAlt] = m_table_row_bg_alt;

    style.Colors[ImGuiCol_TitleBg] = m_title_bg;
    style.Colors[ImGuiCol_TitleBgActive] = m_title_bg_active;
    style.Colors[ImGuiCol_TitleBgCollapsed] = m_title_bg_collapsed;

    style.Colors[ImGuiCol_MenuBarBg] = m_window_bg;

    style.Colors[ImGuiCol_DockingPreview] = m_docking_preview;
    style.Colors[ImGuiCol_DockingEmptyBg] = m_docking_empty_bg;

    style.Colors[ImGuiCol_CheckMark] = m_checkmark;
    style.Colors[ImGuiCol_ResizeGrip] = m_resize_grip;
    style.Colors[ImGuiCol_ResizeGripHovered] = m_resize_grip_hovered;
    style.Colors[ImGuiCol_ResizeGripActive] = m_resize_grip_active;

    style.Colors[ImGuiCol_NavHighlight] = m_accent;
    style.Colors[ImGuiCol_NavWindowingHighlight] = color_utils::with_alpha(m_accent, 0.7f);
    style.Colors[ImGuiCol_NavWindowingDimBg] = m_modal_dim_bg;

    style.Colors[ImGuiCol_PlotLines] = m_accent;
    style.Colors[ImGuiCol_PlotLinesHovered] = m_accent;
    style.Colors[ImGuiCol_PlotHistogram] = m_accent;
    style.Colors[ImGuiCol_PlotHistogramHovered] = color_utils::lighten(m_accent, 0.2f);
}


static app_theme g_current_theme;

app_theme &current_theme()
{
    return g_current_theme;
}

void set_theme(const core::palette &palette)
{
    g_current_theme.apply_palette(palette);
    g_current_theme.apply_to_imgui();
}

} // namespace clrsync::gui::theme
