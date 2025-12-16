#include "color_scheme_editor.hpp"
#include "template_editor.hpp"
#include "theme_applier.hpp"
#include "imgui.h"
#include <iostream>
#include <ranges>

color_scheme_editor::color_scheme_editor()
{
    const auto &current = m_controller.current_palette();
    
    if (!current.colors().empty())
    {
        theme_applier::apply_to_imgui(current);
        m_preview.apply_palette(current);
    }
    else
    {
        std::cout << "WARNING: No palette loaded, skipping theme application\n";
    }
}

void color_scheme_editor::notify_palette_changed()
{
    if (m_template_editor)
    {
        m_template_editor->apply_current_palette(m_controller.current_palette());
    }
}

void color_scheme_editor::apply_themes()
{
    const auto &current = m_controller.current_palette();
    theme_applier::apply_to_imgui(current);
    m_preview.apply_palette(current);
    notify_palette_changed();
}

void color_scheme_editor::render_controls_and_colors()
{
    ImGui::Begin("Color Schemes");

    render_controls();
    ImGui::Separator();

    ImGui::BeginChild("ColorTableContent", ImVec2(0, 0), false);
    m_color_table.render(m_controller.current_palette(), m_controller, 
                        [this]() { apply_themes(); });
    ImGui::EndChild();

    ImGui::End();
}

void color_scheme_editor::render_preview()
{
    ImGui::Begin("Color Preview");
    
    m_preview.render(m_controller.current_palette());
    
    ImGui::End();
}

void color_scheme_editor::render_controls()
{
    const auto &current = m_controller.current_palette();
    const auto &palettes = m_controller.palettes();

    const float avail_width = ImGui::GetContentRegionAvail().x;

    ImGui::Text("Color Scheme:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(std::min(200.0f, avail_width * 0.3f));
    if (ImGui::BeginCombo("##scheme", current.name().c_str()))
    {
        for (const auto &name : palettes | std::views::keys)
        {
            const bool selected = current.name() == name;
            if (ImGui::Selectable(name.c_str(), selected))
            {
                m_controller.select_palette(name);
                apply_themes();
            }
            if (selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    ImGui::SameLine();

    static char new_palette_name_buf[128] = "";
    if (ImGui::Button("New"))
    {
        new_palette_name_buf[0] = 0;
        ImGui::OpenPopup("New Palette");
    }

    if (ImGui::BeginPopupModal("New Palette", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("New palette name:");
        ImGui::InputText("##new_palette_input", new_palette_name_buf,
                         IM_ARRAYSIZE(new_palette_name_buf));

        ImGui::Separator();

        if (ImGui::Button("Create", ImVec2(120, 0)))
        {
            if (strlen(new_palette_name_buf) > 0)
            {
                m_controller.create_palette(new_palette_name_buf);
                m_controller.select_palette(new_palette_name_buf);
                apply_themes();
                new_palette_name_buf[0] = 0;
            }
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            new_palette_name_buf[0] = 0;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    ImGui::SameLine();
    if (ImGui::Button("Save"))
    {
        m_controller.save_current_palette();
    }

    ImGui::SameLine();
    if (ImGui::Button("Delete"))
    {
        m_controller.delete_current_palette();
    }

    ImGui::SameLine();
    if (ImGui::Button("Apply"))
    {
        m_controller.apply_current_theme();
    }
}
