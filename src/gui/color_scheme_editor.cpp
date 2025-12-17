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

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(6, 8));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 5));

    // Palette selector
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Palette:");
    ImGui::SameLine();
    
    ImGui::SetNextItemWidth(200.0f);
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
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Select a color palette to edit");

    ImGui::SameLine();
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 8);

    // Action buttons
    static char new_palette_name_buf[128] = "";
    if (ImGui::Button(" + New "))
    {
        new_palette_name_buf[0] = 0;
        ImGui::OpenPopup("New Palette");
    }
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Create a new palette");

    if (ImGui::BeginPopupModal("New Palette", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Enter a name for the new palette:");
        ImGui::Spacing();
        
        ImGui::SetNextItemWidth(250);
        ImGui::InputTextWithHint("##new_palette_input", "Palette name...",
                                 new_palette_name_buf, IM_ARRAYSIZE(new_palette_name_buf));

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        bool can_create = strlen(new_palette_name_buf) > 0;
        
        if (!can_create)
            ImGui::BeginDisabled();
            
        if (ImGui::Button("Create", ImVec2(120, 0)))
        {
            m_controller.create_palette(new_palette_name_buf);
            m_controller.select_palette(new_palette_name_buf);
            apply_themes();
            new_palette_name_buf[0] = 0;
            ImGui::CloseCurrentPopup();
        }
        
        if (!can_create)
            ImGui::EndDisabled();

        ImGui::SameLine();

        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            new_palette_name_buf[0] = 0;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    ImGui::SameLine();
    if (ImGui::Button(" Save "))
    {
        m_controller.save_current_palette();
    }
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Save current palette to file");

    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.2f, 0.2f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.3f, 0.3f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.5f, 0.1f, 0.1f, 1.0f));
    if (ImGui::Button(" Delete "))
    {
        m_show_delete_confirmation = true;
    }
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Delete current palette");
    ImGui::PopStyleColor(3);

    if (m_show_delete_confirmation)
    {
        ImGui::OpenPopup("Delete Palette?");
        m_show_delete_confirmation = false;
    }

    if (ImGui::BeginPopupModal("Delete Palette?", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.4f, 1.0f), 
                          "Are you sure you want to delete '%s'?", current.name().c_str());
        ImGui::Text("This action cannot be undone.");
        
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.2f, 0.2f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.3f, 0.3f, 1.0f));
        if (ImGui::Button("Delete", ImVec2(120, 0)))
        {
            m_controller.delete_current_palette();
            apply_themes();
            ImGui::CloseCurrentPopup();
        }
        ImGui::PopStyleColor(2);
        
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    ImGui::SameLine();
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 16);
    
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.5f, 0.7f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.6f, 0.8f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.15f, 0.4f, 0.6f, 1.0f));
    if (ImGui::Button(" Apply Theme "))
    {
        m_controller.apply_current_theme();
    }
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Apply current palette to all enabled templates");
    ImGui::PopStyleColor(3);
    
    ImGui::PopStyleVar(2);
}
