#ifndef CLRSYNC_GUI_LAYOUT_MAIN_LAYOUT_HPP
#define CLRSYNC_GUI_LAYOUT_MAIN_LAYOUT_HPP

namespace clrsync::gui::layout
{

class main_layout
{
  public:
    void setup_dockspace(bool &first_time);
    void render_menu_bar();

    bool should_show_about() const { return m_show_about; }
    bool should_show_settings() const { return m_show_settings; }
    void clear_actions()
    {
        m_show_about = false;
        m_show_settings = false;
    }

  private:
    bool m_show_about = false;
    bool m_show_settings = false;
};

} // namespace clrsync::gui::layout

#endif // CLRSYNC_GUI_LAYOUT_MAIN_LAYOUT_HPP
