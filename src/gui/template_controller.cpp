#include "template_controller.hpp"
#include "core/config/config.hpp"

template_controller::template_controller()
{
    m_templates = m_template_manager.templates();
}

void template_controller::set_template_enabled(const std::string& key, bool enabled)
{
    auto it = m_templates.find(key);
    if (it != m_templates.end()) {
        it->second.set_enabled(enabled);
        clrsync::core::config::instance().update_template(key, it->second);
    }
}

void template_controller::set_template_output_path(const std::string& key, const std::string& path)
{
    auto it = m_templates.find(key);
    if (it != m_templates.end()) {
        it->second.set_output_path(path);
        clrsync::core::config::instance().update_template(key, it->second);
    }
}

void template_controller::set_template_reload_command(const std::string& key, const std::string& cmd)
{
    auto it = m_templates.find(key);
    if (it != m_templates.end()) {
        it->second.set_reload_command(cmd);
        clrsync::core::config::instance().update_template(key, it->second);
    }
}

void template_controller::refresh()
{
    m_templates = m_template_manager.templates();
}
