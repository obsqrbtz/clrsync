#ifndef CLRSYNC_GLFW_OPENGL_HPP
#define CLRSYNC_GLFW_OPENGL_HPP

#define GL_SILENCE_DEPRECATION

#include <GLFW/glfw3.h>

#include "gui/backend/backend.hpp"

namespace clrsync::gui::backend{

    class glfw_opengl_backend : public backend_interface{
    public:
        glfw_opengl_backend();
        ~glfw_opengl_backend();

        bool initialize(const window_config& config) override;
        void shutdown() override;
        bool should_close() const override;
        void begin_frame() override;
        void end_frame() override;

        void* get_native_window() const override;
        void* get_graphics_context() const override;
        
        std::string get_glfw_version() const;
        std::string get_glfw_platform() const;
        
        bool init_imgui_backend() override;
        void shutdown_imgui_backend() override;
        void imgui_new_frame() override;
        void imgui_render_draw_data(void* draw_data) override;

    private:
        GLFWwindow* m_window = nullptr;
    };
}

#endif // CLRSYNC_GLFW_OPENGL_HPP
