#include "toolbar.hpp"
#include "colors.hpp"
#include "imgui.h"
#include <vector>

namespace clrsync::gui::widgets
{

void push_button_intent(button_intent intent)
{
    switch (intent)
    {
    case button_intent::primary:
        push_info_button_style();
        break;
    case button_intent::danger:
        push_error_button_style();
        break;
    case button_intent::success:
        push_success_button_style();
        break;
    case button_intent::warning:
        push_warning_button_style();
        break;
    case button_intent::neutral:
        break;
    }
}

void pop_button_intent(button_intent intent)
{
    if (intent != button_intent::neutral)
        pop_button_style();
}

namespace
{
float button_width(const std::string &label)
{
    return ImGui::CalcTextSize(label.c_str()).x + ImGui::GetStyle().FramePadding.x * 2.0f;
}

void draw_button(const toolbar_item &item)
{
    push_button_intent(item.intent);

    const bool disabled = !item.enabled;
    if (disabled)
        ImGui::BeginDisabled();

    if (ImGui::Button(item.label.c_str()) && item.on_click)
        item.on_click();

    if (disabled)
        ImGui::EndDisabled();

    pop_button_intent(item.intent);

    if (!item.tooltip.empty() && ImGui::IsItemHovered())
        ImGui::SetTooltip("%s", item.tooltip.c_str());
}
} // namespace

void toolbar::render(const std::vector<toolbar_item> &items, const std::function<void()> &leading,
                     float spacing)
{
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,
                        ImVec2(spacing, ImGui::GetStyle().ItemSpacing.y));

    bool have_leading = false;
    if (leading)
    {
        leading();
        have_leading = true;
    }

    if (have_leading)
        ImGui::SameLine(0, spacing);
    const float avail = ImGui::GetContentRegionAvail().x;

    const int n = static_cast<int>(items.size());
    std::vector<float> widths(n);
    for (int i = 0; i < n; ++i)
        widths[i] = button_width(items[i].label);

    const float overflow_w = button_width(" ... ");

    auto width_for = [&](int k, bool reserve_overflow) {
        float w = 0.0f;
        for (int i = 0; i < k; ++i)
        {
            if (i > 0)
                w += spacing;
            w += widths[i];
        }
        if (reserve_overflow)
            w += (k > 0 ? spacing : 0.0f) + overflow_w;
        return w;
    };

    int visible = n;
    if (width_for(n, false) > avail)
    {
        visible = 0;
        for (int k = 1; k <= n; ++k)
        {
            if (width_for(k, true) <= avail)
                visible = k;
            else
                break;
        }
    }

    for (int i = 0; i < visible; ++i)
    {
        if (i > 0 || have_leading)
            ImGui::SameLine(0, spacing);
        draw_button(items[i]);
    }

    if (visible < n)
    {
        if (visible > 0 || have_leading)
            ImGui::SameLine(0, spacing);

        if (ImGui::Button(" ... "))
            ImGui::OpenPopup(m_id);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("More actions");

        if (ImGui::BeginPopup(m_id))
        {
            for (int i = visible; i < n; ++i)
            {
                const toolbar_item &item = items[i];
                ImVec4 tint;
                bool tinted = false;
                switch (item.intent)
                {
                case button_intent::primary:
                    tint = theme().info();
                    tinted = true;
                    break;
                case button_intent::danger:
                    tint = theme().error();
                    tinted = true;
                    break;
                default:
                    break;
                }
                if (tinted)
                    ImGui::PushStyleColor(ImGuiCol_Text, tint);

                if (ImGui::MenuItem(item.label.c_str(), nullptr, false, item.enabled) &&
                    item.on_click)
                    item.on_click();

                if (tinted)
                    ImGui::PopStyleColor();
            }
            ImGui::EndPopup();
        }
    }

    ImGui::PopStyleVar();
}

} // namespace clrsync::gui::widgets
