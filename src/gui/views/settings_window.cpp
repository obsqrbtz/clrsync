#include "gui/views/settings_window.hpp"
#include "core/common/error.hpp"
#include "core/config/config.hpp"
#include "gui/widgets/section_header.hpp"
#include "gui/ui_manager.hpp"
#include "imgui.h"

settings_window::settings_window(clrsync::gui::ui_manager* ui_mgr)
    : m_ui_manager(ui_mgr)
{
    if (m_ui_manager)
        m_available_fonts = m_ui_manager->get_system_fonts();

    setup_widgets();
    load_settings();
}

void settings_window::setup_widgets()
{
    m_form.set_path_browse_callback([this](const std::string& current_path) -> std::string {
        if (m_ui_manager)
            return m_ui_manager->select_folder_dialog("Select Directory", current_path);
        return "";
    });
}

void settings_window::render()
{
    if (!m_visible)
        return;

    ImGui::SetNextWindowSize(ImVec2(700, 500), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_FirstUseEver,
                            ImVec2(0.5f, 0.5f));

    if (ImGui::Begin("Settings", &m_visible, ImGuiWindowFlags_NoCollapse))
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

        m_error.render(m_current_palette);

        ImGui::Separator();

        clrsync::gui::widgets::settings_buttons_callbacks callbacks{
            .on_ok = [this]() {
                apply_settings();
                if (!m_error.has_error())
                {
                    m_visible = false;
                    m_settings_changed = false;
                }
            },
            .on_apply = [this]() {
                apply_settings();
                if (!m_error.has_error())
                    m_settings_changed = false;
            },
            .on_reset = [this]() { reset_to_defaults(); },
            .on_cancel = [this]() {
                load_settings();
                m_visible = false;
            }
        };
        m_buttons.render(callbacks, m_settings_changed);
    }
    ImGui::End();
}

void settings_window::load_settings()
{
    auto& cfg = clrsync::core::config::instance();

    m_default_theme = cfg.default_theme();
    m_palettes_path = cfg.palettes_path();
    m_font = cfg.font();
    m_font_size = cfg.font_size();

    m_selected_font_idx = 0;
    for (int i = 0; i < static_cast<int>(m_available_fonts.size()); i++)
    {
        if (m_available_fonts[i] == m_font)
        {
            m_selected_font_idx = i;
            break;
        }
    }

    m_error.clear();
    m_settings_changed = false;
}

void settings_window::apply_settings()
{
    auto& cfg = clrsync::core::config::instance();

    if (m_default_theme.empty())
    {
        m_error.set("Default theme cannot be empty");
        return;
    }

    if (m_palettes_path.empty())
    {
        m_error.set("Palettes path cannot be empty");
        return;
    }

    if (m_font.empty())
    {
        m_error.set("Font cannot be empty");
        return;
    }

    if (m_font_size < 8 || m_font_size > 48)
    {
        m_error.set("Font size must be between 8 and 48");
        return;
    }

    auto result1 = cfg.set_default_theme(m_default_theme);
    if (!result1)
    {
        m_error.set("Failed to set default theme: " + result1.error().description());
        return;
    }

    auto result2 = cfg.set_palettes_path(m_palettes_path);
    if (!result2)
    {
        m_error.set("Failed to set palettes path: " + result2.error().description());
        return;
    }

    auto result3 = cfg.set_font(m_font);
    if (!result3)
    {
        m_error.set("Failed to set font: " + result3.error().description());
        return;
    }

    auto result4 = cfg.set_font_size(m_font_size);
    if (!result4)
    {
        m_error.set("Failed to set font size: " + result4.error().description());
        return;
    }

    if (m_ui_manager && !m_ui_manager->reload_font(m_font.c_str(), m_font_size))
    {
        m_error.set("Failed to load font: " + m_font);
        return;
    }

    m_error.clear();
    m_settings_changed = false;
}

void settings_window::render_general_tab()
{
    using namespace clrsync::gui::widgets;

    section_header("Theme Settings", m_current_palette);

    form_field_config theme_cfg{
        .label = "Default Theme",
        .tooltip = "The default color scheme to load on startup",
        .field_width = -100.0f
    };
    if (m_form.render_text(theme_cfg, m_default_theme))
        m_settings_changed = true;

    section_header("Path Settings", m_current_palette);

    form_field_config path_cfg{
        .label = "Palettes Directory",
        .tooltip = "Directory where color palettes are stored\nSupports ~ for home directory",
        .field_width = -1.0f,
        .type = field_type::path
    };
    if (m_form.render_path(path_cfg, m_palettes_path))
        m_settings_changed = true;
}

void settings_window::render_appearance_tab()
{
    using namespace clrsync::gui::widgets;

    section_header("Font Settings", m_current_palette);

    form_field_config font_cfg{
        .label = "Font Family",
        .tooltip = "Select font family for the application interface",
        .field_width = -1.0f,
        .type = field_type::combo
    };
    if (m_form.render_combo(font_cfg, m_available_fonts, m_selected_font_idx, m_font))
        m_settings_changed = true;

    ImGui::Spacing();

    form_field_config size_cfg{
        .label = "Font Size",
        .tooltip = "Font size for the application interface (8-48)",
        .field_width = 120.0f,
        .type = field_type::slider,
        .min_value = 8.0f,
        .max_value = 48.0f,
        .format = "%d px",
        .show_reset = true,
        .default_value = 14
    };
    if (m_form.render_slider(size_cfg, m_font_size))
        m_settings_changed = true;
}

void settings_window::reset_to_defaults()
{
    m_default_theme = "dark";
    m_palettes_path = "~/.config/clrsync/palettes";
    m_font = "JetBrains Mono Nerd Font";
    m_font_size = 14;
    m_error.clear();
    m_settings_changed = true;
}
