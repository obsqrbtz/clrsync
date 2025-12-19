set(GUI_SOURCES
    main.cpp
        views/color_scheme_editor.cpp
        views/color_table_renderer.cpp
        views/preview_renderer.cpp
        controllers/theme_applier.cpp
        views/template_editor.cpp
        controllers/palette_controller.cpp
        controllers/template_controller.cpp
        views/about_window.cpp
        views/settings_window.cpp
        widgets/colors.cpp
        widgets/dialogs.cpp
        widgets/palette_selector.cpp
        widgets/input_dialog.cpp
        widgets/action_buttons.cpp
        widgets/styled_checkbox.cpp
        widgets/autocomplete.cpp
        widgets/form_field.cpp
        widgets/error_message.cpp
        widgets/settings_buttons.cpp
        widgets/section_header.cpp
        widgets/link_button.cpp
        widgets/centered_text.cpp
        widgets/validation_message.cpp
        widgets/template_controls.cpp
        layout/main_layout.cpp
        platform/windows/font_loader_windows.cpp
        platform/linux/font_loader_linux.cpp
        platform/macos/font_loader_macos.cpp
        platform/linux/file_browser_linux.cpp
        platform/windows/file_browser_windows.cpp
        ${CMAKE_SOURCE_DIR}/lib/color_text_edit/TextEditor.cpp
        backend/glfw_opengl.cpp
        ui_manager.cpp
)

if(MACOS)
    list(APPEND GUI_SOURCES
        platform/macos/file_browser_macos.mm
    )
endif()

if(WIN32)
    add_executable(clrsync_gui WIN32 ${GUI_SOURCES})
else()
    add_executable(clrsync_gui ${GUI_SOURCES})
endif()

target_include_directories(clrsync_gui PRIVATE 
    ${CMAKE_SOURCE_DIR}/src 
    SYSTEM ${CMAKE_SOURCE_DIR}/lib
)

if(WIN32)
    target_link_libraries(clrsync_gui PRIVATE 
        clrsync_core 
        glfw 
        imgui 
        OpenGL::GL
        shell32
        ole32
        uuid
        comdlg32
        shlwapi
    )
    if (MSVC)
        target_link_options(clrsync_gui PRIVATE /ENTRY:mainCRTStartup)
    endif()
elseif(APPLE)
    target_link_libraries(clrsync_gui PRIVATE
        clrsync_core
        glfw
        imgui
        OpenGL::GL
        "-framework Cocoa"
    )
else() 
    target_link_libraries(clrsync_gui PRIVATE
        clrsync_core 
        imgui
        ${GLFW_LIBRARIES} 
        ${WAYLAND_LIBS}
        X11 
        Xrandr 
        Xi 
        Fontconfig::Fontconfig 
        OpenGL::GL
        ${GTK3_LIBRARIES}
    )
    
    target_include_directories(clrsync_gui PRIVATE ${GTK3_INCLUDE_DIRS})
    target_compile_options(clrsync_gui PRIVATE ${GTK3_CFLAGS_OTHER})
endif()
