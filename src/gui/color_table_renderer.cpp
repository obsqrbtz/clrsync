#include "color_table_renderer.hpp"
#include "imgui.h"
#include <vector>

void color_table_renderer::render_color_row(const std::string &name,
                                            const clrsync::core::palette& current,
                                            palette_controller& controller,
                                            const OnColorChangedCallback& on_changed)
{
    const auto &colors = current.colors();
    auto it = colors.find(name);
    if (it == colors.end())
        return;

    const clrsync::core::color &col = it->second;

    ImGui::TableNextRow();

    ImGui::TableSetColumnIndex(0);
    ImGui::TextUnformatted(name.c_str());

    ImGui::TableSetColumnIndex(1);
    {
        std::string hex_str = col.to_hex_string();
        char buf[9];
        strncpy(buf, hex_str.c_str(), sizeof(buf));
        buf[8] = 0;

        ImGui::SetNextItemWidth(-FLT_MIN);
        if (ImGui::InputText(("##hex_" + name).c_str(), buf, sizeof(buf),
                             ImGuiInputTextFlags_CharsUppercase))
        {
            try
            {
                clrsync::core::color new_color;
                new_color.from_hex_string(buf);
                controller.set_color(name, new_color);
                if (on_changed)
                    on_changed();
            }
            catch (...)
            {
            }
        }
    }

    ImGui::TableSetColumnIndex(2);
    ImGui::PushID(name.c_str());
    float c[4] = {((col.hex() >> 24) & 0xFF) / 255.0f, ((col.hex() >> 16) & 0xFF) / 255.0f,
                  ((col.hex() >> 8) & 0xFF) / 255.0f, (col.hex() & 0xFF) / 255.0f};

    ImGui::SetNextItemWidth(-FLT_MIN);
    if (ImGui::ColorEdit4(("##color_" + name).c_str(), c,
                          ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel |
                              ImGuiColorEditFlags_AlphaBar))
    {
        uint32_t r = (uint32_t)(c[0] * 255.0f);
        uint32_t g = (uint32_t)(c[1] * 255.0f);
        uint32_t b = (uint32_t)(c[2] * 255.0f);
        uint32_t a = (uint32_t)(c[3] * 255.0f);
        uint32_t hex = (r << 24) | (g << 16) | (b << 8) | a;

        controller.set_color(name, clrsync::core::color(hex));
        if (on_changed)
            on_changed();
    }

    ImGui::PopID();
}

void color_table_renderer::render(const clrsync::core::palette& current,
                                  palette_controller& controller,
                                  const OnColorChangedCallback& on_changed)
{
    if (current.colors().empty())
    {
        ImGui::Text("No palette loaded");
        return;
    }

    ImGui::Text("Color Variables");
    ImGui::Separator();

    auto draw_table = [&](const char *title, const std::vector<const char *> &keys) {
        ImGui::TextUnformatted(title);

        if (ImGui::BeginTable(title, 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
        {
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 160.0f);
            ImGui::TableSetupColumn("HEX", ImGuiTableColumnFlags_WidthFixed, 90.0f);
            ImGui::TableSetupColumn("Preview", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableHeadersRow();

            for (auto *k : keys)
                render_color_row(k, current, controller, on_changed);

            ImGui::EndTable();
        }

        ImGui::Spacing();
    };

    draw_table("General UI", {"background", "on_background", "surface", "on_surface",
                              "surface_variant", "on_surface_variant", "foreground",
                              "cursor", "accent"});

    draw_table("Borders", {"border_focused", "border"});

    draw_table("Semantic Colors", {"success", "info", "warning", "error",
                                   "on_success", "on_info", "on_warning", "on_error"});

    draw_table("Editor", {"editor_background", "editor_command", "editor_comment",
                          "editor_disabled", "editor_emphasis", "editor_error",
                          "editor_inactive", "editor_line_number", "editor_link",
                          "editor_main", "editor_selected", "editor_selection_inactive",
                          "editor_string", "editor_success", "editor_warning"});

    draw_table("Terminal (Base16)", {"base00", "base01", "base02", "base03",
                                     "base04", "base05", "base06", "base07",
                                     "base08", "base09", "base0A", "base0B",
                                     "base0C", "base0D", "base0E", "base0F"});
}
