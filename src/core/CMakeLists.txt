set(CORE_SOURCES
    palette/color.cpp
    io/toml_file.cpp
    config/config.cpp
    utils.cpp
    version.cpp
    theme/theme_template.cpp
)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

add_library(clrsync_core SHARED ${CORE_SOURCES})

target_include_directories(clrsync_core PUBLIC 
    ${CMAKE_SOURCE_DIR}/src 
    SYSTEM ${CMAKE_SOURCE_DIR}/lib
)

target_compile_definitions(clrsync_core PUBLIC
    CLRSYNC_DATADIR=\"${CMAKE_INSTALL_FULL_DATADIR}/clrsync\"
)
