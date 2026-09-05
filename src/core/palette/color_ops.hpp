#ifndef CLRSYNC_CORE_PALETTE_COLOR_OPS_HPP
#define CLRSYNC_CORE_PALETTE_COLOR_OPS_HPP

#include <vector>

#include "core/palette/color.hpp"

namespace clrsync::core::color_ops
{

struct oklab
{
    float l;
    float a;
    float b;
};

struct oklch
{
    float l; // 0..1
    float c; // 0..0.4
    float h; // 0..360
};

oklab srgb_to_oklab(const rgb &value);
rgb oklab_to_srgb(const oklab &value);
oklch oklab_to_oklch(const oklab &value);
oklab oklch_to_oklab(const oklch &value);

oklch to_oklch(const color &value);
color from_oklch(const oklch &value, uint8_t alpha = 0xFF);

float relative_luminance(const color &value);

float contrast_ratio(const color &a, const color &b);

float lightness(const color &value);
float chroma(const color &value);
float hue(const color &value);

color with_lightness(const color &value, float l);
color with_chroma(const color &value, float c);
color adjust_lightness(const color &value, float delta);

bool is_dark(const color &value);

color step_surface(const color &value, float amount, bool dark_theme);

color ensure_contrast(const color &fg, const color &bg, float target);

bool nearest_hue(const std::vector<color> &candidates, float target_hue, float min_chroma,
                 color &out);

color synthesize_hue(const color &reference, float target_hue, float min_chroma);

float hue_distance(float a, float b);

} // namespace clrsync::core::color_ops

#endif
