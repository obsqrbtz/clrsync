#ifndef CLRSYNC_GUI_ABOUT_WINDOW_HPP
#define CLRSYNC_GUI_ABOUT_WINDOW_HPP

class about_window
{
public:
    about_window();
    void render();
    void show() { m_visible = true; }
    void hide() { m_visible = false; }
    bool is_visible() const { return m_visible; }

private:
    bool m_visible{false};
};

#endif // CLRSYNC_GUI_ABOUT_WINDOW_HPP