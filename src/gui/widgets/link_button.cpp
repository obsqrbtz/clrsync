#include "link_button.hpp"
#include "imgui.h"
#include <cstdlib>

namespace clrsync::gui::widgets
{

void open_url(const std::string& url)
{
#ifdef _WIN32
    std::string cmd = "start " + url;
#elif __APPLE__
    std::string cmd = "open " + url;
#else
    std::string cmd = "xdg-open " + url;
#endif
    std::system(cmd.c_str());
}

bool link_button(const std::string& label, const std::string& url, float width)
{
    bool clicked = ImGui::Button(label.c_str(), ImVec2(width, 0));
    if (clicked)
        open_url(url);
    return clicked;
}

} // namespace clrsync::gui::widgets
