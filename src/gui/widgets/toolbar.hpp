#ifndef CLRSYNC_GUI_WIDGETS_TOOLBAR_HPP
#define CLRSYNC_GUI_WIDGETS_TOOLBAR_HPP

#include <functional>
#include <string>
#include <vector>

namespace clrsync::gui::widgets
{

enum class button_intent
{
    neutral,
    primary,
    danger,
    success,
    warning,
};

struct toolbar_item
{
    std::string label;
    std::string tooltip;
    std::function<void()> on_click;
    bool enabled = true;
    button_intent intent = button_intent::neutral;
};

class toolbar
{
  public:
    void render(const std::vector<toolbar_item> &items, const std::function<void()> &leading = {},
                float spacing = 8.0f);

    void set_id(const char *id)
    {
        m_id = id;
    }

  private:
    const char *m_id = "##toolbar";
};

void push_button_intent(button_intent intent);
void pop_button_intent(button_intent intent);

} // namespace clrsync::gui::widgets

#endif // CLRSYNC_GUI_WIDGETS_TOOLBAR_HPP
