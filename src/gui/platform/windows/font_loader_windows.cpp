#include "core/config/config.hpp"
#include "gui/platform/font_loader.hpp"
#include "imgui_internal.h"
#include <algorithm>
#include <imgui.h>
#include <windows.h>
#include <winreg.h>

static std::string search_registry_for_font(HKEY root_key, const char *subkey,
                                            const std::string &font_name_lower,
                                            const char *default_font_dir)
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

        result = RegEnumValueA(hKey, index++, value_name, &value_name_size, nullptr, &type,
                               value_data, &value_data_size);

        if (result != ERROR_SUCCESS)
            break;

        if (type != REG_SZ)
            continue;

        std::string reg_font_name = value_name;
        std::transform(reg_font_name.begin(), reg_font_name.end(), reg_font_name.begin(),
                       ::tolower);

        std::string reg_font_name_clean = reg_font_name;
        size_t type_pos = reg_font_name_clean.find(" (");
        if (type_pos != std::string::npos)
            reg_font_name_clean = reg_font_name_clean.substr(0, type_pos);

        if (reg_font_name_clean == font_name_lower)
        {
            std::string font_file = reinterpret_cast<char *>(value_data);

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

std::string font_loader::find_font_windows(const char *font_name)
{
    std::string font_name_lower = font_name;
    std::transform(font_name_lower.begin(), font_name_lower.end(), font_name_lower.begin(),
                   ::tolower);

    char windows_dir[MAX_PATH];
    GetWindowsDirectoryA(windows_dir, MAX_PATH);
    std::string system_fonts_dir = std::string(windows_dir) + "\\Fonts";

    // First, try system-wide fonts (HKEY_LOCAL_MACHINE)
    std::string path = search_registry_for_font(
        HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Fonts",
        font_name_lower, system_fonts_dir.c_str());

    if (!path.empty())
        return path;

    // If not found, try per-user fonts (HKEY_CURRENT_USER)
    char local_appdata[MAX_PATH];
    if (GetEnvironmentVariableA("LOCALAPPDATA", local_appdata, MAX_PATH) > 0)
    {
        std::string user_fonts_dir = std::string(local_appdata) + "\\Microsoft\\Windows\\Fonts";

        path = search_registry_for_font(HKEY_CURRENT_USER,
                                        "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Fonts",
                                        font_name_lower, user_fonts_dir.c_str());
    }

    return path;
}

std::string font_loader::find_font_path(const char *font_name)
{
    return find_font_windows(font_name);
}

ImFont *font_loader::load_font(const char *font_name, float size_px)
{
    std::string path = find_font_path(font_name);
    if (path.empty())
        return nullptr;

    float scale = ImGui::GetIO().DisplayFramebufferScale.y;
    return ImGui::GetIO().Fonts->AddFontFromFileTTF(path.c_str(), size_px * scale);
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

    auto enumerate_registry_fonts = [&fonts](HKEY root_key, const char *subkey) {
        HKEY hKey;
        if (RegOpenKeyExA(root_key, subkey, 0, KEY_READ, &hKey) != ERROR_SUCCESS)
            return;

        char value_name[512];
        DWORD value_name_size;
        DWORD index = 0;

        while (true)
        {
            value_name_size = sizeof(value_name);
            LONG result = RegEnumValueA(hKey, index++, value_name, &value_name_size, nullptr,
                                        nullptr, nullptr, nullptr);

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

    enumerate_registry_fonts(HKEY_LOCAL_MACHINE,
                             "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Fonts");
    enumerate_registry_fonts(HKEY_CURRENT_USER,
                             "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Fonts");

    std::sort(fonts.begin(), fonts.end());
    return fonts;
}
