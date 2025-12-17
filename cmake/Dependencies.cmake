find_package(OpenGL REQUIRED)

if(WIN32)
    include(FetchContent)

    FetchContent_Declare(
        freetype
        URL https://download.savannah.gnu.org/releases/freetype/freetype-2.14.1.tar.gz
    )
    FetchContent_MakeAvailable(freetype)
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
    pkg_check_modules(WAYLAND_CLIENT wayland-client)
    pkg_check_modules(WAYLAND_EGL wayland-egl)
endif()

if (LINUX)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
endif()

if(USE_SYSTEM_GLFW)
    pkg_check_modules(GLFW REQUIRED glfw3)
else()
    include(FetchContent)
    FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.4
    )
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
    
    FetchContent_MakeAvailable(glfw)
    set(GLFW_FOUND TRUE)
    set(GLFW_INCLUDE_DIRS ${glfw_SOURCE_DIR}/include)
    set(GLFW_LIBRARIES glfw)
endif()

set(FREETYPE_EXTRA_LIBS "")
if(BROTLIDEC_LIBRARY AND BROTLICOMMON_LIBRARY)
    list(APPEND FREETYPE_EXTRA_LIBS ${BROTLIDEC_LIBRARY} ${BROTLICOMMON_LIBRARY})
    message(STATUS "Found Brotli libraries")
endif()

if(HARFBUZZ_FOUND)
    list(APPEND FREETYPE_EXTRA_LIBS ${HARFBUZZ_LIBRARIES})
    message(STATUS "Found HarfBuzz")
endif()

set(WAYLAND_LIBS "")
if(WAYLAND_CLIENT_FOUND)
    list(APPEND WAYLAND_LIBS ${WAYLAND_CLIENT_LIBRARIES})
    message(STATUS "Found Wayland client")
endif()
if(WAYLAND_EGL_FOUND)
    list(APPEND WAYLAND_LIBS ${WAYLAND_EGL_LIBRARIES})
    message(STATUS "Found Wayland EGL")
endif()

