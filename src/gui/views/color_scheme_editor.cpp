#include "color_scheme_editor.hpp"
#include "core/palette/hellwal_generator.hpp"
#include "core/palette/matugen_generator.hpp"
#include "core/palette/pywal16_generator.hpp"
#include "gui/controllers/theme_applier.hpp"
#include "gui/layout/ui_layout.hpp"
#include "gui/platform/file_browser.hpp"
#include "gui/widgets/dialogs.hpp"
#include "gui/widgets/input_dialog.hpp"
#include "gui/widgets/palette_selector.hpp"
#include "gui/widgets/toolbar.hpp"
#include "imgui.h"
#include "settings_window.hpp"
#include "template_editor.hpp"
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <optional>
#include <vector>

void color_scheme_editor::refresh_available_generators()
{
    m_available_generators.clear();
    m_generator_labels.clear();

    clrsync::core::hellwal_generator hellwal_gen;
    if (hellwal_gen.supports_current_system())
    {
        m_available_generators.push_back(clrsync::gui::widgets::palette_generator_kind::hellwal);
        m_generator_labels.push_back("hellwal");
    }

    clrsync::core::matugen_generator matugen_gen;
    if (matugen_gen.supports_current_system())
    {
        m_available_generators.push_back(clrsync::gui::widgets::palette_generator_kind::matugen);
        m_generator_labels.push_back("matugen");
    }

    clrsync::core::pywal16_generator pywal16_gen;
    if (pywal16_gen.supports_current_system())
    {
        m_available_generators.push_back(clrsync::gui::widgets::palette_generator_kind::pywal16);
        m_generator_labels.push_back("pywal16");
    }

    if (m_generate_state.generator_idx < 0 ||
        m_generate_state.generator_idx >= static_cast<int>(m_available_generators.size()))
        m_generate_state.generator_idx = 0;
}

std::optional<clrsync::gui::widgets::palette_generator_kind> color_scheme_editor::
    selected_generator_kind() const
{
    if (m_generate_state.generator_idx < 0 ||
        m_generate_state.generator_idx >= static_cast<int>(m_available_generators.size()))
        return std::nullopt;

    return m_available_generators[m_generate_state.generator_idx];
}

namespace
{
bool palette_has_colors(const clrsync::core::palette &pal)
{
    return !pal.colors().empty();
}
} // namespace

