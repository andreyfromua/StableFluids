#pragma once

namespace stable_fluids::platform {

/// Free functions that concentrate the desktop-vs-web platform `#ifdef`s for
/// graphics setup, so `Application` reads the same on both targets. GRASP Pure
/// Fabrication: a cohesive home for behaviour that belongs to no domain object.

/// Apply the GLFW window hints that select the graphics API and context version
/// for the current build target. Call before `glfwCreateWindow`.
void apply_gl_window_hints();

/// Resolve GL function pointers. Desktop loads OpenGL 4.1 Core through GLAD; the
/// web target is a no-op because Emscripten links the WebGL/GLES3 symbols
/// directly. Returns false on failure.
bool load_gl_functions();

/// Enable the GL extensions the solver requires on the current target. On the
/// web this turns on `EXT_color_buffer_float` (float render targets) and
/// `OES_texture_float_linear` (linear-filtered 32-bit float velocity); on
/// desktop it is a no-op because 4.1 Core supports both unconditionally.
void enable_required_extensions();

}  // namespace stable_fluids::platform
