add_executable(code_08_raster main_08.cpp)

find_package(OpenGL)

# Copy the shaders directory to the build directory
add_custom_command(
    TARGET code_08_raster POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders
        $<TARGET_FILE_DIR:code_08_raster>/shaders
)

target_include_directories(code_08_raster PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}        # anchors relative includes here
)

set_property(TARGET code_08_raster PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

target_link_libraries(code_08_raster
    PRIVATE
        glfw
        glad
	glm
        OpenGL::GL
)
	
# Collect all files inside the folder
file(GLOB_RECURSE COMMON_FILES
    ${CMAKE_SOURCE_DIR}/src/common/*
)


# Add them to the target so the IDE shows them
target_sources(code_08_raster PRIVATE ${COMMON_FILES})

# Organize them into a virtual folder in the IDE
source_group(TREE ${CMAKE_SOURCE_DIR}/src/common PREFIX "common" FILES ${COMMON_FILES})


# Collect all files inside the folder
file(GLOB_RECURSE COMMON_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders/*
)

# Add them to the target so the IDE shows them
target_sources(code_08_raster PRIVATE ${COMMON_FILES})

# Organize them into a virtual folder in the IDE
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/shaders PREFIX "shaders" FILES ${COMMON_FILES})