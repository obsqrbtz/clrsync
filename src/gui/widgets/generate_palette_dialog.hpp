#ifndef CLRSYNC_GUI_WIDGETS_GENERATE_PALETTE_DIALOG_HPP
#define CLRSYNC_GUI_WIDGETS_GENERATE_PALETTE_DIALOG_HPP

#include "core/palette/palette.hpp"
#include "form_field.hpp"
#include "gui/widgets/error_message.hpp"
#include <functional>
#include <optional>
#include <string>
#include <vector>

namespace clrsync::gui::widgets
{

enum class palette_generator_kind
{
    hellwal,
    matugen,
    pywal16
};

struct generate_palette_state
{
    int generator_idx = 0;
    std::string image_path;
    bool neon = false;
    bool dark = true;
    bool light = false;
    bool color = false;
    float dark_offset = 0.0f;
    float bright_offset = 0.0f;
    bool invert = false;
    float gray_scale = 0.0f;
    std::string matugen_mode{"dark"};
    std::string matugen_type{"scheme-tonal-spot"};
    float matugen_contrast = 0.0f;
    int matugen_source_color_index = 0;
    bool matugen_use_color = false;
    float matugen_color_vec[3]{1.0f, 0.0f, 0.0f};
    std::string matugen_color_hex{"FF0000"};
    bool pywal16_use_background = false;
    float pywal16_background_vec[3]{0.063f, 0.071f, 0.071f};
    std::string pywal16_background;
    bool pywal16_use_foreground = false;
    float pywal16_foreground_vec[3]{0.765f, 0.765f, 0.765f};
    std::string pywal16_foreground;
    std::string pywal16_backend;
    bool pywal16_use_saturate = false;
    float pywal16_saturate = 0.5f;
    bool pywal16_light = false;
};

class generate_palette_dialog
{
  public:
    generate_palette_dialog();

    void open();
    void set_error(const std::string &message);
    void render(generate_palette_state& state,
                const std::vector<palette_generator_kind>& available_generators,
                const std::vector<std::string>& generator_labels, const core::palette& palette,
                const std::function<std::optional<std::string>()>& on_generate);

  private:
    void render_generator_row(generate_palette_state& state,
                              const std::vector<std::string>& generator_labels);
    void render_hellwal_options(generate_palette_state& state, const core::palette& palette);
    void render_matugen_options(generate_palette_state& state, const core::palette& palette);
    void render_pywal16_options(generate_palette_state& state, const core::palette& palette);
    bool render_footer(bool can_generate, const core::palette& palette);

    form_field m_form;
    error_message m_error;
    bool m_is_open = false;
};

} // namespace clrsync::gui::widgets

#endif // CLRSYNC_GUI_WIDGETS_GENERATE_PALETTE_DIALOG_HPP
