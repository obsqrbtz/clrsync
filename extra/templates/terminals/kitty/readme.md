# Kitty

1. Download the [template file](kitty.conf)

2. Configure the template in `~/.config/clrsync/config.toml`:

```toml
[templates.kitty]
enabled = true
input_path = '~/.config/clrsync/templates/kitty.conf'
output_path = '~/.config/kitty/clrsync.conf'
reload_cmd = 'pkill -SIGUSR1 kitty'
```

3. Import the generated color scheme in `~/.config/kitty/kitty.conf`:

```conf
include clrsync.conf
```
