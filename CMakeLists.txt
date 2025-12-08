cmake_minimum_required(VERSION 3.25)
project(clrsync VERSION 0.1.2 LANGUAGES CXX)

include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
    set(CMAKE_INSTALL_PREFIX "C:/Program Files/clrsync")
    set(CMAKE_INSTALL_BINDIR "bin")
    set(CMAKE_INSTALL_LIBDIR "lib")
    set(CMAKE_INSTALL_DATADIR "share")
    set(CMAKE_INSTALL_FULL_DATADIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}")
endif()

set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_INSTALL_RPATH "$ORIGIN/../${CMAKE_INSTALL_LIBDIR}")

configure_file(
    ${CMAKE_SOURCE_DIR}/src/core/version.hpp.in
    ${CMAKE_SOURCE_DIR}/src/core/version.hpp
    @ONLY
)

configure_file(
    ${CMAKE_SOURCE_DIR}/PKGBUILD.in
    ${CMAKE_SOURCE_DIR}/PKGBUILD
    @ONLY
)

find_package(OpenGL REQUIRED)

if(WIN32)
    include(FetchContent)
    FetchContent_Declare(
        freetype
        URL https://download.savannah.gnu.org/releases/freetype/freetype-2.14.1.tar.gz
    )
    FetchContent_MakeAvailable(freetype)

    FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.3.10
    )
    FetchContent_MakeAvailable(glfw)

else()
    find_package(Freetype REQUIRED)
    find_package(PkgConfig REQUIRED)
    find_package(Fontconfig REQUIRED)
    pkg_check_modules(GLFW REQUIRED glfw3)
endif()

set(CORE_SOURCES
    src/core/palette/color.cpp
    src/core/io/toml_file.cpp
    src/core/config/config.cpp
    src/core/utils.cpp
    src/core/version.cpp
    src/core/theme/theme_template.cpp
)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
add_library(clrsync_core SHARED ${CORE_SOURCES})
target_include_directories(clrsync_core PUBLIC src SYSTEM lib)
target_compile_definitions(clrsync_core PUBLIC
    CLRSYNC_DATADIR=\"${CMAKE_INSTALL_FULL_DATADIR}/clrsync\"
)

add_executable(clrsync_cli src/cli/main.cpp)
target_include_directories(clrsync_cli PRIVATE src SYSTEM lib)
target_link_libraries(clrsync_cli PRIVATE clrsync_core)

set(GUI_SOURCES
    src/gui/main.cpp
    src/gui/color_scheme_editor.cpp
    src/gui/template_editor.cpp
    src/gui/palette_controller.cpp
    src/gui/template_controller.cpp
    lib/color_text_edit/TextEditor.cpp
    src/gui/imgui_helpers.cpp
    src/gui/imgui_helpers.hpp
    src/gui/about_window.cpp
    src/gui/settings_window.cpp
    src/gui/font_loader.cpp
)
add_executable(clrsync_gui ${GUI_SOURCES})
target_include_directories(clrsync_gui PRIVATE src SYSTEM lib)
# if(WIN32)
#     set_target_properties(clrsync_gui PROPERTIES
#         LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup"
#     )
# endif()

if(WIN32)
    target_link_libraries(clrsync_gui PRIVATE clrsync_core glfw freetype imgui OpenGL::GL)
else()
    target_include_directories(clrsync_gui PRIVATE ${FREETYPE_INCLUDE_DIRS} ${GLFW_INCLUDE_DIRS})
    target_link_libraries(clrsync_gui PRIVATE clrsync_core imgui ${FREETYPE_LIBRARIES} ${GLFW_LIBRARIES} X11 Xrandr Xi Fontconfig::Fontconfig OpenGL::GL)
endif()

set(imgui_SOURCE_DIR lib/imgui)
add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
    ${imgui_SOURCE_DIR}/misc/freetype/imgui_freetype.cpp
)

target_include_directories(imgui PUBLIC SYSTEM
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

if(WIN32)
    target_include_directories(imgui PUBLIC ${GLFW_INCLUDE_DIRS} ${freetype_SOURCE_DIR}/include)
else()
    target_include_directories(imgui PUBLIC ${GLFW_INCLUDE_DIRS} ${FREETYPE_INCLUDE_DIRS})
endif()

target_link_libraries(imgui PUBLIC glfw OpenGL::GL freetype)
target_compile_definitions(imgui PUBLIC IMGUI_ENABLE_FREETYPE)

install(TARGETS clrsync_core
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        COMPONENT Core
)

install(TARGETS clrsync_cli
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        COMPONENT CLI
)

install(TARGETS clrsync_gui
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        COMPONENT GUI
)

install(FILES
        example_config/config.toml
        DESTINATION ${CMAKE_INSTALL_DATADIR}/clrsync
        COMPONENT Core
)

install(DIRECTORY example_config/templates
        DESTINATION ${CMAKE_INSTALL_DATADIR}/clrsync
        COMPONENT Core
        FILES_MATCHING PATTERN "*"
)

install(DIRECTORY example_config/palettes
        DESTINATION ${CMAKE_INSTALL_DATADIR}/clrsync
        COMPONENT Core
        FILES_MATCHING PATTERN "*.toml"
)

if(UNIX AND NOT APPLE)
    install(FILES resources/clrsync.desktop
        DESTINATION ${CMAKE_INSTALL_DATADIR}/applications
        COMPONENT Core
    )
endif()

set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE.txt")
set(CPACK_COMPONENTS_ALL Core GUI CLI)

set(CPACK_PACKAGE_NAME "clrsync")
set(CPACK_PACKAGE_VENDOR "Daniel Dada")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Color scheme manager")

set(CPACK_COMPONENTS_ALL Core GUI CLI)
set(CPACK_COMPONENT_CORE_DISPLAY_NAME "Core Library")
set(CPACK_COMPONENT_CORE_DESCRIPTION "clrsync core library and default configs (required)")
set(CPACK_COMPONENT_CORE_REQUIRED ON)
set(CPACK_COMPONENT_GUI_DISPLAY_NAME "GUI Application")
set(CPACK_COMPONENT_GUI_DESCRIPTION "clrsync GUI app")
set(CPACK_COMPONENT_GUI_DEPENDS Core)
set(CPACK_COMPONENT_CLI_DISPLAY_NAME "Command Line Tool")
set(CPACK_COMPONENT_CLI_DESCRIPTION "clrsync CLI app")
set(CPACK_COMPONENT_CLI_DEPENDS Core)

set(CPACK_GENERATOR "NSIS")

set(CPACK_NSIS_INSTALLED_NAME "clrsync")
set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")

set(CPACK_NSIS_MODIFY_PATH ON)
set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)

set(CPACK_NSIS_MENU_LINKS "bin/clrsync_gui.exe" "clrsync")
set(CPACK_NSIS_CREATE_DESKTOP_LINKS "bin/clrsync_gui.exe;clrsync")

message(STATUS "CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "CMAKE_INSTALL_DATADIR: ${CMAKE_INSTALL_DATADIR}")
message(STATUS "CMAKE_INSTALL_FULL_DATADIR: ${CMAKE_INSTALL_FULL_DATADIR}")
message(STATUS "CMAKE_SIZEOF_VOID_P: ${CMAKE_SIZEOF_VOID_P}")

include(CPack)
