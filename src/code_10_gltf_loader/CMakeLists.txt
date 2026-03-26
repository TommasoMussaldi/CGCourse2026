add_executable(code_10_gltf_loader
    main_10.cpp
    ${IMGUI_SOURCES}       # <-- i .cpp di ImGui
)

find_package(OpenGL)

# Include directories
target_include_directories(code_10_gltf_loader PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}       # include locali
    ${IMGUI_INCLUDES}                 # <-- include di imgui + backend
)

# Per Visual Studio: directory di lavoro
set_property(TARGET code_10_gltf_loader PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

# Link libraries
target_link_libraries(code_10_gltf_loader PRIVATE
    glfw
    glad
    glm
    OpenGL::GL
)

# ---------------------------
# COMMON FILES (cartella src/common)
# ---------------------------
file(GLOB_RECURSE COMMON_FILES
    ${CMAKE_SOURCE_DIR}/src/common/*
)
target_sources(code_10_gltf_loader PRIVATE ${COMMON_FILES})
source_group(TREE ${CMAKE_SOURCE_DIR}/src/common PREFIX "common" FILES ${COMMON_FILES})

# ---------------------------
# SHADERS (cartella shaders)
# ---------------------------
file(GLOB_RECURSE SHADER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders/*
)
target_sources(code_10_gltf_loader PRIVATE ${SHADER_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/shaders PREFIX "shaders" FILES ${SHADER_FILES})

# ---------------------------
# IMGUI (cartella virtuale)
# ---------------------------
# Separa core e backends per avere due sottocartelle: imgui e imgui/backends
set(_IMGUI_CORE_SOURCES ${IMGUI_SOURCES})
list(FILTER _IMGUI_CORE_SOURCES EXCLUDE REGEX "/backends/")

set(_IMGUI_BACKEND_SOURCES ${IMGUI_SOURCES})
list(FILTER _IMGUI_BACKEND_SOURCES INCLUDE REGEX "/backends/")

# Crea gruppi nell'IDE (Visual Studio/Xcode, ecc.)
# 1) Tutti i file ImGui in una singola cartella "imgui" (flat)
#    (decommenta questa riga se preferisci una sola cartella)
# source_group("imgui" FILES ${IMGUI_SOURCES})

# 2) Oppure, meglio: struttura con sottocartelle "imgui" e "imgui/backends"
source_group("imgui"           FILES ${_IMGUI_CORE_SOURCES})
source_group("imgui/backends"  FILES ${_IMGUI_BACKEND_SOURCES})