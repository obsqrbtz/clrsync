project('clrsync', 'cpp',
        version : '0.1.0',
        default_options : [
            'cpp_std=c++20',
            'buildtype=debug',
        ],
)

imgui_dep = dependency('imgui-docking',
                       fallback : ['imgui-docking', 'imgui_dep'],
                       default_options : [
                           'default_library=static',
                           'opengl=enabled',
                           'glfw=enabled',
                       ]
)

imgui_dep = declare_dependency(
    compile_args: ['-DIMGUI_ENABLE_FREETYPE'],
    dependencies: [imgui_dep]
)

glfw_dep = dependency('glfw3', fallback : ['glfw', 'glfw_dep'])
gl_dep = dependency('gl')

freetype_dep = dependency('freetype2')

inc_dirs = include_directories('src')
lib_inc_dirs = include_directories('lib')

cpp_args = []
if host_machine.system() == 'windows'
    compiler = meson.get_compiler('cpp')
    if compiler.get_id() == 'msvc'
        cpp_args += ['/EHsc']
    endif
endif

core_sources = [
    'src/core/palette/color.cpp',
    'src/core/io/toml_file.cpp',
    'src/core/config/config.cpp',
    'src/core/utils.cpp',
    'src/core/version.cpp',
    'src/core/theme/theme_template.cpp',
]

text_edit_sources = [
    'lib/color_text_edit/TextEditor.cpp',
]

clrsync_core = static_library(
    'clrsync_core',
    core_sources,
    include_directories : [inc_dirs, lib_inc_dirs],
    cpp_args : cpp_args,
)

clrsync_core_dep = declare_dependency(
    link_with : clrsync_core,
    include_directories : [inc_dirs, lib_inc_dirs],
)

clrsync_cli = executable(
    'clrsync_cli',
    'src/cli/main.cpp',
    include_directories : [inc_dirs, lib_inc_dirs],
    link_with : clrsync_core,
    cpp_args : cpp_args,
)

gui_sources = [
    'src/gui/main.cpp',
    'src/gui/color_scheme_editor.cpp',
    'src/gui/template_editor.cpp',
    'src/gui/palette_controller.cpp',
    'src/gui/template_controller.cpp',
    'src/gui/imgui_helpers.cpp',
    'src/gui/about_window.cpp',
    'src/gui/settings_window.cpp',
    'src/gui/font_loader.cpp',
]

clrsync_gui_link_args = []

fontconfig_dep = dependency('', required : false)
if host_machine.system() == 'linux'
    fontconfig_dep = dependency('fontconfig', required : true)
    clrsync_gui_link_args += ['-lX11', '-lXrandr', '-lXi']
endif

clrsync_gui = executable(
    'clrsync_gui',
    gui_sources,
    text_edit_sources,
    include_directories : [inc_dirs, lib_inc_dirs],
    dependencies : [
        clrsync_core_dep,
        imgui_dep,
        freetype_dep,
        glfw_dep,
        gl_dep,
        fontconfig_dep,
    ],
    cpp_args : cpp_args,
    link_args : clrsync_gui_link_args
)