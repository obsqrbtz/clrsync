set(IMGUI_SOURCE_DIR ${CMAKE_SOURCE_DIR}/lib/imgui)

add_library(imgui STATIC
    ${IMGUI_SOURCE_DIR}/imgui.cpp
    ${IMGUI_SOURCE_DIR}/imgui_draw.cpp
    ${IMGUI_SOURCE_DIR}/imgui_widgets.cpp
    ${IMGUI_SOURCE_DIR}/imgui_tables.cpp
    ${IMGUI_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
    ${IMGUI_SOURCE_DIR}/misc/freetype/imgui_freetype.cpp
)

target_include_directories(imgui PUBLIC SYSTEM
    ${IMGUI_SOURCE_DIR}
    ${IMGUI_SOURCE_DIR}/backends
)

target_compile_definitions(imgui PUBLIC IMGUI_ENABLE_FREETYPE)

if(WIN32)
    target_include_directories(imgui PUBLIC ${GLFW_INCLUDE_DIRS} ${freetype_SOURCE_DIR}/include)
    target_link_libraries(imgui PUBLIC freetype)
else()
    target_include_directories(imgui PUBLIC ${GLFW_INCLUDE_DIRS} ${FREETYPE_INCLUDE_DIRS})
    target_link_libraries(imgui PRIVATE 
        Freetype::Freetype 
        ${FREETYPE_EXTRA_LIBS}
        ZLIB::ZLIB 
        BZip2::BZip2 
        PNG::PNG
    )
endif()
