#include "color_scheme_editor.hpp"
#include "gui/controllers/theme_applier.hpp"
#include "gui/widgets/dialogs.hpp"
#include "gui/widgets/palette_selector.hpp"
#include "gui/widgets/input_dialog.hpp"
#include "gui/widgets/action_buttons.hpp"
#include "imgui.h"
#include "settings_window.hpp"
#include "template_editor.hpp"
#include <iostream>

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
    
    setup_widgets();
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

    m_palette_selector.render(m_controller, 200.0f);

    ImGui::SameLine();
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 8);

    if (ImGui::Button(" + New "))
    {
        m_new_palette_dialog.open("New Palette", "Enter a name for the new palette:", "Palette name...");
    }
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Create a new palette");

    m_new_palette_dialog.render();

    ImGui::SameLine();
    m_action_buttons.render(current);

    if (m_show_delete_confirmation)
    {
        ImGui::OpenPopup("Delete Palette?");
        m_show_delete_confirmation = false;
    }

    clrsync::gui::widgets::delete_confirmation_dialog("Delete Palette?", current.name(), "palette",
                                                    current, [this]() {
                                                        m_controller.delete_current_palette();
                                                        apply_themes();
                                                    });

    ImGui::PopStyleVar(2);
}

void color_scheme_editor::setup_widgets()
{
    m_palette_selector.set_on_selection_changed([this](const std::string &name) {
        m_controller.select_palette(name);
        apply_themes();
    });
    
    m_new_palette_dialog.set_on_submit([this](const std::string &name) {
        m_controller.create_palette(name);
        m_controller.select_palette(name);
        apply_themes();
    });
    
    m_action_buttons.add_button({
        " Save ",
        "Save current palette to file",
        [this]() { m_controller.save_current_palette(); }
    });
    
    m_action_buttons.add_button({
        " Delete ",
        "Delete current palette",
        [this]() { m_show_delete_confirmation = true; },
        true,
        true
    });
    
    m_action_buttons.add_button({
        " Apply Theme ",
        "Apply current palette to all enabled templates",
        [this]() { m_controller.apply_current_theme(); }
    });
    
    m_action_buttons.set_spacing(16.0f);
}
