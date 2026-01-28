#ifndef CLRSYNC_CORE_PALETTE_GENERATOR_HPP
#define CLRSYNC_CORE_PALETTE_GENERATOR_HPP

#include <string>
#include "core/palette/palette.hpp"

namespace clrsync::core
{
class generator
{
  public:
    generator() = default;
    virtual ~generator() = default;

    virtual palette generate_from_image(const std::string &image_path) = 0;
};
} // namespace clrsync::core

#endif
