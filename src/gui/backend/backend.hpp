#ifndef CLRSYNC_BACKEND_HPP
#define CLRSYNC_BACKEND_HPP

#include <string>

namespace clrsync::gui::backend{

    struct window_config{
        std::string title = "clrsync";
        int width = 1280;
        int height = 720;
        bool decorated = true;
        bool transparent_framebuffer = true;
        float clear_color[4] = {0.1f, 0.1f, 0.1f, 0.0f};
    };
    class backend_interface{
    public:
        virtual ~backend_interface() = default;

        virtual bool initialize(const window_config& config) = 0;
        virtual void shutdown() = 0;
        virtual bool should_close() const = 0;
        virtual void begin_frame() = 0;
        virtual void end_frame() = 0;

        virtual void* get_native_window() const = 0;
        virtual void* get_graphics_context() const = 0;
        
        virtual bool init_imgui_backend() = 0;
        virtual void shutdown_imgui_backend() = 0;
        virtual void imgui_new_frame() = 0;
        virtual void imgui_render_draw_data(void* draw_data) = 0;
    };

}

#endif // CLRSYNC_BACKEND_HPP
