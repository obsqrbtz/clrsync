#include "pywal16_generator.hpp"

#include "core/common/process.hpp"
#include "core/common/utils.hpp"
#include "core/palette/color.hpp"
#include "core/palette/json_utils.hpp"
#include "core/palette/pywal16_mappings.hpp"

#include <fstream>
#include <filesystem>
#include <stdexcept>
#include <string>

namespace clrsync::core
{
using json = json_utils::json;

namespace
{
constexpr const char *COLORS_JSON_PATH = "~/.cache/wal/colors.json";

static std::string read_text_file(const std::filesystem::path &path)
{
    std::ifstream in(path, std::ios::binary);
    if (!in)
        return {};
    return std::string((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
}

static bool set_hex_color(palette &pal, const std::string &key, const std::string &hex_value)
{
    const std::string hex = json_utils::normalize_hex_string(hex_value);
    if (hex.empty())
        return false;
    try
    {
        color col;
        col.from_hex_string(hex);
        pal.set_color(key, col);
        return true;
    }
    catch (...)
    {
        return false;
    }
}

static bool wallpaper_matches_image(const json &doc, const std::filesystem::path &image_path)
{
    if (!doc.contains("wallpaper") || !doc["wallpaper"].is_string())
        return true;
    const std::filesystem::path wallpaper_path =
        normalize_path(doc["wallpaper"].get<std::string>());
    const std::filesystem::path expected_path = normalize_path(image_path.string());
    if (wallpaper_path == expected_path)
        return true;
    std::error_code ec;
    if (std::filesystem::exists(wallpaper_path, ec) && std::filesystem::exists(expected_path, ec))
        return std::filesystem::equivalent(wallpaper_path, expected_path, ec);
    return false;
}

static palette parse_pywal_colors_json(const std::string &content,
                                       const std::filesystem::path &image_path,
                                       const std::string &pal_name)
{
    if (content.empty())
        return {};

    json doc;
    if (!json_utils::parse_json_output(content, doc))
        return {};

    if (!wallpaper_matches_image(doc, image_path))
        return {};

    palette pal;
    pal.set_name(pal_name);
    pal.set_file_path(image_path.string());

    if (doc.contains("special") && doc["special"].is_object())
    {
        const json &special = doc["special"];
        if (special.contains("background") && special["background"].is_string())
            set_hex_color(pal, "background", special["background"].get<std::string>());
        if (special.contains("foreground") && special["foreground"].is_string())
            set_hex_color(pal, "foreground", special["foreground"].get<std::string>());
        if (special.contains("cursor") && special["cursor"].is_string())
            set_hex_color(pal, "cursor", special["cursor"].get<std::string>());
    }

    if (doc.contains("colors") && doc["colors"].is_object())
    {
        const json &colors = doc["colors"];
        for (int i = 0; i < 16; ++i)
        {
            const std::string key = "color" + std::to_string(i);
            if (colors.contains(key) && colors[key].is_string())
                set_hex_color(pal, key, colors[key].get<std::string>());
        }
    }

    auto get_color_by_index = [&](int idx) -> const color & {
        return pal.get_color("color" + std::to_string(idx));
    };
    apply_index_mappings(pal, PYWAL16_COLOR_MAPPINGS, PYWAL16_COLOR_MAPPINGS_COUNT,
                         get_color_by_index);
    return pal;
}
} // namespace

palette pywal16_generator::generate_from_image(const std::string &image_path)
{
    options default_opts{};
    return generate_from_image(image_path, default_opts);
}

palette pywal16_generator::generate_from_image(const std::string &image_path, const options &opts)
{
    ensure_supported("pywal16");

    std::filesystem::path p(image_path);
    const std::string pal_name = std::string("pywal16:") + p.filename().string();

    std::vector<std::string> args = {"wal", "-i", image_path, "-n", "-s", "-e", "-t"};
    if (opts.light)
        args.push_back("-l");
    if (!opts.background.empty())
    {
        args.push_back("-b");
        args.push_back(opts.background);
    }
    if (!opts.foreground.empty())
    {
        args.push_back("--fg");
        args.push_back(opts.foreground);
    }
    if (!opts.backend.empty())
    {
        args.push_back("--backend");
        args.push_back(opts.backend);
    }
    if (opts.saturate >= 0.0f && opts.saturate <= 1.0f)
    {
        args.push_back("--saturate");
        args.push_back(std::to_string(opts.saturate));
    }

    int exit_code = -1;
    const std::string output = run_process_capture_output(args, &exit_code);
    if (exit_code != 0)
        throw std::runtime_error(process_failure_message(output, "wal failed"));

    const std::filesystem::path colors_path = normalize_path(COLORS_JSON_PATH);
    const std::string colors_json = read_text_file(colors_path);
    palette pal = parse_pywal_colors_json(colors_json, p, pal_name);
    if (pal.name().empty())
        throw std::runtime_error("wal did not produce a colorscheme");
    return pal;
}

} // namespace clrsync::core
