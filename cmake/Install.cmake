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
    include(${CMAKE_SOURCE_DIR}/cmake/Icons.cmake)

    install(FILES resources/clrsync.desktop
        DESTINATION ${CMAKE_INSTALL_DATADIR}/applications
        COMPONENT Core
    )

    foreach(SIZE IN LISTS CLRSYNC_ICON_SIZES)
        install(FILES ${CLRSYNC_ICON_PNG_DIR}/clrsync-${SIZE}.png
            DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/${SIZE}x${SIZE}/apps
            RENAME clrsync.png
            COMPONENT Core
        )
    endforeach()

    install(FILES ${CMAKE_SOURCE_DIR}/assets/icons/clrsync.svg
        DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps
        RENAME clrsync.svg
        COMPONENT Core
    )
endif()
