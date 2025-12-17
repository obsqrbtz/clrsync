#include "font_loader.hpp"
#include "core/config/config.hpp"
#include "imgui_internal.h"
#include <imgui.h>
#include <algorithm>

#if defined(_WIN32)

#include <algorithm>
#include <windows.h>
#include <winreg.h>

static std::string search_registry_for_font(HKEY root_key, const char* subkey, const std::string& font_name_lower, const char* default_font_dir)
{
    HKEY hKey;
    LONG result = RegOpenKeyExA(root_key, subkey, 0, KEY_READ, &hKey);

    if (result != ERROR_SUCCESS)
        return {};

    char value_name[512];
    BYTE value_data[512];
    DWORD value_name_size, value_data_size, type;
    DWORD index = 0;

    std::string found_path;

    while (true)
    {
        value_name_size = sizeof(value_name);
        value_data_size = sizeof(value_data);

        result = RegEnumValueA(hKey, index++, value_name, &value_name_size, nullptr, &type, value_data, &value_data_size);

        if (result != ERROR_SUCCESS)
            break;

        if (type != REG_SZ)
            continue;

        std::string reg_font_name = value_name;
        std::transform(reg_font_name.begin(), reg_font_name.end(), reg_font_name.begin(), ::tolower);

        std::string reg_font_name_clean = reg_font_name;
        size_t type_pos = reg_font_name_clean.find(" (");
        if (type_pos != std::string::npos)
            reg_font_name_clean = reg_font_name_clean.substr(0, type_pos);

        if (reg_font_name_clean == font_name_lower)
        {
            std::string font_file = reinterpret_cast<char*>(value_data);

            // If path is not absolute, prepend default font directory
            if (font_file.find(":\\") == std::string::npos)
            {
                found_path = std::string(default_font_dir) + "\\" + font_file;
            }
            else
            {
                found_path = font_file;
            }
            break;
        }
    }

    RegCloseKey(hKey);
    return found_path;
}

std::string font_loader::find_font_windows(const char* font_name)
{
    std::string font_name_lower = font_name;
    std::transform(font_name_lower.begin(), font_name_lower.end(), font_name_lower.begin(), ::tolower);

    char windows_dir[MAX_PATH];
    GetWindowsDirectoryA(windows_dir, MAX_PATH);
    std::string system_fonts_dir = std::string(windows_dir) + "\\Fonts";

    // First, try system-wide fonts (HKEY_LOCAL_MACHINE)
    std::string path = search_registry_for_font(
        HKEY_LOCAL_MACHINE,
        "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Fonts",
        font_name_lower,
        system_fonts_dir.c_str()
    );

    if (!path.empty())
        return path;

    // If not found, try per-user fonts (HKEY_CURRENT_USER)
    char local_appdata[MAX_PATH];
    if (GetEnvironmentVariableA("LOCALAPPDATA", local_appdata, MAX_PATH) > 0)
    {
        std::string user_fonts_dir = std::string(local_appdata) + "\\Microsoft\\Windows\\Fonts";

        path = search_registry_for_font(
            HKEY_CURRENT_USER,
            "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Fonts",
            font_name_lower,
            user_fonts_dir.c_str()
        );
    }

    return path;
}
#endif

#if defined(__APPLE__)

#include <CoreText/CoreText.h>

std::vector<unsigned char> font_loader::load_font_macos(const char* font_name)
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
    Boolean success = CFURLCreateDataAndPropertiesFromResource(kCFAllocatorDefault, url, &data, nullptr, nullptr, nullptr);
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

#endif

#if !defined(_WIN32) && !defined(__APPLE__)

#include <fontconfig/fontconfig.h>

std::string font_loader::find_font_linux(const char* font_name)
{
    FcInit();

    FcPattern* pattern = FcNameParse(reinterpret_cast<const FcChar8*>(font_name));
    if (!pattern)
        return {};

    FcConfigSubstitute(nullptr, pattern, FcMatchPattern);
    FcDefaultSubstitute(pattern);

    FcResult result;
    FcPattern* match = FcFontMatch(nullptr, pattern, &result);

    std::string out;

    if (match)
    {
        FcChar8* file = nullptr;
        if (FcPatternGetString(match, FC_FILE, 0, &file) == FcResultMatch)
            out = reinterpret_cast<const char*>(file);

        FcPatternDestroy(match);
    }

    FcPatternDestroy(pattern);
    return out;
}

#endif

std::string font_loader::find_font_path(const char* font_name)
{
#if defined(_WIN32)
    return find_font_windows(font_name);

#elif defined(__APPLE__)
    (void)font_name;
    return {};

#else
    return find_font_linux(font_name);

#endif
}

ImFont* font_loader::load_font(const char* font_name, float size_px)
{
#if defined(__APPLE__)
    std::vector<unsigned char> buf = load_font_macos(font_name);
    if (buf.empty())
        return nullptr;

    return ImGui::GetIO().Fonts->AddFontFromMemoryTTF(
        buf.data(),
        static_cast<int>(buf.size()),
        size_px
    );

#else
    std::string path = find_font_path(font_name);
    if (path.empty())
        return nullptr;

    float scale = ImGui::GetIO().DisplayFramebufferScale.y;
    return ImGui::GetIO().Fonts->AddFontFromFileTTF(
        path.c_str(),
        size_px * scale
    );
#endif
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

#if defined(_WIN32)
    auto enumerate_registry_fonts = [&fonts](HKEY root_key, const char* subkey)
    {
        HKEY hKey;
        if (RegOpenKeyExA(root_key, subkey, 0, KEY_READ, &hKey) != ERROR_SUCCESS)
            return;

        char value_name[512];
        DWORD value_name_size;
        DWORD index = 0;

        while (true)
        {
            value_name_size = sizeof(value_name);
            LONG result = RegEnumValueA(hKey, index++, value_name, &value_name_size, nullptr, nullptr, nullptr, nullptr);
            
            if (result != ERROR_SUCCESS)
                break;

            std::string font_name = value_name;
            size_t pos = font_name.find(" (");
            if (pos != std::string::npos)
                font_name = font_name.substr(0, pos);
            
            if (std::find(fonts.begin(), fonts.end(), font_name) == fonts.end())
                fonts.push_back(font_name);
        }

        RegCloseKey(hKey);
    };

    enumerate_registry_fonts(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Fonts");
    enumerate_registry_fonts(HKEY_CURRENT_USER, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Fonts");

#elif defined(__APPLE__)
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
                CTFontDescriptorRef descriptor = (CTFontDescriptorRef)CFArrayGetValueAtIndex(fontDescriptors, i);
                CFStringRef fontName = (CFStringRef)CTFontDescriptorCopyAttribute(descriptor, kCTFontDisplayNameAttribute);
                
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

#else
    FcInit();
    FcPattern* pattern = FcPatternCreate();
    FcObjectSet* os = FcObjectSetBuild(FC_FAMILY, nullptr);
    FcFontSet* fs = FcFontList(nullptr, pattern, os);
    
    if (fs)
    {
        for (int i = 0; i < fs->nfont; i++)
        {
            FcChar8* family = nullptr;
            if (FcPatternGetString(fs->fonts[i], FC_FAMILY, 0, &family) == FcResultMatch)
            {
                std::string font_name = reinterpret_cast<const char*>(family);
                if (std::find(fonts.begin(), fonts.end(), font_name) == fonts.end())
                    fonts.push_back(font_name);
            }
        }
        FcFontSetDestroy(fs);
    }
    
    FcObjectSetDestroy(os);
    FcPatternDestroy(pattern);
#endif

    std::sort(fonts.begin(), fonts.end());
    return fonts;
}
