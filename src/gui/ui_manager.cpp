#include "gui/ui_manager.hpp"
#include "gui/backend/backend.hpp"

#include <imgui.h>

namespace clrsync::gui
{
ui_manager::ui_manager(backend::backend_interface* backend)
    : m_backend(backend)
{
}

ui_manager::~ui_manager()
{
    shutdown();
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
}