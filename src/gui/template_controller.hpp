#ifndef CLRSYNC_GUI_TEMPLATE_CONTROLLER_HPP
#define CLRSYNC_GUI_TEMPLATE_CONTROLLER_HPP

#include "core/theme/theme_template.hpp"
#include "core/theme/template_manager.hpp"
#include "core/io/toml_file.hpp"
#include <string>
#include <unordered_map>

class template_controller {
public:
    template_controller();
    [[nodiscard]] const std::unordered_map<std::string, clrsync::core::theme_template>& templates() const { return m_templates; }
    void set_template_enabled(const std::string& key, bool enabled);
    void set_template_output_path(const std::string& key, const std::string& path);
    void set_template_reload_command(const std::string& key, const std::string& cmd);
    void refresh();
    
private:
    clrsync::core::template_manager<clrsync::core::io::toml_file> m_template_manager;
    std::unordered_map<std::string, clrsync::core::theme_template> m_templates;
};

#endif // CLRSYNC_GUI_TEMPLATE_CONTROLLER_HPP
