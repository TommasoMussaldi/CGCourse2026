add_executable(code_05_assonometry main_05.cpp)

find_package(OpenGL)


target_include_directories(code_05_assonometry PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}        # anchors relative includes here
)

set_property(TARGET code_05_assonometry PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

target_link_libraries(code_05_assonometry
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
target_sources(code_05_assonometry PRIVATE ${COMMON_FILES})

# Organize them into a virtual folder in the IDE
source_group(TREE ${CMAKE_SOURCE_DIR}/src/common PREFIX "common" FILES ${COMMON_FILES})


# Collect all files inside the folder
file(GLOB_RECURSE COMMON_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders/*
)

# Add them to the target so the IDE shows them
target_sources(code_05_assonometry PRIVATE ${COMMON_FILES})

# Organize them into a virtual folder in the IDE
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/shaders PREFIX "shaders" FILES ${COMMON_FILES})