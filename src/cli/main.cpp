#include <cstdlib>
#include <iostream>
#include <string>

#include <argparse/argparse.hpp>

#include "core/common/error.hpp"
#include "core/common/utils.hpp"
#include "core/common/version.hpp"
#include "core/config/config.hpp"
#include "core/io/toml_file.hpp"
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

    std::cout << program << std::endl;

    return 0;
}
