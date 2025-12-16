#include "template_editor.hpp"
#include "core/config/config.hpp"
#include "core/theme/theme_template.hpp"
#include "core/utils.hpp"
#include "imgui.h"
#include <filesystem>
#include <fstream>
#include <ranges>

template_editor::template_editor() : m_template_name("new_template")
{
    TextEditor::LanguageDefinition lang;
    lang.mName = "Template";

    lang.mCommentStart = "/*";
    lang.mCommentEnd = "*/";
    lang.mSingleLineComment = "#";

    lang.mTokenRegexStrings.push_back(std::make_pair<std::string, TextEditor::PaletteIndex>(
        "\\{[a-zA-Z_][a-zA-Z0-9_]*(\\.[a-zA-Z_][a-zA-Z0-9_]*)?\\}",
        TextEditor::PaletteIndex::KnownIdentifier));

    lang.mTokenRegexStrings.push_back(std::make_pair<std::string, TextEditor::PaletteIndex>(
        "\"([^\"]*)\"", TextEditor::PaletteIndex::String));
    lang.mTokenRegexStrings.push_back(std::make_pair<std::string, TextEditor::PaletteIndex>(
        "'([^']*)'", TextEditor::PaletteIndex::String));

    m_editor.SetLanguageDefinition(lang);
    m_editor.SetText("# Enter your template here\n# Use {color_key} for color variables\n# "
                     "Examples: {color.hex}, {color.rgb}, {color.r}\n\n");
    m_editor.SetShowWhitespaces(false);
}

void template_editor::apply_current_palette(const clrsync::core::palette &pal)
{
    auto colors = pal.colors();
    if (colors.empty())
        return;
    auto get_color_u32 = [&](const std::string &key, const std::string &fallback = "") -> uint32_t {
        auto it = colors.find(key);
        if (it == colors.end() && !fallback.empty())
        {
            it = colors.find(fallback);
        }

        if (it != colors.end())
        {
            const auto &col = it->second;
            const uint32_t hex = col.hex();
            // Convert from RRGGBBAA to AABBGGRR (ImGui format)
            const uint32_t r = (hex >> 24) & 0xFF;
            const uint32_t g = (hex >> 16) & 0xFF;
            const uint32_t b = (hex >> 8) & 0xFF;
            const uint32_t a = hex & 0xFF;
            return (a << 24) | (b << 16) | (g << 8) | r;
        }
        return 0xFFFFFFFF;
    };

    auto palette = m_editor.GetPalette();

    palette[int(TextEditor::PaletteIndex::Default)] = get_color_u32("editor_main", "foreground");
    palette[int(TextEditor::PaletteIndex::Keyword)] = get_color_u32("editor_command", "accent");
    palette[int(TextEditor::PaletteIndex::Number)] = get_color_u32("editor_warning", "warning");
    palette[int(TextEditor::PaletteIndex::String)] = get_color_u32("editor_string", "success");
    palette[int(TextEditor::PaletteIndex::CharLiteral)] = get_color_u32("editor_string", "success");
    palette[int(TextEditor::PaletteIndex::Punctuation)] =
        get_color_u32("editor_main", "foreground");
    palette[int(TextEditor::PaletteIndex::Preprocessor)] =
        get_color_u32("editor_emphasis", "accent");
    palette[int(TextEditor::PaletteIndex::Identifier)] = get_color_u32("editor_main", "foreground");
    palette[int(TextEditor::PaletteIndex::KnownIdentifier)] = get_color_u32("editor_link", "info");
    palette[int(TextEditor::PaletteIndex::PreprocIdentifier)] =
        get_color_u32("editor_link", "info");

    palette[int(TextEditor::PaletteIndex::Comment)] =
        get_color_u32("editor_comment", "editor_inactive");
    palette[int(TextEditor::PaletteIndex::MultiLineComment)] =
        get_color_u32("editor_comment", "editor_inactive");

    palette[int(TextEditor::PaletteIndex::Background)] =
        get_color_u32("editor_background", "background");
    palette[int(TextEditor::PaletteIndex::Cursor)] = get_color_u32("cursor", "accent");

    palette[int(TextEditor::PaletteIndex::Selection)] =
        get_color_u32("editor_selected", "surface_variant");
    palette[int(TextEditor::PaletteIndex::ErrorMarker)] = get_color_u32("editor_error", "error");
    palette[int(TextEditor::PaletteIndex::Breakpoint)] = get_color_u32("editor_error", "error");

    palette[int(TextEditor::PaletteIndex::LineNumber)] =
        get_color_u32("editor_line_number", "editor_inactive");
    palette[int(TextEditor::PaletteIndex::CurrentLineFill)] = get_color_u32("surface_variant");
    palette[int(TextEditor::PaletteIndex::CurrentLineFillInactive)] = get_color_u32("surface");

    palette[int(TextEditor::PaletteIndex::CurrentLineEdge)] =
        get_color_u32("border_focused", "border");

    m_editor.SetPalette(palette);
}

