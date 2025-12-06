#include <memory>

#include <GLFW/glfw3.h>

#include "core/config/config.hpp"
#include "core/io/toml_file.hpp"
#include "core/utils.hpp"

#include "color_scheme_editor.hpp"
#include "gui/font_loader.hpp"
#include "gui/settings_window.hpp"
#include "imgui_helpers.hpp"
#include "template_editor.hpp"
#include "about_window.hpp"


int main(int, char**)
{
    auto config_path = clrsync::core::get_default_config_path();
    auto conf = std::make_unique<clrsync::core::io::toml_file>(config_path);
    clrsync::core::config::instance().initialize(std::move(conf));

    std::filesystem::path base = config_path;
    static std::string ini_path = (base.parent_path() / "layout.ini").string();
    bool first_time = !std::filesystem::exists(ini_path);

    GLFWwindow* window = init_glfw();
    if (!window) return 1;

    init_imgui(window, ini_path);

    font_loader loader;

    ImFont* font =
        loader.load_font(clrsync::core::config::instance().font().c_str(), clrsync::core::config::instance().font_size());

    if (font)
        ImGui::GetIO().FontDefault = font;

    color_scheme_editor colorEditor;
    template_editor templateEditor;
    about_window aboutWindow;
    settings_window settingsWindow;
    
    colorEditor.set_template_editor(&templateEditor);
    templateEditor.apply_current_palette(colorEditor.controller().current_palette());

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        loader.push_default_font();
        begin_frame();

        render_menu_bar(&aboutWindow, &settingsWindow);
        setup_main_dockspace(first_time); 
        colorEditor.render_controls_and_colors();
        colorEditor.render_preview();
        templateEditor.render();
        aboutWindow.render();
        settingsWindow.render();

        loader.pop_font();
        end_frame(window);
    }
    shutdown(window);
    return 0;
}