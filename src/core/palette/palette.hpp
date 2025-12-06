#ifndef CLRSYNC_CORE_PALETTE_PALETTE_HPP
#define CLRSYNC_CORE_PALETTE_PALETTE_HPP

#include <string>
#include <unordered_map>

#include <core/palette/color.hpp>

namespace clrsync::core
{
class palette
{
  public:
    palette() = default;
    palette(const std::string &name) : m_name(name)
    {
    }

    const std::string &file_path() const
    {
        return m_file_path;
    }

    void set_file_path(const std::string &path)
    {
        m_file_path = path;
    }

    const std::string &name() const
    {
        return m_name;
    }
    const color &get_color(const std::string &key) const
    {
        auto it = m_colors.find(key);
        if (it != m_colors.end())
        {
            return it->second;
        }
        static color default_color{};
        return default_color;
    }

    const std::unordered_map<std::string, color> &colors() const
    {
        return m_colors;
    }

    void set_name(const std::string &name)
    {
        m_name = name;
    }
    void set_color(const std::string &key, const color &col)
    {
        m_colors[key] = col;
    }

  private:
    std::string m_name{};
    std::unordered_map<std::string, color> m_colors{};
    std::string m_file_path{};
};
} // namespace clrsync::core
#endif