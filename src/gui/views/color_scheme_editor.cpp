#include "color_scheme_editor.hpp"
#include "gui/controllers/theme_applier.hpp"
#include "gui/helpers/imgui_helpers.hpp"
#include "imgui.h"
#include "settings_window.hpp"
#include "template_editor.hpp"
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
    if (m_settings_window)
    {
        m_settings_window->set_palette(m_controller.current_palette());
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
        ImGui::InputTextWithHint("##new_palette_input", "Palette name...", new_palette_name_buf,
                                 IM_ARRAYSIZE(new_palette_name_buf));

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
    auto error = palette_utils::get_color(current, "error");
    auto error_hover = ImVec4(error.x * 1.1f, error.y * 1.1f, error.z * 1.1f, error.w);
    auto error_active = ImVec4(error.x * 0.8f, error.y * 0.8f, error.z * 0.8f, error.w);
    auto on_error = palette_utils::get_color(current, "on_error");
    ImGui::PushStyleColor(ImGuiCol_Button, error);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, error_hover);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, error_active);
    ImGui::PushStyleColor(ImGuiCol_Text, on_error);
    if (ImGui::Button(" Delete "))
    {
        m_show_delete_confirmation = true;
    }
    ImGui::PopStyleColor(4);
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Delete current palette");

    if (m_show_delete_confirmation)
    {
        ImGui::OpenPopup("Delete Palette?");
        m_show_delete_confirmation = false;
    }

    palette_utils::render_delete_confirmation_popup("Delete Palette?", current.name(), "palette",
                                                    current, [this]() {
                                                        m_controller.delete_current_palette();
                                                        apply_themes();
                                                    });

    ImGui::SameLine();
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 16);

    if (ImGui::Button(" Apply Theme "))
    {
        m_controller.apply_current_theme();
    }
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Apply current palette to all enabled templates");

    ImGui::PopStyleVar(2);
}
