#include "matugen_generator.hpp"

#include "core/common/process.hpp"
#include "core/palette/color.hpp"
#include "core/palette/json_utils.hpp"
#include "core/palette/matugen_mappings.hpp"

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <filesystem>
#include <string>

namespace clrsync::core
{
using json = json_utils::json;

static palette parse_matugen_output(const std::string &out, const matugen_generator::options &opts,
                                    const std::string &pal_name, const std::string &file_path)
{
    if (out.empty())
        return {};

    json doc;
    if (!json_utils::parse_json_output(out, doc))
        return {};

    palette pal;
    pal.set_name(pal_name);
    pal.set_file_path(file_path);

    for (std::size_t i = 0; i < MATUGEN_COLOR_MAPPINGS_COUNT; ++i)
    {
        const key_color_mapping &mapping = MATUGEN_COLOR_MAPPINGS[i];
        const std::string matu_key = mapping.source_key;
        std::string hex;
        if (matu_key.rfind("base16.", 0) == 0)
        {
            std::string base_key = matu_key.substr(7);
            std::transform(base_key.begin(), base_key.end(), base_key.begin(),
                           [](unsigned char c) { return std::tolower(c); });
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
                pal.set_color(mapping.palette_key, col);
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
    ensure_supported("matugen");

    std::filesystem::path p(image_path);
    std::vector<std::string> args = {"matugen", "image", image_path};
    if (!opts.type.empty())
    {
        args.push_back("--type");
        args.push_back(opts.type);
    }
    if (!opts.mode.empty())
    {
        args.push_back("--mode");
        args.push_back(opts.mode);
    }
    if (opts.contrast != 0.0f)
    {
        args.push_back("--contrast");
        args.push_back(std::to_string(opts.contrast));
    }
    args.push_back("--json");
    args.push_back("hex");
    args.push_back("--dry-run");
    if (opts.source_color_index >= 0 && opts.source_color_index <= 3)
    {
        args.push_back("--source-color-index");
        args.push_back(std::to_string(opts.source_color_index));
    }

    std::string out = run_process_capture_output(args);
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
    ensure_supported("matugen");

    std::string c = color_hex;
    if (!c.empty() && c[0] == '#')
        c = c.substr(1);
    std::vector<std::string> args = {"matugen", "color", "hex", c};
    if (!opts.type.empty())
    {
        args.push_back("--type");
        args.push_back(opts.type);
    }
    if (!opts.mode.empty())
    {
        args.push_back("--mode");
        args.push_back(opts.mode);
    }
    if (opts.contrast != 0.0f)
    {
        args.push_back("--contrast");
        args.push_back(std::to_string(opts.contrast));
    }
    args.push_back("--json");
    args.push_back("hex");
    args.push_back("--dry-run");

    std::string out = run_process_capture_output(args);
    if (out.empty())
        return {};

    return parse_matugen_output(out, opts, std::string("matugen:color:") + color_hex, color_hex);
}
} // namespace clrsync::core
