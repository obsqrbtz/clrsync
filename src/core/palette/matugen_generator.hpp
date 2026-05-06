#ifndef CLRSYNC_CORE_PALETTE_MATUGEN_GENERATOR_HPP
#define CLRSYNC_CORE_PALETTE_MATUGEN_GENERATOR_HPP

#include "core/palette/palette.hpp"
#include "generator.hpp"

namespace clrsync::core
{
class matugen_generator : public generator
{
  public:
    matugen_generator()
        : generator({generator::system::windows, generator::system::linux,
                     generator::system::macos})
    {
    }
    ~matugen_generator() override = default;

    struct options
    {
        std::string type = "scheme-tonal-spot";
        std::string mode = "dark";
        float contrast = 0.0f; // -1..1
      int source_color_index = 0; // 0..3
    };

    palette generate_from_image(const std::string &image_path) override;
    palette generate_from_image(const std::string &image_path, const options &opts);
    palette generate_from_color(const std::string &color_hex);
    palette generate_from_color(const std::string &color_hex, const options &opts);
};
} // namespace clrsync::core

#endif
