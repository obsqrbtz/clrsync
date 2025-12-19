#include "gui/backend/glfw_opengl.hpp"
#include <iostream>
#include <string>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

namespace clrsync::gui::backend
{

glfw_opengl_backend::glfw_opengl_backend() = default;

glfw_opengl_backend::~glfw_opengl_backend()
{
    glfw_opengl_backend::shutdown();
}

bool glfw_opengl_backend::initialize(const window_config &config)
{
    glfwSetErrorCallback([](int error, const char* description) {
        std::cerr << "GLFW Error " << error << ": " << description << std::endl;
    });
    
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
#ifdef __APPLE__
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
#else
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
#endif
    glfwWindowHint(GLFW_RESIZABLE,GLFW_TRUE);
    glfwWindowHint(GLFW_DECORATED, config.decorated ? GLFW_TRUE : GLFW_FALSE);
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, config.transparent_framebuffer ? GLFW_TRUE : GLFW_FALSE);

    m_window = glfwCreateWindow(config.width, config.height, config.title.c_str(), nullptr, nullptr);
    if (!m_window)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1);

    return true;
}

void glfw_opengl_backend::shutdown()
{
    if (m_window)
    {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }
    glfwTerminate();
}

bool glfw_opengl_backend::should_close() const
{
    return m_window && glfwWindowShouldClose(m_window);
}

void glfw_opengl_backend::begin_frame()
{
    glfwPollEvents();
    
    if (m_window)
    {
        int display_w, display_h;
        glfwGetFramebufferSize(m_window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }
}

void glfw_opengl_backend::end_frame()
{
    if (m_window)
    {
        glfwSwapBuffers(m_window);
    }
}

void *glfw_opengl_backend::get_native_window() const
{
    return static_cast<void *>(m_window);
}

void *glfw_opengl_backend::get_graphics_context() const
{
    return static_cast<void *>(m_window);
}

std::string glfw_opengl_backend::get_glfw_version() const
{
    return glfwGetVersionString();
}

std::string glfw_opengl_backend::get_glfw_platform() const
{
    switch (glfwGetPlatform()) {
        case GLFW_PLATFORM_WAYLAND: return "Wayland";
        case GLFW_PLATFORM_X11: return "X11";
        case GLFW_PLATFORM_COCOA: return "Cocoa";
        case GLFW_PLATFORM_WIN32: return "Win32";
        default: return "Unknown";
    }
}

bool glfw_opengl_backend::init_imgui_backend()
{
    if (!m_window)
        return false;
    
    if (!ImGui_ImplGlfw_InitForOpenGL(m_window, true))
        return false;
    
#ifdef __APPLE__
    const char* glsl_version = "#version 150";
#else
    const char* glsl_version = "#version 120";
#endif
    
    if (!ImGui_ImplOpenGL3_Init(glsl_version))
    {
        ImGui_ImplGlfw_Shutdown();
        return false;
    }
    
    return true;
}

void glfw_opengl_backend::shutdown_imgui_backend()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
}

void glfw_opengl_backend::imgui_new_frame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
}

void glfw_opengl_backend::imgui_render_draw_data(void* draw_data)
{
    ImGui_ImplOpenGL3_RenderDrawData(static_cast<ImDrawData*>(draw_data));
}

} // namespace clrsync::gui::backend