std::optional<std::string> color_scheme_editor::execute_palette_generation()
{
    const auto selected_kind = selected_generator_kind();
    if (!selected_kind)
        return "No palette generator is available on this system.";

    try
    {
        if (*selected_kind == clrsync::gui::widgets::palette_generator_kind::hellwal)
        {
            clrsync::core::hellwal_generator gen;
            if (!gen.supports_current_system())
            {
                return std::string("hellwal is not supported on ") +
                       clrsync::core::generator::system_name(
                           clrsync::core::generator::current_system());
            }
            clrsync::core::hellwal_generator::options opts;
            opts.neon = m_generate_state.neon;
            opts.dark = m_generate_state.dark;
            opts.light = m_generate_state.light;
            opts.color = m_generate_state.color;
            opts.dark_offset = m_generate_state.dark_offset;
            opts.bright_offset = m_generate_state.bright_offset;
            opts.invert = m_generate_state.invert;
            opts.gray_scale = m_generate_state.gray_scale;

            auto image_path = m_generate_state.image_path;
            if (image_path.empty())
            {
                image_path = file_dialogs::open_file_dialog("Select Image", "",
                                                            file_dialogs::image_file_filters());
            }

            auto pal = gen.generate_from_image(image_path, opts);
            if (!palette_has_colors(pal))
                return "hellwal did not produce any colors.";
            if (pal.name().empty())
            {
                std::filesystem::path p(image_path);
                pal.set_name(std::string("hellwal:") + p.filename().string());
            }
            m_controller.set_current_palette(pal);
            apply_themes();
        }
        else if (*selected_kind == clrsync::gui::widgets::palette_generator_kind::matugen)
        {
            clrsync::core::matugen_generator gen;
            if (!gen.supports_current_system())
            {
                return std::string("matugen is not supported on ") +
                       clrsync::core::generator::system_name(
                           clrsync::core::generator::current_system());
            }
            clrsync::core::matugen_generator::options opts;
            opts.mode = m_generate_state.matugen_mode;
            opts.type = m_generate_state.matugen_type;
            opts.contrast = m_generate_state.matugen_contrast;
            opts.source_color_index = m_generate_state.matugen_source_color_index;

            clrsync::core::palette pal;
            if (m_generate_state.matugen_use_color)
            {
                std::string hex = m_generate_state.matugen_color_hex;
                if (!hex.empty() && hex[0] == '#')
                    hex = hex.substr(1);
                pal = gen.generate_from_color(hex, opts);
                if (!palette_has_colors(pal))
                    return "matugen did not produce any colors.";
                if (pal.name().empty())
                    pal.set_name(std::string("matugen:color:") + hex);
            }
            else
            {
                auto image_path = m_generate_state.image_path;
                if (image_path.empty())
                {
                    image_path = file_dialogs::open_file_dialog("Select Image", "",
                                                                file_dialogs::image_file_filters());
                }
                pal = gen.generate_from_image(image_path, opts);
                if (!palette_has_colors(pal))
                    return "matugen did not produce any colors.";
                if (pal.name().empty())
                {
                    std::filesystem::path p(image_path);
                    pal.set_name(std::string("matugen:") + p.filename().string());
                }
            }
            m_controller.set_current_palette(pal);
            apply_themes();
        }
        else if (*selected_kind == clrsync::gui::widgets::palette_generator_kind::pywal16)
        {
            clrsync::core::pywal16_generator gen;
            if (!gen.supports_current_system())
            {
                return std::string("pywal16 is not supported on ") +
                       clrsync::core::generator::system_name(
                           clrsync::core::generator::current_system());
            }
            clrsync::core::pywal16_generator::options opts;
            if (m_generate_state.pywal16_use_background)
                opts.background = m_generate_state.pywal16_background;
            if (m_generate_state.pywal16_use_foreground)
                opts.foreground = m_generate_state.pywal16_foreground;
            opts.backend = m_generate_state.pywal16_backend;
            opts.light = m_generate_state.pywal16_light;
            if (m_generate_state.pywal16_use_saturate)
                opts.saturate = m_generate_state.pywal16_saturate;

            auto image_path = m_generate_state.image_path;
            if (image_path.empty())
            {
                image_path = file_dialogs::open_file_dialog("Select Image", "",
                                                            file_dialogs::image_file_filters());
            }

            auto pal = gen.generate_from_image(image_path, opts);
            if (!palette_has_colors(pal))
                return "pywal16 did not produce any colors.";
            if (pal.name().empty())
            {
                std::filesystem::path p(image_path);
                pal.set_name(std::string("pywal16:") + p.filename().string());
            }
            m_controller.set_current_palette(pal);
            apply_themes();
        }
    }
    catch (const std::exception &e)
    {
        return e.what();
    }

    return std::nullopt;
}

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

    refresh_available_generators();
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
    m_color_table.render(
        m_controller.current_palette(), m_controller, [this]() { apply_themes(); },
        [this](const std::string &key) {
            if (m_template_editor)
                m_template_editor->insert_token(key);
        });
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

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(clrsync::gui::layout::BUTTON_SPACING, 8));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 5));

    // Combo width shrinks with the panel so it never crowds out the buttons.
    const float row_avail = ImGui::GetContentRegionAvail().x;
    const float combo_width = std::clamp(row_avail * 0.35f, 120.0f, 220.0f);

    auto leading = [&]() {
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Palette:");
        ImGui::SameLine();
        m_palette_selector.render(m_controller, combo_width);
    };

    using clrsync::gui::widgets::button_intent;
    using clrsync::gui::widgets::toolbar_item;
    std::vector<toolbar_item> items = {
        {" Apply Theme ", "Apply current palette to all enabled templates",
         [this]() { m_controller.apply_current_theme(); }, true, button_intent::primary},
        {" Save ", "Save current palette to file",
         [this]() { m_controller.save_current_palette(); }},
        {"New ", "Create a new palette",
         [this]() {
             m_new_palette_dialog.open("New Palette",
                                       "Enter a name for the new palette:", "Palette name...");
         }},
        {" Generate ", "Generate a palette from an image or color",
         [this]() { m_generate_palette_dialog.open(); }},
        {" Delete ", "Delete current palette", [this]() { m_show_delete_confirmation = true; },
         true, button_intent::danger},
    };

    m_toolbar.render(items, leading, clrsync::gui::layout::BUTTON_SPACING);

    m_new_palette_dialog.render();
    m_generate_dialog.render();
    m_generate_palette_dialog.render(m_generate_state, m_available_generators, m_generator_labels,
                                     current, [this]() { return execute_palette_generation(); });

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

    m_generate_dialog.set_on_submit([this](const std::string &image_path) {
        m_generate_state.image_path = image_path;
        for (int i = 0; i < static_cast<int>(m_available_generators.size()); ++i)
        {
            if (m_available_generators[i] == clrsync::gui::widgets::palette_generator_kind::hellwal)
            {
                m_generate_state.generator_idx = i;
                break;
            }
        }
        if (const std::optional<std::string> error = execute_palette_generation())
            m_generate_palette_dialog.set_error(*error);
    });
    m_generate_dialog.set_path_browse_callback(
        [this](const std::string &current_path) -> std::string {
            return file_dialogs::open_file_dialog("Select Image", current_path,
                                                  file_dialogs::image_file_filters());
        });

    m_toolbar.set_id("##palette_toolbar_overflow");
}
