#include "main_layout.hpp"
#include "imgui.h"
#include "imgui_internal.h"

namespace clrsync::gui::layout
{

void main_layout::render_menu_bar()
{
    ImGuiViewport *vp = ImGui::GetMainViewport();

    const float bar_height = 30.0f;

    ImGui::SetNextWindowPos(vp->Pos);
    ImGui::SetNextWindowSize(ImVec2(vp->Size.x, bar_height));
    ImGui::SetNextWindowViewport(vp->ID);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
                             ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                             ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 2));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 3));

    ImGui::Begin("##TopBar", nullptr, flags);

    ImGuiStyle &style = ImGui::GetStyle();
    style.FrameBorderSize = 1.0f;

    const char *settings_label = "Settings";
    const char *about_label = "About";

    ImVec2 settings_size = ImGui::CalcTextSize(settings_label);
    ImVec2 about_size = ImGui::CalcTextSize(about_label);

    float total_width = settings_size.x + style.FramePadding.x * 2.0f + about_size.x +
                        style.FramePadding.x * 2.0f + style.ItemSpacing.x;

    float pos_x = ImGui::GetWindowWidth() - total_width - style.WindowPadding.x;

    float button_height = ImGui::GetFrameHeight();
    float window_height = ImGui::GetWindowHeight();
    float center_y = (window_height - button_height) * 0.5f;

    ImGui::SetCursorPos(ImVec2(pos_x, center_y));

    if (ImGui::Button(settings_label))
    {
        m_show_settings = true;
    }

    ImGui::SameLine();
    ImGui::SetCursorPosY(center_y);

    if (ImGui::Button(about_label))
    {
        m_show_about = true;
    }

    ImGui::End();

    ImGui::PopStyleVar(4);
}

void main_layout::setup_dockspace(bool &first_time)
{
    const ImGuiViewport *viewport = ImGui::GetMainViewport();
    const float topbar_height = 32.0f;
    ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + topbar_height));
    ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, viewport->Size.y - topbar_height));
    ImGui::SetNextWindowViewport(viewport->ID);

    constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                                       ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                                       ImGuiWindowFlags_NoBringToFrontOnFocus |
                                       ImGuiWindowFlags_NoNavFocus;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 3));
    ImGui::Begin("MainDockSpace", nullptr, flags);
    ImGui::PopStyleVar(4);

    ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");

    if (first_time)
    {
        first_time = false;

        ImGui::DockBuilderRemoveNode(dockspace_id);
        ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

        ImGuiID center, right;
        ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Right, 0.5f, &right, &center);

        ImGuiDockNode *center_node = ImGui::DockBuilderGetNode(center);
        if (center_node)
        {
            center_node->LocalFlags |= ImGuiDockNodeFlags_CentralNode;
        }

        ImGui::DockBuilderDockWindow("Color Schemes", right);
        ImGui::DockBuilderDockWindow("Color Preview", center);
        ImGui::DockBuilderDockWindow("Templates", center);

        ImGui::DockBuilderFinish(dockspace_id);
    }

    ImGui::DockSpace(dockspace_id, ImVec2{0, 0}, ImGuiDockNodeFlags_None);
    ImGui::End();
}

} // namespace clrsync::gui::layout
