#include <string>

#include "GLFW/glfw3.h"
#include "gui/settings_window.hpp"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "imgui_helpers.hpp"

#include "imgui_internal.h"

GLFWwindow * init_glfw()
{
    if (!glfwInit()) return nullptr;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);

    GLFWwindow* w = glfwCreateWindow(1280, 720, "clrsync", nullptr, nullptr);
    if (!w) return nullptr;

    glfwMakeContextCurrent(w);
    glfwSwapInterval(1);
    return w;
}

void init_imgui(GLFWwindow* window, const std::string& ini_path)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.IniFilename = ini_path.c_str();

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");
}

void begin_frame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void render_menu_bar(about_window* about, settings_window* settings)
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Settings"))
            {
                if (settings)
                    settings->show();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit"))
            {
                // Will be handled by checking window should close
                glfwSetWindowShouldClose(glfwGetCurrentContext(), GLFW_TRUE);
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Help"))
        {
            if (ImGui::MenuItem("About"))
            {
                if (about)
                    about->show();
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void setup_main_dockspace(bool& first_time)
{
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);

    constexpr ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus |
        ImGuiWindowFlags_MenuBar;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0,0));
    ImGui::Begin("MainDockSpace", nullptr, flags);
    ImGui::PopStyleVar(3);

    ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");

    if (first_time)
    {
        first_time = false;

        ImGui::DockBuilderRemoveNode(dockspace_id);
        ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

        ImGuiID center, right;
        ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Right, 0.5f, &right, &center);

        ImGuiDockNode* center_node = ImGui::DockBuilderGetNode(center);
        if (center_node)
        {
            center_node->LocalFlags |= ImGuiDockNodeFlags_CentralNode;
        }

        ImGui::DockBuilderDockWindow("Color Schemes", right);
        ImGui::DockBuilderDockWindow("Templates", center);
        ImGui::DockBuilderDockWindow("Color Preview", center);
        
        ImGui::DockBuilderFinish(dockspace_id);
    }

    ImGui::DockSpace(dockspace_id, ImVec2{0,0}, ImGuiDockNodeFlags_None);
    ImGui::End();
}

void end_frame(GLFWwindow* window)
{
    ImGui::Render();
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    glViewport(0, 0, w, h);
    glClearColor(0.1f, 0.1f, 0.1f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
}

void shutdown(GLFWwindow* window)
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
}