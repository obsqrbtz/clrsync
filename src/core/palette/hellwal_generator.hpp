#ifndef CLRSYNC_CORE_PALETTE_HELLWAL_GENERATOR_HPP
#define CLRSYNC_CORE_PALETTE_HELLWAL_GENERATOR_HPP

#include "core/palette/palette.hpp"
#include "generator.hpp"

namespace clrsync::core
{
class hellwal_generator : public generator
{
  public:
    hellwal_generator() = default;
    ~hellwal_generator() override = default;

    struct options
    {
        bool neon = false;
        bool dark = true;
        bool light = false;
        bool color = false;
        float dark_offset = 0.0f;
        float bright_offset = 0.0f;
        bool invert = false;
        float gray_scale = 0.0f;
    };

    palette generate_from_image(const std::string &image_path) override;
    palette generate_from_image(const std::string &image_path, const options &opts);
};
} // namespace clrsync::core

#endif
