#ifndef CLRSYNC_CORE_PALETTE_GENERATOR_MAPPINGS_HPP
#define CLRSYNC_CORE_PALETTE_GENERATOR_MAPPINGS_HPP

#include "core/palette/palette.hpp"

#include <cstddef>
#include <functional>

namespace clrsync::core
{

struct index_color_mapping
{
    const char *palette_key;
    int source_index;
};

struct key_color_mapping
{
    const char *palette_key;
    const char *source_key;
};

inline void apply_index_mappings(
    palette &pal,
    const index_color_mapping *mappings,
    std::size_t count,
    const std::function<const color &(int index)> &get_by_index)
{
    for (std::size_t i = 0; i < count; ++i)
        pal.set_color(mappings[i].palette_key, get_by_index(mappings[i].source_index));
}

} // namespace clrsync::core

#endif
