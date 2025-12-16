#include "toml_file.hpp"
#include "core/utils.hpp"
#include <filesystem>
#include <fstream>
#include <vector>

namespace clrsync::core::io
{
toml_file::toml_file(std::string path)
{
    m_path = expand_user(path);
}

Result<void> toml_file::parse()
{
    if (!std::filesystem::exists(m_path))
        return Err<void>(error_code::file_not_found, "File does not exist", m_path);
    
    m_file = toml::parse_file(m_path);
    return Ok();
}

const std::string toml_file::get_string_value(const std::string &section,
                                              const std::string &key) const
{
    return m_file[section][key].value_or("");
}

uint32_t toml_file::get_uint_value(const std::string &section, const std::string &key) const
{
    return m_file[section][key].value_or(0);
}

uint32_t toml_file::get_bool_value(const std::string &section, const std::string &key) const
{
    return m_file[section][key].value_or(false);
}

std::map<std::string, value_type> toml_file::get_table(const std::string &section_path) const
{
    auto parts = split(section_path, '.');
    const toml::table *tbl = m_file.as_table();

    for (const auto &part : parts)
    {
        if (auto subtbl = tbl->at(part).as_table())
            tbl = subtbl;
        else
            return {};
    }

    std::map<std::string, value_type> result;

    for (const auto &p : *tbl)
    {
        const auto &val = p.second;

        if (auto b = val.value<bool>())
            result[std::string(p.first.str())] = *b;
        else if (auto s = val.value<std::string>())
            result[std::string(p.first.str())] = *s;
        else if (auto i = val.value<int64_t>())
            result[std::string(p.first.str())] = static_cast<uint32_t>(*i);
        else if (auto d = val.value<double>())
            result[std::string(p.first.str())] = static_cast<uint32_t>(*d);
        else
            result[std::string(p.first.str())] = {}; // fallback for unsupported types
    }

    return result;
}

void toml_file::insert_or_update_value(const std::string &section, const std::string &key,
                                       const value_type &value)
{
    toml::table *tbl = m_file.as_table();

    std::stringstream ss(section);
    std::string part;

    while (std::getline(ss, part, '.'))
    {
        auto *sub = (*tbl)[part].as_table();
        if (!sub)
        {
            (*tbl).insert_or_assign(part, toml::table{});
            sub = (*tbl)[part].as_table();
        }
        tbl = sub;
    }

    std::visit([&](auto &&v) { tbl->insert_or_assign(key, v); }, value);
}

Result<void> toml_file::save_file()
{
    try {
        std::filesystem::create_directories(std::filesystem::path(m_path).parent_path());
    } catch (const std::exception& e) {
        return Err<void>(error_code::dir_create_failed, e.what(), m_path);
    }
    
    std::ofstream stream(m_path, std::ios::binary);
    if (!stream)
        return Err<void>(error_code::file_write_failed, "Failed to open file for writing", m_path);
    
    stream << m_file;
    if (!stream)
        return Err<void>(error_code::file_write_failed, "Failed to write to file", m_path);
    
    return Ok();
}

std::vector<std::string> toml_file::split(const std::string &s, char delim) const
{
    std::vector<std::string> result;
    std::string current;
    for (char c : s)
    {
        if (c == delim)
        {
            if (!current.empty())
            {
                result.push_back(current);
                current.clear();
            }
        }
        else
        {
            current += c;
        }
    }
    if (!current.empty())
        result.push_back(current);
    return result;
}
} // namespace clrsync::core::io
