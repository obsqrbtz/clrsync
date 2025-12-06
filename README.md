# clrsync

A theme management tool for synchronizing color schemes across multiple applications. clrsync allows you to define color palettes once and apply them consistently to all your terminal emulators, editors, and other configurable applications.

## Features

- **Unified Color Management**: Define color palettes in TOML format and apply them across multiple applications
- **CLI & GUI**: Choose between a command-line interface or a graphical editor
- **Live Reload**: Define post-apply hooks (configurable per template)
- **Flexible Color Formats**: Support for HEX, RGB, HSL with multi-component access (e.g., `{color.r}`, `{color.hex}`, `{color.hsl}`)
- **Pre-built Themes**: Includes popular themes

## Building

### Prerequisites

- C++20 compatible compiler (GCC, Clang, or MSVC)
- CMake or Meson
- OpenGL

### Using CMake

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

### Using Meson

```bash
meson setup builddir
meson compile -C builddir
```

## Installation

After building, you'll have:
- `clrsync_cli` - CLI
- `clrsync_gui` - GUI
- `libclrsync_core` - Shared lib

## Configuration

Create a configuration file at `~/.config/clrsync/config.toml`:

```toml
[general]
palettes_path = "~/.config/clrsync/palettes"
default_theme = "dark"

[templates.kitty]
input_path = "~/.config/clrsync/templates/kitty.conf"
output_path = "~/.config/kitty/clrsync.conf"
enabled = true
reload_cmd = "pkill -SIGUSR1 kitty"
```

### Palette Files

Create palette files in your `palettes_path` directory:

```toml
# ~/.config/clrsync/palettes/dark.toml
[general]
name = "dark"

[colors]
background = "#111318FF"
surface = "#1E1F25FF"
surface_variant = "#282A2FFF"

foreground = "#E2E2E9FF"
foreground_secondary = "#A8ABB3FF"

accent = "#00AA56FF"
outline = "#44474FFF"
shadow = "#00000080"
cursor = "#FFFFFFFF"

error = "#FF5F5FFF"
warning = "#FFC966FF"
success = "#6AD68BFF"
info = "#5DB2FFFF"

term_black   = "#111318FF"
term_red     = "#FF5F5FFF"
term_green   = "#00AA56FF"
term_yellow  = "#FFC966FF"
term_blue    = "#5DB2FFFF"
term_magenta = "#DEBCDFFF"
term_cyan    = "#86C9FFFF"
term_white   = "#E2E2E9FF"

term_black_bright   = "#33353AFF"
term_red_bright     = "#FFB780FF"
term_green_bright   = "#00CC6AFF"
term_yellow_bright  = "#FFD580FF"
term_blue_bright    = "#86C9FFFF"
term_magenta_bright = "#F0D6F0FF"
term_cyan_bright    = "#BFEFFFFF"
term_white_bright   = "#FFFFFFFF"
```

### Template Files

Create template files using color variables with flexible format specifiers:

```conf
# ~/.config/clrsync/templates/kitty.conf
cursor              {foreground}
cursor_text_color   {background}

foreground            {foreground}
background            {background}
selection_foreground  {foreground_secondary}
selection_background  {surface}
url_color             {accent}

color0      {background}
color1      {term_red}
color2      {term_green}
color3      {term_yellow}
color4      {term_blue}
color5      {term_magenta}
color6      {term_cyan}
color7      {term_white}
```

#### Color Format Specifiers

Access color components using dot notation:

```conf
# HEX formats
{color}                    # Default: #RRGGBB
{color.hex}                # #RRGGBB
{color.hex_stripped}       # RRGGBB
{color.hexa}               # #RRGGBBAA
{color.hexa_stripped}      # RRGGBBAA

# RGB components (0-255)
{color.rgb}                # rgb(r, g, b)
{color.r}                  # Red component
{color.g}                  # Green component
{color.b}                  # Blue component

# RGBA (alpha normalized 0-1)
{color.rgba}               # rgba(r, g, b, a)
{color.a}                  # Alpha component

# HSL (hue 0-360, saturation/lightness 0-1)
{color.hsl}                # hsl(h, s, l)
{color.h}                  # Hue component
{color.s}                  # Saturation component
{color.l}                  # Lightness component

# HSLA (hue 0-360, saturation/lightness/alpha 0-1)
{color.hsla}               # hsla(h, s, l, a)
{color.a}                  # Alpha component
```

## Usage

### CLI

List available themes:
```bash
clrsync_cli --list-themes
```

Apply the default theme:
```bash
clrsync_cli --apply
```

Apply a specific theme:
```bash
clrsync_cli --apply --theme rose-pine
```

Apply a theme from a file path:
```bash
clrsync_cli --apply --path /path/to/theme.toml
```

Show available color variables:
```bash
clrsync_cli --show-vars
```

Use a custom config file:
```bash
clrsync_cli --config /path/to/config.toml --apply
```

### GUI

Launch the graphical editor:
```bash
clrsync_gui
```

The GUI provides:
- **Color Scheme Editor**: Visual palette editor with color pickers
- **Template Editor**: Edit template files
- **Live Preview**: See changes in real-time

## Example Themes

The project includes several pre-configured themes in `example_config/palettes/`:
- `dark.toml`
- `light.toml`
- `flexoki.toml`
- `flexoki-light.toml`
- `rose-pine.toml`
- `rose-pine-moon.toml`
- `rose-pine-dawn.toml`

## Acknowledgments

This project uses the following open-source libraries:

- **[Dear ImGui](https://github.com/ocornut/imgui)** - Bloat-free graphical user interface library for C++
- **[GLFW](https://www.glfw.org/)** - Multi-platform library for OpenGL, OpenGL ES and Vulkan development
- **[toml++](https://github.com/marzer/tomlplusplus)** - Header-only TOML config file parser and serializer for C++17
- **[argparse](https://github.com/p-ranav/argparse)** - Argument Parser for Modern C++
- **[ImGuiColorTextEdit](https://github.com/BalazsJako/ImGuiColorTextEdit)** - Syntax highlighting text editor for ImGui

Special thanks to the creators of the included color schemes:
- **[Flexoki](https://stephango.com/flexoki)** by Steph Ango
- **[Rosé Pine](https://rosepinetheme.com/)** by the Rosé Pine team

