# Ghostty

1. Download the [template file](ghostty)

2. Configure the template in `~/.config/clrsync/config.toml`:

```toml
[templates.ghostty]
enabled = true
input_path = '~/.config/clrsync/templates/ghostty'
output_path = '~/.config/ghostty/themes/clrsync'
reload_cmd = 'pkill -SIGUSR2 ghostty'
```

3. Set the generated color scheme in `~/.config/ghostty/config`:

```conf
theme = "clrsync"
```
