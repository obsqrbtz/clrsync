add_executable(clrsync_cli main.cpp)

target_include_directories(clrsync_cli PRIVATE 
    ${CMAKE_SOURCE_DIR}/src 
    SYSTEM ${CMAKE_SOURCE_DIR}/lib
)

target_link_libraries(clrsync_cli PRIVATE clrsync_core)
