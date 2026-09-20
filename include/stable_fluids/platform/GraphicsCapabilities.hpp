#pragma once

#include "stable_fluids/gl/GlHeaders.hpp"

namespace stable_fluids {

/// Queries the current GL/WebGL context for the float-texture capabilities the
/// solver depends on, and derives the storage-format policy that follows.
///
/// GRASP Information Expert: the one place that knows which features are
/// available and therefore how the velocity fields must be allocated. Desktop
/// OpenGL 4.1 Core renders to and linearly filters 32-bit float textures
/// unconditionally; WebGL 2.0 gates both behind extensions, so the browser
/// build must probe and, if necessary, fall back to half-float velocity.
class GraphicsCapabilities {
public:
    /// Probe the current context. Requires a current GL context.
    static GraphicsCapabilities detect();

    bool color_buffer_float() const noexcept { return color_buffer_float_; }
    bool float_linear()       const noexcept { return float_linear_; }

    /// Internal format for the velocity (u, v) fields, which are linearly
    /// sampled during advection and buoyancy. R32F when full-float linear
    /// filtering is available, otherwise R16F (half-float linear filtering is
    /// core in WebGL 2.0 and always safe).
    GLint velocity_internal_format() const noexcept {
        return float_linear_ ? GL_R32F : GL_R16F;
    }
    GLenum velocity_type() const noexcept {
        return float_linear_ ? GL_FLOAT : GL_HALF_FLOAT;
    }

private:
    bool color_buffer_float_ = true;
    bool float_linear_       = true;
};

}  // namespace stable_fluids
