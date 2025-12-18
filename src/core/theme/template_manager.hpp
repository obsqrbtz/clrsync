#ifndef CLRSYNC_CORE_THEME_TEMPLATE_MANAGER_HPP
#define CLRSYNC_CORE_THEME_TEMPLATE_MANAGER_HPP

#include "core/config/config.hpp"
#include "core/theme/theme_template.hpp"
#include <string>
#include <unordered_map>

namespace clrsync::core
{

template <typename FileType> class template_manager
{
  public:
    template_manager() = default;
    std::unordered_map<std::string, theme_template> &templates()
    {
        auto themes = config::instance().templates();
        m_templates.clear();
        for (const auto &t : themes)
        {
            m_templates.insert({t.first, t.second});
        }
        return m_templates;
    }

  private:
    std::unordered_map<std::string, theme_template> m_templates{};
};

} // namespace clrsync::core

#endif