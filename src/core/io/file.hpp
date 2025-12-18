#ifndef CLRSYNC_CORE_IO_FILE_HPP
#define CLRSYNC_CORE_IO_FILE_HPP
#include "core/common/error.hpp"
#include <cstdint>
#include <map>
#include <string>
#include <variant>

using value_type = std::variant<std::string, uint32_t, int, bool>;

namespace clrsync::core::io
{
class file
{
  public:
    file() = default;
    file(std::string path) {};
    virtual ~file() = default;
    virtual Result<void> parse()
    {
        return Ok();
    };
    virtual const std::string get_string_value(const std::string &section,
                                               const std::string &key) const
    {
        return {};
    }
    virtual uint32_t get_uint_value(const std::string &section, const std::string &key) const
    {
        return {};
    }
    virtual uint32_t get_bool_value(const std::string &section, const std::string &key) const
    {
        return {};
    }
    virtual std::map<std::string, value_type> get_table(const std::string &section_path) const
    {
        return {};
    }
    virtual void set_value(const std::string &section, const std::string &key,
                           const value_type &value)
    {
        insert_or_update_value(section, key, value);
    }
    virtual void insert_or_update_value(const std::string &section, const std::string &key,
                                        const value_type &value) {};
    virtual void remove_section(const std::string &section) {};
    virtual Result<void> save_file()
    {
        return Ok();
    };
};
} // namespace clrsync::core::io
#endif