void template_editor::render()
{
    ImGui::Begin("Templates");

    render_controls();
    ImGui::Separator();

    const float panel_width = ImGui::GetContentRegionAvail().x;
    constexpr float left_panel_width = 200.0f;
    const float right_panel_width = panel_width - left_panel_width - 10;

    ImGui::BeginChild("TemplateList", ImVec2(left_panel_width, 0), true);
    render_template_list();
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild("EditorPanel", ImVec2(right_panel_width, 0), false);
    render_editor();
    ImGui::EndChild();

    if (m_show_delete_confirmation)
    {
        ImGui::OpenPopup("Delete Template?");
        m_show_delete_confirmation = false;
    }

    if (ImGui::BeginPopupModal("Delete Template?", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Are you sure you want to delete '%s'?", m_template_name.c_str());
        ImGui::Text("This action cannot be undone.");
        ImGui::Separator();

        if (ImGui::Button("Delete", ImVec2(120, 0)))
        {
            bool success = m_template_controller.remove_template(m_template_name);
            if (success)
            {
                new_template();
                refresh_templates();
            }
            else
            {
                m_validation_error = "Failed to delete template";
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

    ImGui::End();
}

void template_editor::render_controls()
{
    if (ImGui::Button("New Template"))
    {
        new_template();
    }

    if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) &&
        ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_S))
    {
        save_template();
    }

    ImGui::SameLine();
    if (ImGui::Button("Save"))
    {
        save_template();
    }

    if (m_is_editing_existing)
    {
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.6f, 0.1f, 0.1f, 1.0f));
        if (ImGui::Button("Delete"))
        {
            delete_template();
        }
        ImGui::PopStyleColor(3);
    }

    ImGui::SameLine();
    ImGui::Text("Template Name:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(150.0f);
    char name_buf[256] = {0};
    snprintf(name_buf, sizeof(name_buf), "%s", m_template_name.c_str());
    if (ImGui::InputText("##template_name", name_buf, sizeof(name_buf)))
    {
        m_template_name = name_buf;
        if (!m_template_name.empty())
        {
            m_validation_error = "";
        }
    }

    ImGui::SameLine();
    if (ImGui::Checkbox("Enabled", &m_enabled))
    {
        if (m_is_editing_existing)
        {
            m_template_controller.set_template_enabled(m_template_name, m_enabled);
        }
    }

    ImGui::Text("Input Path:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(-FLT_MIN);
    char input_path_buf[512] = {0};
    snprintf(input_path_buf, sizeof(input_path_buf), "%s", m_input_path.c_str());
    if (ImGui::InputText("##input_path", input_path_buf, sizeof(input_path_buf)))
    {
        m_input_path = input_path_buf;
        if (!m_input_path.empty())
        {
            m_validation_error = "";
        }
        if (m_is_editing_existing)
        {
            m_template_controller.set_template_input_path(m_template_name, m_input_path);
        }
    }

    ImGui::Text("Output Path:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(-FLT_MIN);
    char path_buf[512] = {0};
    snprintf(path_buf, sizeof(path_buf), "%s", m_output_path.c_str());
    if (ImGui::InputText("##output_path", path_buf, sizeof(path_buf)))
    {
        m_output_path = path_buf;
        if (!m_output_path.empty())
        {
            m_validation_error = "";
        }
        if (m_is_editing_existing)
        {
            m_template_controller.set_template_output_path(m_template_name, m_output_path);
        }
    }

    ImGui::Text("Reload Command:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(-FLT_MIN);
    char reload_buf[512] = {0};
    snprintf(reload_buf, sizeof(reload_buf), "%s", m_reload_command.c_str());
    if (ImGui::InputText("##reload_cmd", reload_buf, sizeof(reload_buf)))
    {
        m_reload_command = reload_buf;
        if (m_is_editing_existing)
        {
            m_template_controller.set_template_reload_command(m_template_name, m_reload_command);
        }
    }

    if (!m_validation_error.empty())
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
        ImGui::TextWrapped("%s", m_validation_error.c_str());
        ImGui::PopStyleColor();
    }
}

void template_editor::render_editor()
{
    if (!m_is_editing_existing)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.8f, 0.4f, 1.0f));
        ImGui::Text("New Template");
        ImGui::PopStyleColor();
    }
    else
    {
        ImGui::Text("%s", m_template_name.c_str());
        auto trim_right = [](const std::string &s) -> std::string {
            size_t end = s.find_last_not_of("\r\n");
            return (end == std::string::npos) ? "" : s.substr(0, end + 1);
        };

        std::string current_content = trim_right(m_editor.GetText());
        std::string saved_content = trim_right(m_saved_content);

        m_has_unsaved_changes = (current_content != saved_content);
        if (m_has_unsaved_changes)
        {
            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.6f, 0.2f, 1.0f));
            ImGui::Text("●");
            ImGui::PopStyleColor();
        }
    }

    ImGui::Separator();

    m_editor.Render("##TemplateEditor", ImVec2(0, 0), true);
}

