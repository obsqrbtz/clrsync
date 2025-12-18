#include <iostream>
#include <memory>

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "core/common/error.hpp"
#include "core/common/utils.hpp"
#include "core/config/config.hpp"
#include "core/io/toml_file.hpp"

#include "gui/backend/glfw_opengl.hpp"
#include "gui/helpers/imgui_helpers.hpp"
#include "gui/platform/font_loader.hpp"
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

    printf("GLFW Version: %s\n", glfwGetVersionString());

    auto backend = clrsync::gui::backend::glfw_opengl_backend();
    auto window_config = clrsync::gui::backend::window_config();
    window_config.title = "clrsync";
    window_config.width = 1280;
    window_config.height = 720;
    window_config.transparent_framebuffer = true;

    if (!backend.initialize(window_config))
    {
        std::cerr << "Failed to initialize backend." << std::endl;
        return 1;
    }

    std::cout << "GLFW runtime platform: ";
    switch (glfwGetPlatform()) {
        case GLFW_PLATFORM_WAYLAND: std::cout << "Wayland\n"; break;
        case GLFW_PLATFORM_X11: std::cout << "X11\n"; break;
        case GLFW_PLATFORM_COCOA: std::cout << "Cocoa\n"; break;
        case GLFW_PLATFORM_WIN32: std::cout << "Win32\n"; break;
        default: std::cout << "Unknown\n";
    }

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

    font_loader loader;

    ImFont *font = loader.load_font(clrsync::core::config::instance().font().c_str(),
                                    clrsync::core::config::instance().font_size());

    if (font)
        ImGui::GetIO().FontDefault = font;

    color_scheme_editor colorEditor;
    template_editor templateEditor;
    about_window aboutWindow;
    settings_window settingsWindow;

    colorEditor.set_template_editor(&templateEditor);
    colorEditor.set_settings_window(&settingsWindow);
    templateEditor.apply_current_palette(colorEditor.controller().current_palette());
    settingsWindow.set_palette(colorEditor.controller().current_palette());

    while (!backend.should_close())
    {
        backend.begin_frame();
        
        loader.push_default_font();
        ui_manager.begin_frame();

        render_menu_bar(&aboutWindow, &settingsWindow);
        setup_main_dockspace(first_time);
        templateEditor.render();
        colorEditor.render_controls_and_colors();
        colorEditor.render_preview();
        aboutWindow.render(colorEditor.controller().current_palette());
        settingsWindow.render();

        loader.pop_font();
        
        ui_manager.end_frame();
        backend.end_frame();
    }

    ui_manager.shutdown();
    backend.shutdown();
    return 0;
}