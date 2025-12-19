#include <iostream>
#include <memory>

#include "core/common/error.hpp"
#include "core/common/utils.hpp"
#include "core/config/config.hpp"
#include "core/io/toml_file.hpp"

#include "gui/backend/glfw_opengl.hpp"
#include "gui/layout/main_layout.hpp"
#include "gui/ui_manager.hpp"
#include "gui/views/about_window.hpp"
#include "gui/views/color_scheme_editor.hpp"
#include "gui/views/settings_window.hpp"
#include "gui/views/template_editor.hpp"

int main(int, char **)
{
    auto config_path = clrsync::core::get_default_config_path();
    auto conf = std::make_unique<clrsync::core::io::toml_file>(config_path);

    auto init_result = clrsync::core::config::instance().initialize(std::move(conf));
    if (!init_result)
    {
        std::cerr << "Fatal error: " << init_result.error().description() << std::endl;
        std::cerr
            << "Hint: Set CLRSYNC_CONFIG_PATH environment variable or ensure config exists at: "
            << config_path << std::endl;
        return 1;
    }

    std::filesystem::path base = config_path;
    static std::string ini_path = (base.parent_path() / "layout.ini").string();
    bool first_time = !std::filesystem::exists(ini_path);

    auto backend = clrsync::gui::backend::glfw_opengl_backend();
    auto window_config = clrsync::gui::backend::window_config();
    window_config.title = "clrsync";
    window_config.width = 1280;
    window_config.height = 720;
    window_config.decorated = true;
    window_config.transparent_framebuffer = true;

    if (!backend.initialize(window_config))
    {
        std::cerr << "Failed to initialize backend." << std::endl;
        return 1;
    }

    std::cout << "GLFW Version: " << backend.get_glfw_version() << std::endl;
    std::cout << "GLFW runtime platform: " << backend.get_glfw_platform() << std::endl;

    clrsync::gui::ui_manager ui_manager(&backend);
    
    clrsync::gui::ui_config ui_cfg;
    ui_cfg.ini_path = ini_path;
    ui_cfg.enable_docking = true;
    ui_cfg.enable_keyboard_nav = true;
    
    if (!ui_manager.initialize(ui_cfg))
    {
        std::cerr << "Failed to initialize UI manager." << std::endl;
        return 1;
    }

    clrsync::gui::layout::main_layout main_layout;
    color_scheme_editor colorEditor;
    template_editor templateEditor(&ui_manager);
    about_window aboutWindow;
    settings_window settingsWindow(&ui_manager);

    colorEditor.set_template_editor(&templateEditor);
    colorEditor.set_settings_window(&settingsWindow);
    templateEditor.apply_current_palette(colorEditor.controller().current_palette());
    settingsWindow.set_palette(colorEditor.controller().current_palette());

    while (!backend.should_close())
    {
        backend.begin_frame();
        
        ui_manager.push_default_font();
        ui_manager.begin_frame();

        main_layout.render_menu_bar();
        main_layout.setup_dockspace(first_time);

        if (main_layout.should_show_about())
            aboutWindow.show();
        if (main_layout.should_show_settings())
            settingsWindow.show();
        main_layout.clear_actions();

        templateEditor.render();
        colorEditor.render_controls_and_colors();
        colorEditor.render_preview();
        aboutWindow.render(colorEditor.controller().current_palette());
        settingsWindow.render();

        ui_manager.pop_font();
        
        ui_manager.end_frame();
        backend.end_frame();
    }

    ui_manager.shutdown();
    backend.shutdown();
    return 0;
}