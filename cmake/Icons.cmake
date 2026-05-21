set(CLRSYNC_ICON_PNG_DIR ${CMAKE_SOURCE_DIR}/assets/icons/png)
set(CLRSYNC_ICON_SIZES 16 24 32 48 64 128 256 512)

function(clrsync_embed_window_icons target)
    find_program(XXD NAMES xxd)
    if(NOT XXD)
        message(WARNING "xxd not found; ${target} will be built without embedded window icons")
        return()
    endif()
    set(EMBED_SIZES 32 256)
    set(EMBEDDED_SOURCES "")
    foreach(SIZE IN LISTS EMBED_SIZES)
        set(ICON_PNG ${CLRSYNC_ICON_PNG_DIR}/clrsync-${SIZE}.png)
        set(ICON_CPP ${CMAKE_CURRENT_BINARY_DIR}/clrsync_icon_${SIZE}.cpp)
        add_custom_command(
            OUTPUT ${ICON_CPP}
            COMMAND ${XXD} -i -n clrsync_icon_${SIZE}_png ${ICON_PNG} > ${ICON_CPP}
            DEPENDS ${ICON_PNG}
            COMMENT "Embedding clrsync ${SIZE}px window icon"
            VERBATIM
        )
        list(APPEND EMBEDDED_SOURCES ${ICON_CPP})
    endforeach()
    target_sources(${target} PRIVATE ${EMBEDDED_SOURCES})
    target_link_libraries(${target} PRIVATE PNG::PNG)
endfunction()
