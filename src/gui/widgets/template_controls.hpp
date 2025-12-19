#ifndef CLRSYNC_GUI_WIDGETS_TEMPLATE_CONTROLS_HPP
#define CLRSYNC_GUI_WIDGETS_TEMPLATE_CONTROLS_HPP

#include "core/palette/palette.hpp"
#include "gui/widgets/form_field.hpp"
#include "gui/widgets/validation_message.hpp"
#include <functional>
#include <string>

namespace clrsync::gui::widgets
{

struct template_control_state
{
    std::string name;
    std::string input_path;
    std::string output_path;
    std::string reload_command;
    bool enabled{true};
    bool is_editing_existing{false};
};

struct template_control_callbacks
{
    std::function<void()> on_new;
    std::function<void()> on_save;
    std::function<void()> on_delete;
    std::function<void(bool)> on_enabled_changed;
    std::function<std::string(const std::string&)> on_browse_input;
    std::function<std::string(const std::string&)> on_browse_output;
    std::function<void(const std::string&)> on_input_path_changed;
    std::function<void(const std::string&)> on_output_path_changed;
    std::function<void(const std::string&)> on_reload_command_changed;
};

class template_controls
{
  public:
    template_controls();

    void render(template_control_state& state,
                const template_control_callbacks& callbacks,
                const core::palette& palette,
                validation_message& validation);

  private:
    void render_action_buttons(template_control_state& state,
                               const template_control_callbacks& callbacks,
                               const core::palette& palette);

    void render_fields(template_control_state& state,
                       const template_control_callbacks& callbacks);

    form_field m_form;
};

} // namespace clrsync::gui::widgets

#endif // CLRSYNC_GUI_WIDGETS_TEMPLATE_CONTROLS_HPP
