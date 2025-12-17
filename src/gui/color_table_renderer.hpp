#ifndef CLRSYNC_GUI_COLOR_TABLE_RENDERER_HPP
#define CLRSYNC_GUI_COLOR_TABLE_RENDERER_HPP

#include "core/palette/palette.hpp"
#include "palette_controller.hpp"
#include <functional>
#include <string>

class color_table_renderer
{
public:
    using OnColorChangedCallback = std::function<void()>;
    
    void render(const clrsync::core::palette& palette, 
                palette_controller& controller,
                const OnColorChangedCallback& on_changed);

private:
    void render_color_row(const std::string& name,
                         const clrsync::core::palette& palette,
                         palette_controller& controller,
                         const OnColorChangedCallback& on_changed);

    struct screen_pick_state
    {
        bool active{false};
        std::string key;
        float rgba[4]{0.0f, 0.0f, 0.0f, 1.0f};
        float alpha{1.0f};
        bool has_sample{false};
    };

    screen_pick_state m_screen_pick;
};

#endif // CLRSYNC_GUI_COLOR_TABLE_RENDERER_HPP
