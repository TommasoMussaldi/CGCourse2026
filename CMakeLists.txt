cmake_minimum_required(VERSION 3.16)

project(CGCourse LANGUAGES CXX C)
add_compile_definitions(GLM_ENABLE_EXPERIMENTAL)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)



# -----------------------------
# External libraries
# -----------------------------

# GLFW
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_SHARED_LIBRARY OFF CACHE BOOL "" FORCE)
add_subdirectory(external/glfw)

# GLAD
add_library(glad STATIC external/glad/src/glad.c)
target_include_directories(glad PUBLIC external/glad/include)
if(UNIX AND NOT APPLE)
  target_link_libraries(glad PUBLIC dl)
endif()

# GLM (interface)
add_library(glm INTERFACE)
target_include_directories(glm INTERFACE external)

# tinygltf (header-only)
target_include_directories(glm INTERFACE external/tinygltf)

# IMGUI
set(IMGUI_SOURCES
    ${CMAKE_SOURCE_DIR}/external/imgui/imgui.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui/imgui_draw.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui/imgui_tables.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui/imgui_widgets.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui/backends/imgui_impl_glfw.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui/backends/imgui_impl_opengl3.cpp
)

set(IMGUI_INCLUDES
    ${CMAKE_SOURCE_DIR}/external/imgui
    ${CMAKE_SOURCE_DIR}/external/imgui/backends
)

# -------------------------------------------------
# Project sources
# -------------------------------------------------
add_subdirectory(src)