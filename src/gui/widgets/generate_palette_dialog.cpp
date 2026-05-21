#include "generate_palette_dialog.hpp"
#include "colors.hpp"
#include "gui/layout/ui_layout.hpp"
#include "section_header.hpp"
#include "gui/platform/file_browser.hpp"
#include "imgui.h"
#include <cstdio>
#include <cstring>
#include <optional>

namespace clrsync::gui::widgets
{

namespace
{
constexpr float WINDOW_WIDTH = 520.0f;
constexpr float DIALOG_LABEL_WIDTH = 132.0f;
constexpr float MODAL_BUTTON_WIDTH = 120.0f;
constexpr const char* WINDOW_TITLE = "Generate Palette";

std::optional<palette_generator_kind> selected_kind(
    const generate_palette_state& state,
    const std::vector<palette_generator_kind>& available_generators)
{
    if (state.generator_idx < 0 ||
        state.generator_idx >= static_cast<int>(available_generators.size()))
        return std::nullopt;
    return available_generators[state.generator_idx];
}

bool begin_field_table(const char* id)
{
    const float width = ImGui::GetContentRegionAvail().x;
    if (!ImGui::BeginTable(id, 2,
                           ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_PadOuterX,
                           ImVec2(width, 0.0f)))
        return false;
    ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, DIALOG_LABEL_WIDTH);
    ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);
    return true;
}

void table_label(const char* text)
{
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::AlignTextToFramePadding();
    ImGui::TextUnformatted(text);
    ImGui::TableSetColumnIndex(1);
}

void table_slider(const char* label, const char* id, float* value, float min_v, float max_v,
                  const char* format)
{
    table_label(label);
    ImGui::SetNextItemWidth(-FLT_MIN);
    ImGui::SliderFloat(id, value, min_v, max_v, format);
}
} // namespace

generate_palette_dialog::generate_palette_dialog()
{
    m_form.set_path_browse_callback([](const std::string& current_path) -> std::string {
        return file_dialogs::open_file_dialog("Select Image", current_path,
                                              file_dialogs::image_file_filters());
    });
}

void generate_palette_dialog::open()
{
    m_is_open = true;
}

void generate_palette_dialog::render(generate_palette_state& state,
                                     const std::vector<palette_generator_kind>& available_generators,
                                     const std::vector<std::string>& generator_labels,
                                     const core::palette& palette,
                                     const std::function<void()>& on_generate)
{
    if (!m_is_open)
        return;

    if (state.generator_idx < 0 ||
        state.generator_idx >= static_cast<int>(available_generators.size()))
        state.generator_idx = 0;

    ImGui::SetNextWindowSize(ImVec2(WINDOW_WIDTH, 0), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing,
                            ImVec2(0.5f, 0.5f));

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 14));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(layout::BUTTON_SPACING, 10));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 6));

    if (!ImGui::Begin(WINDOW_TITLE, &m_is_open,
                      ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse))
    {
        ImGui::PopStyleVar(3);
        return;
    }

    ImGui::TextDisabled("Generate a palette from an image using a system tool.");
    ImGui::Spacing();

    render_generator_row(state, generator_labels);

    const auto kind = selected_kind(state, available_generators);
    if (kind == palette_generator_kind::hellwal)
        render_hellwal_options(state, palette);
    else if (kind == palette_generator_kind::matugen)
        render_matugen_options(state, palette);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    const bool submit_generate = render_footer(kind.has_value(), palette);

    ImGui::End();
    ImGui::PopStyleVar(3);

    if (submit_generate && on_generate)
        on_generate();
}

void generate_palette_dialog::render_generator_row(
    generate_palette_state& state, const std::vector<std::string>& generator_labels)
{
    if (!begin_field_table("##gen_main"))
        return;

    table_label("Generator");
    if (generator_labels.empty())
    {
        ImGui::TextDisabled("No supported generators on this system");
    }
    else
    {
        std::vector<const char*> items;
        items.reserve(generator_labels.size());
        for (const auto& label : generator_labels)
            items.push_back(label.c_str());
        ImGui::SetNextItemWidth(-FLT_MIN);
        ImGui::Combo("##gen_select", &state.generator_idx, items.data(),
                     static_cast<int>(items.size()));
    }

    ImGui::EndTable();
    ImGui::Spacing();
}

