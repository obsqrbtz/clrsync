#include "form_field.hpp"
#include "imgui.h"
#include <cstring>

namespace clrsync::gui::widgets
{

form_field::form_field() = default;

void form_field::render_label(const form_field_config& config)
{
    ImGui::AlignTextToFramePadding();
    ImGui::Text("%s:", config.label.c_str());
    if (config.label_width > 0)
    {
        ImGui::SameLine(config.label_width);
    }
    else
    {
        ImGui::SameLine();
    }
}

void form_field::render_tooltip(const form_field_config& config)
{
    if (!config.tooltip.empty() && ImGui::IsItemHovered())
    {
        ImGui::SetTooltip("%s", config.tooltip.c_str());
    }
}

bool form_field::render_text(const form_field_config& config, std::string& value)
{
    render_label(config);
    
    if (config.field_width > 0)
    {
        ImGui::SetNextItemWidth(config.field_width);
    }
    else if (config.field_width < 0)
    {
        ImGui::SetNextItemWidth(config.field_width);
    }

    char buffer[512] = {0};
    strncpy(buffer, value.c_str(), sizeof(buffer) - 1);
    
    bool changed = false;
    std::string id = "##" + config.label;
    
    if (config.type == field_type::text_with_hint)
    {
        changed = ImGui::InputTextWithHint(id.c_str(), config.hint.c_str(), buffer, sizeof(buffer));
    }
    else
    {
        changed = ImGui::InputText(id.c_str(), buffer, sizeof(buffer));
    }
    
    if (changed)
    {
        value = buffer;
    }
    
    render_tooltip(config);
    return changed;
}

bool form_field::render_number(const form_field_config& config, int& value)
{
    render_label(config);
    
    if (config.field_width > 0)
    {
        ImGui::SetNextItemWidth(config.field_width);
    }
    else if (config.field_width < 0)
    {
        ImGui::SetNextItemWidth(config.field_width);
    }
    
    std::string id = "##" + config.label;
    bool changed = ImGui::InputInt(id.c_str(), &value);
    
    if (value < (int)config.min_value) value = (int)config.min_value;
    if (value > (int)config.max_value) value = (int)config.max_value;
    
    render_tooltip(config);
    return changed;
}

bool form_field::render_number(const form_field_config& config, float& value)
{
    render_label(config);
    
    if (config.field_width > 0)
    {
        ImGui::SetNextItemWidth(config.field_width);
    }
    else if (config.field_width < 0)
    {
        ImGui::SetNextItemWidth(config.field_width);
    }
    
    std::string id = "##" + config.label;
    bool changed = ImGui::InputFloat(id.c_str(), &value);
    
    if (value < config.min_value) value = config.min_value;
    if (value > config.max_value) value = config.max_value;
    
    render_tooltip(config);
    return changed;
}

bool form_field::render_path(const form_field_config& config, std::string& value)
{
    render_label(config);
    
    float browse_button_width = 80.0f;
    float available_width = ImGui::GetContentRegionAvail().x;
    float input_width = available_width - browse_button_width - ImGui::GetStyle().ItemSpacing.x;
    
    if (config.field_width > 0)
    {
        input_width = config.field_width - browse_button_width - ImGui::GetStyle().ItemSpacing.x;
    }
    
    ImGui::SetNextItemWidth(input_width);
    
    char buffer[512] = {0};
    strncpy(buffer, value.c_str(), sizeof(buffer) - 1);
    
    std::string input_id = "##" + config.label + "_input";
    bool changed = false;
    
    if (config.type == field_type::text_with_hint)
    {
        changed = ImGui::InputTextWithHint(input_id.c_str(), config.hint.c_str(), buffer, sizeof(buffer));
    }
    else
    {
        changed = ImGui::InputText(input_id.c_str(), buffer, sizeof(buffer));
    }
    
    if (changed)
    {
        value = buffer;
    }
    
    ImGui::SameLine();
    
    std::string button_id = "Browse##" + config.label;
    if (ImGui::Button(button_id.c_str()))
    {
        if (m_path_browse_callback)
        {
            std::string selected_path = m_path_browse_callback(value);
            if (!selected_path.empty())
            {
                value = selected_path;
                changed = true;
            }
        }
    }
    
    render_tooltip(config);
    return changed;
}

void form_field::render_readonly(const form_field_config& config, const std::string& value)
{
    render_label(config);
    
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "%s", value.c_str());
    
    render_tooltip(config);
}

void form_field::set_path_browse_callback(const std::function<std::string(const std::string&)>& callback)
{
    m_path_browse_callback = callback;
}

} // namespace clrsync::gui::widgets