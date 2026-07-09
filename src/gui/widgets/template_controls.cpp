#include "template_controls.hpp"
#include "gui/layout/ui_layout.hpp"
#include "imgui.h"
#include "styled_checkbox.hpp"
#include <vector>

namespace clrsync::gui::widgets
{

using layout::BROWSE_BUTTON_WIDTH;
using layout::BUTTON_SPACING;
using layout::FORM_LABEL_WIDTH;

template_controls::template_controls() = default;

void template_controls::render(template_control_state &state,
                               const template_control_callbacks &callbacks,
                               const core::palette &palette, validation_message &validation)
{
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(BUTTON_SPACING, 8));

    render_action_buttons(state, callbacks, palette);

    ImGui::PopStyleVar();

    ImGui::Spacing();

    if (ImGui::CollapsingHeader("Template Settings", ImGuiTreeNodeFlags_DefaultOpen))
    {
        render_fields(state, callbacks);
    }

    validation.render(palette);
}

void template_controls::render_action_buttons(template_control_state &state,
                                              const template_control_callbacks &callbacks,
                                              const core::palette &palette)
{
    auto leading = [&]() {
        bool old_enabled = state.enabled;
        styled_checkbox checkbox;
        checkbox.render("Enabled", &state.enabled, palette,
                        state.enabled ? checkbox_style::success : checkbox_style::error);

        if (old_enabled != state.enabled && state.is_editing_existing &&
            callbacks.on_enabled_changed)
            callbacks.on_enabled_changed(state.enabled);

        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Enable/disable this template for theme application");
    };

    std::vector<toolbar_item> items = {
        {" Save ", "Save template (Ctrl+S)",
         [&]() {
             if (callbacks.on_save)
                 callbacks.on_save();
         }},
        {" New ", "Create a new template",
         [&]() {
             if (callbacks.on_new)
                 callbacks.on_new();
         }},
    };

    if (state.is_editing_existing)
    {
        items.push_back({" Delete ", "Delete this template",
                         [&]() {
                             if (callbacks.on_delete)
                                 callbacks.on_delete();
                         },
                         true, button_intent::danger});
    }

    m_toolbar.set_id("##template_toolbar_overflow");
    m_toolbar.render(items, leading, BUTTON_SPACING);
}

void template_controls::render_fields(template_control_state &state,
                                      const template_control_callbacks &callbacks)
{
    if (!ImGui::BeginTable("##template_fields", 3,
                           ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_PadOuterX))
        return;

    ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, FORM_LABEL_WIDTH);
    ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthFixed, BROWSE_BUTTON_WIDTH);

    auto label_cell = [](const char *text) {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted(text);
        ImGui::TableSetColumnIndex(1);
    };

    label_cell("Name:");
    ImGui::SetNextItemWidth(-FLT_MIN);
    char name_buf[256] = {0};
    snprintf(name_buf, sizeof(name_buf), "%s", state.name.c_str());
    if (ImGui::InputText("##template_name", name_buf, sizeof(name_buf)))
        state.name = name_buf;
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Unique name for this template");

    label_cell("Input:");
    ImGui::SetNextItemWidth(-FLT_MIN);
    char input_path_buf[512] = {0};
    snprintf(input_path_buf, sizeof(input_path_buf), "%s", state.input_path.c_str());
    if (ImGui::InputTextWithHint("##input_path", "Path to template file...", input_path_buf,
                                 sizeof(input_path_buf)))
    {
        state.input_path = input_path_buf;
        if (state.is_editing_existing && callbacks.on_input_path_changed)
            callbacks.on_input_path_changed(state.input_path);
    }
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Path where the template source file is stored");
    ImGui::TableSetColumnIndex(2);
    if (ImGui::Button("Browse##input"))
    {
        if (callbacks.on_browse_input)
        {
            std::string selected = callbacks.on_browse_input(state.input_path);
            if (!selected.empty())
            {
                state.input_path = selected;
                if (state.is_editing_existing && callbacks.on_input_path_changed)
                    callbacks.on_input_path_changed(state.input_path);
            }
        }
    }
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Browse for template source file");

    label_cell("Output:");
    ImGui::SetNextItemWidth(-FLT_MIN);
    char path_buf[512] = {0};
    snprintf(path_buf, sizeof(path_buf), "%s", state.output_path.c_str());
    if (ImGui::InputTextWithHint("##output_path", "Path for generated config...", path_buf,
                                 sizeof(path_buf)))
    {
        state.output_path = path_buf;
        if (state.is_editing_existing && callbacks.on_output_path_changed)
            callbacks.on_output_path_changed(state.output_path);
    }
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Path where the processed config will be written");
    ImGui::TableSetColumnIndex(2);
    if (ImGui::Button("Browse##output"))
    {
        if (callbacks.on_browse_output)
        {
            std::string selected = callbacks.on_browse_output(state.output_path);
            if (!selected.empty())
            {
                state.output_path = selected;
                if (state.is_editing_existing && callbacks.on_output_path_changed)
                    callbacks.on_output_path_changed(state.output_path);
            }
        }
    }
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Browse for output path");

    label_cell("Reload:");
    ImGui::SetNextItemWidth(-FLT_MIN);
    char reload_buf[512] = {0};
    snprintf(reload_buf, sizeof(reload_buf), "%s", state.reload_command.c_str());
    if (ImGui::InputTextWithHint("##reload_cmd", "Command to reload app (optional)...", reload_buf,
                                 sizeof(reload_buf)))
    {
        state.reload_command = reload_buf;
        if (state.is_editing_existing && callbacks.on_reload_command_changed)
            callbacks.on_reload_command_changed(state.reload_command);
    }
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Shell command to run after applying theme (e.g., 'pkill -USR1 kitty')");

    ImGui::EndTable();
}

} // namespace clrsync::gui::widgets
