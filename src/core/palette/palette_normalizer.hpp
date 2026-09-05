#ifndef CLRSYNC_CORE_PALETTE_PALETTE_NORMALIZER_HPP
#define CLRSYNC_CORE_PALETTE_PALETTE_NORMALIZER_HPP

#include <vector>

#include "core/palette/color.hpp"
#include "core/palette/palette.hpp"

namespace clrsync::core
{

struct normalize_options
{
    bool enabled = true;

    float contrast_body = 4.5f;   // foreground, editor text, active line numbers
    float contrast_muted = 3.0f;  // comments, gutter, inactive text
    float contrast_accent = 3.0f; // accents and syntax colours

    float surface_step = 0.055f;

    float min_semantic_chroma = 0.04f;

    float bright_step = 0.10f;
};

void normalize_palette(palette &pal, const std::vector<color> &source_colors,
                       const normalize_options &opts);

} // namespace clrsync::core

#endif
