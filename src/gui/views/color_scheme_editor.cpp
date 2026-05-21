#include "color_scheme_editor.hpp"
#include "gui/layout/ui_layout.hpp"
#include "core/palette/hellwal_generator.hpp"
#include "core/palette/matugen_generator.hpp"
#include "gui/controllers/theme_applier.hpp"
#include "gui/platform/file_browser.hpp"
#include "gui/widgets/action_buttons.hpp"
#include "gui/widgets/dialogs.hpp"
#include "gui/widgets/input_dialog.hpp"
#include "gui/widgets/palette_selector.hpp"
#include "imgui.h"
#include "settings_window.hpp"
#include "template_editor.hpp"
#include <filesystem>
#include <iostream>

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

    if (m_generate_state.generator_idx < 0 ||
        m_generate_state.generator_idx >= static_cast<int>(m_available_generators.size()))
        m_generate_state.generator_idx = 0;
}

std::optional<clrsync::gui::widgets::palette_generator_kind>
color_scheme_editor::selected_generator_kind() const
{
    if (m_generate_state.generator_idx < 0 ||
        m_generate_state.generator_idx >= static_cast<int>(m_available_generators.size()))
        return std::nullopt;

    return m_available_generators[m_generate_state.generator_idx];
}

void color_scheme_editor::execute_palette_generation()
{
    const auto selected_kind = selected_generator_kind();
    if (!selected_kind)
        return;

    try
    {
        if (*selected_kind == clrsync::gui::widgets::palette_generator_kind::hellwal)
        {
            clrsync::core::hellwal_generator gen;
            if (!gen.supports_current_system())
            {
                std::cerr << "Generation failed: hellwal is not supported on "
                          << clrsync::core::generator::system_name(
                                 clrsync::core::generator::current_system())
                          << std::endl;
                return;
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
            if (pal.name().empty())
            {
                std::filesystem::path p(image_path);
                pal.set_name(std::string("hellwal:") + p.filename().string());
            }
            m_controller.import_palette(pal);
            m_controller.select_palette(pal.name());
            apply_themes();
        }
        else if (*selected_kind == clrsync::gui::widgets::palette_generator_kind::matugen)
        {
            clrsync::core::matugen_generator gen;
            if (!gen.supports_current_system())
            {
                std::cerr << "Generation failed: matugen is not supported on "
                          << clrsync::core::generator::system_name(
                                 clrsync::core::generator::current_system())
                          << std::endl;
                return;
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
                if (pal.name().empty())
                {
                    std::filesystem::path p(image_path);
                    pal.set_name(std::string("matugen:") + p.filename().string());
                }
            }
            m_controller.import_palette(pal);
            m_controller.select_palette(pal.name());
            apply_themes();
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Generation failed: " << e.what() << std::endl;
    }
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

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,
                        ImVec2(clrsync::gui::layout::BUTTON_SPACING, 8));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 5));

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Palette:");
    ImGui::SameLine();

    m_palette_selector.render(m_controller, 200.0f);

    ImGui::SameLine(0, clrsync::gui::layout::BUTTON_SPACING);

    if (ImGui::Button(" + New "))
    {
        m_new_palette_dialog.open("New Palette",
                                  "Enter a name for the new palette:", "Palette name...");
    }
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Create a new palette");

    m_new_palette_dialog.render();
    m_generate_dialog.render();

    ImGui::SameLine(0, clrsync::gui::layout::BUTTON_SPACING);
    m_action_buttons.render(current);

    ImGui::SameLine(0, clrsync::gui::layout::BUTTON_SPACING);
    if (ImGui::Button("Generate"))
        m_generate_palette_dialog.open();

    m_generate_palette_dialog.render(m_generate_state, m_available_generators, m_generator_labels,
                                     current, [this]() { execute_palette_generation(); });

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
            if (m_available_generators[i] ==
                clrsync::gui::widgets::palette_generator_kind::hellwal)
            {
                m_generate_state.generator_idx = i;
                break;
            }
        }
        execute_palette_generation();
    });
    m_generate_dialog.set_path_browse_callback(
        [this](const std::string &current_path) -> std::string {
            return file_dialogs::open_file_dialog("Select Image", current_path,
                                                  file_dialogs::image_file_filters());
        });

    m_action_buttons.add_button({" Save ", "Save current palette to file",
                                 [this]() { m_controller.save_current_palette(); }});

    m_action_buttons.add_button({" Delete ", "Delete current palette",
                                 [this]() { m_show_delete_confirmation = true; }, true, true});

    m_action_buttons.add_button({" Apply Theme ", "Apply current palette to all enabled templates",
                                 [this]() { m_controller.apply_current_theme(); },
                                 true, false, false, true});

    m_action_buttons.set_spacing(clrsync::gui::layout::BUTTON_SPACING);
}
