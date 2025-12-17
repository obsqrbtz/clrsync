#include "screen_eyedropper.hpp"

#include <cstdint>

#if defined(__APPLE__)
#include <ApplicationServices/ApplicationServices.h>
#endif

#if defined(__linux__)
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

namespace clrsync::gui
{
bool sample_screen_rgb(float out_rgb[3])
{
#ifdef _WIN32
    POINT pt;
    if (!GetCursorPos(&pt))
        return false;

    HDC hdc = GetDC(nullptr);
    if (!hdc)
        return false;

    const COLORREF color = GetPixel(hdc, pt.x, pt.y);
    ReleaseDC(nullptr, hdc);

    if (color == CLR_INVALID)
        return false;

    const uint8_t r = GetRValue(color);
    const uint8_t g = GetGValue(color);
    const uint8_t b = GetBValue(color);

    out_rgb[0] = r / 255.0f;
    out_rgb[1] = g / 255.0f;
    out_rgb[2] = b / 255.0f;
    return true;
#elif defined(__APPLE__)
    CGEventRef event = CGEventCreate(nullptr);
    if (!event)
        return false;
    const CGPoint pt = CGEventGetLocation(event);
    CFRelease(event);

    const CGRect rect = CGRectMake(pt.x, pt.y, 1, 1);
    CGImageRef image =
        CGWindowListCreateImage(rect, kCGWindowListOptionOnScreenOnly, kCGNullWindowID,
                                kCGWindowImageDefault);
    if (!image)
        return false;

    uint8_t pixel[4] = {0, 0, 0, 255};
    CGColorSpaceRef color_space = CGColorSpaceCreateDeviceRGB();
    if (!color_space)
    {
        CGImageRelease(image);
        return false;
    }

    CGContextRef ctx = CGBitmapContextCreate(pixel, 1, 1, 8, 4, color_space,
                                             kCGImageAlphaPremultipliedLast |
                                                 kCGBitmapByteOrder32Big);
    CGColorSpaceRelease(color_space);
    if (!ctx)
    {
        CGImageRelease(image);
        return false;
    }

    CGContextDrawImage(ctx, CGRectMake(0, 0, 1, 1), image);
    CGContextRelease(ctx);
    CGImageRelease(image);

    out_rgb[0] = pixel[0] / 255.0f;
    out_rgb[1] = pixel[1] / 255.0f;
    out_rgb[2] = pixel[2] / 255.0f;
    return true;
#elif defined(__linux__)
    Display *display = XOpenDisplay(nullptr);
    if (!display)
        return false;

    const Window root = DefaultRootWindow(display);
    Window root_ret, child_ret;
    int root_x = 0, root_y = 0;
    int win_x = 0, win_y = 0;
    unsigned int mask = 0;

    if (!XQueryPointer(display, root, &root_ret, &child_ret, &root_x, &root_y, &win_x, &win_y,
                       &mask))
    {
        XCloseDisplay(display);
        return false;
    }

    XImage *img = XGetImage(display, root, root_x, root_y, 1, 1, AllPlanes, ZPixmap);
    if (!img)
    {
        XCloseDisplay(display);
        return false;
    }

    const unsigned long pixel = XGetPixel(img, 0, 0);

    auto channel_from_mask = [](unsigned long px, unsigned long mask_val) -> uint8_t {
        if (mask_val == 0)
            return 0;

        unsigned int shift = 0;
        while ((mask_val & 1UL) == 0)
        {
            mask_val >>= 1;
            shift++;
        }

        unsigned int bits = 0;
        while (mask_val & 1UL)
        {
            mask_val >>= 1;
            bits++;
        }

        if (bits == 0)
            return 0;

        const unsigned long max_val = (bits >= 32) ? 0xFFFFFFFFUL : ((1UL << bits) - 1UL);
        const unsigned long val = (px >> shift) & max_val;
        const unsigned long scaled = (val * 255UL) / max_val;
        return static_cast<uint8_t>(scaled & 0xFF);
    };

    const uint8_t r = channel_from_mask(pixel, img->red_mask);
    const uint8_t g = channel_from_mask(pixel, img->green_mask);
    const uint8_t b = channel_from_mask(pixel, img->blue_mask);

    XDestroyImage(img);
    XCloseDisplay(display);

    out_rgb[0] = r / 255.0f;
    out_rgb[1] = g / 255.0f;
    out_rgb[2] = b / 255.0f;
    return true;
#else
    (void)out_rgb;
    return false;
#endif
}
} // namespace clrsync::gui