void template_editor::render_template_list()
{
    ImGui::Text("Templates");
    ImGui::Separator();

    if (!m_is_editing_existing)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.8f, 0.4f, 1.0f));
        ImGui::Selectable("* New Template *", true);
        ImGui::PopStyleColor();
        ImGui::Separator();
    }

    const auto &templates = m_template_controller.templates();

    for (const auto &key : templates | std::views::keys)
    {
        const bool selected = (m_template_name == key && m_is_editing_existing);
        if (ImGui::Selectable(key.c_str(), selected))
        {
            load_template(key);
        }
    }
}

bool template_editor::is_valid_path(const std::string &path)
{
    const std::string invalid_chars = "<>|\"";
    for (const char c : invalid_chars)
    {
        if (path.find(c) != std::string::npos)
        {
            return false;
        }
    }

    if (path.find_first_not_of(" \t\n\r./\\") == std::string::npos)
    {
        return false;
    }

    try
    {
        std::filesystem::path fs_path(path);

        const auto parent = fs_path.parent_path();

        if (parent.empty())
        {
            return false;
        }

        if (!parent.empty() && !std::filesystem::exists(parent))
        {
            if (parent.string().find_first_of("<>|\"") != std::string::npos)
            {
                return false;
            }
        }

        const auto filename = fs_path.filename().string();
        if (filename.empty() || filename == "." || filename == "..")
        {
            return false;
        }

        return true;
    }
    catch (...)
    {
        return false;
    }
}

