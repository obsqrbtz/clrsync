#ifndef CLRSYNC_CORE_CONFIG_HPP
#define CLRSYNC_CORE_CONFIG_HPP

#include <core/io/file.hpp>
#include <core/theme/theme_template.hpp>
#include <filesystem>
#include <memory>
#include <string>

namespace clrsync::core
{
class config
{
  public:
    static config &instance();

    void initialize(std::unique_ptr<clrsync::core::io::file> file);

    const std::string font() const;
    const uint32_t font_size() const;
    const std::string &palettes_path();
    const std::string default_theme() const;
    const std::unordered_map<std::string, clrsync::core::theme_template> templates();
    const clrsync::core::theme_template &template_by_name(const std::string &name) const;
    std::filesystem::path get_user_config_dir();

    
    void set_default_theme(const std::string &theme);
    void set_palettes_path(const std::string &path);
    void set_font(const std::string &font);
    void set_font_size(int font_size);

    void update_template(const std::string &key,
                         const clrsync::core::theme_template &theme_template);
    static std::filesystem::path get_data_dir();

  private:
    config() = default;
    config(const config &) = delete;
    config &operator=(const config &) = delete;

    std::string m_palettes_dir{};
    std::unique_ptr<io::file> m_file;
    std::unordered_map<std::string, theme_template> m_themes{};
    void copy_default_configs();
};
} // namespace clrsync::core

#endif