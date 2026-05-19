#ifdef _WIN32

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "gui/platform/window_icon.hpp"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace
{

constexpr int APP_ICON_RESOURCE_ID = 1;

HICON load_app_icon(int width, int height)
{
    HINSTANCE instance = GetModuleHandleW(nullptr);
    HICON icon = static_cast<HICON>(LoadImageW(instance, MAKEINTRESOURCEW(APP_ICON_RESOURCE_ID),
                                               IMAGE_ICON, width, height, LR_DEFAULTCOLOR));
    if (!icon && (width != 0 || height != 0))
    {
        icon = static_cast<HICON>(LoadImageW(instance, MAKEINTRESOURCEW(APP_ICON_RESOURCE_ID),
                                             IMAGE_ICON, 0, 0, LR_DEFAULTSIZE));
    }
    return icon;
}

void apply_icon(HWND hwnd, UINT icon_type, HICON icon)
{
    if (!icon)
        return;
    HICON previous = reinterpret_cast<HICON>(SendMessageW(hwnd, WM_SETICON, icon_type,
                                                          reinterpret_cast<LPARAM>(icon)));
    if (previous)
        DestroyIcon(previous);
}

} // namespace

namespace clrsync::gui::platform
{

void set_window_icon(GLFWwindow *window)
{
    HWND hwnd = glfwGetWin32Window(window);
    if (!hwnd)
        return;
    apply_icon(hwnd, ICON_SMALL,
               load_app_icon(GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON)));
    apply_icon(hwnd, ICON_BIG,
               load_app_icon(GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON)));
}

} // namespace clrsync::gui::platform

#endif // _WIN32
