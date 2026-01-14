# Qt

1. Download the templates:
   - Kvantum: [kvantum.kvconfig](kvantum.kvconfig) and [kvantum.svg](kvantum.svg)
   - Qt5ct/Qt6ct: [qtct.conf](qtct.conf)

2. Configure the templates in `~/.config/clrsync/config.toml`:

```toml
[templates.kvantum]
enabled = true
input_path = '~/.config/clrsync/templates/kvantum/kvantum.kvconfig'
output_path = '~/.config/Kvantum/clrsync/clrsync.kvconfig'
reload_cmd = ''

[templates.kvantum-svg]
enabled = true
input_path = '~/.config/clrsync/templates/kvantum/kvantum.svg'
output_path = '~/.config/Kvantum/clrsync/clrsync.svg'
reload_cmd = ''

[templates.qt5ct]
enabled = true
input_path = '~/.config/clrsync/templates/qtct.conf'
output_path = '~/.config/qt5ct/colors/clrsync.conf'
reload_cmd = ''

[templates.qt6ct]
enabled = true
input_path = '~/.config/clrsync/templates/qtct.conf'
output_path = '~/.config/qt6ct/colors/clrsync.conf'
reload_cmd = ''
```

3. Set the theme in `~/.config/Kvantum/kvantum.kvconfig`:

```conf
[General]
theme=clrsync
```

4. Set the theme in `~/.config/qt5ct/qt5ct.conf` and `~/.config/qt6ct/qt6ct.conf`:

```conf
[Appearance]
color_scheme_path=$HOME/.config/qt5ct/colors/clrsync.conf
custom_palette=true
```
