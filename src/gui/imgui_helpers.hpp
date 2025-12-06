#ifndef CLRSYNC_IMGUI_HELPERS_HPP
#define CLRSYNC_IMGUI_HELPERS_HPP

#include "gui/about_window.hpp"
#include <string>

struct GLFWwindow;
class settings_window;

GLFWwindow * init_glfw();
void init_imgui(GLFWwindow* window, const std::string& ini_path);
void begin_frame();
void setup_main_dockspace(bool& first_time);
void end_frame(GLFWwindow* window);
void shutdown(GLFWwindow* window);
void render_menu_bar(about_window* about, settings_window* settings);

#endif // CLRSYNC_IMGUI_HELPERS_HPP
