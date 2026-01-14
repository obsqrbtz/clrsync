# Visual Studio Code

1. Install the [clrsync VS Code theme](https://marketplace.visualstudio.com/items?itemName=obsqrbtz.clrsync)

2. Download the [template file](code.json)

3. Configure the template in `~/.config/clrsync/config.toml`:

```toml
[templates.vscode]
enabled = true
input_path = '~/.config/clrsync/templates/code.json'
output_path = '~/.vscode/extensions/obsqrbtz.clrsync-1.0.2/themes/clrsync-color-theme.json'
reload_cmd = ''
```

4. Set the `clrsync` color scheme in VS Code
