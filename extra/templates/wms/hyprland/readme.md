# Hyprland

1. Download the [template file](hyprland.conf)

2. Configure the template in `~/.config/clrsync/config.toml`:

```toml
[templates.hyprland]
enabled = true
input_path = '~/.config/clrsync/templates/hyprland.conf'
output_path = '~/.config/hypr/clrsync.conf'
reload_cmd = ''
```

3. Source the generated config in `~/.config/hypr/hyprland.conf`:

```conf
source = clrsync.conf
```
