#include "theme_template.hpp"
#include "core/common/utils.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>

namespace clrsync::core
{
theme_template::theme_template(const std::string &name, const std::string &template_path,
                               const std::string &out_path)
    : m_name(name), m_template_path(normalize_path(template_path).string()),
      m_output_path(normalize_path(out_path).string())
{
}

const std::string &theme_template::name() const
{
    return m_name;
}

void theme_template::set_name(const std::string &name)
{
    m_name = name;
}

const std::string &theme_template::template_path() const
{
    return m_template_path;
}

void theme_template::set_template_path(const std::string &path)
{
    m_template_path = normalize_path(path).string();
}

const std::string &theme_template::output_path() const
{
    return m_output_path;
}

void theme_template::set_output_path(const std::string &path)
{
    m_output_path = normalize_path(path).string();
}

Result<void> theme_template::load_template()
{
    if (!std::filesystem::exists(m_template_path))
    {
        return Err<void>(error_code::template_not_found, "Template file is missing",
                         m_template_path);
    }

    std::ifstream input(m_template_path, std::ios::binary);
    if (!input)
    {
        return Err<void>(error_code::template_load_failed, "Failed to open template file",
                         m_template_path);
    }

    m_template_data.assign(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>());
    return Ok();
}

void theme_template::apply_palette(const core::palette &palette)
{
    m_processed_data = m_template_data;

    for (const auto &[key, color] : palette.colors())
    {
        // simple replacement: {foreground}
        replace_all(m_processed_data, "{" + key + "}", color.format("hex"));

        // mutli-component: {foreground.r}, {foreground.rgb}, etc.
        std::string prefix = "{" + key + ".";
        size_t pos = 0;
        while ((pos = m_processed_data.find(prefix, pos)) != std::string::npos)
        {
            size_t end = m_processed_data.find('}', pos);
            if (end == std::string::npos)
                break;

            const size_t field_start = pos + prefix.size();
            std::string field = m_processed_data.substr(field_start, end - field_start);

            std::string value = color.format(field);

            m_processed_data.replace(pos, end - pos + 1, value);

            pos += value.size();
        }
    }
}

Result<void> theme_template::save_output() const
{
    try
    {
        std::filesystem::create_directories(std::filesystem::path(m_output_path).parent_path());
    }
    catch (const std::exception &e)
    {
        return Err<void>(error_code::dir_create_failed, e.what(), m_output_path);
    }

    std::ofstream output(m_output_path, std::ios::binary);
    if (!output)
    {
        return Err<void>(error_code::file_write_failed, "Failed to open output file for writing",
                         m_output_path);
    }

    output << m_processed_data;
    if (!output)
    {
        return Err<void>(error_code::file_write_failed, "Failed to write to output file",
                         m_output_path);
    }

    return Ok();
}

const std::string &theme_template::raw_template() const
{
    return m_template_data;
}

const std::string &theme_template::processed_template() const
{
    return m_processed_data;
}

void theme_template::replace_all(std::string &str, const std::string &from, const std::string &to)
{
    if (from.empty())
        return;

    size_t pos = 0;
    while ((pos = str.find(from, pos)) != std::string::npos)
    {
        str.replace(pos, from.length(), to);
        pos += to.length();
    }
}

const std::string &theme_template::reload_command() const
{
    return m_reload_cmd;
}

void theme_template::set_reload_command(const std::string &cmd)
{
    m_reload_cmd = cmd;
}

bool theme_template::enabled() const
{
    return m_enabled;
}

void theme_template::set_enabled(bool enabled)
{
    m_enabled = enabled;
}

} // namespace clrsync::core
