#include "colors.hpp"

namespace clrsync::gui::widgets
{

ImVec4 palette_color(const core::palette &pal, const std::string &key,
                     const std::string &fallback)
{
    auto colors = pal.colors();
    if (colors.empty())
        return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

    auto it = colors.find(key);
    if (it == colors.end() && !fallback.empty())
    {
        it = colors.find(fallback);
    }

    if (it != colors.end())
    {
        const auto &col = it->second;
        const uint32_t hex = col.hex();
        const float r = ((hex >> 24) & 0xFF) / 255.0f;
        const float g = ((hex >> 16) & 0xFF) / 255.0f;
        const float b = ((hex >> 8) & 0xFF) / 255.0f;
        const float a = (hex & 0xFF) / 255.0f;
        return ImVec4(r, g, b, a);
    }
    return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
}

uint32_t palette_color_u32(const core::palette &pal, const std::string &key,
                           const std::string &fallback)
{
    auto colors = pal.colors();
    if (colors.empty())
        return 0xFFFFFFFF;

    auto it = colors.find(key);
    if (it == colors.end() && !fallback.empty())
    {
        it = colors.find(fallback);
    }

    if (it != colors.end())
    {
        const auto &col = it->second;
        const uint32_t hex = col.hex();
        const uint32_t r = (hex >> 24) & 0xFF;
        const uint32_t g = (hex >> 16) & 0xFF;
        const uint32_t b = (hex >> 8) & 0xFF;
        const uint32_t a = hex & 0xFF;
        return (a << 24) | (b << 16) | (g << 8) | r;
    }
    return 0xFFFFFFFF;
}

} // namespace clrsync::gui::widgets
