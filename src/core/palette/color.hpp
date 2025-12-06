#ifndef CLRSYNC_CORE_PALETTE_COLOR_HPP
#define CLRSYNC_CORE_PALETTE_COLOR_HPP

#include <cstdint>
#include <string>

namespace clrsync::core
{

struct rgb
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

struct rgba
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

struct hsl
{
    float h;
    float s;
    float l;
};

struct hsla
{
    float h;
    float s;
    float l;
    float a;
};

class color
{
  public:
    color() = default;
    explicit color(uint32_t hex) : m_hex(hex)
    {
    }
    uint32_t hex() const;

    rgb to_rgb() const;

    rgba to_rgba() const;

    hsl to_hsl() const;

    hsla to_hsla() const;

    void from_hex_string(const std::string &str);

    const std::string to_hex_string() const;

    const std::string to_hex_string_with_alpha() const;

    std::string format(const std::string& field) const;

    void set(uint32_t hex);

  private:
    uint32_t m_hex = 0x00000000;
};
} // namespace clrsync::core

#endif