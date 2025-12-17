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
};

#endif // CLRSYNC_GUI_COLOR_TABLE_RENDERER_HPP
