#ifdef __linux__

#include <GLFW/glfw3.h>
#include <png.h>
#include <vector>

#include "gui/platform/window_icon.hpp"

extern unsigned char clrsync_icon_32_png[];
extern unsigned int clrsync_icon_32_png_len;
extern unsigned char clrsync_icon_256_png[];
extern unsigned int clrsync_icon_256_png_len;

namespace
{

struct EmbeddedIcon
{
    const unsigned char *data;
    unsigned int size;
};

bool decodePng(const unsigned char *data, size_t size, std::vector<unsigned char> &pixels, int &width,
               int &height)
{
    png_image image{};
    image.version = PNG_IMAGE_VERSION;
    if (!png_image_begin_read_from_memory(&image, data, size))
        return false;
    image.format = PNG_FORMAT_RGBA;
    pixels.resize(PNG_IMAGE_SIZE(image));
    if (!png_image_finish_read(&image, nullptr, pixels.data(), 0, nullptr))
    {
        png_image_free(&image);
        return false;
    }
    width = static_cast<int>(image.width);
    height = static_cast<int>(image.height);
    png_image_free(&image);
    return true;
}

} // namespace

namespace clrsync::gui::platform
{

void set_window_icon(GLFWwindow *window)
{
    const EmbeddedIcon resources[] = {
        {clrsync_icon_32_png, clrsync_icon_32_png_len},
        {clrsync_icon_256_png, clrsync_icon_256_png_len},
    };
    std::vector<GLFWimage> images;
    std::vector<std::vector<unsigned char>> pixel_buffers;
    images.reserve(std::size(resources));
    pixel_buffers.reserve(std::size(resources));
    for (const auto &resource : resources)
    {
        std::vector<unsigned char> pixels;
        int width = 0;
        int height = 0;
        if (!decodePng(resource.data, resource.size, pixels, width, height))
            continue;
        pixel_buffers.push_back(std::move(pixels));
        GLFWimage image{};
        image.width = width;
        image.height = height;
        image.pixels = pixel_buffers.back().data();
        images.push_back(image);
    }
    if (!images.empty())
        glfwSetWindowIcon(window, static_cast<int>(images.size()), images.data());
}

} // namespace clrsync::gui::platform

#endif // __linux__
