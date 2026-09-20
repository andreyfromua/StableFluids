#include "stable_fluids/platform/Platform.hpp"

#include "stable_fluids/gl/GlHeaders.hpp"

#include <GLFW/glfw3.h>

#include <cstdio>

namespace stable_fluids::platform {

void apply_gl_window_hints() {
#if STABLE_FLUIDS_WEB
    glfwWindowHint(GLFW_CLIENT_API,        GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_SAMPLES, 0);  // antialias:false — cheaper mobile backbuffer
#else
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE,        GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#  ifndef NDEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT,  GLFW_TRUE);
#  endif
#endif
    glfwWindowHint(GLFW_RESIZABLE,     GLFW_TRUE);
    glfwWindowHint(GLFW_SRGB_CAPABLE,  GLFW_FALSE);
}

bool load_gl_functions() {
#if STABLE_FLUIDS_WEB
    return true;  // WebGL 2.0 / GLES3 symbols are provided by Emscripten.
#else
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::fprintf(stderr, "Failed to load OpenGL functions through GLAD.\n");
        return false;
    }
    return true;
#endif
}

void enable_required_extensions() {
#if STABLE_FLUIDS_WEB
    const EMSCRIPTEN_WEBGL_CONTEXT_HANDLE ctx =
        emscripten_webgl_get_current_context();
    emscripten_webgl_enable_extension(ctx, "EXT_color_buffer_float");
    emscripten_webgl_enable_extension(ctx, "OES_texture_float_linear");
#endif
}

}  // namespace stable_fluids::platform
