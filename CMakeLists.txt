cmake_minimum_required(VERSION 3.16)
project(stable_fluids
        VERSION 0.1.0
        DESCRIPTION "Real-time 2D Stable Fluids simulator (OpenGL 4.1 Core)"
        LANGUAGES C CXX)

# ---------------------------------------------------------------------------
# Options
# ---------------------------------------------------------------------------
option(STABLE_FLUIDS_BUILD_TESTS "Build CPU and GPU tests"                 ON)
option(STABLE_FLUIDS_WARNINGS_AS_ERRORS "Promote warnings to errors"       OFF)
option(STABLE_FLUIDS_ENABLE_SANITIZERS "Enable ASan/UBSan in debug builds" OFF)
option(STABLE_FLUIDS_FETCH_GLFW "Fall back to FetchContent for GLFW"       ON)
set(STABLE_FLUIDS_IMGUI_SOURCE_DIR "" CACHE PATH
    "Path to a local Dear ImGui source tree; empty fetches the pinned release")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Default build type" FORCE)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(CompilerWarnings)
include(Sanitizers)
include(FetchContent)

# ---------------------------------------------------------------------------
# OpenGL + GLFW dependencies
# ---------------------------------------------------------------------------
find_package(OpenGL REQUIRED)

set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)

# Prefer an installed GLFW (Homebrew, apt, vcpkg). Fall back to FetchContent.
find_package(glfw3 3.3 QUIET)
if(NOT glfw3_FOUND)
    if(STABLE_FLUIDS_FETCH_GLFW)
        message(STATUS "GLFW not found locally; fetching pinned source...")
        FetchContent_Declare(
            glfw
            GIT_REPOSITORY https://github.com/glfw/glfw.git
            GIT_TAG        3.3.9
        )
        FetchContent_MakeAvailable(glfw)
    else()
        message(FATAL_ERROR "GLFW not found and STABLE_FLUIDS_FETCH_GLFW is OFF.")
    endif()
endif()

# ---------------------------------------------------------------------------
# Dear ImGui
# ---------------------------------------------------------------------------
if(STABLE_FLUIDS_IMGUI_SOURCE_DIR)
    set(imgui_SOURCE_DIR "${STABLE_FLUIDS_IMGUI_SOURCE_DIR}")
else()
    FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG        v1.92.8
        GIT_SHALLOW    TRUE
    )
    FetchContent_MakeAvailable(imgui)
endif()

add_library(imgui STATIC
    "${imgui_SOURCE_DIR}/imgui.cpp"
    "${imgui_SOURCE_DIR}/imgui_draw.cpp"
    "${imgui_SOURCE_DIR}/imgui_tables.cpp"
    "${imgui_SOURCE_DIR}/imgui_widgets.cpp"
    "${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp"
    "${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp"
)
target_include_directories(imgui PUBLIC
    "${imgui_SOURCE_DIR}"
    "${imgui_SOURCE_DIR}/backends"
)
target_link_libraries(imgui PUBLIC glfw OpenGL::GL)

# ---------------------------------------------------------------------------
# Hand-rolled minimal GLAD loader for OpenGL 4.1 Core
# ---------------------------------------------------------------------------
add_library(glad STATIC third_party/glad/src/glad.c)
target_include_directories(glad PUBLIC third_party/glad/include)
set_target_properties(glad PROPERTIES POSITION_INDEPENDENT_CODE ON)

# ---------------------------------------------------------------------------
# Stable Fluids core library
# ---------------------------------------------------------------------------
set(STABLE_FLUIDS_SOURCES
    src/Application.cpp
    src/FluidGrid.cpp
    src/FluidSolver.cpp
    src/Framebuffer.cpp
    src/InputController.cpp
    src/Renderer.cpp
    src/ShaderProgram.cpp
    src/Texture2D.cpp
    src/UserInterface.cpp
)

add_library(stable_fluids_core STATIC ${STABLE_FLUIDS_SOURCES})
target_include_directories(stable_fluids_core PUBLIC include)
target_link_libraries(stable_fluids_core PUBLIC glad glfw OpenGL::GL imgui)
target_compile_definitions(stable_fluids_core PUBLIC
    STABLE_FLUIDS_SHADER_DIR="${CMAKE_CURRENT_SOURCE_DIR}/shaders"
)
stable_fluids_set_warnings(stable_fluids_core)
stable_fluids_enable_sanitizers(stable_fluids_core)

# ---------------------------------------------------------------------------
# Application executable
# ---------------------------------------------------------------------------
add_executable(stable_fluids src/main.cpp)
target_link_libraries(stable_fluids PRIVATE stable_fluids_core)
stable_fluids_set_warnings(stable_fluids)
stable_fluids_enable_sanitizers(stable_fluids)

# Copy shaders next to the binary so the application can find them without
# relying on the source tree.
add_custom_command(TARGET stable_fluids POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/shaders"
            "$<TARGET_FILE_DIR:stable_fluids>/shaders"
)

# ---------------------------------------------------------------------------
# Tests
# ---------------------------------------------------------------------------
if(STABLE_FLUIDS_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
