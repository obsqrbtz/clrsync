#include <cstdlib>
#include <iostream>
#include <string>

#include <argparse/argparse.hpp>

#include "core/common/error.hpp"
#include "core/common/utils.hpp"
#include "core/common/version.hpp"
#include "core/config/config.hpp"
#include "core/io/toml_file.hpp"
#include "core/palette/hellwal_generator.hpp"
#include "core/palette/matugen_generator.hpp"
#include "core/palette/pywal16_generator.hpp"
#include "core/palette/palette_file.hpp"
#include "core/palette/palette_manager.hpp"
#include "core/theme/theme_renderer.hpp"
#include "core/theme/theme_template.hpp"

void handle_show_vars()
{
    clrsync::core::print_color_keys();
}

void handle_list_themes()
{
    auto palette_manager = clrsync::core::palette_manager<clrsync::core::io::toml_file>();
    palette_manager.load_palettes_from_directory(clrsync::core::config::instance().palettes_path());

    const auto &palettes = palette_manager.palettes();
    std::cout << "Available themes:" << std::endl;
    for (const auto &p : palettes)
    {
        std::cout << " - " << p.first << std::endl;
    }
}

int handle_apply_theme(const argparse::ArgumentParser &program, const std::string &default_theme)
{
    clrsync::core::theme_renderer<clrsync::core::io::toml_file> renderer;
    std::string theme_identifier;
    clrsync::core::Result<void> result = clrsync::core::Ok();

    if (program.is_used("--theme"))
    {
        theme_identifier = program.get<std::string>("--theme");
        result = renderer.apply_theme(theme_identifier);
    }
    else if (program.is_used("--path"))
    {
        theme_identifier = program.get<std::string>("--path");
        result = renderer.apply_theme_from_path(theme_identifier);
    }
    else
    {
        if (default_theme.empty())
        {
            std::cerr << "Default theme is not set or missing." << std::endl;
            return 1;
        }
        theme_identifier = default_theme;
        result = renderer.apply_theme(theme_identifier);
    }

    if (!result)
    {
        std::cerr << "Failed to apply theme: " << result.error().description() << std::endl;
        return 1;
    }

    std::cout << "Applied theme " << theme_identifier << std::endl;
    return 0;
}

clrsync::core::Result<void> initialize_config(const std::string &config_path)
{
    auto conf = std::make_unique<clrsync::core::io::toml_file>(config_path);
    return clrsync::core::config::instance().initialize(std::move(conf));
}

void setup_argument_parser(argparse::ArgumentParser &program)
{
    program.add_argument("-a", "--apply").help("applies default theme").flag();

    program.add_argument("-c", "--config")
        .default_value(clrsync::core::get_default_config_path())
        .help("sets config file path")
        .metavar("PATH");

    program.add_argument("-l", "--list-themes").help("lists available themes").flag();

    program.add_argument("-s", "--show-vars").help("shows color keys").flag();

    auto &group = program.add_mutually_exclusive_group();
    group.add_argument("-t", "--theme").help("sets theme <theme_name> to apply");
    group.add_argument("-p", "--path").help("sets theme file <path/to/theme> to apply");

    program.add_argument("-g", "--generate").nargs(1).help("generate palette from <image path>");
    program.add_argument("--generate-color")
        .nargs(1)
        .help("generate palette from a color (hex), used with --generator matugen");

    program.add_argument("--generator")
        .default_value(std::string("hellwal"))
        .help("palette generator to use (hellwal)")
        .metavar("GENERATOR");

    program.add_argument("--matugen-type")
        .default_value(std::string("scheme-tonal-spot"))
        .help("matugen: Sets a custom color scheme type")
        .metavar("TYPE");
    program.add_argument("--matugen-mode")
        .default_value(std::string("dark"))
        .help("matugen: Which mode to use for the color scheme (light,dark)")
        .metavar("MODE");
    program.add_argument("--matugen-contrast")
        .default_value(std::string("0.0"))
        .help("matugen: contrast value from -1 to 1")
        .metavar("FLOAT");
    program.add_argument("--matugen-source-color-index")
        .default_value(std::string("0"))
        .help("matugen: source color index (0..3)")
        .metavar("INT");

    // hellwal generator options
    program.add_argument("--hellwal-neon").help("hellwal: enable neon mode").flag();
    program.add_argument("--hellwal-dark").help("hellwal: prefer dark palettes").flag();
    program.add_argument("--hellwal-light").help("hellwal: prefer light palettes").flag();
    program.add_argument("--hellwal-color").help("hellwal: enable color mode").flag();
    program.add_argument("--hellwal-invert").help("hellwal: invert colors").flag();
    program.add_argument("--hellwal-dark-offset")
        .default_value(std::string("0.0"))
        .help("hellwal: dark offset (float)")
        .metavar("FLOAT");
    program.add_argument("--hellwal-bright-offset")
        .default_value(std::string("0.0"))
        .help("hellwal: bright offset (float)")
        .metavar("FLOAT");
    program.add_argument("--hellwal-gray-scale")
        .default_value(std::string("0.0"))
        .help("hellwal: gray scale factor (float)")
        .metavar("FLOAT");

    program.add_argument("--pywal16-background")
        .help("pywal16: custom background color (hex)")
        .metavar("COLOR");
    program.add_argument("--pywal16-foreground")
        .help("pywal16: custom foreground color (hex)")
        .metavar("COLOR");
    program.add_argument("--pywal16-backend")
        .help("pywal16: color extraction backend")
        .metavar("BACKEND");
    program.add_argument("--pywal16-saturate")
        .default_value(std::string("-1.0"))
        .help("pywal16: color saturation from 0.0 to 1.0")
        .metavar("FLOAT");
    program.add_argument("--pywal16-light").help("pywal16: generate a light colorscheme").flag();
}

