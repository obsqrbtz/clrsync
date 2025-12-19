#ifndef CLRSYNC_UI_MANAGER_HPP
#define CLRSYNC_UI_MANAGER_HPP
#include <string>
#include <vector>

class font_loader;
struct ImFont;

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

    void push_default_font();
    void pop_font();

    std::vector<std::string> get_system_fonts() const;
    bool reload_font(const char* font_name, float size);

    std::string open_file_dialog(const std::string& title = "Open File",
                                  const std::string& initial_path = "",
                                  const std::vector<std::string>& filters = {});
    std::string save_file_dialog(const std::string& title = "Save File",
                                  const std::string& initial_path = "",
                                  const std::vector<std::string>& filters = {});
    std::string select_folder_dialog(const std::string& title = "Select Folder",
                                      const std::string& initial_path = "");

private:
    backend::backend_interface *m_backend;
    void *m_imgui_context = nullptr;
    font_loader *m_font_loader = nullptr;
};
}

#endif
