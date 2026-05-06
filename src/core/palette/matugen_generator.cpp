#include "matugen_generator.hpp"

#include "core/palette/color.hpp"
#include "core/palette/json_utils.hpp"

#include <array>
#include <cstdio>
#include <filesystem>
#include <string>
#include <unordered_map>

#ifdef _WIN32
#define popen _popen
#define pclose _pclose
#endif

namespace clrsync::core
{
using json = json_utils::json;

static std::string run_command_capture_output(const std::string &cmd)
{
    std::array<char, 4096> buffer;
    std::string result;
    FILE *pipe = popen(cmd.c_str(), "r");
    if (!pipe)
        return {};
    while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe) != nullptr)
    {
        result += buffer.data();
    }
    int rc = pclose(pipe);
    (void)rc;
    return result;
}

static palette parse_matugen_output(const std::string &out, const matugen_generator::options &opts,
                                    const std::string &pal_name, const std::string &file_path)
{
    if (out.empty())
        return {};

    json doc;
    if (!json_utils::parse_json_output(out, doc))
        return {};

    std::unordered_map<std::string, std::string> clrsync_to_matu = {
        {"accent", "primary"},
        {"accent_secondary", "secondary"},
        {"background", "background"},
        {"foreground", "on_surface"},
        {"on_background", "on_background"},
        
        {"surface", "surface_container"},
        {"on_surface", "on_surface"},
        {"surface_variant", "surface_variant"},
        {"on_surface_variant", "on_surface_variant"},
        
        {"border", "outline_variant"},
        {"border_focused", "outline"},
        {"cursor", "on_surface"},
        
        {"success", "primary"},
        {"on_success", "on_primary"},
        {"info", "tertiary"},
        {"on_info", "on_tertiary"},
        {"warning", "secondary"},
        {"on_warning", "on_secondary"},
        {"error", "error"},
        {"on_error", "on_error"},
        
        // Editor - Basic
        {"editor_background", "background"},
        {"editor_foreground", "on_surface"},
        {"editor_line_highlight", "surface_container"},
        {"editor_selection", "primary_container"},
        {"editor_selection_inactive", "surface_container_low"},
        {"editor_cursor", "on_surface"},
        {"editor_whitespace", "outline_variant"},

        // Editor - Gutter
        {"editor_gutter_background", "background"},
        {"editor_gutter_foreground", "outline"},
        {"editor_line_number", "outline"},
        {"editor_line_number_active", "on_surface"},

        // Editor - Syntax
        {"editor_comment", "outline"},
        {"editor_string", "tertiary"},
        {"editor_number", "secondary"},
        {"editor_boolean", "secondary"},
        {"editor_keyword", "primary"},
        {"editor_operator", "on_surface"},
        {"editor_function", "tertiary_container"},
        {"editor_variable", "on_surface"},
        {"editor_parameter", "tertiary"},
        {"editor_property", "tertiary"},
        {"editor_constant", "secondary"},
        {"editor_type", "primary"},
        {"editor_class", "primary"},
        {"editor_interface", "primary"},
        {"editor_enum", "primary"},
        {"editor_namespace", "primary_container"},
        {"editor_attribute", "tertiary_container"},
        {"editor_decorator", "tertiary_container"},
        {"editor_tag", "error"},
        {"editor_punctuation", "on_surface"},
        {"editor_link", "primary_container"},
        {"editor_regex", "secondary_container"},
        {"editor_escape_character", "secondary"},

        // Editor - Diagnostics
        {"editor_invalid", "error"},
        {"editor_error", "error"},
        {"editor_error_background", "error_container"},
        {"editor_warning", "secondary"},
        {"editor_warning_background", "secondary_container"},
        {"editor_info", "tertiary"},
        {"editor_info_background", "tertiary_container"},
        {"editor_hint", "primary"},
        {"editor_hint_background", "primary_container"},

        // Editor - UI Elements
        {"editor_active_line_border", "outline_variant"},
        {"editor_indent_guide", "outline_variant"},
        {"editor_indent_guide_active", "outline"},
        {"editor_bracket_match", "primary"},
        {"editor_search_match", "tertiary_container"},
        {"editor_search_match_active", "tertiary"},
        {"editor_find_range_highlight", "tertiary_container"},

        // Editor - Diff
        {"editor_deleted", "error"},
        {"editor_inserted", "primary"},
        {"editor_modified", "secondary"},
        {"editor_ignored", "outline_variant"},
        {"editor_folded_background", "surface_container"},
        
        {"base00", "background"},
        {"base01", "base16.base01"},
        {"base02", "base16.base02"},
        {"base03", "base16.base03"},
        {"base04", "base16.base04"},
        {"base05", "base16.base05"},
        {"base06", "base16.base06"},
        {"base07", "base16.base07"},
        {"base08", "base16.base08"},
        {"base09", "base16.base09"},
        {"base0A", "base16.base0A"},
        {"base0B", "base16.base0B"},
        {"base0C", "base16.base0C"},
        {"base0D", "base16.base0D"},
        {"base0E", "base16.base0E"},
        {"base0F", "base16.base0F"},
    };

    palette pal;
    pal.set_name(pal_name);
    pal.set_file_path(file_path);

    for (const auto &[clrsync_key, matu_key] : clrsync_to_matu)
    {
        std::string hex;
        if (matu_key.find("base16.") == 0)
        {
            std::string base_key = matu_key.substr(7);
            if (doc.contains("base16") && doc["base16"].contains(base_key) &&
                doc["base16"][base_key].contains(opts.mode) &&
                doc["base16"][base_key][opts.mode].contains("color"))
            {
                hex = doc["base16"][base_key][opts.mode]["color"];
            }
        }
        else
        {
            if (doc.contains("colors") && doc["colors"].contains(matu_key) &&
                doc["colors"][matu_key].contains(opts.mode) &&
                doc["colors"][matu_key][opts.mode].contains("color"))
            {
                hex = doc["colors"][matu_key][opts.mode]["color"];
            }
        }
        if (!hex.empty())
        {
            hex = json_utils::normalize_hex_string(hex);
            if (!hex.empty())
            {
                color col;
                col.from_hex_string(hex);
                pal.set_color(clrsync_key, col);
            }
        }
    }

    return pal;
}

