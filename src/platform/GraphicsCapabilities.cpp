#include "stable_fluids/platform/GraphicsCapabilities.hpp"

namespace stable_fluids {

namespace {

#if STABLE_FLUIDS_WEB
// Scan the core-profile / GLES3 extension list for `name`.
bool has_extension(const char* name) {
    GLint count = 0;
    glGetIntegerv(GL_NUM_EXTENSIONS, &count);
    for (GLint i = 0; i < count; ++i) {
        const GLubyte* ext = glGetStringi(GL_EXTENSIONS, static_cast<GLuint>(i));
        if (ext == nullptr) continue;
        const char* s = reinterpret_cast<const char*>(ext);
        for (const char *a = s, *b = name; ; ++a, ++b) {
            if (*a != *b) break;
            if (*a == '\0') return true;
        }
    }
    return false;
}
#endif

}  // namespace

GraphicsCapabilities GraphicsCapabilities::detect() {
    GraphicsCapabilities caps;  // desktop keeps the default-true members
#if STABLE_FLUIDS_WEB
    caps.color_buffer_float_ = has_extension("EXT_color_buffer_float") ||
                               has_extension("WEBGL_color_buffer_float");
    caps.float_linear_       = has_extension("OES_texture_float_linear");
#endif
    return caps;
}

}  // namespace stable_fluids
