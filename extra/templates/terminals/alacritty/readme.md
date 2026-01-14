# Alacritty

1. Download the [template file](alacritty.toml)

2. Configure the template in `~/.config/clrsync/config.toml`:

```toml
[templates.alacritty]
enabled = true
input_path = '~/.config/clrsync/templates/alacritty.toml'
output_path = '~/.config/alacritty/clrsync.toml'
reload_cmd = ''
```

3. Import the generated color scheme in `~/.config/alacritty/alacritty.toml`:

```toml
[general]
import = ["clrsync.toml"]
```