palette matugen_generator::generate_from_image(const std::string &image_path)
{
    options default_opts{};
    return generate_from_image(image_path, default_opts);
}

palette matugen_generator::generate_from_image(const std::string &image_path, const options &opts)
{
    std::filesystem::path p(image_path);
    std::string cmd = "matugen image '" + image_path + "'";
    if (!opts.type.empty())
        cmd += " --type '" + opts.type + "'";
    if (!opts.mode.empty())
        cmd += " --mode " + opts.mode;
    if (opts.contrast != 0.0f)
        cmd += " --contrast " + std::to_string(opts.contrast);
    cmd += " --json hex --dry-run";
    if (opts.source_color_index >= 0 && opts.source_color_index <= 3)
    {
        cmd += " --source-color-index ";
        cmd += std::to_string(opts.source_color_index);
    }


    std::string out = run_command_capture_output(cmd);
    if (out.empty())
        return {};
    return parse_matugen_output(out, opts, std::string("matugen:") + p.filename().string(),
                                image_path);
}

palette matugen_generator::generate_from_color(const std::string &color_hex)
{
    options default_opts{};
    return generate_from_color(color_hex, default_opts);
}

palette matugen_generator::generate_from_color(const std::string &color_hex, const options &opts)
{
    std::string c = color_hex;
    if (!c.empty() && c[0] == '#')
        c = c.substr(1);
    std::string cmd = "matugen color hex '" + c + "'";
    if (!opts.type.empty())
        cmd += " --type '" + opts.type + "'";
    if (!opts.mode.empty())
        cmd += " --mode " + opts.mode;
    if (opts.contrast != 0.0f)
        cmd += " --contrast " + std::to_string(opts.contrast);
    cmd += " --json hex --dry-run";


    std::string out = run_command_capture_output(cmd);
    if (out.empty())
        return {};

    return parse_matugen_output(out, opts, std::string("matugen:color:") + color_hex, color_hex);
}
} // namespace clrsync::core
