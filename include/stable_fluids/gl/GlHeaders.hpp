#pragma once

/// Single seam that decides which OpenGL headers the project sees.
///
/// The desktop build loads OpenGL 4.1 Core through GLAD; the browser build
/// targets WebGL 2.0 (OpenGL ES 3.0), whose symbols Emscripten provides
/// directly with no loader. Every translation unit that needs GL types or
/// enums includes this header instead of `<glad/glad.h>` so the `#ifdef`
/// lives in exactly one place.
///
/// `STABLE_FLUIDS_WEB` is 1 on the Emscripten/WebGL target and 0 on desktop;
/// prefer it over raw `__EMSCRIPTEN__` checks elsewhere in the codebase.

#if defined(__EMSCRIPTEN__)
#  define STABLE_FLUIDS_WEB 1
#  include <GLES3/gl3.h>
#  include <emscripten.h>
#  include <emscripten/html5.h>
#else
#  define STABLE_FLUIDS_WEB 0
#  include <glad/glad.h>
#endif
