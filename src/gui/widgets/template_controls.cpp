#include "template_controls.hpp"
#include "colors.hpp"
#include "styled_checkbox.hpp"
#include "imgui.h"

namespace clrsync::gui::widgets
{

template_controls::template_controls() = default;

void template_controls::render(template_control_state& state,
                               const template_control_callbacks& callbacks,
                               const core::palette& palette,
                               validation_message& validation)
{
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 8));

    render_action_buttons(state, callbacks, palette);

    ImGui::PopStyleVar();

    ImGui::Spacing();

    render_fields(state, callbacks);

    validation.render(palette);
}

void template_controls::render_action_buttons(template_control_state& state,
                                              const template_control_callbacks& callbacks,
                                              const core::palette& palette)
{
    if (ImGui::Button(" + New "))
    {
        if (callbacks.on_new)
            callbacks.on_new();
    }
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Create a new template");

    ImGui::SameLine();
    if (ImGui::Button(" Save "))
    {
        if (callbacks.on_save)
            callbacks.on_save();
    }
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Save template (Ctrl+S)");

    if (state.is_editing_existing)
    {
        ImGui::SameLine();
        auto error = palette_color(palette, "error");
        auto error_hover = ImVec4(error.x * 1.1f, error.y * 1.1f, error.z * 1.1f, error.w);
        auto error_active = ImVec4(error.x * 0.8f, error.y * 0.8f, error.z * 0.8f, error.w);
        auto on_error = palette_color(palette, "on_error");
        ImGui::PushStyleColor(ImGuiCol_Button, error);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, error_hover);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, error_active);
        ImGui::PushStyleColor(ImGuiCol_Text, on_error);
        if (ImGui::Button(" Delete "))
        {
            if (callbacks.on_delete)
                callbacks.on_delete();
        }
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Delete this template");
        ImGui::PopStyleColor(4);
    }

    ImGui::SameLine();
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);

    bool old_enabled = state.enabled;
    styled_checkbox checkbox;
    checkbox.render("Enabled", &state.enabled, palette,
                    state.enabled ? checkbox_style::success : checkbox_style::error);

    if (old_enabled != state.enabled && state.is_editing_existing)
    {
        if (callbacks.on_enabled_changed)
            callbacks.on_enabled_changed(state.enabled);
    }
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Enable/disable this template for theme application");
}

void template_controls::render_fields(template_control_state& state,
                                      const template_control_callbacks& callbacks)
{
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Name:");
    ImGui::SameLine(80);
    ImGui::SetNextItemWidth(180.0f);
    char name_buf[256] = {0};
    snprintf(name_buf, sizeof(name_buf), "%s", state.name.c_str());
    if (ImGui::InputText("##template_name", name_buf, sizeof(name_buf)))
    {
        state.name = name_buf;
    }
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Unique name for this template");

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Input:");
    ImGui::SameLine(80);
    ImGui::SetNextItemWidth(-120.0f);
    char input_path_buf[512] = {0};
    snprintf(input_path_buf, sizeof(input_path_buf), "%s", state.input_path.c_str());
    if (ImGui::InputTextWithHint("##input_path", "Path to template file...", input_path_buf,
                                 sizeof(input_path_buf)))
    {
        state.input_path = input_path_buf;
        if (state.is_editing_existing && callbacks.on_input_path_changed)
            callbacks.on_input_path_changed(state.input_path);
    }
    ImGui::SameLine();
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
        ImGui::SetTooltip("Path where the template source file is stored");

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Output:");
    ImGui::SameLine(80);
    ImGui::SetNextItemWidth(-120.0f);
    char path_buf[512] = {0};
    snprintf(path_buf, sizeof(path_buf), "%s", state.output_path.c_str());
    if (ImGui::InputTextWithHint("##output_path", "Path for generated config...", path_buf,
                                 sizeof(path_buf)))
    {
        state.output_path = path_buf;
        if (state.is_editing_existing && callbacks.on_output_path_changed)
            callbacks.on_output_path_changed(state.output_path);
    }
    ImGui::SameLine();
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
        ImGui::SetTooltip("Path where the processed config will be written");

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Reload:");
    ImGui::SameLine(80);
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
}

} // namespace clrsync::gui::widgets
