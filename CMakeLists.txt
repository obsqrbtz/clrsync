cmake_minimum_required(VERSION 3.25)
project(clrsync VERSION 0.1.3 LANGUAGES CXX)

include(GNUInstallDirs)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -----------------------------
# Platform-specific install dirs
# -----------------------------
if(WIN32)
    set(CMAKE_INSTALL_PREFIX "C:/Program Files/clrsync")
    set(CMAKE_INSTALL_BINDIR "bin")
    set(CMAKE_INSTALL_LIBDIR "lib")
    set(CMAKE_INSTALL_DATADIR "share")
    set(CMAKE_INSTALL_FULL_DATADIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}")
endif()

# -----------------------------
# RPATH settings
# -----------------------------
set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_INSTALL_RPATH "$ORIGIN/../${CMAKE_INSTALL_LIBDIR}")

# -----------------------------
# Templates
# -----------------------------
configure_file(
    ${CMAKE_SOURCE_DIR}/src/core/version.hpp.in
    ${CMAKE_SOURCE_DIR}/src/core/version.hpp
    @ONLY
)

configure_file(
    ${CMAKE_SOURCE_DIR}/AUR/PKGBUILD.in
    ${CMAKE_SOURCE_DIR}/AUR/PKGBUILD
    @ONLY
)

# -----------------------------
# Find packages
# -----------------------------
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
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(glfw)
else()
    find_package(Freetype REQUIRED)
    find_package(PkgConfig REQUIRED)
    find_package(Fontconfig REQUIRED)
    find_package(ZLIB REQUIRED)
    find_package(BZip2 REQUIRED)
    find_package(PNG REQUIRED)

    find_library(BROTLIDEC_LIBRARY NAMES brotlidec)
    find_library(BROTLICOMMON_LIBRARY NAMES brotlicommon)

    pkg_check_modules(HARFBUZZ harfbuzz)
    pkg_check_modules(GLFW REQUIRED glfw3)
    pkg_check_modules(WAYLAND_CLIENT wayland-client)
    pkg_check_modules(WAYLAND_EGL wayland-egl)

    include(CheckSymbolExists)
    check_symbol_exists(glfwGetWaylandDisplay "GLFW/glfw3.h" GLFW_HAS_WAYLAND)
endif()

# -----------------------------
# Extra libraries for Freetype
# -----------------------------
set(FREETYPE_EXTRA_LIBS "")
if(BROTLIDEC_LIBRARY AND BROTLICOMMON_LIBRARY)
    list(APPEND FREETYPE_EXTRA_LIBS ${BROTLIDEC_LIBRARY} ${BROTLICOMMON_LIBRARY})
    message(STATUS "Found Brotli libraries")
endif()

if(HARFBUZZ_FOUND)
    list(APPEND FREETYPE_EXTRA_LIBS ${HARFBUZZ_LIBRARIES})
    message(STATUS "Found HarfBuzz")
endif()

# -----------------------------
# Wayland
# -----------------------------
set(WAYLAND_LIBS "")
if(WAYLAND_CLIENT_FOUND)
    list(APPEND WAYLAND_LIBS ${WAYLAND_CLIENT_LIBRARIES})
    message(STATUS "Found Wayland client")
endif()
if(WAYLAND_EGL_FOUND)
    list(APPEND WAYLAND_LIBS ${WAYLAND_EGL_LIBRARIES})
    message(STATUS "Found Wayland EGL")
endif()

# -----------------------------
# Core
# -----------------------------
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

# -----------------------------
# CLI
# -----------------------------
add_executable(clrsync_cli src/cli/main.cpp)
target_include_directories(clrsync_cli PRIVATE src SYSTEM lib)
target_link_libraries(clrsync_cli PRIVATE clrsync_core)

# -----------------------------
# GUI
# -----------------------------
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

# imgui
set(IMGUI_SOURCE_DIR lib/imgui)
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
    target_link_libraries(clrsync_gui PRIVATE clrsync_core glfw freetype imgui OpenGL::GL)
else()
    target_include_directories(imgui PUBLIC ${GLFW_INCLUDE_DIRS} ${FREETYPE_INCLUDE_DIRS})
    target_link_libraries(clrsync_gui PRIVATE
        clrsync_core imgui Freetype::Freetype ${FREETYPE_EXTRA_LIBS}
        ZLIB::ZLIB BZip2::BZip2 PNG::PNG
        ${GLFW_LIBRARIES} ${WAYLAND_LIBS}
        X11 Xrandr Xi Fontconfig::Fontconfig OpenGL::GL
    )
endif()

# -----------------------------
# Installation
# -----------------------------
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

install(FILES example_config/config.toml
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

# -----------------------------
# CPack configuration
# -----------------------------
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE.txt")
set(CPACK_PACKAGE_NAME "clrsync")
set(CPACK_PACKAGE_VENDOR "Daniel Dada")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Color scheme manager")
set(CPACK_GENERATOR "NSIS;DEB;RPM")

# Components
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

# NSIS
set(CPACK_NSIS_INSTALLED_NAME "clrsync")
set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")
set(CPACK_NSIS_MODIFY_PATH ON)
set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
set(CPACK_NSIS_MENU_LINKS "bin/clrsync_gui.exe" "clrsync")
set(CPACK_NSIS_CREATE_DESKTOP_LINKS "bin/clrsync_gui.exe;clrsync")

# Debian
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Daniel Dada <dan@binarygoose.dev>")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.31), libglfw3, libfreetype6")
set(CPACK_DEBIAN_PACKAGE_SECTION "utils")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")

# RPM
set(CPACK_RPM_PACKAGE_LICENSE "MIT")
set(CPACK_RPM_PACKAGE_GROUP "Applications/System")
set(CPACK_RPM_PACKAGE_URL "https://github.com/obsqrbtz/clrsync")
set(CPACK_RPM_PACKAGE_REQUIRES "freetype, glfw, fontconfig")

include(CPack)

message(STATUS "CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "CMAKE_INSTALL_FULL_DATADIR: ${CMAKE_INSTALL_FULL_DATADIR}")
