#include "settings_window.hpp"
#include "core/config/config.hpp"
#include "gui/font_loader.hpp"
#include "imgui.h"
#include <cstring>

settings_window::settings_window()
    : m_font_size(14)
{
    m_default_theme[0] = '\0';
    m_palettes_path[0] = '\0';
    m_font[0] = '\0';
    load_settings();
}

void settings_window::render()
{
    if (!m_visible)
        return;

    ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);
    
    if (ImGui::Begin("Settings", &m_visible))
    {
        ImGui::Text("General Settings");
        ImGui::Separator();
        ImGui::Spacing();
        
        ImGui::Text("Default Theme:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(300.0f);
        ImGui::InputText("##default_theme", m_default_theme, sizeof(m_default_theme));
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("The default color scheme to load on startup");
        }
        
        ImGui::Spacing();
        
        ImGui::Text("Palettes Path:");
        ImGui::SetNextItemWidth(-FLT_MIN);
        ImGui::InputText("##palettes_path", m_palettes_path, sizeof(m_palettes_path));
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Directory where color palettes are stored\nSupports ~ for home directory");
        }
        
        ImGui::Spacing();
        
        ImGui::Text("Font:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(300.0f);
        ImGui::InputText("##font", m_font, sizeof(m_font));
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Font");
        }
        
        ImGui::Spacing();
        
        ImGui::Text("Font Size:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(100.0f);
        ImGui::InputInt("##font_size", &m_font_size, 1, 1);
        if (m_font_size < 8) m_font_size = 8;
        if (m_font_size > 48) m_font_size = 48;
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Font size");
        }
        
        ImGui::Spacing();
        ImGui::Spacing();
        
        if (!m_error_message.empty())
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
            ImGui::TextWrapped("%s", m_error_message.c_str());
            ImGui::PopStyleColor();
            ImGui::Spacing();
        }
        
        ImGui::Separator();
        ImGui::Spacing();
        
        if (ImGui::Button("OK", ImVec2(120, 0)))
        {
            apply_settings();
            m_visible = false;
            m_error_message = "";
        }
        
        ImGui::SameLine();
        if (ImGui::Button("Apply", ImVec2(120, 0)))
        {
            apply_settings();
        }
        
        ImGui::SameLine();
        if (ImGui::Button("Reset to Defaults", ImVec2(150, 0)))
        {
            strncpy(m_default_theme, "dark", sizeof(m_default_theme));
            strncpy(m_palettes_path, "~/.config/clrsync/palettes", sizeof(m_palettes_path));
            strncpy(m_font, "JetBrains Mono Nerd Font", sizeof(m_font));
            m_font_size = 14;
            m_error_message = "";
        }
        
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            load_settings();
            m_visible = false;
            m_error_message = "";
        }
    }
    ImGui::End();
}

void settings_window::load_settings()
{
    try
    {
        auto& cfg = clrsync::core::config::instance();
        
        std::string default_theme = cfg.default_theme();
        strncpy(m_default_theme, default_theme.c_str(), sizeof(m_default_theme) - 1);
        m_default_theme[sizeof(m_default_theme) - 1] = '\0';
        
        std::string palettes_path = cfg.palettes_path();
        strncpy(m_palettes_path, palettes_path.c_str(), sizeof(m_palettes_path) - 1);
        m_palettes_path[sizeof(m_palettes_path) - 1] = '\0';
        
        std::string font = cfg.font();
        strncpy(m_font, font.c_str(), sizeof(m_font) - 1);
        m_font[sizeof(m_font) - 1] = '\0';
        
        m_font_size = cfg.font_size();
        
        m_error_message = "";
    }
    catch (const std::exception& e)
    {
        m_error_message = std::string("Failed to load settings: ") + e.what();
        
        // Set defaults on error
        strncpy(m_default_theme, "dark", sizeof(m_default_theme));
        strncpy(m_palettes_path, "~/.config/clrsync/palettes", sizeof(m_palettes_path));
        strncpy(m_font, "JetBrains Mono Nerd Font", sizeof(m_font));
        m_font_size = 14;
    }
}

void settings_window::apply_settings()
{
    try
    {
        auto& cfg = clrsync::core::config::instance();
        
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
        
        cfg.set_default_theme(m_default_theme);
        cfg.set_palettes_path(m_palettes_path);
        cfg.set_font(m_font);
        cfg.set_font_size(m_font_size);


        font_loader fn_loader;
        auto font = fn_loader.load_font(m_font, m_font_size);
        if (font)
            ImGui::GetIO().FontDefault = font;
        
        m_error_message = "";
    }
    catch (const std::exception& e)
    {
        m_error_message = std::string("Failed to apply settings: ") + e.what();
    }
}