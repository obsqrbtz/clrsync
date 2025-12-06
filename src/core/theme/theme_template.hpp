#ifndef clrsync_CORE_IO_THEME_TEMPLATE_HPP
#define clrsync_CORE_IO_THEME_TEMPLATE_HPP

#include <core/palette/palette.hpp>
#include <string>

namespace clrsync::core
{

class theme_template
{
  public:
    theme_template() = default;
    theme_template(const std::string &name, const std::string &template_path,
                   const std::string &out_path);

    const std::string &name() const;

    void set_name(const std::string &name);

    const std::string &template_path() const;

    void set_template_path(const std::string &path);

    const std::string &output_path() const;

    void set_output_path(const std::string &path);

    void load_template();

    void apply_palette(const core::palette &palette);

    void save_output() const;

    const std::string &raw_template() const;

    const std::string &processed_template() const;

    const std::string &reload_command() const;

    void set_reload_command(const std::string &cmd);

    bool enabled() const;

    void set_enabled(bool enabled);

  private:
    std::string m_name{};
    std::string m_template_path{};
    std::string m_output_path{};
    bool m_enabled = true;
    std::string m_template_data{};
    std::string m_processed_data{};
    std::string m_reload_cmd{};

    static void replace_all(std::string &str, const std::string &from, const std::string &to);
};
} // namespace clrsync::core

#endif