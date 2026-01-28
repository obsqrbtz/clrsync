#include "hellwal_generator.hpp"

#include "core/palette/color.hpp"

#include <array>
#include <cstdio>
#include <filesystem>
#include <regex>
#include <string>

namespace clrsync::core
{
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

palette hellwal_generator::generate_from_image(const std::string &image_path)
{
    options default_opts{};
    return generate_from_image(image_path, default_opts);
}

palette hellwal_generator::generate_from_image(const std::string &image_path, const options &opts)
{
    palette pal;

    std::filesystem::path p(image_path);
    pal.set_name("hellwal:" + p.filename().string());
    pal.set_file_path(image_path);

    std::string cmd = "hellwal -i '" + image_path + "' --json";
    if (opts.neon)
        cmd += " --neon-mode";
    if (opts.dark)
        cmd += " --dark";
    if (opts.light)
        cmd += " --light";
    if (opts.color)
        cmd += " --color";
    if (opts.dark_offset > 0.0f)
        cmd += " --dark-offset " + std::to_string(opts.dark_offset);
    if (opts.bright_offset > 0.0f)
        cmd += " --bright-offset " + std::to_string(opts.bright_offset);
    if (opts.invert)
        cmd += " --invert";
    if (opts.gray_scale > 0.0f)
        cmd += " --gray-scale " + std::to_string(opts.gray_scale);

    std::string out = run_command_capture_output(cmd);
    if (out.empty())
        return {};

    std::regex special_re(
        "\"(background|foreground|cursor|border)\"\\s*:\\s*\"(#[0-9A-Fa-f]{6,8})\"");
    for (std::sregex_iterator it(out.begin(), out.end(), special_re), end; it != end; ++it)
    {
        std::smatch m = *it;
        std::string key = m[1].str();
        std::string hex = m[2].str();
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

    std::regex color_re("\"color(\\d{1,2})\"\\s*:\\s*\"(#[0-9A-Fa-f]{6,8})\"");
    for (std::sregex_iterator it(out.begin(), out.end(), color_re), end; it != end; ++it)
    {
        std::smatch m = *it;
        int idx = std::stoi(m[1].str());
        if (idx < 0 || idx > 15)
            continue;
        std::string hex = m[2].str();

        std::string key = "base0";
        if (idx < 10)
            key += std::to_string(idx);
        else
            key += static_cast<char>('A' + (idx - 10));

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

    auto get_color_by_index = [&](int idx) -> const color & {
        std::string key = "base0";
        if (idx < 10)
            key += std::to_string(idx);
        else
            key += static_cast<char>('A' + (idx - 10));
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

    pal.set_color("editor_background", get_color_by_index(0));
    pal.set_color("editor_main", get_color_by_index(7));
    pal.set_color("editor_comment", get_color_by_index(8));
    pal.set_color("editor_disabled", get_color_by_index(8));
    pal.set_color("editor_inactive", get_color_by_index(8));
    pal.set_color("editor_string", get_color_by_index(2));
    pal.set_color("editor_command", get_color_by_index(5));
    pal.set_color("editor_emphasis", get_color_by_index(11));
    pal.set_color("editor_link", get_color_by_index(4));
    pal.set_color("editor_line_number", get_color_by_index(8));
    pal.set_color("editor_selected", get_color_by_index(8));
    pal.set_color("editor_selection_inactive", get_color_by_index(8));
    pal.set_color("editor_error", get_color_by_index(1));
    pal.set_color("editor_warning", get_color_by_index(3));
    pal.set_color("editor_success", get_color_by_index(2));

    return pal;
}

} // namespace clrsync::core
