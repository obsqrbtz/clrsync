#include "input_dialog.hpp"
#include "imgui.h"
#include <cstring>
#include <string>

namespace clrsync::gui::widgets
{

input_dialog::input_dialog() : m_input_buffer{0}, m_is_open(false), m_focus_input(false) {}

void input_dialog::open(const std::string &title, const std::string &prompt, const std::string &hint)
{
    m_title = title;
    m_prompt = prompt;
    m_hint = hint;
    m_input_buffer[0] = 0;
    m_is_open = true;
    m_focus_input = true;
    ImGui::OpenPopup(m_title.c_str());
}

bool input_dialog::render()
{
    bool submitted = false;
    
    if (!m_is_open)
        return false;

    if (ImGui::BeginPopupModal(m_title.c_str(), &m_is_open, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("%s", m_prompt.c_str());
        ImGui::Spacing();

        ImGui::SetNextItemWidth(250);
        if (m_focus_input)
        {
            ImGui::SetKeyboardFocusHere();
            m_focus_input = false;
        }

        bool enter_pressed = ImGui::InputTextWithHint("##input", m_hint.c_str(), m_input_buffer,
                                                     IM_ARRAYSIZE(m_input_buffer), 
                                                     ImGuiInputTextFlags_EnterReturnsTrue);

        ImGui::SameLine();
        if (ImGui::Button("Browse"))
        {
            if (m_on_browse)
            {
                std::string initial = m_input_buffer;
                std::string res = m_on_browse(initial);
                if (!res.empty())
                {
                    std::strncpy(m_input_buffer, res.c_str(), IM_ARRAYSIZE(m_input_buffer) - 1);
                    m_input_buffer[IM_ARRAYSIZE(m_input_buffer) - 1] = '\0';
                }
            }
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        bool can_submit = strlen(m_input_buffer) > 0;
        bool submit_clicked = false;

        if (!can_submit)
            ImGui::BeginDisabled();

        if (ImGui::Button("OK", ImVec2(120, 0)) || (enter_pressed && can_submit))
        {
            if (m_on_submit)
            {
                m_on_submit(m_input_buffer);
            }
            submitted = true;
            submit_clicked = true;
            m_is_open = false;
            ImGui::CloseCurrentPopup();
        }

        if (!can_submit)
            ImGui::EndDisabled();

        ImGui::SameLine();

        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            if (m_on_cancel)
            {
                m_on_cancel();
            }
            m_is_open = false;
            ImGui::CloseCurrentPopup();
        }

        if (ImGui::IsKeyPressed(ImGuiKey_Escape))
        {
            if (m_on_cancel)
            {
                m_on_cancel();
            }
            m_is_open = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
    else
    {
        m_is_open = false;
    }

    return submitted;
}

void input_dialog::set_on_submit(const std::function<void(const std::string &)> &callback)
{
    m_on_submit = callback;
}

void input_dialog::set_on_cancel(const std::function<void()> &callback)
{
    m_on_cancel = callback;
}

void input_dialog::set_path_browse_callback(const std::function<std::string(const std::string &)> &callback)
{
    m_on_browse = callback;
}

} // namespace clrsync::gui::widgets