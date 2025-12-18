#ifndef CLRSYNC_UI_MANAGER_HPP
#define CLRSYNC_UI_MANAGER_HPP
#include <string>

namespace clrsync::gui::backend
{
class backend_interface;
}

namespace clrsync::gui
{

struct ui_config
{
    std::string ini_path = "";
    bool enable_docking = true;
    bool enable_keyboard_nav = true;
};

class ui_manager
{
public:
    explicit ui_manager(backend::backend_interface *backend);
    ~ui_manager();

    bool initialize(const ui_config& config = ui_config());
    void shutdown();

    void begin_frame();
    void end_frame();

private:
    backend::backend_interface *m_backend;
    void *m_imgui_context = nullptr;
};
}

#endif
