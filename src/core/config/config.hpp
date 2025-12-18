#ifndef CLRSYNC_CORE_CONFIG_HPP
#define CLRSYNC_CORE_CONFIG_HPP

#include "core/common/error.hpp"
#include "core/io/file.hpp"
#include "core/theme/theme_template.hpp"
#include <filesystem>
#include <memory>
#include <string>

namespace clrsync::core
{
class config
{
  public:
    static config &instance();

    Result<void> initialize(std::unique_ptr<clrsync::core::io::file> file);

    const std::string font() const;
    const uint32_t font_size() const;
    const std::string &palettes_path();
    const std::string default_theme() const;
    const std::unordered_map<std::string, clrsync::core::theme_template> templates();
    Result<const clrsync::core::theme_template *> template_by_name(const std::string &name) const;
    std::filesystem::path get_user_config_dir();

    Result<void> set_default_theme(const std::string &theme);
    Result<void> set_palettes_path(const std::string &path);
    Result<void> set_font(const std::string &font);
    Result<void> set_font_size(int font_size);

    Result<void> update_template(const std::string &key,
                                 const clrsync::core::theme_template &theme_template);
    Result<void> remove_template(const std::string &key);
    static std::filesystem::path get_data_dir();

  private:
    config() = default;
    config(const config &) = delete;
    config &operator=(const config &) = delete;

    std::string m_palettes_dir{};
    std::unique_ptr<io::file> m_file;
    std::unordered_map<std::string, theme_template> m_themes{};
    static void copy_file(const std::filesystem::path &src, const std::filesystem::path &dst);
    static void copy_dir(const std::filesystem::path &src, const std::filesystem::path &dst);
    void copy_default_configs();
};
} // namespace clrsync::core

#endif