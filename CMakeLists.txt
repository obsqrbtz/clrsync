cmake_minimum_required(VERSION 3.25)
project(clrsync VERSION 1.0.0 LANGUAGES CXX)

include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(WRITE "${CMAKE_SOURCE_DIR}/.clangd" 
    "CompileFlags:\n  CompilationDatabase: ${CMAKE_BINARY_DIR}\n")

option(USE_SYSTEM_GLFW "Use system-installed GLFW instead of fetching it statically" OFF)
message(STATUS "USE_SYSTEM_GLFW: ${USE_SYSTEM_GLFW}")

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

if(DEFINED CLRSYNC_SEMVER)
  set(SEMVER "${CLRSYNC_SEMVER}")
else()
  find_package(Git QUIET)

    if(GIT_FOUND AND EXISTS "${CMAKE_SOURCE_DIR}/.git")
    execute_process(
        COMMAND git describe --tags --long --always
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_DESCRIBE
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
    endif()

    if(GIT_DESCRIBE MATCHES "^[vV]?[0-9]+\\.[0-9]+\\.[0-9]+-[0-9]+-g[0-9a-f]+")
    string(REGEX REPLACE
        "^[vV]?([0-9]+\\.[0-9]+\\.[0-9]+)-([0-9]+)-g([0-9a-f]+)"
        "\\1+git.g\\3"
        SEMVER "${GIT_DESCRIBE}"
    )
    elseif(GIT_DESCRIBE)
    set(SEMVER "${PROJECT_VERSION}.git.${GIT_DESCRIBE}")
    else()
    set(SEMVER "${PROJECT_VERSION}")
    endif()
endif()

message(STATUS "clrsync version: ${SEMVER}")

configure_file(
    ${CMAKE_SOURCE_DIR}/src/core/common/version.hpp.in
    ${CMAKE_SOURCE_DIR}/src/core/common/version.hpp
    @ONLY
)

configure_file(
    ${CMAKE_SOURCE_DIR}/VERSION.in
    ${CMAKE_SOURCE_DIR}/VERSION
    @ONLY
)

configure_file(
    ${CMAKE_SOURCE_DIR}/AUR/PKGBUILD.in
    ${CMAKE_SOURCE_DIR}/AUR/PKGBUILD
    @ONLY
)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
include(Dependencies)
include(ImGui)

add_subdirectory(src/core)
add_subdirectory(src/cli)
add_subdirectory(src/gui)

include(Install)
include(Packaging)

message(STATUS "CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "CMAKE_INSTALL_FULL_DATADIR: ${CMAKE_INSTALL_FULL_DATADIR}")

message(STATUS "CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "CMAKE_INSTALL_FULL_DATADIR: ${CMAKE_INSTALL_FULL_DATADIR}")
