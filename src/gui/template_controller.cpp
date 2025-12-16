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
        (void)clrsync::core::config::instance().update_template(key, it->second);
    }
}

void template_controller::set_template_input_path(const std::string& key, const std::string& path)
{
    auto it = m_templates.find(key);
    if (it != m_templates.end()) {
        it->second.set_template_path(path);
        (void)clrsync::core::config::instance().update_template(key, it->second);
    }
}

void template_controller::set_template_output_path(const std::string& key, const std::string& path)
{
    auto it = m_templates.find(key);
    if (it != m_templates.end()) {
        it->second.set_output_path(path);
        (void)clrsync::core::config::instance().update_template(key, it->second);
    }
}

void template_controller::set_template_reload_command(const std::string& key, const std::string& cmd)
{
    auto it = m_templates.find(key);
    if (it != m_templates.end()) {
        it->second.set_reload_command(cmd);
        (void)clrsync::core::config::instance().update_template(key, it->second);
    }
}

bool template_controller::remove_template(const std::string& key)
{
    auto result = clrsync::core::config::instance().remove_template(key);
    if (result) {
        m_templates.erase(key);
        return true;
    }
    return false;
}

void template_controller::refresh()
{
    m_templates = m_template_manager.templates();
}
