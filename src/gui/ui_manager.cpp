#include "gui/ui_manager.hpp"
#include "gui/backend/backend.hpp"
#include "gui/platform/font_loader.hpp"
#include "gui/platform/file_browser.hpp"
#include "core/config/config.hpp"

#include <imgui.h>

namespace clrsync::gui
{
ui_manager::ui_manager(backend::backend_interface* backend)
    : m_backend(backend)
{
    m_font_loader = new font_loader();
}

ui_manager::~ui_manager()
{
    shutdown();
    delete m_font_loader;
    m_font_loader = nullptr;
}

bool ui_manager::initialize(const ui_config& config)
{
    IMGUI_CHECKVERSION();
    m_imgui_context = ImGui::CreateContext();
    if (!m_imgui_context)
    {
        return false;
    }

    ImGuiIO& io = ImGui::GetIO();
    
    if (config.enable_docking)
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    
    if (config.enable_keyboard_nav)
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    
    if (!config.ini_path.empty())
        io.IniFilename = config.ini_path.c_str();

    ImGui::StyleColorsDark();

    if (!m_backend->init_imgui_backend())
    {
        ImGui::DestroyContext(static_cast<ImGuiContext*>(m_imgui_context));
        m_imgui_context = nullptr;
        return false;
    }

    ImFont *font = m_font_loader->load_font(
        clrsync::core::config::instance().font().c_str(),
        clrsync::core::config::instance().font_size());

    if (font)
        io.FontDefault = font;

    return true;
}

void ui_manager::shutdown()
{
    if (m_imgui_context)
    {
        m_backend->shutdown_imgui_backend();
        ImGui::DestroyContext(static_cast<ImGuiContext*>(m_imgui_context));
        m_imgui_context = nullptr;
    }
}

void ui_manager::begin_frame()
{
    m_backend->imgui_new_frame();
    ImGui::NewFrame();
}

void ui_manager::end_frame()
{
    ImGui::Render();
    m_backend->imgui_render_draw_data(ImGui::GetDrawData());
}

void ui_manager::push_default_font()
{
    if (m_font_loader)
        m_font_loader->push_default_font();
}

void ui_manager::pop_font()
{
    if (m_font_loader)
        m_font_loader->pop_font();
}

std::vector<std::string> ui_manager::get_system_fonts() const
{
    if (m_font_loader)
        return m_font_loader->get_system_fonts();
    return {};
}

bool ui_manager::reload_font(const char* font_name, float size)
{
    if (!m_font_loader)
        return false;
    
    ImFont* font = m_font_loader->load_font(font_name, size);
    if (font)
    {
        ImGui::GetIO().FontDefault = font;
        return true;
    }
    return false;
}

std::string ui_manager::open_file_dialog(const std::string& title,
                                          const std::string& initial_path,
                                          const std::vector<std::string>& filters)
{
    return file_dialogs::open_file_dialog(title, initial_path, filters);
}

std::string ui_manager::save_file_dialog(const std::string& title,
                                          const std::string& initial_path,
                                          const std::vector<std::string>& filters)
{
    return file_dialogs::save_file_dialog(title, initial_path, filters);
}

std::string ui_manager::select_folder_dialog(const std::string& title,
                                              const std::string& initial_path)
{
    return file_dialogs::select_folder_dialog(title, initial_path);
}
}
