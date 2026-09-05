#include "core/palette/color_ops.hpp"

#include <algorithm>
#include <cmath>

namespace clrsync::core::color_ops
{
namespace
{

constexpr float PI_F = 3.14159265358979323846f;

float clamp01(float v)
{
    return std::clamp(v, 0.0f, 1.0f);
}

float srgb_to_linear(float channel)
{
    return channel <= 0.04045f ? channel / 12.92f : std::pow((channel + 0.055f) / 1.055f, 2.4f);
}

float linear_to_srgb(float channel)
{
    return channel <= 0.0031308f ? channel * 12.92f
                                 : 1.055f * std::pow(channel, 1.0f / 2.4f) - 0.055f;
}

uint8_t to_byte(float channel)
{
    return static_cast<uint8_t>(std::lround(clamp01(channel) * 255.0f));
}

} // namespace

oklab srgb_to_oklab(const rgb &value)
{
    const float r = srgb_to_linear(value.r / 255.0f);
    const float g = srgb_to_linear(value.g / 255.0f);
    const float b = srgb_to_linear(value.b / 255.0f);

    const float l = 0.4122214708f * r + 0.5363325363f * g + 0.0514459929f * b;
    const float m = 0.2119034982f * r + 0.6806995451f * g + 0.1073969566f * b;
    const float s = 0.0883024619f * r + 0.2817188376f * g + 0.6299787005f * b;

    const float l_ = std::cbrt(l);
    const float m_ = std::cbrt(m);
    const float s_ = std::cbrt(s);

    return oklab{
        0.2104542553f * l_ + 0.7936177850f * m_ - 0.0040720468f * s_,
        1.9779984951f * l_ - 2.4285922050f * m_ + 0.4505937099f * s_,
        0.0259040371f * l_ + 0.7827717662f * m_ - 0.8086757660f * s_,
    };
}

rgb oklab_to_srgb(const oklab &value)
{
    const float l_ = value.l + 0.3963377774f * value.a + 0.2158037573f * value.b;
    const float m_ = value.l - 0.1055613458f * value.a - 0.0638541728f * value.b;
    const float s_ = value.l - 0.0894841775f * value.a - 1.2914855480f * value.b;

    const float l = l_ * l_ * l_;
    const float m = m_ * m_ * m_;
    const float s = s_ * s_ * s_;

    const float r = 4.0767416621f * l - 3.3077115913f * m + 0.2309699292f * s;
    const float g = -1.2684380046f * l + 2.6097574011f * m - 0.3413193965f * s;
    const float b = -0.0041960863f * l - 0.7034186147f * m + 1.7076147010f * s;

    return rgb{to_byte(linear_to_srgb(r)), to_byte(linear_to_srgb(g)), to_byte(linear_to_srgb(b))};
}

oklch oklab_to_oklch(const oklab &value)
{
    const float c = std::sqrt(value.a * value.a + value.b * value.b);
    float h = std::atan2(value.b, value.a) * 180.0f / PI_F;
    if (h < 0.0f)
        h += 360.0f;
    return oklch{value.l, c, h};
}

oklab oklch_to_oklab(const oklch &value)
{
    const float rad = value.h * PI_F / 180.0f;
    return oklab{value.l, value.c * std::cos(rad), value.c * std::sin(rad)};
}

oklch to_oklch(const color &value)
{
    return oklab_to_oklch(srgb_to_oklab(value.to_rgb()));
}

color from_oklch(const oklch &value, uint8_t alpha)
{
    const rgb out = oklab_to_srgb(oklch_to_oklab(value));
    color result;
    result.set((static_cast<uint32_t>(out.r) << 24) | (static_cast<uint32_t>(out.g) << 16) |
               (static_cast<uint32_t>(out.b) << 8) | alpha);
    return result;
}

float relative_luminance(const color &value)
{
    const rgb c = value.to_rgb();
    const float r = srgb_to_linear(c.r / 255.0f);
    const float g = srgb_to_linear(c.g / 255.0f);
    const float b = srgb_to_linear(c.b / 255.0f);
    return 0.2126f * r + 0.7152f * g + 0.0722f * b;
}

float contrast_ratio(const color &a, const color &b)
{
    const float la = relative_luminance(a);
    const float lb = relative_luminance(b);
    const float lighter = std::max(la, lb);
    const float darker = std::min(la, lb);
    return (lighter + 0.05f) / (darker + 0.05f);
}

float lightness(const color &value)
{
    return to_oklch(value).l;
}

float chroma(const color &value)
{
    return to_oklch(value).c;
}

float hue(const color &value)
{
    return to_oklch(value).h;
}

color with_lightness(const color &value, float l)
{
    oklch c = to_oklch(value);
    c.l = clamp01(l);
    return from_oklch(c, value.to_rgba().a);
}

color with_chroma(const color &value, float c_value)
{
    oklch c = to_oklch(value);
    c.c = std::max(0.0f, c_value);
    return from_oklch(c, value.to_rgba().a);
}

color adjust_lightness(const color &value, float delta)
{
    return with_lightness(value, lightness(value) + delta);
}

bool is_dark(const color &value)
{
    return relative_luminance(value) < 0.18f;
}

color step_surface(const color &value, float amount, bool dark_theme)
{
    return adjust_lightness(value, dark_theme ? amount : -amount);
}

color ensure_contrast(const color &fg, const color &bg, float target)
{
    if (contrast_ratio(fg, bg) >= target)
        return fg;

    const bool go_lighter = relative_luminance(bg) < 0.5f;
    const oklch base = to_oklch(fg);

    float lo = base.l;
    float hi = go_lighter ? 1.0f : 0.0f;

    color best = from_oklch(oklch{go_lighter ? 1.0f : 0.0f, base.c, base.h}, fg.to_rgba().a);
    if (contrast_ratio(best, bg) < target)
        return best;

    for (int i = 0; i < 24; ++i)
    {
        const float mid = (lo + hi) * 0.5f;
        const color candidate = from_oklch(oklch{mid, base.c, base.h}, fg.to_rgba().a);
        if (contrast_ratio(candidate, bg) >= target)
        {
            best = candidate;
            hi = mid;
        }
        else
        {
            lo = mid;
        }
    }
    return best;
}

float hue_distance(float a, float b)
{
    float diff = std::fabs(a - b);
    while (diff > 360.0f)
        diff -= 360.0f;
    return diff > 180.0f ? 360.0f - diff : diff;
}

bool nearest_hue(const std::vector<color> &candidates, float target_hue, float min_chroma,
                 color &out)
{
    bool found = false;
    float best_distance = 0.0f;

    for (const color &candidate : candidates)
    {
        const oklch c = to_oklch(candidate);
        if (c.c < min_chroma)
            continue;

        const float distance = hue_distance(c.h, target_hue);
        if (!found || distance < best_distance)
        {
            found = true;
            best_distance = distance;
            out = candidate;
        }
    }

    return found;
}

color synthesize_hue(const color &reference, float target_hue, float min_chroma)
{
    oklch c = to_oklch(reference);
    c.h = target_hue;
    c.c = std::max(c.c, min_chroma);
    return from_oklch(c, reference.to_rgba().a);
}

} // namespace clrsync::core::color_ops
