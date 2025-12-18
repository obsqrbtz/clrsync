#ifdef __linux__

#include "core/config/config.hpp"
#include "gui/platform/font_loader.hpp"
#include "imgui_internal.h"
#include <algorithm>
#include <fontconfig/fontconfig.h>
#include <imgui.h>

std::string font_loader::find_font_linux(const char *font_name)
{
    FcInit();

    FcPattern *pattern = FcNameParse(reinterpret_cast<const FcChar8 *>(font_name));
    if (!pattern)
        return {};

    FcConfigSubstitute(nullptr, pattern, FcMatchPattern);
    FcDefaultSubstitute(pattern);

    FcResult result;
    FcPattern *match = FcFontMatch(nullptr, pattern, &result);

    std::string out;

    if (match)
    {
        FcChar8 *file = nullptr;
        if (FcPatternGetString(match, FC_FILE, 0, &file) == FcResultMatch)
            out = reinterpret_cast<const char *>(file);

        FcPatternDestroy(match);
    }

    FcPatternDestroy(pattern);
    return out;
}

std::string font_loader::find_font_path(const char *font_name)
{
    return find_font_linux(font_name);
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

    FcInit();
    FcPattern *pattern = FcPatternCreate();
    FcObjectSet *os = FcObjectSetBuild(FC_FAMILY, nullptr);
    FcFontSet *fs = FcFontList(nullptr, pattern, os);

    if (fs)
    {
        for (int i = 0; i < fs->nfont; i++)
        {
            FcChar8 *family = nullptr;
            if (FcPatternGetString(fs->fonts[i], FC_FAMILY, 0, &family) == FcResultMatch)
            {
                std::string font_name = reinterpret_cast<const char *>(family);
                if (std::find(fonts.begin(), fonts.end(), font_name) == fonts.end())
                    fonts.push_back(font_name);
            }
        }
        FcFontSetDestroy(fs);
    }

    FcObjectSetDestroy(os);
    FcPatternDestroy(pattern);

    std::sort(fonts.begin(), fonts.end());
    return fonts;
}

#endif
