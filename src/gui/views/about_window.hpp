#ifndef CLRSYNC_GUI_ABOUT_WINDOW_HPP
#define CLRSYNC_GUI_ABOUT_WINDOW_HPP

#include "core/palette/palette.hpp"

class about_window
{
  public:
    about_window();
    void render(const clrsync::core::palette &pal);
    void render()
    {
        render(m_default_palette);
    }
    void show()
    {
        m_visible = true;
    }
    void hide()
    {
        m_visible = false;
    }
    bool is_visible() const
    {
        return m_visible;
    }

  private:
    bool m_visible{false};
    clrsync::core::palette m_default_palette;
};

#endif // CLRSYNC_GUI_ABOUT_WINDOW_HPP