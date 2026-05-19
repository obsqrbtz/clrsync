#include "hellwal_generator.hpp"

#include "core/common/process.hpp"
#include "core/palette/color.hpp"
#include "core/palette/json_utils.hpp"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <string>

namespace clrsync::core
{
using json = json_utils::json;

static void collect_palette_colors(const json &node, palette &pal)
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
                    const bool is_special_key = key == "background" || key == "foreground" ||
                                                key == "cursor" || key == "border";
                    const bool is_color_key =
                        key.size() >= 6 && key.rfind("color", 0) == 0 &&
                        key.size() <= 7 &&
                        std::all_of(key.begin() + 5, key.end(), [](unsigned char c) {
                            return std::isdigit(c) != 0;
                        });

                    if (is_special_key || is_color_key)
                    {
                        try
                        {
                            color col;
                            col.from_hex_string(hex);
                            pal.set_color(key, col);
                        }
                        catch (...)
                        {
                        }
                    }
                }
            }

            collect_palette_colors(value, pal);
        }
    }
    else if (node.is_array())
    {
        for (const auto &value : node)
            collect_palette_colors(value, pal);
    }
}

palette hellwal_generator::generate_from_image(const std::string &image_path)
{
    options default_opts{};
    return generate_from_image(image_path, default_opts);
}

