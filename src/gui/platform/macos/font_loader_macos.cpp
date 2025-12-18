#ifdef __APPLE__

#include "core/config/config.hpp"
#include "gui/platform/font_loader.hpp"
#include "imgui_internal.h"
#include <CoreText/CoreText.h>
#include <algorithm>
#include <imgui.h>

std::vector<unsigned char> font_loader::load_font_macos(const char *font_name)
{
    std::vector<unsigned char> out;

    CFStringRef cf_name = CFStringCreateWithCString(nullptr, font_name, kCFStringEncodingUTF8);
    if (!cf_name)
        return out;

    CTFontDescriptorRef desc = CTFontDescriptorCreateWithNameAndSize(cf_name, 12);
    CFRelease(cf_name);

    if (!desc)
        return out;

    CFURLRef url = (CFURLRef)CTFontDescriptorCopyAttribute(desc, kCTFontURLAttribute);
    CFRelease(desc);

    if (!url)
        return out;

    CFDataRef data = nullptr;
    Boolean success = CFURLCreateDataAndPropertiesFromResource(kCFAllocatorDefault, url, &data,
                                                               nullptr, nullptr, nullptr);
    CFRelease(url);

    if (success && data)
    {
        CFIndex size = CFDataGetLength(data);
        if (size > 100)
        {
            out.resize(size);
            CFDataGetBytes(data, CFRangeMake(0, size), out.data());
        }
        CFRelease(data);
    }

    return out;
}

std::string font_loader::find_font_path(const char *font_name)
{
    (void)font_name;
    return {};
}

ImFont *font_loader::load_font(const char *font_name, float size_px)
{
    std::vector<unsigned char> buf = load_font_macos(font_name);
    if (buf.empty())
        return nullptr;

    return ImGui::GetIO().Fonts->AddFontFromMemoryTTF(buf.data(), static_cast<int>(buf.size()),
                                                      size_px);
}

void font_loader::push_default_font()
{
    ImGui::PushFont(ImGui::GetDefaultFont(), clrsync::core::config::instance().font_size());
}

void font_loader::pop_font()
{
    ImGui::PopFont();
}

std::vector<std::string> font_loader::get_system_fonts()
{
    std::vector<std::string> fonts;

    CTFontCollectionRef collection = CTFontCollectionCreateFromAvailableFonts(nullptr);
    if (collection)
    {
        CFArrayRef fontDescriptors = CTFontCollectionCreateMatchingFontDescriptors(collection);
        CFRelease(collection);

        if (fontDescriptors)
        {
            CFIndex count = CFArrayGetCount(fontDescriptors);
            for (CFIndex i = 0; i < count; i++)
            {
                CTFontDescriptorRef descriptor =
                    (CTFontDescriptorRef)CFArrayGetValueAtIndex(fontDescriptors, i);
                CFStringRef fontName = (CFStringRef)CTFontDescriptorCopyAttribute(
                    descriptor, kCTFontDisplayNameAttribute);

                if (fontName)
                {
                    char buffer[256];
                    if (CFStringGetCString(fontName, buffer, sizeof(buffer), kCFStringEncodingUTF8))
                    {
                        std::string font_name = buffer;
                        if (std::find(fonts.begin(), fonts.end(), font_name) == fonts.end())
                            fonts.push_back(font_name);
                    }
                    CFRelease(fontName);
                }
            }
            CFRelease(fontDescriptors);
        }
    }

    std::sort(fonts.begin(), fonts.end());
    return fonts;
}

#endif