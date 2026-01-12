#ifndef CLRSYNC_CORE_THEME_THEME_RENDERER_HPP
#define CLRSYNC_CORE_THEME_THEME_RENDERER_HPP
#include "core/common/error.hpp"
#include "core/config/config.hpp"
#include "core/palette/palette_manager.hpp"
#include "core/theme/template_manager.hpp"
#include <iostream>
#include <string>
#include <thread>

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

    Result<void> apply_theme(const std::string &theme_name)
    {
        auto palette = m_pal_manager.get_palette(theme_name);
        if (!palette)
            return Err<void>(error_code::palette_not_found, "Palette not found", theme_name);
        return apply_palette_to_all_templates(*palette);
    }

    Result<void> apply_theme_from_path(const std::string &path)
    {
        auto palette = m_pal_manager.load_palette_from_file(path);
        return apply_palette_to_all_templates(palette);
    }

  private:
    palette_manager<FileType> m_pal_manager;
    template_manager<FileType> m_template_manager;

    Result<void> apply_palette_to_all_templates(const palette &pal)
    {
        for (auto &t_pair : m_template_manager.templates())
        {
            auto &tmpl = t_pair.second;
            if (!tmpl.enabled())
                continue;

            auto load_result = tmpl.load_template();
            if (!load_result)
                return load_result;

            tmpl.apply_palette(pal);

            auto save_result = tmpl.save_output();
            if (!save_result)
                return save_result;

            if (!tmpl.reload_command().empty())
            {
                std::string cmd = tmpl.reload_command();
                std::thread([cmd]() {
                    int result = std::system(cmd.c_str());
                    if (result != 0)
                    {
                        std::cerr << "Warning: Reload command '" << cmd
                                  << "' failed with code " << result << "\n";
                    }
                }).detach();
            }
        }
        return Ok();
    }
};

} // namespace clrsync::core

#endif