#include "gui/views/settings_window.hpp"
#include "core/common/error.hpp"
#include "core/config/config.hpp"
#include "gui/widgets/colors.hpp"
#include "gui/ui_manager.hpp"
#include "imgui.h"
#include <cstring>

settings_window::settings_window(clrsync::gui::ui_manager* ui_mgr)
    : m_font_size(14), m_selected_font_idx(0), m_settings_changed(false), m_current_tab(0),
      m_ui_manager(ui_mgr)
{
    m_default_theme[0] = '\0';
    m_palettes_path[0] = '\0';
    m_font[0] = '\0';

    if (m_ui_manager)
        m_available_fonts = m_ui_manager->get_system_fonts();

    load_settings();
}

void settings_window::render()
{
    if (!m_visible)
        return;

    ImGui::SetNextWindowSize(ImVec2(700, 500), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_FirstUseEver,
                            ImVec2(0.5f, 0.5f));

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse;
    if (ImGui::Begin("Settings", &m_visible, window_flags))
    {
        if (ImGui::BeginTabBar("SettingsTabs", ImGuiTabBarFlags_None))
        {
            if (ImGui::BeginTabItem("General"))
            {
                render_general_tab();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Appearance"))
            {
                render_appearance_tab();
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

        render_status_messages();

        ImGui::Separator();
        render_action_buttons();
    }
    ImGui::End();
}

void settings_window::load_settings()
{
    auto &cfg = clrsync::core::config::instance();

    std::string default_theme = cfg.default_theme();
    strncpy(m_default_theme, default_theme.c_str(), sizeof(m_default_theme) - 1);
    m_default_theme[sizeof(m_default_theme) - 1] = '\0';

    std::string palettes_path = cfg.palettes_path();
    strncpy(m_palettes_path, palettes_path.c_str(), sizeof(m_palettes_path) - 1);
    m_palettes_path[sizeof(m_palettes_path) - 1] = '\0';

    std::string font = cfg.font();
    strncpy(m_font, font.c_str(), sizeof(m_font) - 1);
    m_font[sizeof(m_font) - 1] = '\0';

    m_selected_font_idx = 0;
    for (int i = 0; i < static_cast<int>(m_available_fonts.size()); i++)
    {
        if (m_available_fonts[i] == font)
        {
            m_selected_font_idx = i;
            break;
        }
    }

    m_font_size = cfg.font_size();

    m_error_message.clear();
    m_settings_changed = false;
}

void settings_window::apply_settings()
{
    auto &cfg = clrsync::core::config::instance();

    if (strlen(m_default_theme) == 0)
    {
        m_error_message = "Default theme cannot be empty";
        return;
    }

    if (strlen(m_palettes_path) == 0)
    {
        m_error_message = "Palettes path cannot be empty";
        return;
    }

    if (strlen(m_font) == 0)
    {
        m_error_message = "Font cannot be empty";
        return;
    }

    if (m_font_size < 8 || m_font_size > 48)
    {
        m_error_message = "Font size must be between 8 and 48";
        return;
    }

    auto result1 = cfg.set_default_theme(m_default_theme);
    if (!result1)
    {
        m_error_message = "Failed to set default theme: " + result1.error().description();
        return;
    }

    auto result2 = cfg.set_palettes_path(m_palettes_path);
    if (!result2)
    {
        m_error_message = "Failed to set palettes path: " + result2.error().description();
        return;
    }

    auto result3 = cfg.set_font(m_font);
    if (!result3)
    {
        m_error_message = "Failed to set font: " + result3.error().description();
        return;
    }

    auto result4 = cfg.set_font_size(m_font_size);
    if (!result4)
    {
        m_error_message = "Failed to set font size: " + result4.error().description();
        return;
    }

    if (m_ui_manager)
    {
        if (!m_ui_manager->reload_font(m_font, m_font_size))
        {
            m_error_message = "Failed to load font: " + std::string(m_font);
            return;
        }
    }

    m_error_message.clear();
    m_settings_changed = false;
}

void settings_window::render_general_tab()
{
    ImGui::Spacing();

    auto accent_color = clrsync::gui::widgets::palette_color(m_current_palette, "accent");
    ImGui::TextColored(accent_color, "Theme Settings");
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Text("Default Theme:");
    ImGui::SameLine();
    show_help_marker("The default color scheme to load on startup");
    ImGui::SetNextItemWidth(-100.0f);
    if (ImGui::InputText("##default_theme", m_default_theme, sizeof(m_default_theme)))
        m_settings_changed = true;

    ImGui::Spacing();

    ImGui::TextColored(accent_color, "Path Settings");
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Text("Palettes Directory:");
    ImGui::SameLine();
    show_help_marker("Directory where color palettes are stored\nSupports ~ for home directory");
    ImGui::SetNextItemWidth(-120.0f);
    if (ImGui::InputText("##palettes_path", m_palettes_path, sizeof(m_palettes_path)))
        m_settings_changed = true;
    ImGui::SameLine();
    if (ImGui::Button("Browse"))
    {
        std::string selected_path =
            m_ui_manager->select_folder_dialog("Select Palettes Directory", m_palettes_path);
        if (!selected_path.empty())
        {
            strncpy(m_palettes_path, selected_path.c_str(), sizeof(m_palettes_path) - 1);
            m_palettes_path[sizeof(m_palettes_path) - 1] = '\0';
            m_settings_changed = true;
        }
    }
}

void settings_window::render_appearance_tab()
{
    ImGui::Spacing();

    auto accent_color = clrsync::gui::widgets::palette_color(m_current_palette, "accent");
    ImGui::TextColored(accent_color, "Font Settings");
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Text("Font Family:");
    ImGui::SameLine();
    show_help_marker("Select font family for the application interface");
    ImGui::SetNextItemWidth(-1.0f);
    if (ImGui::BeginCombo("##font", m_font))
    {
        for (int i = 0; i < static_cast<int>(m_available_fonts.size()); i++)
        {
            bool is_selected = (i == m_selected_font_idx);
            if (ImGui::Selectable(m_available_fonts[i].c_str(), is_selected))
            {
                m_selected_font_idx = i;
                strncpy(m_font, m_available_fonts[i].c_str(), sizeof(m_font) - 1);
                m_font[sizeof(m_font) - 1] = '\0';
                m_settings_changed = true;
            }
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    ImGui::Spacing();

    ImGui::Text("Font Size:");
    ImGui::SameLine();
    show_help_marker("Font size for the application interface (8-48)");
    ImGui::SetNextItemWidth(120.0f);
    int old_size = m_font_size;
    if (ImGui::SliderInt("##font_size", &m_font_size, 8, 48, "%d px"))
    {
        if (old_size != m_font_size)
            m_settings_changed = true;
    }

    ImGui::SameLine();
    if (ImGui::Button("Reset"))
    {
        m_font_size = 14;
        m_settings_changed = true;
    }
}

void settings_window::render_status_messages()
{
    if (!m_error_message.empty())
    {
        ImGui::Spacing();

        auto error_bg_color = clrsync::gui::widgets::palette_color(m_current_palette, "error");
        auto error_text_color = clrsync::gui::widgets::palette_color(m_current_palette, "on_error");

        ImGui::PushStyleColor(ImGuiCol_ChildBg, error_bg_color);
        ImGui::PushStyleColor(ImGuiCol_Border, error_bg_color);
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 4.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1.0f);

        if (ImGui::BeginChild("##error_box", ImVec2(0, 0),
                              ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_Borders))
        {
            ImGui::PushStyleColor(ImGuiCol_Text, error_text_color);
            ImGui::TextWrapped("Error: %s", m_error_message.c_str());
            ImGui::PopStyleColor();

            ImGui::Spacing();

            ImGui::PushStyleColor(ImGuiCol_Button,
                                  ImVec4(error_bg_color.x * 0.8f, error_bg_color.y * 0.8f,
                                         error_bg_color.z * 0.8f, error_bg_color.w));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                                  ImVec4(error_bg_color.x * 0.6f, error_bg_color.y * 0.6f,
                                         error_bg_color.z * 0.6f, error_bg_color.w));
            ImGui::PushStyleColor(ImGuiCol_Text, error_text_color);

            if (ImGui::Button("Dismiss##error"))
                m_error_message.clear();

            ImGui::PopStyleColor(3);
        }
        ImGui::EndChild();

        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor(2);
    }
}

void settings_window::render_action_buttons()
{
    ImGui::Spacing();

    float button_width = 100.0f;
    float spacing = ImGui::GetStyle().ItemSpacing.x;
    float window_width = ImGui::GetContentRegionAvail().x;

    float total_buttons_width = 4 * button_width + 3 * spacing;
    float start_pos = (window_width - total_buttons_width) * 0.5f;

    if (start_pos > 0)
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + start_pos);

    if (ImGui::Button("OK", ImVec2(button_width, 0)))
    {
        apply_settings();
        if (m_error_message.empty())
        {
            m_visible = false;
            m_settings_changed = false;
        }
    }

    ImGui::SameLine();

    bool apply_disabled = !m_settings_changed;
    if (apply_disabled)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.5f);
    }

    if (ImGui::Button("Apply", ImVec2(button_width, 0)) && !apply_disabled)
    {
        apply_settings();
        if (m_error_message.empty())
        {
            m_settings_changed = false;
        }
    }

    if (apply_disabled)
    {
        ImGui::PopStyleVar();
    }

    ImGui::SameLine();

    if (ImGui::Button("Reset", ImVec2(button_width, 0)))
    {
        reset_to_defaults();
    }

    ImGui::SameLine();

    if (ImGui::Button("Cancel", ImVec2(button_width, 0)))
    {
        load_settings();
        m_visible = false;
        m_error_message.clear();
        m_settings_changed = false;
    }
}

void settings_window::show_help_marker(const char *desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::BeginItemTooltip())
    {
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

void settings_window::reset_to_defaults()
{
    strncpy(m_default_theme, "dark", sizeof(m_default_theme));
    strncpy(m_palettes_path, "~/.config/clrsync/palettes", sizeof(m_palettes_path));
    strncpy(m_font, "JetBrains Mono Nerd Font", sizeof(m_font));
    m_font_size = 14;
    m_error_message.clear();
    m_settings_changed = true;
}
