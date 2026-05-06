#include "color_scheme_editor.hpp"
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
#include <cstdio>
#include <filesystem>
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
        m_new_palette_dialog.open("New Palette",
                                  "Enter a name for the new palette:", "Palette name...");
    }
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Create a new palette");

    m_new_palette_dialog.render();
    m_generate_dialog.render();

    ImGui::SameLine();
    m_action_buttons.render(current);

    ImGui::SameLine();
    ImGui::SameLine();
    if (ImGui::Button("Generate"))
    {
        m_show_generate_modal = true;
    }

    if (m_show_generate_modal)
    {
        ImGui::OpenPopup("Generate Palette");
        m_show_generate_modal = false;
    }

    if (ImGui::BeginPopupModal("Generate Palette", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Generator:");
        const char *generators[] = {"hellwal", "matugen"};
        ImGui::SameLine();
        ImGui::SetNextItemWidth(160.0f);
        ImGui::Combo("##gen_select", &m_generator_idx, generators, IM_ARRAYSIZE(generators));

        if (m_generator_idx == 0) // hellwal
        {
            ImGui::Separator();
            ImGui::Text("hellwal options");
            ImGui::Spacing();

            // image selector
            ImGui::Text("Image:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(300.0f);
            {
                char buf[1024];
                std::strncpy(buf, m_gen_image_path.c_str(), sizeof(buf));
                buf[sizeof(buf) - 1] = '\0';
                if (ImGui::InputText("##gen_image", buf, sizeof(buf)))
                {
                    m_gen_image_path = buf;
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("Browse##gen_image"))
            {
                std::string res = file_dialogs::open_file_dialog("Select Image", m_gen_image_path,
                                                                 {"png", "jpg", "jpeg", "bmp"});
                if (!res.empty())
                    m_gen_image_path = res;
            }

            ImGui::Checkbox("Neon mode", &m_gen_neon);

            ImGui::Text("Modes (can combine):");
            ImGui::Checkbox("Dark", &m_gen_dark);
            ImGui::SameLine();
            ImGui::Checkbox("Light", &m_gen_light);
            ImGui::SameLine();
            ImGui::Checkbox("Color", &m_gen_color);

            ImGui::SliderFloat("Dark offset", &m_gen_dark_offset, 0.0f, 1.0f);
            ImGui::SliderFloat("Bright offset", &m_gen_bright_offset, 0.0f, 1.0f);
            ImGui::Checkbox("Invert colors", &m_gen_invert);
            ImGui::SliderFloat("Gray scale", &m_gen_gray_scale, 0.0f, 1.0f);
        }

        if (m_generator_idx == 1) // matugen
        {
            ImGui::Separator();
            ImGui::Text("matugen options");
            ImGui::Spacing();

            ImGui::Text("Image:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(300.0f);
            {
                char buf[1024];
                std::strncpy(buf, m_gen_image_path.c_str(), sizeof(buf));
                buf[sizeof(buf) - 1] = '\0';
                if (ImGui::InputText("##gen_image", buf, sizeof(buf)))
                {
                    m_gen_image_path = buf;
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("Browse##gen_image"))
            {
                std::string res = file_dialogs::open_file_dialog("Select Image", m_gen_image_path,
                                                                 {"png", "jpg", "jpeg", "bmp"});
                if (!res.empty())
                    m_gen_image_path = res;
            }

            ImGui::Text("Mode:");
            ImGui::SameLine();
            const char *modes[] = {"dark", "light"};
            int mode_idx = (m_matugen_mode == "light") ? 1 : 0;
            ImGui::SetNextItemWidth(120.0f);
            ImGui::Combo("##matugen_mode", &mode_idx, modes, IM_ARRAYSIZE(modes));
            m_matugen_mode = (mode_idx == 1) ? "light" : "dark";

            ImGui::Text("Type:");
            ImGui::SameLine();
            const char *types[] = {"scheme-content",     "scheme-expressive", "scheme-fidelity",
                                   "scheme-fruit-salad", "scheme-monochrome", "scheme-neutral",
                                   "scheme-rainbow",     "scheme-tonal-spot"};
            int type_idx = 7; // default index for scheme-tonal-spot
            for (int i = 0; i < IM_ARRAYSIZE(types); ++i)
            {
                if (m_matugen_type == types[i])
                {
                    type_idx = i;
                    break;
                }
            }
            ImGui::SetNextItemWidth(260.0f);
            ImGui::Combo("##matugen_type", &type_idx, types, IM_ARRAYSIZE(types));
            m_matugen_type = types[type_idx];

            ImGui::SliderFloat("Contrast", &m_matugen_contrast, -1.0f, 1.0f);

            ImGui::SliderInt("Source Color Index", &m_matugen_source_color_index, 0, 3);

            ImGui::Spacing();
            ImGui::Checkbox("Use color (instead of image)", &m_matugen_use_color);
            if (m_matugen_use_color)
            {
                ImGui::Text("Color:");
                ImGui::SameLine();
                ImGui::ColorEdit3("##matugen_color", m_matugen_color_vec);
                // update hex string from vec
                int r = static_cast<int>(m_matugen_color_vec[0] * 255.0f + 0.5f);
                int g = static_cast<int>(m_matugen_color_vec[1] * 255.0f + 0.5f);
                int b = static_cast<int>(m_matugen_color_vec[2] * 255.0f + 0.5f);
                char hexbuf[8];
                std::snprintf(hexbuf, sizeof(hexbuf), "%02X%02X%02X", r, g, b);
                m_matugen_color_hex = hexbuf;
            }
        }

        ImGui::Separator();
        if (ImGui::Button("Generate", ImVec2(120, 0)))
        {
            try
            {
                if (m_generator_idx == 0)
                {
                    clrsync::core::hellwal_generator gen;
                    clrsync::core::hellwal_generator::options opts;
                    opts.neon = m_gen_neon;
                    opts.dark = m_gen_dark;
                    opts.light = m_gen_light;
                    opts.color = m_gen_color;
                    opts.dark_offset = m_gen_dark_offset;
                    opts.bright_offset = m_gen_bright_offset;
                    opts.invert = m_gen_invert;
                    opts.gray_scale = m_gen_gray_scale;

                    auto image_path = m_gen_image_path;
                    if (image_path.empty())
                    {
                        image_path = file_dialogs::open_file_dialog("Select Image", "",
                                                                    {"png", "jpg", "jpeg", "bmp"});
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
                else if (m_generator_idx == 1)
                {
                    clrsync::core::matugen_generator gen;
                    clrsync::core::matugen_generator::options opts;
                    opts.mode = m_matugen_mode;
                    opts.type = m_matugen_type;
                    opts.contrast = m_matugen_contrast;
                    opts.source_color_index = m_matugen_source_color_index;

                    auto image_path = m_gen_image_path;

                    clrsync::core::palette pal;
                    if (m_matugen_use_color)
                    {
                        // pass hex without '#' to generator
                        std::string hex = m_matugen_color_hex;
                        if (!hex.empty() && hex[0] == '#')
                            hex = hex.substr(1);
                        pal = gen.generate_from_color(hex, opts);
                        if (pal.name().empty())
                        {
                            pal.set_name(std::string("matugen:color:") + hex);
                        }
                    }
                    else
                    {
                        if (image_path.empty())
                        {
                            image_path = file_dialogs::open_file_dialog(
                                "Select Image", "", {"png", "jpg", "jpeg", "bmp"});
                        }
                        pal = gen.generate_from_image(image_path, opts);
                        if (pal.name().empty())
                        {
                            std::filesystem::path p(image_path);
                            pal.set_name(std::string("matugen:") + p.filename().string());
                        }
                    }
                    if (pal.name().empty())
                    {
                        std::filesystem::path p(image_path);
                        pal.set_name(std::string("matugen:") + p.filename().string());
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
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

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
        try
        {
            clrsync::core::hellwal_generator gen;
            auto pal = gen.generate_from_image(image_path);
            if (pal.name().empty())
            {
                std::filesystem::path p(image_path);
                pal.set_name(std::string("hellwal:") + p.filename().string());
            }
            m_controller.import_palette(pal);
            m_controller.select_palette(pal.name());
            apply_themes();
        }
        catch (const std::exception &e)
        {
            std::cerr << "Failed to generate palette: " << e.what() << std::endl;
        }
    });
    m_generate_dialog.set_path_browse_callback(
        [this](const std::string &current_path) -> std::string {
            return file_dialogs::open_file_dialog("Select Image", current_path,
                                                  {"png", "jpg", "jpeg", "bmp"});
        });

    m_action_buttons.add_button({" Save ", "Save current palette to file",
                                 [this]() { m_controller.save_current_palette(); }});

    m_action_buttons.add_button({" Delete ", "Delete current palette",
                                 [this]() { m_show_delete_confirmation = true; }, true, true});

    m_action_buttons.add_button({" Apply Theme ", "Apply current palette to all enabled templates",
                                 [this]() { m_controller.apply_current_theme(); }});

    m_action_buttons.set_spacing(16.0f);
}
