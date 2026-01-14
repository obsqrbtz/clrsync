# Firefox

1. Download the [template file](userChrome.css)

2. Configure the template in `~/.config/clrsync/config.toml`:

```toml
[templates.firefox]
enabled = true
input_path = '~/.config/clrsync/templates/userChrome.css'
output_path = '~/.mozilla/firefox/<profile>/chrome/userChrome.css'
reload_cmd = ''
```

3. Enable `toolkit.legacyUserProfileCustomizations.stylesheets` in `about:config`

4. Restart Firefox to apply the theme