void generate_palette_dialog::render_hellwal_options(generate_palette_state& state,
                                                     const core::palette& palette)
{
    section_header("hellwal options", palette);

    form_field_config image_cfg;
    image_cfg.label = "Image";
    image_cfg.label_width = DIALOG_LABEL_WIDTH;
    image_cfg.field_width = -1.0f;
    image_cfg.type = field_type::path;
    image_cfg.hint = "Select an image file...";
    image_cfg.tooltip = "Wallpaper or photo used as the color source";
    m_form.render_path(image_cfg, state.image_path);

    ImGui::Spacing();

    if (begin_field_table("##gen_hellwal_flags"))
    {
        table_label("Neon mode");
        ImGui::Checkbox("Enable neon highlights", &state.neon);
        ImGui::EndTable();
    }

    ImGui::Spacing();
    ImGui::TextUnformatted("Modes (can combine)");
    ImGui::Checkbox("Dark", &state.dark);
    ImGui::SameLine(0, layout::BUTTON_SPACING * 3);
    ImGui::Checkbox("Light", &state.light);
    ImGui::SameLine(0, layout::BUTTON_SPACING * 3);
    ImGui::Checkbox("Color", &state.color);

    ImGui::Spacing();

    if (begin_field_table("##gen_hellwal_sliders"))
    {
        table_slider("Dark offset", "##dark_offset", &state.dark_offset, 0.0f, 1.0f, "%.3f");
        table_slider("Bright offset", "##bright_offset", &state.bright_offset, 0.0f, 1.0f, "%.3f");
        table_label("Invert colors");
        ImGui::Checkbox("Invert extracted colors", &state.invert);
        table_slider("Gray scale", "##gray_scale", &state.gray_scale, 0.0f, 1.0f, "%.3f");
        ImGui::EndTable();
    }
}

void generate_palette_dialog::render_matugen_options(generate_palette_state& state,
                                                     const core::palette& palette)
{
    section_header("matugen options", palette);

    if (!state.matugen_use_color)
    {
        form_field_config image_cfg;
        image_cfg.label = "Image";
        image_cfg.label_width = DIALOG_LABEL_WIDTH;
        image_cfg.field_width = -1.0f;
        image_cfg.type = field_type::path;
        image_cfg.hint = "Select an image file...";
        m_form.render_path(image_cfg, state.image_path);
        ImGui::Spacing();
    }

    if (begin_field_table("##gen_matugen"))
    {
        table_label("Mode");
        const char* modes[] = {"dark", "light"};
        int mode_idx = (state.matugen_mode == "light") ? 1 : 0;
        ImGui::SetNextItemWidth(-FLT_MIN);
        if (ImGui::Combo("##matugen_mode", &mode_idx, modes, IM_ARRAYSIZE(modes)))
            state.matugen_mode = (mode_idx == 1) ? "light" : "dark";

        table_label("Type");
        const char* types[] = {"scheme-content",     "scheme-expressive", "scheme-fidelity",
                               "scheme-fruit-salad", "scheme-monochrome", "scheme-neutral",
                               "scheme-rainbow",     "scheme-tonal-spot"};
        int type_idx = 7;
        for (int i = 0; i < IM_ARRAYSIZE(types); ++i)
        {
            if (state.matugen_type == types[i])
            {
                type_idx = i;
                break;
            }
        }
        ImGui::SetNextItemWidth(-FLT_MIN);
        if (ImGui::Combo("##matugen_type", &type_idx, types, IM_ARRAYSIZE(types)))
            state.matugen_type = types[type_idx];

        table_slider("Contrast", "##matugen_contrast", &state.matugen_contrast, -1.0f, 1.0f,
                     "%.2f");

        table_label("Source color");
        ImGui::SetNextItemWidth(-FLT_MIN);
        ImGui::SliderInt("##matugen_source", &state.matugen_source_color_index, 0, 3);

        table_label("Input");
        ImGui::Checkbox("Use solid color instead of image", &state.matugen_use_color);

        if (state.matugen_use_color)
        {
            table_label("Color");
            if (ImGui::ColorEdit3("##matugen_color", state.matugen_color_vec))
            {
                const int r = static_cast<int>(state.matugen_color_vec[0] * 255.0f + 0.5f);
                const int g = static_cast<int>(state.matugen_color_vec[1] * 255.0f + 0.5f);
                const int b = static_cast<int>(state.matugen_color_vec[2] * 255.0f + 0.5f);
                char hexbuf[8];
                std::snprintf(hexbuf, sizeof(hexbuf), "%02X%02X%02X", r, g, b);
                state.matugen_color_hex = hexbuf;
            }
        }

        ImGui::EndTable();
    }
}

bool generate_palette_dialog::render_footer(bool can_generate, const core::palette& palette)
{
    (void)palette;
    bool submit_generate = false;

    if (!can_generate)
        ImGui::BeginDisabled();

    const float spacing = ImGui::GetStyle().ItemSpacing.x;
    const float total_width = 2.0f * MODAL_BUTTON_WIDTH + spacing;
    const float start_x = (ImGui::GetContentRegionAvail().x - total_width) * 0.5f;
    if (start_x > 0.0f)
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + start_x);

    push_success_button_style();
    if (ImGui::Button("Generate", ImVec2(MODAL_BUTTON_WIDTH, 0)))
    {
        submit_generate = true;
        m_is_open = false;
    }
    pop_button_style();

    if (!can_generate)
        ImGui::EndDisabled();

    ImGui::SameLine(0, layout::BUTTON_SPACING);

    if (ImGui::Button("Cancel", ImVec2(MODAL_BUTTON_WIDTH, 0)))
        m_is_open = false;

    return submit_generate;
}

} // namespace clrsync::gui::widgets
