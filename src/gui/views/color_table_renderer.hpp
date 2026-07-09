#ifndef CLRSYNC_GUI_COLOR_TABLE_RENDERER_HPP
#define CLRSYNC_GUI_COLOR_TABLE_RENDERER_HPP

#include "core/palette/palette.hpp"
#include "gui/controllers/palette_controller.hpp"
#include <functional>
#include <string>

class color_table_renderer
{
  public:
    using OnColorChangedCallback = std::function<void()>;
    using OnInsertTokenCallback = std::function<void(const std::string &)>;

    void render(const clrsync::core::palette &palette, palette_controller &controller,
                const OnColorChangedCallback &on_changed,
                const OnInsertTokenCallback &on_insert_token = {});

  private:
    void render_color_row(const std::string &name, const clrsync::core::palette &palette,
                          palette_controller &controller, const OnColorChangedCallback &on_changed,
                          const OnInsertTokenCallback &on_insert_token);

    bool matches_filter(const std::string &name) const;

    char m_filter_text[128] = {0};
    bool m_show_only_modified{false};
};

#endif // CLRSYNC_GUI_COLOR_TABLE_RENDERER_HPP
