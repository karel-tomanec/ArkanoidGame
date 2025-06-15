
# Recursively collect all .cpp and .hpp files
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

add_executable (Arkanoid ${SOURCES})

target_include_directories(Arkanoid PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link SDL
target_link_libraries(Arkanoid PRIVATE SDL3::SDL3 SDL3_ttf::SDL3_ttf)

# Copy assets
add_custom_command(TARGET Arkanoid POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:Arkanoid>/assets
)


# Make working dir consistent in Visual Studio
set_target_properties(Arkanoid PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:Arkanoid>"
)