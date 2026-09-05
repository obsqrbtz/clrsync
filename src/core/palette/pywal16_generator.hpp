#ifndef CLRSYNC_CORE_PALETTE_PYWAL16_GENERATOR_HPP
#define CLRSYNC_CORE_PALETTE_PYWAL16_GENERATOR_HPP

#include "core/palette/palette.hpp"
#include "core/palette/palette_normalizer.hpp"
#include "generator.hpp"

namespace clrsync::core
{
class pywal16_generator : public generator
{
  public:
    pywal16_generator() : generator({generator::system::linux_os})
    {
    }
    ~pywal16_generator() override = default;

    struct options
    {
        std::string background;
        std::string foreground;
        std::string backend;
        float saturate = -1.0f;
        bool light = false;

        normalize_options normalize{};
    };

    palette generate_from_image(const std::string &image_path) override;
    palette generate_from_image(const std::string &image_path, const options &opts);
};
} // namespace clrsync::core

#endif
