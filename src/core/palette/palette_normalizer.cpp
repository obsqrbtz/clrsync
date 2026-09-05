#include "core/palette/palette_normalizer.hpp"

#include <algorithm>
#include <array>
#include <string>

#include "core/palette/color_ops.hpp"

namespace clrsync::core
{
namespace ops = color_ops;

namespace
{

struct hue_slot
{
    const char *key;
    float hue;
};

constexpr std::array<hue_slot, 6> ANSI_HUES = {{
    {"term1", 29.0f},  // red
    {"term2", 145.0f}, // green
    {"term3", 100.0f}, // yellow
    {"term4", 250.0f}, // blue
    {"term5", 330.0f}, // magenta
    {"term6", 195.0f}, // cyan
}};

constexpr float HUE_ERROR = 29.0f;
constexpr float HUE_WARNING = 70.0f;
constexpr float HUE_SUCCESS = 145.0f;
constexpr float HUE_INFO = 250.0f;

bool has_key(const palette &pal, const std::string &key)
{
    return pal.colors().find(key) != pal.colors().end();
}

color role(const palette &pal, const std::string &key, const color &fallback)
{
    auto it = pal.colors().find(key);
    return it != pal.colors().end() ? it->second : fallback;
}

void set_contrasted(palette &pal, const std::string &key, const color &fallback,
                    const color &ground, float target)
{
    auto it = pal.colors().find(key);
    const bool present = it != pal.colors().end();
    if (present && ops::contrast_ratio(it->second, ground) >= target)
        return;
    pal.set_color(key, ops::ensure_contrast(present ? it->second : fallback, ground, target));
}

color on_color(const color &ground)
{
    color light;
    light.set(0xF4F4F4FF);
    color dark;
    dark.set(0x111111FF);
    return ops::contrast_ratio(light, ground) >= ops::contrast_ratio(dark, ground) ? light : dark;
}

void build_surface_ramp(palette &pal, const color &background, bool dark, float step)
{
    const std::array<const char *, 4> tiers = {"surface", "surface_variant", "surface_container",
                                               "surface_high"};

    color previous = background;
    for (std::size_t i = 0; i < tiers.size(); ++i)
    {
        const std::string key = tiers[i];
        const color expected = ops::step_surface(previous, step, dark);

        color candidate = role(pal, key, expected);

        const float delta = ops::lightness(candidate) - ops::lightness(previous);
        const float signed_delta = dark ? delta : -delta;
        if (signed_delta < step * 0.6f)
            candidate = expected;

        pal.set_color(key, candidate);
        previous = candidate;
    }
}

void assign_ansi(palette &pal, const std::vector<color> &sources, const color &background,
                 const color &foreground, const color &accent, bool dark,
                 const normalize_options &opts)
{
    std::vector<color> pool;
    for (const color &c : sources)
    {
        if (ops::chroma(c) >= opts.min_semantic_chroma)
            pool.push_back(c);
    }

    struct candidate
    {
        std::size_t slot;
        std::size_t source;
        float distance;
    };

    std::vector<candidate> ranked;
    for (std::size_t s = 0; s < ANSI_HUES.size(); ++s)
    {
        for (std::size_t i = 0; i < pool.size(); ++i)
            ranked.push_back({s, i, ops::hue_distance(ops::hue(pool[i]), ANSI_HUES[s].hue)});
    }
    std::sort(ranked.begin(), ranked.end(),
              [](const candidate &a, const candidate &b) { return a.distance < b.distance; });

    std::array<bool, 6> slot_taken{};
    std::vector<bool> source_taken(pool.size(), false);
    std::array<color, 6> assigned{};

    for (const candidate &c : ranked)
    {
        if (c.distance > 60.0f)
            break;
        if (slot_taken[c.slot] || source_taken[c.source])
            continue;
        slot_taken[c.slot] = true;
        source_taken[c.source] = true;
        assigned[c.slot] = pool[c.source];
    }

    std::vector<color> placed;
    for (std::size_t s = 0; s < ANSI_HUES.size(); ++s)
    {
        color value = slot_taken[s] ? assigned[s]
                                    : ops::synthesize_hue(accent, ANSI_HUES[s].hue,
                                                          opts.min_semantic_chroma * 2.0f);
        value = ops::ensure_contrast(value, background, opts.contrast_accent);

        for (int attempt = 0; attempt < 6; ++attempt)
        {
            const bool clashes = std::any_of(placed.begin(), placed.end(), [&](const color &other) {
                return other.hex() == value.hex();
            });
            if (!clashes)
                break;
            value = ops::ensure_contrast(ops::step_surface(value, opts.surface_step, dark),
                                         background, opts.contrast_accent);
        }

        placed.push_back(value);
        pal.set_color(ANSI_HUES[s].key, value);

        const std::string bright_key = "term" + std::to_string(s + 9);
        pal.set_color(bright_key, ops::step_surface(value, opts.bright_step, dark));
    }

    const color surface_high = role(pal, "surface_high", background);
    pal.set_color("term0", background);
    pal.set_color("term7", foreground);
    pal.set_color("term8", ops::step_surface(surface_high, opts.surface_step, dark));
    pal.set_color("term15",
                  ops::ensure_contrast(foreground, background, opts.contrast_body + 2.0f));
}

void assign_base16(palette &pal, const color &background, const color &foreground, bool dark,
                   const normalize_options &opts)
{
    pal.set_color("base00", background);
    pal.set_color("base01", role(pal, "surface", background));
    pal.set_color("base02", role(pal, "surface_container", background));
    pal.set_color("base03", ops::ensure_contrast(role(pal, "surface_high", background), background,
                                                 opts.contrast_muted));

    const color base05 = ops::ensure_contrast(foreground, background, opts.contrast_body);
    pal.set_color("base04", ops::step_surface(base05, -opts.surface_step, dark));
    pal.set_color("base05", base05);
    pal.set_color("base06", ops::step_surface(base05, opts.surface_step, dark));
    pal.set_color("base07", ops::step_surface(base05, opts.surface_step * 2.0f, dark));

    pal.set_color("base08", role(pal, "term1", foreground));  // red
    pal.set_color("base09", role(pal, "term9", foreground));  // orange
    pal.set_color("base0A", role(pal, "term3", foreground));  // yellow
    pal.set_color("base0B", role(pal, "term2", foreground));  // green
    pal.set_color("base0C", role(pal, "term6", foreground));  // cyan
    pal.set_color("base0D", role(pal, "term4", foreground));  // blue
    pal.set_color("base0E", role(pal, "term5", foreground));  // magenta
    pal.set_color("base0F", role(pal, "term11", foreground)); // brown
}

void assign_semantics(palette &pal, const std::vector<color> &sources, const color &background,
                      const color &accent, const normalize_options &opts)
{
    struct semantic
    {
        const char *key;
        const char *on_key;
        float hue;
    };

    const std::array<semantic, 4> semantics = {{
        {"error", "on_error", HUE_ERROR},
        {"warning", "on_warning", HUE_WARNING},
        {"success", "on_success", HUE_SUCCESS},
        {"info", "on_info", HUE_INFO},
    }};

    for (const semantic &sem : semantics)
    {
        color value;
        if (!ops::nearest_hue(sources, sem.hue, opts.min_semantic_chroma, value) ||
            ops::hue_distance(ops::hue(value), sem.hue) > 45.0f)
        {
            value = ops::synthesize_hue(accent, sem.hue, opts.min_semantic_chroma * 2.5f);
        }
        value = ops::ensure_contrast(value, background, opts.contrast_accent);
        pal.set_color(sem.key, value);
        pal.set_color(sem.on_key, on_color(value));
    }
}

void assign_editor(palette &pal, const color &background, const color &foreground, bool dark,
                   const normalize_options &opts)
{
    const color surface = role(pal, "surface", background);
    const color surface_variant = role(pal, "surface_variant", background);
    const color surface_container = role(pal, "surface_container", background);
    const color surface_high = role(pal, "surface_high", background);

    const color red = role(pal, "term1", foreground);
    const color green = role(pal, "term2", foreground);
    const color yellow = role(pal, "term3", foreground);
    const color blue = role(pal, "term4", foreground);
    const color magenta = role(pal, "term5", foreground);
    const color cyan = role(pal, "term6", foreground);
    const color orange = role(pal, "term9", foreground);

    pal.set_color("editor_background", background);
    pal.set_color("editor_gutter_background", background);
    // Structural roles follow the ramp so panels, selections and gutters stay
    // in step with the rest of the UI.
    pal.set_color("editor_line_highlight", surface);
    pal.set_color("editor_selection", surface_container);
    // A tier above `surface` so an inactive selection stays distinguishable
    // from the plain editor ground it sits on.
    pal.set_color("editor_selection_inactive", surface_variant);
    pal.set_color("editor_folded_background", surface);
    pal.set_color("editor_active_line_border", surface_high);
    pal.set_color("editor_indent_guide", surface_container);
    pal.set_color("editor_indent_guide_active", surface_high);
    pal.set_color("editor_whitespace", surface_high);

    set_contrasted(pal, "editor_foreground", foreground, background, opts.contrast_body);
    set_contrasted(pal, "editor_cursor", role(pal, "cursor", foreground), background,
                   opts.contrast_body);
    set_contrasted(pal, "editor_line_number_active", foreground, background, opts.contrast_body);
    set_contrasted(pal, "editor_operator", foreground, background, opts.contrast_body);
    set_contrasted(pal, "editor_variable", foreground, background, opts.contrast_body);
    set_contrasted(pal, "editor_punctuation", foreground, background, opts.contrast_body);

    const color muted = ops::step_surface(surface_high, opts.surface_step * 2.0f, dark);
    set_contrasted(pal, "editor_comment", muted, background, opts.contrast_muted);
    set_contrasted(pal, "editor_gutter_foreground", muted, background, opts.contrast_muted);
    set_contrasted(pal, "editor_line_number", muted, background, opts.contrast_muted);
    set_contrasted(pal, "editor_ignored", muted, background, opts.contrast_muted);

    const float accent_target = opts.contrast_accent;
    set_contrasted(pal, "editor_string", green, background, accent_target);
    set_contrasted(pal, "editor_number", orange, background, accent_target);
    set_contrasted(pal, "editor_boolean", orange, background, accent_target);
    set_contrasted(pal, "editor_constant", orange, background, accent_target);
    set_contrasted(pal, "editor_escape_character", orange, background, accent_target);
    set_contrasted(pal, "editor_keyword", magenta, background, accent_target);
    set_contrasted(pal, "editor_regex", magenta, background, accent_target);
    set_contrasted(pal, "editor_function", blue, background, accent_target);
    set_contrasted(pal, "editor_link", blue, background, accent_target);
    set_contrasted(pal, "editor_namespace", blue, background, accent_target);
    set_contrasted(pal, "editor_type", cyan, background, accent_target);
    set_contrasted(pal, "editor_class", cyan, background, accent_target);
    set_contrasted(pal, "editor_interface", cyan, background, accent_target);
    set_contrasted(pal, "editor_enum", cyan, background, accent_target);
    set_contrasted(pal, "editor_bracket_match", cyan, background, accent_target);
    set_contrasted(pal, "editor_property", yellow, background, accent_target);
    set_contrasted(pal, "editor_attribute", yellow, background, accent_target);
    set_contrasted(pal, "editor_decorator", yellow, background, accent_target);
    set_contrasted(pal, "editor_parameter", foreground, background, opts.contrast_muted);
    set_contrasted(pal, "editor_tag", red, background, accent_target);

    const color error = role(pal, "error", red);
    const color warning = role(pal, "warning", yellow);
    const color info = role(pal, "info", blue);
    const color success = role(pal, "success", green);

    pal.set_color("editor_error", error);
    pal.set_color("editor_invalid", error);
    pal.set_color("editor_warning", warning);
    pal.set_color("editor_info", info);
    pal.set_color("editor_hint", success);
    pal.set_color("editor_deleted", error);
    pal.set_color("editor_inserted", success);
    pal.set_color("editor_modified", warning);

    auto tinted_ground = [&](const color &hue_source) {
        ops::oklch c = ops::to_oklch(background);
        c.h = ops::hue(hue_source);
        c.c = std::max(c.c, 0.03f);
        c.l = ops::lightness(background) + (dark ? 0.05f : -0.05f);
        return ops::from_oklch(c);
    };
    pal.set_color("editor_error_background", tinted_ground(error));
    pal.set_color("editor_warning_background", tinted_ground(warning));
    pal.set_color("editor_info_background", tinted_ground(info));
    pal.set_color("editor_hint_background", tinted_ground(success));

    const color accent = role(pal, "accent", foreground);
    pal.set_color("editor_search_match", tinted_ground(accent));
    pal.set_color("editor_search_match_active", accent);
    pal.set_color("editor_find_range_highlight", tinted_ground(accent));
}

} // namespace

void normalize_palette(palette &pal, const std::vector<color> &source_colors,
                       const normalize_options &opts)
{
    if (!opts.enabled)
        return;

    color fallback_bg;
    fallback_bg.set(0x111111FF);
    const color background = role(pal, "background", fallback_bg);
    const bool dark = ops::is_dark(background);

    color fallback_fg = on_color(background);
    color foreground =
        ops::ensure_contrast(role(pal, "foreground", fallback_fg), background, opts.contrast_body);
    pal.set_color("background", background);
    pal.set_color("foreground", foreground);

    build_surface_ramp(pal, background, dark, opts.surface_step);

    color accent = role(pal, "accent", foreground);
    if (ops::chroma(accent) < opts.min_semantic_chroma)
    {
        color picked;
        if (ops::nearest_hue(source_colors, ops::hue(accent), opts.min_semantic_chroma, picked))
            accent = picked;
    }
    accent = ops::ensure_contrast(accent, background, opts.contrast_accent);
    pal.set_color("accent", accent);
    pal.set_color("on_accent", on_color(accent));

    color accent_secondary = role(pal, "accent_secondary", accent);
    if (ops::chroma(accent_secondary) < opts.min_semantic_chroma)
        accent_secondary =
            ops::synthesize_hue(accent, ops::hue(accent) + 40.0f, opts.min_semantic_chroma * 2.0f);
    accent_secondary = ops::ensure_contrast(accent_secondary, background, opts.contrast_accent);
    pal.set_color("accent_secondary", accent_secondary);
    pal.set_color("on_accent_secondary", on_color(accent_secondary));

    std::vector<color> pool = source_colors;
    pool.push_back(accent);
    pool.push_back(accent_secondary);

    assign_semantics(pal, pool, background, accent, opts);
    assign_ansi(pal, pool, background, foreground, accent, dark, opts);
    assign_base16(pal, background, foreground, dark, opts);
    assign_editor(pal, background, foreground, dark, opts);

    const color surface = role(pal, "surface", background);
    const color surface_variant = role(pal, "surface_variant", background);
    set_contrasted(pal, "on_background", foreground, background, opts.contrast_body);
    set_contrasted(pal, "on_surface", foreground, surface, opts.contrast_body);
    set_contrasted(pal, "on_surface_variant", foreground, surface_variant, opts.contrast_muted);

    if (!has_key(pal, "cursor"))
        pal.set_color("cursor", foreground);
    pal.set_color("cursor", ops::ensure_contrast(role(pal, "cursor", foreground), background,
                                                 opts.contrast_body));

    const color surface_high = role(pal, "surface_high", background);
    pal.set_color("border", ops::step_surface(surface_high, opts.surface_step * 0.5f, dark));
    pal.set_color("border_focused",
                  ops::step_surface(surface_high, opts.surface_step * 1.5f, dark));
}

} // namespace clrsync::core
