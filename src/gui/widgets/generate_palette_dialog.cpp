#include "generate_palette_dialog.hpp"
#include "colors.hpp"
#include "core/palette/color.hpp"
#include "gui/layout/ui_layout.hpp"
#include "section_header.hpp"
#include "gui/platform/file_browser.hpp"
#include "imgui.h"
#include <cstdio>
#include <cstring>
#include <optional>
#include <string>

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

void sync_hex_string_to_rgb(const std::string& hex, float rgb[3])
{
    if (hex.empty())
        return;
    try
    {
        clrsync::core::color col;
        col.from_hex_string(hex);
        const clrsync::core::rgb channels = col.to_rgb();
        rgb[0] = channels.r / 255.0f;
        rgb[1] = channels.g / 255.0f;
        rgb[2] = channels.b / 255.0f;
    }
    catch (...)
    {
    }
}

void sync_rgb_to_hex_string(const float rgb[3], std::string& hex)
{
    const int r = static_cast<int>(rgb[0] * 255.0f + 0.5f);
    const int g = static_cast<int>(rgb[1] * 255.0f + 0.5f);
    const int b = static_cast<int>(rgb[2] * 255.0f + 0.5f);
    char buf[16];
    std::snprintf(buf, sizeof(buf), "#%02X%02X%02X", r, g, b);
    hex = buf;
}

void render_hex_color_swatch(const char* id_prefix, float rgb[3], std::string& hex_value)
{
    const ImVec2 swatch_size(layout::COLOR_SWATCH_SIZE, layout::COLOR_SWATCH_SIZE);
    const ImVec4 color_vec(rgb[0], rgb[1], rgb[2], 1.0f);
    const std::string button_id = std::string("##") + id_prefix + "_swatch";
    const std::string popup_id = std::string("##") + id_prefix + "_picker";
    const std::string picker_id = std::string("##") + id_prefix + "_picker3";
    if (ImGui::ColorButton(button_id.c_str(), color_vec, ImGuiColorEditFlags_NoTooltip,
                           swatch_size))
        ImGui::OpenPopup(popup_id.c_str());
    if (ImGui::BeginPopup(popup_id.c_str()))
    {
        if (ImGui::ColorPicker3(picker_id.c_str(), rgb,
                               ImGuiColorEditFlags_NoSidePreview |
                                   ImGuiColorEditFlags_DisplayRGB))
            sync_rgb_to_hex_string(rgb, hex_value);
        ImGui::EndPopup();
    }
}

void render_optional_hex_color_row(const char* label, const char* id_prefix, bool& enabled,
                                   float rgb[3], std::string& hex_value)
{
    table_label(label);
    if (ImGui::Checkbox((std::string("Override##") + id_prefix).c_str(), &enabled) && enabled &&
        hex_value.empty())
        sync_rgb_to_hex_string(rgb, hex_value);
    if (enabled)
    {
        if (!hex_value.empty())
            sync_hex_string_to_rgb(hex_value, rgb);
        ImGui::SameLine(0.0f, layout::BUTTON_SPACING);
        render_hex_color_swatch(id_prefix, rgb, hex_value);
    }
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
    m_error.clear();
    m_is_open = true;
}

void generate_palette_dialog::set_error(const std::string &message)
{
    m_error.set(message);
    m_is_open = true;
}

void generate_palette_dialog::render(generate_palette_state& state,
                                     const std::vector<palette_generator_kind>& available_generators,
                                     const std::vector<std::string>& generator_labels,
                                     const core::palette& palette,
                                     const std::function<std::optional<std::string>()>& on_generate)
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
    else if (kind == palette_generator_kind::pywal16)
        render_pywal16_options(state, palette);

    m_error.render(palette);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    const bool submit_generate = render_footer(kind.has_value(), palette);

    ImGui::End();
    ImGui::PopStyleVar(3);

    if (submit_generate && on_generate)
    {
        if (const std::optional<std::string> error = on_generate())
            m_error.set(*error);
        else
            m_is_open = false;
    }
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

void generate_palette_dialog::render_pywal16_options(generate_palette_state& state,
                                                     const core::palette& palette)
{
    section_header("pywal16 options", palette);

    form_field_config image_cfg;
    image_cfg.label = "Image";
    image_cfg.label_width = DIALOG_LABEL_WIDTH;
    image_cfg.field_width = -1.0f;
    image_cfg.type = field_type::path;
    image_cfg.hint = "Select an image file...";
    image_cfg.tooltip = "Wallpaper or photo used as the color source";
    m_form.render_path(image_cfg, state.image_path);

    ImGui::Spacing();

    if (begin_field_table("##gen_pywal16"))
    {
        table_label("Light scheme");
        ImGui::Checkbox("Generate light colorscheme", &state.pywal16_light);

        table_label("Backend");
        const char* backends[] = {"default",       "haishoku",         "colorz",
                                  "modern_colorthief", "colorthief", "wal",
                                  "fast_colorthief",   "okthief"};
        int backend_idx = 0;
        for (int i = 1; i < IM_ARRAYSIZE(backends); ++i)
        {
            if (state.pywal16_backend == backends[i])
            {
                backend_idx = i;
                break;
            }
        }
        ImGui::SetNextItemWidth(-FLT_MIN);
        if (ImGui::Combo("##pywal16_backend", &backend_idx, backends, IM_ARRAYSIZE(backends)))
            state.pywal16_backend = (backend_idx == 0) ? "" : backends[backend_idx];

        table_label("Saturation");
        ImGui::Checkbox("Override saturation", &state.pywal16_use_saturate);
        if (state.pywal16_use_saturate)
        {
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::SliderFloat("##pywal16_saturate", &state.pywal16_saturate, 0.0f, 1.0f, "%.2f");
        }

        render_optional_hex_color_row("Background", "pywal16_bg", state.pywal16_use_background,
                                      state.pywal16_background_vec, state.pywal16_background);
        render_optional_hex_color_row("Foreground", "pywal16_fg", state.pywal16_use_foreground,
                                      state.pywal16_foreground_vec, state.pywal16_foreground);

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
        submit_generate = true;
    pop_button_style();

    if (!can_generate)
        ImGui::EndDisabled();

    ImGui::SameLine(0, layout::BUTTON_SPACING);

    if (ImGui::Button("Cancel", ImVec2(MODAL_BUTTON_WIDTH, 0)))
        m_is_open = false;

    return submit_generate;
}

} // namespace clrsync::gui::widgets
