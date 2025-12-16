#ifndef CLRSYNC_CORE_IO_TOML_FILE_HPP
#define CLRSYNC_CORE_IO_TOML_FILE_HPP
#include <core/io/file.hpp>
#include <core/error.hpp>
#include <string>
#include <toml/toml.hpp>

namespace clrsync::core::io
{
class toml_file : public file
{
  public:
    explicit toml_file(std::string path);
    Result<void> parse() override;
    const std::string get_string_value(const std::string &section,
                                       const std::string &key) const override;
    uint32_t get_uint_value(const std::string &section, const std::string &key) const override;
    uint32_t get_bool_value(const std::string &section, const std::string &key) const override;
    std::map<std::string, value_type> get_table(const std::string &section_path) const override;
    void insert_or_update_value(const std::string &section, const std::string &key,
                                const value_type &value) override;
    Result<void> save_file() override;

  private:
    toml::parse_result m_file{};
    std::string m_path{};

    std::vector<std::string> split(const std::string &s, char delim) const;
};
} // namespace clrsync::core::io

#endif