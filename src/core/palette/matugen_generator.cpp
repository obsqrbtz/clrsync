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

static const json *find_object_member(const json &node, const std::string &key)
{
    if (node.is_object())
    {
        auto it = node.find(key);
        if (it != node.end() && it->is_object())
            return &(*it);
        for (const auto &entry : node.items())
        {
            if (const json *found = find_object_member(entry.value(), key))
                return found;
        }
    }
    else if (node.is_array())
    {
        for (const auto &entry : node)
        {
            if (const json *found = find_object_member(entry, key))
                return found;
        }
    }
    return nullptr;
}

static void collect_hex_values(const json &node, std::unordered_map<std::string, std::string> &out)
{
    if (node.is_object())
    {
        for (const auto &entry : node.items())
        {
            const std::string key = entry.key();
            const json &value = entry.value();
            if (value.is_string())
            {
                const std::string hex = json_utils::normalize_hex_string(value.get<std::string>());
                if (!hex.empty())
                {
                    std::string normalized_key = key;
                    for (auto &c : normalized_key)
                        if (c == '-')
                            c = '_';
                    out[normalized_key] = hex;
                }
            }

            collect_hex_values(value, out);
        }
    }
    else if (node.is_array())
    {
        for (const auto &value : node)
            collect_hex_values(value, out);
    }
}

static palette parse_matugen_output(const std::string &out, const matugen_generator::options &opts,
                                    const std::string &pal_name, const std::string &file_path)
{
    if (out.empty())
        return {};

    json doc;
    if (!json_utils::parse_json_output(out, doc))
        return {};

    const json *section = nullptr;
    if (const json *colors = find_object_member(doc, "colors"))
    {
        auto it = colors->find(opts.mode);
        if (it != colors->end() && it->is_object())
            section = &(*it);
    }
    if (!section)
    {
        auto it = doc.find(opts.mode);
        if (it != doc.end() && it->is_object())
            section = &(*it);
    }
    if (!section)
        section = &doc;

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
        {"base01", "surface_container_lowest"},
        {"base02", "surface_container_low"},
        {"base03", "outline_variant"},
        {"base04", "on_surface_variant"},
        {"base05", "on_surface"},
        {"base06", "inverse_on_surface"},
        {"base07", "surface_bright"},
        {"base08", "error"},
        {"base09", "tertiary"},
        {"base0A", "secondary"},
        {"base0B", "primary"},
        {"base0C", "tertiary_container"},
        {"base0D", "primary_container"},
        {"base0E", "secondary_container"},
        {"base0F", "on_primary_container"},
    };

    std::unordered_map<std::string, std::string> matu_kv_map;
    collect_hex_values(*section, matu_kv_map);

    palette pal;
    pal.set_name(pal_name);
    pal.set_file_path(file_path);

    for (const auto &[clrsync_key, matu_key] : clrsync_to_matu)
    {
        auto matu_it = matu_kv_map.find(matu_key);
        if (matu_it == matu_kv_map.end())
            continue;
        color col;
        col.from_hex_string(matu_it->second);
        pal.set_color(clrsync_key, col);
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
