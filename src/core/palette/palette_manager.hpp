#ifndef CLRSYNC_CORE_PALETTE_PALETTE_MANAGER_HPP
#define CLRSYNC_CORE_PALETTE_PALETTE_MANAGER_HPP

#include "core/utils.hpp"
#include <iostream>
#include <string>
#include <unordered_map>

#include <core/config/config.hpp>
#include <core/palette/palette.hpp>
#include <core/palette/palette_file.hpp>
#include <filesystem>

namespace clrsync::core
{
template <typename FileType> class palette_manager
{
  public:
    palette_manager() = default;
    void load_palettes_from_directory(const std::string &directory_path)
    {
        auto directory_path_expanded = expand_user(directory_path);
        if (!std::filesystem::exists(directory_path_expanded))
            return;
        for (const auto &entry : std::filesystem::directory_iterator(directory_path_expanded))
        {
            if (entry.is_regular_file())
            {
                palette_file<FileType> pal_file(entry.path().string());
                if (pal_file.parse())
                    add_palette(pal_file.palette());
            }
        }
    }
    void save_palette_to_file(const palette &pal, const std::string &directory_path) const
    {
        std::string file_path = directory_path + "/" + pal.name() + ".toml";
        palette_file<FileType> pal_file(file_path);
        pal_file.save_palette(pal);
    }

    const palette load_palette_from_file(const std::string &file_path) const
    {
        palette_file<FileType> pal_file(file_path);
        if (pal_file.parse())
            return pal_file.palette(); // TODO: report missing/invalid file
        return {};
    }
    void add_palette(const palette &pal)
    {
        m_palettes[pal.name()] = pal;
    }
    void delete_palette(const std::string &file_path, const std::string &name)
    {
        std::filesystem::remove(file_path);
        m_palettes.erase(name);
    }
    const palette *get_palette(const std::string &name) const
    {
        auto it = m_palettes.find(name);
        if (it != m_palettes.end())
        {
            return &it->second;
        }
        return nullptr;
    }
    const std::unordered_map<std::string, palette> &palettes() const
    {
        return m_palettes;
    }

  private:
    std::unordered_map<std::string, palette> m_palettes{};
};
} // namespace clrsync::core
#endif