#ifndef CLRSYNC_IMGUI_HELPERS_HPP
#define CLRSYNC_IMGUI_HELPERS_HPP

#include "core/palette/palette.hpp"
#include "gui/views/about_window.hpp"
#include "imgui.h"
#include <functional>
#include <string>

struct GLFWwindow;
class settings_window;

GLFWwindow *init_glfw();
void init_imgui(GLFWwindow *window, const std::string &ini_path);
void begin_frame();
void setup_main_dockspace(bool &first_time);
void end_frame(GLFWwindow *window);
void shutdown(GLFWwindow *window);
void render_menu_bar(about_window *about, settings_window *settings);

namespace palette_utils
{
ImVec4 get_color(const clrsync::core::palette &pal, const std::string &key,
                 const std::string &fallback = "");
uint32_t get_color_u32(const clrsync::core::palette &pal, const std::string &key,
                       const std::string &fallback = "");
bool render_delete_confirmation_popup(const std::string &popup_title, const std::string &item_name,
                                      const std::string &item_type,
                                      const clrsync::core::palette &pal,
                                      const std::function<void()> &on_delete);
} // namespace palette_utils

namespace imgui_helpers
{
inline ImVec4 get_palette_color(const clrsync::core::palette &pal, const std::string &key,
                                const std::string &fallback = "")
{
    return palette_utils::get_color(pal, key, fallback);
}
} // namespace imgui_helpers

#endif // CLRSYNC_IMGUI_HELPERS_HPP