int main(int argc, char *argv[])
{
    argparse::ArgumentParser program("clrsync", clrsync::core::version_string());
    setup_argument_parser(program);

    try
    {
        program.parse_args(argc, argv);
    }
    catch (const std::exception &err)
    {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        return 1;
    }

    std::string config_path = program.get<std::string>("--config");

    auto config_result = initialize_config(config_path);
    if (!config_result)
    {
        std::cerr << "Error loading config: " << config_result.error().description() << std::endl;
        return 1;
    }

    if (program.is_used("--show-vars"))
    {
        handle_show_vars();
        return 0;
    }

    if (program.is_used("--list-themes"))
    {
        handle_list_themes();
        return 0;
    }

    if (program.is_used("--apply"))
    {
        const std::string default_theme = clrsync::core::config::instance().default_theme();
        return handle_apply_theme(program, default_theme);
    }

    if (program.is_used("--generate") || program.is_used("--generate-color"))
    {
        std::string image_path;
        if (program.is_used("--generate"))
            image_path = program.get<std::string>("--generate");
        std::string generator_name = program.get<std::string>("--generator");

        clrsync::core::palette pal;
        if (generator_name == "hellwal")
        {
            clrsync::core::hellwal_generator gen;
            if (!gen.supports_current_system())
            {
                std::cerr << "Error: hellwal is not supported on "
                          << clrsync::core::generator::system_name(
                                 clrsync::core::generator::current_system())
                          << ". Supported systems: " << gen.supported_systems_description()
                          << std::endl;
                return 1;
            }

            if (program.is_used("--generate-color"))
            {
                std::cerr << "Error: --generate-color is only supported with --generator matugen" << std::endl;
                return 1;
            }

            clrsync::core::hellwal_generator::options opts{};

            if (program.is_used("--hellwal-neon"))
                opts.neon = true;
            if (program.is_used("--hellwal-dark"))
                opts.dark = true;
            if (program.is_used("--hellwal-light"))
                opts.light = true;
            if (program.is_used("--hellwal-color"))
                opts.color = true;
            if (program.is_used("--hellwal-invert"))
                opts.invert = true;

            try
            {
                std::string s1 = program.get<std::string>("--hellwal-dark-offset");
                opts.dark_offset = std::stof(s1);
            }
            catch (...)
            {
            }

            try
            {
                std::string s2 = program.get<std::string>("--hellwal-bright-offset");
                opts.bright_offset = std::stof(s2);
            }
            catch (...)
            {
            }

            try
            {
                std::string s3 = program.get<std::string>("--hellwal-gray-scale");
                opts.gray_scale = std::stof(s3);
            }
            catch (...)
            {
            }

            pal = gen.generate_from_image(image_path, opts);
        }
        else if (generator_name == "matugen")
        {
            clrsync::core::matugen_generator gen;
            if (!gen.supports_current_system())
            {
                std::cerr << "Error: matugen is not supported on "
                          << clrsync::core::generator::system_name(
                                 clrsync::core::generator::current_system())
                          << ". Supported systems: " << gen.supported_systems_description()
                          << std::endl;
                return 1;
            }

            clrsync::core::matugen_generator::options opts{};

            try
            {
                opts.type = program.get<std::string>("--matugen-type");
            }
            catch (...)
            {
            }

            try
            {
                opts.type = program.get<std::string>("--matugen-type");
            }
            catch (...)
            {
            }

            try
            {
                opts.mode = program.get<std::string>("--matugen-mode");
            }
            catch (...)
            {
            }

            try
            {
                std::string s = program.get<std::string>("--matugen-contrast");
                opts.contrast = std::stof(s);
            }
            catch (...)
            {
            }

            try
            {
                std::string s = program.get<std::string>("--matugen-source-color-index");
                int idx = std::stoi(s);
                if (idx < 0)
                    idx = 0;
                if (idx > 3)
                    idx = 3;
                opts.source_color_index = idx;
            }
            catch (...)
            {
            }

            if (program.is_used("--generate-color"))
            {
                std::string color = program.get<std::string>("--generate-color");
                pal = gen.generate_from_color(color, opts);
            }
            else
            {
                pal = gen.generate_from_image(image_path, opts);
            }
        }
        else if (generator_name == "pywal16")
        {
            clrsync::core::pywal16_generator gen;
            if (!gen.supports_current_system())
            {
                std::cerr << "Error: pywal16 is not supported on "
                          << clrsync::core::generator::system_name(
                                 clrsync::core::generator::current_system())
                          << ". Supported systems: " << gen.supported_systems_description()
                          << std::endl;
                return 1;
            }

            if (program.is_used("--generate-color"))
            {
                std::cerr << "Error: --generate-color is only supported with --generator matugen"
                          << std::endl;
                return 1;
            }

            clrsync::core::pywal16_generator::options opts{};
            if (program.is_used("--pywal16-background"))
                opts.background = program.get<std::string>("--pywal16-background");
            if (program.is_used("--pywal16-foreground"))
                opts.foreground = program.get<std::string>("--pywal16-foreground");
            if (program.is_used("--pywal16-backend"))
                opts.backend = program.get<std::string>("--pywal16-backend");
            if (program.is_used("--pywal16-light"))
                opts.light = true;

            try
            {
                std::string s = program.get<std::string>("--pywal16-saturate");
                const float saturate = std::stof(s);
                if (saturate >= 0.0f && saturate <= 1.0f)
                    opts.saturate = saturate;
            }
            catch (...)
            {
            }

            pal = gen.generate_from_image(image_path, opts);
        }
        else
        {
            std::cerr << "Unknown generator: " << generator_name << std::endl;
            return 1;
        }

        if (pal.name().empty())
        {
            if (program.is_used("--generate-color"))
            {
                std::string color = program.get<std::string>("--generate-color");
                pal.set_name("generated:" + color);
            }
            else
            {
                std::filesystem::path p(image_path);
                pal.set_name("generated:" + p.filename().string());
            }
        }

        auto dir = clrsync::core::config::instance().palettes_path();
        clrsync::core::palette_manager<clrsync::core::io::toml_file> pal_mgr;
        pal_mgr.save_palette_to_file(pal, dir);

        clrsync::core::theme_renderer<clrsync::core::io::toml_file> renderer;
        std::filesystem::path file_path = std::filesystem::path(dir) / (pal.name() + ".toml");
        auto res = renderer.apply_theme_from_path(file_path.string());
        if (!res)
        {
            std::cerr << "Failed to apply generated palette: " << res.error().description()
                      << std::endl;
            return 1;
        }
        std::cout << "Generated and applied palette: " << pal.name() << std::endl;
        return 0;
    }

    std::cout << program << std::endl;

    return 0;
}
