#ifndef CLRSYNC_GUI_WIDGETS_INPUT_DIALOG_HPP
#define CLRSYNC_GUI_WIDGETS_INPUT_DIALOG_HPP

#include <functional>
#include <string>

namespace clrsync::gui::widgets
{

class input_dialog
{
  public:
    input_dialog();

    void open(const std::string &title, const std::string &prompt, const std::string &hint = "");

    bool render();

    void set_on_submit(const std::function<void(const std::string &)> &callback);
    void set_on_cancel(const std::function<void()> &callback);

    bool is_open() const { return m_is_open; }

  private:
    std::string m_title;
    std::string m_prompt;
    std::string m_hint;
    char m_input_buffer[256];
    bool m_is_open;
    bool m_focus_input;
    
    std::function<void(const std::string &)> m_on_submit;
    std::function<void()> m_on_cancel;
};

} // namespace clrsync::gui::widgets

#endif // CLRSYNC_GUI_WIDGETS_INPUT_DIALOG_HPP