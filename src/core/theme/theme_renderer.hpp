#ifndef CLRSYNC_CORE_THEME_THEME_RENDERER_HPP
#define CLRSYNC_CORE_THEME_THEME_RENDERER_HPP
#include <core/config/config.hpp>
#include <core/palette/palette_manager.hpp>
#include <core/theme/template_manager.hpp>
#include <string>

namespace clrsync::core
{

template <typename FileType> class theme_renderer
{
  public:
    theme_renderer()
    {
        auto &cfg = config::instance();
        m_pal_manager.load_palettes_from_directory(cfg.palettes_path());
        m_template_manager = template_manager<FileType>();
    }

    void apply_theme(const std::string &theme_name)
    {
        auto palette = m_pal_manager.get_palette(theme_name);
        if (!palette)
            throw std::runtime_error("Palette not found: " + theme_name);
        apply_palette_to_all_templates(*palette);
    }
    void apply_theme_from_path(const std::string &path)
    {
        auto palette = m_pal_manager.load_palette_from_file(path);
        apply_palette_to_all_templates(palette);
    }

  private:
    palette_manager<FileType> m_pal_manager;
    template_manager<FileType> m_template_manager;

    void apply_palette_to_all_templates(const palette &pal)
    {
        for (auto &t_pair : m_template_manager.templates())
        {
            auto &tmpl = t_pair.second;
            if (!tmpl.enabled())
                continue;
            tmpl.load_template();
            tmpl.apply_palette(pal);
            tmpl.save_output();
            if (!tmpl.reload_command().empty())
            {
                std::system(tmpl.reload_command().c_str());
            }
        }
    }
};

} // namespace clrsync::core

#endif