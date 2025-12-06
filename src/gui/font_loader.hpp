#ifndef CLRSYNC_GUI_SYSTEM_FONT_LOADER_HPP
#define CLRSYNC_GUI_SYSTEM_FONT_LOADER_HPP

#include <string>
#include <vector>
#include <imgui.h>

class font_loader
{
public:
    font_loader() = default;

    // Loads system font by name and returns an ImFont* or nullptr.
    ImFont* load_font(const char* font_name, float size_px);
    void push_default_font();
    void pop_font();

private:
    std::string find_font_path(const char* font_name);

#if defined(_WIN32)
    static std::string find_font_windows(const char* font_name);
#elif defined(__APPLE__)
    std::vector<unsigned char> load_font_macos(const char* font_name);
#else
    std::string find_font_linux(const char* font_name);
#endif
};

#endif // CLRSYNC_GUI_SYSTEM_FONT_LOADER_HPP
