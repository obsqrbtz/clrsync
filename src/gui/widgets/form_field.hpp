#ifndef CLRSYNC_GUI_WIDGETS_FORM_FIELD_HPP
#define CLRSYNC_GUI_WIDGETS_FORM_FIELD_HPP

#include <functional>
#include <string>

namespace clrsync::gui::widgets
{

enum class field_type
{
    text,
    text_with_hint,
    number,
    path,
    readonly_text
};

struct form_field_config
{
    std::string label;
    std::string tooltip;
    float label_width = 80.0f;
    float field_width = -1.0f;
    bool required = false;
    field_type type = field_type::text;
    std::string hint;
    float min_value = 0.0f;
    float max_value = 100.0f;
};

class form_field
{
  public:
    form_field();

    // Render a text input field
    bool render_text(const form_field_config& config, std::string& value);
    
    // Render a number input field
    bool render_number(const form_field_config& config, int& value);
    bool render_number(const form_field_config& config, float& value);
    
    // Render a path input field with browse button
    bool render_path(const form_field_config& config, std::string& value);
    
    // Render readonly text
    void render_readonly(const form_field_config& config, const std::string& value);

    // Set callback for path browsing
    void set_path_browse_callback(const std::function<std::string(const std::string&)>& callback);

  private:
    std::function<std::string(const std::string&)> m_path_browse_callback;
    
    void render_label(const form_field_config& config);
    void render_tooltip(const form_field_config& config);
};

} // namespace clrsync::gui::widgets

#endif // CLRSYNC_GUI_WIDGETS_FORM_FIELD_HPP