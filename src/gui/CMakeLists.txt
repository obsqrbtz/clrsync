set(GUI_SOURCES
    main.cpp
    color_scheme_editor.cpp
    template_editor.cpp
    palette_controller.cpp
    template_controller.cpp
    imgui_helpers.cpp
    imgui_helpers.hpp
    about_window.cpp
    settings_window.cpp
    font_loader.cpp
    ${CMAKE_SOURCE_DIR}/lib/color_text_edit/TextEditor.cpp
)

add_executable(clrsync_gui ${GUI_SOURCES})

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
    )
endif()
