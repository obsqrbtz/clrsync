#include "matugen_generator.hpp"

#include "core/palette/color.hpp"

#include <array>
#include <cstdio>
#include <filesystem>
#include <regex>
#include <string>
#include <unordered_map>

#ifdef _WIN32
#define popen _popen
#define pclose _pclose
#endif

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

static palette parse_matugen_output(const std::string &out, const matugen_generator::options &opts,
                                    const std::string &pal_name, const std::string &file_path)
{
    if (out.empty())
        return {};

    auto extract_json_object = [&](const std::string &s,
                                   const std::string &obj_key) -> std::string {
        std::regex re("\"" + obj_key + "\"\\s*:\\s*\\{");
        std::smatch m;
        if (!std::regex_search(s, m, re))
            return {};
        size_t open_pos = s.find('{', m.position(0));
        if (open_pos == std::string::npos)
            return {};
        size_t i = open_pos + 1;
        int depth = 1;
        for (; i < s.size(); ++i)
        {
            if (s[i] == '{')
                ++depth;
            else if (s[i] == '}')
            {
                --depth;
                if (depth == 0)
                    break;
            }
        }
        if (depth != 0)
            return {};
        return s.substr(open_pos + 1, i - open_pos - 1);
    };

    std::string mode_section = extract_json_object(out, "colors");
    std::string target_section;
    if (!mode_section.empty())
    {
        std::string wrapped = std::string("{") + mode_section + std::string("}");
        target_section = extract_json_object(wrapped, opts.mode);
    }
    if (target_section.empty())
    {
        target_section = extract_json_object(out, opts.mode);
    }
    const std::string &parse_src = (target_section.empty() ? out : target_section);

    std::regex kv_re("\"([a-zA-Z0-9_-]+)\"\\s*:\\s*\"(#?[A-Fa-f0-9]{6,8})\"");

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
        
        {"editor_background", "background"},
        {"editor_main", "on_surface"},
        {"editor_comment", "outline"},
        {"editor_string", "tertiary"},
        {"editor_emphasis", "primary"},
        {"editor_command", "secondary"},
        {"editor_link", "primary_container"},
        {"editor_error", "error"},
        {"editor_warning", "secondary"},
        {"editor_success", "primary"},
        {"editor_disabled", "outline_variant"},
        {"editor_inactive", "outline_variant"},
        {"editor_line_number", "outline"},
        {"editor_selected", "primary_container"},
        {"editor_selection_inactive", "surface_container_low"},
        
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
    auto begin = std::sregex_iterator(parse_src.begin(), parse_src.end(), kv_re);
    auto endit = std::sregex_iterator();
    for (auto it = begin; it != endit; ++it)
    {
        std::smatch match = *it;
        std::string key = match[1].str();
        for (auto &c : key)
            if (c == '-')
                c = '_';
        std::string val = match[2].str();
        matu_kv_map[key] = val;
    }

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