void template_editor::save_template()
{
    std::string trimmed_name = m_template_name;
    trimmed_name.erase(0, trimmed_name.find_first_not_of(" \t\n\r"));
    trimmed_name.erase(trimmed_name.find_last_not_of(" \t\n\r") + 1);

    std::string trimmed_input_path = m_input_path;
    trimmed_input_path.erase(0, trimmed_input_path.find_first_not_of(" \t\n\r"));
    trimmed_input_path.erase(trimmed_input_path.find_last_not_of(" \t\n\r") + 1);

    std::string trimmed_path = m_output_path;
    trimmed_path.erase(0, trimmed_path.find_first_not_of(" \t\n\r"));
    trimmed_path.erase(trimmed_path.find_last_not_of(" \t\n\r") + 1);

    if (trimmed_name.empty())
    {
        m_validation_error = "Error: Template name cannot be empty!";
        return;
    }

    if (trimmed_input_path.empty())
    {
        m_validation_error = "Error: Input path cannot be empty!";
        return;
    }

    if (trimmed_path.empty())
    {
        m_validation_error = "Error: Output path cannot be empty!";
        return;
    }

    if (!is_valid_path(trimmed_path))
    {
        m_validation_error =
            "Error: Output path is invalid! Must be a valid file path with directory.";
        return;
    }

    m_validation_error = "";

    auto &cfg = clrsync::core::config::instance();

    std::filesystem::path template_file = clrsync::core::normalize_path(trimmed_input_path);
    
    // Ensure the parent directory exists
    auto parent_dir = template_file.parent_path();
    if (!parent_dir.empty() && !std::filesystem::exists(parent_dir))
    {
        try
        {
            std::filesystem::create_directories(parent_dir);
        }
        catch (const std::exception& e)
        {
            m_validation_error = "Error: Could not create directory for input path";
            return;
        }
    }

    std::string template_content = m_editor.GetText();

    std::ofstream out(template_file);
    if (!out.is_open())
    {
        m_validation_error = "Failed to write template file";
        return;
    }
    
    out << template_content;
    out.close();

    clrsync::core::theme_template tmpl(trimmed_name, template_file.string(), trimmed_path);
    tmpl.set_reload_command(m_reload_command);
    tmpl.set_enabled(m_enabled);

    auto result = cfg.update_template(trimmed_name, tmpl);
    if (!result)
    {
        m_validation_error = "Error saving template: " + result.error().description();
        return;
    }

    m_template_name = trimmed_name;
    m_input_path = trimmed_input_path;
    m_output_path = trimmed_path;
    m_is_editing_existing = true;
    m_saved_content = m_editor.GetText();
    m_has_unsaved_changes = false;

    refresh_templates();
}

void template_editor::load_template(const std::string &name)
{
    const auto &templates = m_template_controller.templates();
    auto it = templates.find(name);

    if (it != templates.end())
    {
        const auto &tmpl = it->second;
        m_template_name = name;
        m_input_path = tmpl.template_path();
        m_output_path = tmpl.output_path();
        m_reload_command = tmpl.reload_command();
        m_enabled = tmpl.enabled();
        m_is_editing_existing = true;
        m_validation_error = "";

        std::ifstream in(tmpl.template_path());
        if (in.is_open())
        {
            std::string content;
            std::string line;
            while (std::getline(in, line))
            {
                content += line + "\n";
            }
            in.close();

            m_editor.SetText(content);
            m_saved_content = content;
            m_has_unsaved_changes = false;
        }
        else
        {
            m_validation_error = "Error loading template: Failed to open file";
        }
    }
}

void template_editor::new_template()
{
    m_template_name = "new_template";
    std::string default_content =
        "# Enter your template here\n# Use {color_key} for color variables\n# "
        "Examples: {color.hex}, {color.rgb}, {color.r}\n\n";
    m_editor.SetText(default_content);
    m_saved_content = default_content;
    m_input_path = "";
    m_output_path = "";
    m_reload_command = "";
    m_enabled = true;
    m_is_editing_existing = false;
    m_validation_error = "";
    m_has_unsaved_changes = false;
}

void template_editor::delete_template()
{
    if (!m_is_editing_existing || m_template_name.empty())
        return;
    
    m_show_delete_confirmation = true;
}

void template_editor::refresh_templates()
{
    m_template_controller.refresh();
}