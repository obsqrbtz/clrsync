#ifndef CLRSYNC_CORE_PALETTE_GENERATOR_HPP
#define CLRSYNC_CORE_PALETTE_GENERATOR_HPP

#include <algorithm>
#include <initializer_list>
#include <stdexcept>
#include <string>
#include <vector>
#include "core/palette/palette.hpp"

namespace clrsync::core
{
class generator
{
  public:
    enum class system
    {
        windows,
        linux,
        macos,
        unknown
    };

    explicit generator(std::initializer_list<system> supported_systems)
        : m_supported_systems(supported_systems)
    {
    }

    virtual ~generator() = default;

    virtual palette generate_from_image(const std::string &image_path) = 0;

    [[nodiscard]] bool supports_current_system() const
    {
        return supports_system(current_system());
    }

    [[nodiscard]] bool supports_system(system value) const
    {
        return std::find(m_supported_systems.begin(), m_supported_systems.end(), value) !=
               m_supported_systems.end();
    }

    [[nodiscard]] const std::vector<system> &supported_systems() const noexcept
    {
        return m_supported_systems;
    }

    [[nodiscard]] std::string supported_systems_description() const
    {
        std::string out;
        for (const auto system : m_supported_systems)
        {
            if (!out.empty())
                out += ", ";
            out += system_name(system);
        }
        return out;
    }

    static system current_system() noexcept
    {
#ifdef _WIN32
        return system::windows;
#elif defined(__APPLE__)
        return system::macos;
#elif defined(__linux__)
        return system::linux;
#else
        return system::unknown;
#endif
    }

    static std::string system_name(system value)
    {
        switch (value)
        {
        case system::windows:
            return "Windows";
        case system::linux:
            return "Linux";
        case system::macos:
            return "macOS";
        default:
            return "unknown";
        }
    }

    void ensure_supported(const std::string &generator_name) const
    {
        if (supports_current_system())
            return;

        throw std::runtime_error(generator_name + " is not supported on " +
                                 system_name(current_system()) +
                                 ". Supported systems: " + supported_systems_description());
    }

  protected:
    std::vector<system> m_supported_systems;
};
} // namespace clrsync::core

#endif
