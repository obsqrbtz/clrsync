#include "color.hpp"
#include <algorithm>
#include <cstdio>
#include <format>
#include <stdexcept>

namespace clrsync::core
{

uint32_t color::hex() const
{
    return m_hex;
}

rgb color::to_rgb() const
{
    rgb result{};
    result.r = (m_hex >> 24) & 0xFF;
    result.g = (m_hex >> 16) & 0xFF;
    result.b = m_hex >> 8;
    return result;
}

rgba color::to_rgba() const
{
    rgba result{};
    result.r = (m_hex >> 24) & 0xFF;
    result.g = (m_hex >> 16) & 0xFF;
    result.b = (m_hex >> 8) & 0xFF;
    result.a = m_hex & 0xFF;
    return result;
}

hsl color::to_hsl() const
{
    rgb c = to_rgb();
    float r = c.r / 255.0f;
    float g = c.g / 255.0f;
    float b = c.b / 255.0f;

    float max = std::max({r, g, b});
    float min = std::min({r, g, b});
    float h, s, l = (max + min) / 2.0f;

    if (max == min)
    {
        h = s = 0.0f;
    }
    else
    {
        float d = max - min;
        s = l > 0.5f ? d / (2.0f - max - min) : d / (max + min);
        if (max == r)
        {
            h = (g - b) / d + (g < b ? 6.0f : 0.0f);
        }
        else if (max == g)
        {
            h = (b - r) / d + 2.0f;
        }
        else
        {
            h = (r - g) / d + 4.0f;
        }
        h /= 6.0f;
    }

    return hsl{h * 360.0f, s, l};
}

hsla color::to_hsla() const
{
    rgba c = to_rgba();
    float r = c.r / 255.0f;
    float g = c.g / 255.0f;
    float b = c.b / 255.0f;
    float a = c.a / 255.0f;

    float max = std::max({r, g, b});
    float min = std::min({r, g, b});
    float h, s, l = (max + min) / 2.0f;

    if (max == min)
    {
        h = s = 0.0f;
    }
    else
    {
        float d = max - min;
        s = l > 0.5f ? d / (2.0f - max - min) : d / (max + min);
        if (max == r)
        {
            h = (g - b) / d + (g < b ? 6.0f : 0.0f);
        }
        else if (max == g)
        {
            h = (b - r) / d + 2.0f;
        }
        else
        {
            h = (r - g) / d + 4.0f;
        }
        h /= 6.0f;
    }

    return hsla{h * 360.0f, s, l, a};
}

void color::from_hex_string(const std::string &str)
{
    if (str.empty() || str[0] != '#')
        throw std::invalid_argument("Invalid hex color format");

    if (str.size() == 7) {
        uint32_t rgb = static_cast<uint32_t>(std::stoul(str.substr(1), nullptr, 16));
        m_hex = (rgb << 8) | 0xFF;
    }
    else if (str.size() == 9) {
        m_hex = static_cast<uint32_t>(std::stoul(str.substr(1), nullptr, 16));
    }
    else {
        throw std::invalid_argument("Invalid hex color format");
    }
}

const std::string color::to_hex_string() const
{
    char buffer[8];
    std::snprintf(buffer, sizeof(buffer), "#%06X", m_hex >> 8);
    return std::string(buffer);
}

const std::string color::to_hex_string_with_alpha() const
{
    char buffer[10];
    std::snprintf(buffer, sizeof(buffer), "#%08X", m_hex);
    return std::string(buffer);
}

std::string color::format(const std::string& field) const
{
    auto rgb  = to_rgb();
    auto rgba = to_rgba();
    auto hslv = to_hsl();
    auto hslav = to_hsla();

    if (field == "hex") return to_hex_string();
    if (field == "hex_stripped") {
        auto s = to_hex_string();
        return s.substr(1);
    }

    if (field == "hexa") return to_hex_string_with_alpha();
    if (field == "hexa_stripped") {
        auto s = to_hex_string_with_alpha();
        return s.substr(1);
    }

    if (field == "r") return std::to_string(rgb.r);
    if (field == "g") return std::to_string(rgb.g);
    if (field == "b") return std::to_string(rgb.b);

    if (field == "a") {
        float af = rgba.a / 255.0f;
        return std::format("{:.2f}", af);
    }

    if (field == "rgb")
        return std::format("rgb({},{},{})", rgb.r, rgb.g, rgb.b);

    if (field == "rgba")
        return std::format("rgba({},{},{},{:.2f})",
                           rgba.r, rgba.g, rgba.b,
                           rgba.a / 255.0f);

    if (field == "h")
        return std::format("{:.0f}", hslv.h);
    if (field == "s")
        return std::format("{:.2f}", hslv.s);
    if (field == "l")
        return std::format("{:.2f}", hslv.l);

    // TODO: probably unneded
    if (field == "hsla_a")
        return std::format("{:.2f}", hslav.a);

    if (field == "hsl")
        return std::format("hsl({:.0f},{:.2f},{:.2f})",
                           hslv.h, hslv.s, hslv.l);

    if (field == "hsla")
        return std::format("hsla({:.0f},{:.2f},{:.2f},{:.2f})",
                           hslav.h, hslav.s, hslav.l, hslav.a);

    throw std::runtime_error("Unknown color format: " + field);
}

void color::set(uint32_t hex)
{
    m_hex = hex;
}
} // namespace clrsync::core
