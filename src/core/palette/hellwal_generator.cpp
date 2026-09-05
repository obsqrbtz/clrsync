#include "hellwal_generator.hpp"

#include "core/common/process.hpp"
#include "core/palette/color.hpp"
#include "core/palette/json_utils.hpp"
#include "core/palette/palette_normalizer.hpp"

#include <algorithm>
#include <cctype>
#include <string>
#include <vector>

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
                        key.size() >= 6 && key.rfind("color", 0) == 0 && key.size() <= 7 &&
                        std::all_of(key.begin() + 5, key.end(),
                                    [](unsigned char c) { return std::isdigit(c) != 0; });

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

    pal.set_name("hellwal");
    pal.set_file_path(image_path);

    std::vector<std::string> args = {"hellwal", "-i", image_path, "--json", "--skip-term-colors"};
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

    std::vector<color> sources;
    for (int i = 0; i < 16; ++i)
    {
        const std::string key = "color" + std::to_string(i);
        if (pal.colors().find(key) != pal.colors().end())
            sources.push_back(pal.get_color(key));
    }

    normalize_palette(pal, sources, opts.normalize);
    return pal;
}

} // namespace clrsync::core