palette hellwal_generator::generate_from_image(const std::string &image_path, const options &opts)
{
    ensure_supported("hellwal");

    palette pal;

    std::filesystem::path p(image_path);
    pal.set_name("hellwal:" + p.filename().string());
    pal.set_file_path(image_path);

    std::vector<std::string> args = {"hellwal", "-i", image_path, "--json"};
    if (opts.neon)
        args.push_back("--neon-mode");
    if (opts.dark)
        args.push_back("--dark");
    if (opts.light)
        args.push_back("--light");
    if (opts.color)
        args.push_back("--color");
    if (opts.dark_offset > 0.0f)
    {
        args.push_back("--dark-offset");
        args.push_back(std::to_string(opts.dark_offset));
    }
    if (opts.bright_offset > 0.0f)
    {
        args.push_back("--bright-offset");
        args.push_back(std::to_string(opts.bright_offset));
    }
    if (opts.invert)
        args.push_back("--invert");
    if (opts.gray_scale > 0.0f)
    {
        args.push_back("--gray-scale");
        args.push_back(std::to_string(opts.gray_scale));
    }

    std::string out = run_process_capture_output(args);
    if (out.empty())
        return {};

    json doc;
    if (json_utils::parse_json_output(out, doc))
        collect_palette_colors(doc, pal);

    auto get_color_by_index = [&](int idx) -> const color & {
        std::string key = "color" + std::to_string(idx);
        return pal.get_color(key);
    };

    pal.set_color("base00", get_color_by_index(0));
    pal.set_color("base01", get_color_by_index(8));
    pal.set_color("base02", get_color_by_index(8));
    pal.set_color("base03", get_color_by_index(8));
    pal.set_color("base04", get_color_by_index(7));
    pal.set_color("base05", get_color_by_index(7));
    pal.set_color("base06", get_color_by_index(15));
    pal.set_color("base07", get_color_by_index(15));
    pal.set_color("base08", get_color_by_index(1));
    pal.set_color("base09", get_color_by_index(9));
    pal.set_color("base0A", get_color_by_index(3));
    pal.set_color("base0B", get_color_by_index(2));
    pal.set_color("base0C", get_color_by_index(6));
    pal.set_color("base0D", get_color_by_index(4));
    pal.set_color("base0E", get_color_by_index(5));
    pal.set_color("base0F", get_color_by_index(11));

    pal.set_color("accent", get_color_by_index(4));
    pal.set_color("accent_secondary", get_color_by_index(6));
    
    pal.set_color("border", get_color_by_index(8));
    pal.set_color("border_focused", get_color_by_index(4));

    pal.set_color("error", get_color_by_index(1));
    pal.set_color("warning", get_color_by_index(3));
    pal.set_color("success", get_color_by_index(2));
    pal.set_color("info", get_color_by_index(4));

    pal.set_color("on_error", get_color_by_index(0));
    pal.set_color("on_warning", get_color_by_index(0));
    pal.set_color("on_success", get_color_by_index(0));
    pal.set_color("on_info", get_color_by_index(0));

    pal.set_color("surface", get_color_by_index(0));
    pal.set_color("surface_variant", get_color_by_index(8));
    pal.set_color("on_surface", get_color_by_index(7));
    pal.set_color("on_surface_variant", get_color_by_index(7));
    pal.set_color("on_background", get_color_by_index(7));

    // Editor - Basic
    pal.set_color("editor_background", get_color_by_index(0));
    pal.set_color("editor_foreground", get_color_by_index(7));
    pal.set_color("editor_line_highlight", get_color_by_index(8));
    pal.set_color("editor_selection", get_color_by_index(8));
    pal.set_color("editor_selection_inactive", get_color_by_index(8));
    pal.set_color("editor_cursor", get_color_by_index(7));
    pal.set_color("editor_whitespace", get_color_by_index(8));

    // Editor - Gutter
    pal.set_color("editor_gutter_background", get_color_by_index(0));
    pal.set_color("editor_gutter_foreground", get_color_by_index(8));
    pal.set_color("editor_line_number", get_color_by_index(8));
    pal.set_color("editor_line_number_active", get_color_by_index(7));

    // Editor - Syntax
    pal.set_color("editor_comment", get_color_by_index(8));
    pal.set_color("editor_string", get_color_by_index(2));
    pal.set_color("editor_number", get_color_by_index(3));
    pal.set_color("editor_boolean", get_color_by_index(3));
    pal.set_color("editor_keyword", get_color_by_index(5));
    pal.set_color("editor_operator", get_color_by_index(7));
    pal.set_color("editor_function", get_color_by_index(11));
    pal.set_color("editor_variable", get_color_by_index(7));
    pal.set_color("editor_parameter", get_color_by_index(4));
    pal.set_color("editor_property", get_color_by_index(2));
    pal.set_color("editor_constant", get_color_by_index(3));
    pal.set_color("editor_type", get_color_by_index(6));
    pal.set_color("editor_class", get_color_by_index(6));
    pal.set_color("editor_interface", get_color_by_index(6));
    pal.set_color("editor_enum", get_color_by_index(6));
    pal.set_color("editor_namespace", get_color_by_index(4));
    pal.set_color("editor_attribute", get_color_by_index(11));
    pal.set_color("editor_decorator", get_color_by_index(11));
    pal.set_color("editor_tag", get_color_by_index(1));
    pal.set_color("editor_punctuation", get_color_by_index(7));
    pal.set_color("editor_link", get_color_by_index(4));
    pal.set_color("editor_regex", get_color_by_index(5));
    pal.set_color("editor_escape_character", get_color_by_index(3));

    // Editor - Diagnostics
    pal.set_color("editor_invalid", get_color_by_index(1));
    pal.set_color("editor_error", get_color_by_index(1));
    pal.set_color("editor_error_background", get_color_by_index(0));
    pal.set_color("editor_warning", get_color_by_index(3));
    pal.set_color("editor_warning_background", get_color_by_index(0));
    pal.set_color("editor_info", get_color_by_index(4));
    pal.set_color("editor_info_background", get_color_by_index(0));
    pal.set_color("editor_hint", get_color_by_index(2));
    pal.set_color("editor_hint_background", get_color_by_index(0));

    // Editor - UI Elements
    pal.set_color("editor_active_line_border", get_color_by_index(8));
    pal.set_color("editor_indent_guide", get_color_by_index(8));
    pal.set_color("editor_indent_guide_active", get_color_by_index(7));
    pal.set_color("editor_bracket_match", get_color_by_index(6));
    pal.set_color("editor_search_match", get_color_by_index(3));
    pal.set_color("editor_search_match_active", get_color_by_index(3));
    pal.set_color("editor_find_range_highlight", get_color_by_index(3));

    // Editor - Diff
    pal.set_color("editor_deleted", get_color_by_index(1));
    pal.set_color("editor_inserted", get_color_by_index(2));
    pal.set_color("editor_modified", get_color_by_index(3));
    pal.set_color("editor_ignored", get_color_by_index(8));
    pal.set_color("editor_folded_background", get_color_by_index(8));

    return pal;
}

} // namespace clrsync::core
