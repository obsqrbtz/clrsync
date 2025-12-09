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
