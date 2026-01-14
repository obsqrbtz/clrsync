# Neovim

1. Download the [template file](nvim.lua)

2. Configure the template in `~/.config/clrsync/config.toml`:

```toml
[templates.nvim]
enabled = true
input_path = '~/.config/clrsync/templates/nvim.lua'
output_path = '~/.config/nvim/colors/clrsync.lua'
reload_cmd = ''
```

3. Set the colorscheme in your Neovim config:

```lua
vim.cmd.colorscheme 'clrsync'
```
