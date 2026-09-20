#include "stable_fluids/FluidGrid.hpp"

#include <vector>

namespace stable_fluids {

namespace {

TextureSpec velocity_spec(int width, int height,
                          GLint internal_format, GLenum type) {
    TextureSpec spec;
    spec.width            = width;
    spec.height           = height;
    spec.internal_format  = internal_format;
    spec.format           = GL_RED;
    spec.type             = type;
    spec.min_filter       = GL_LINEAR;
    spec.mag_filter       = GL_LINEAR;
    return spec;
}

TextureSpec pressure_spec(int width, int height) {
    TextureSpec spec;
    spec.width            = width;
    spec.height           = height;
    spec.internal_format  = GL_R32F;
    spec.format           = GL_RED;
    spec.type             = GL_FLOAT;
    spec.min_filter       = GL_NEAREST;
    spec.mag_filter       = GL_NEAREST;
    return spec;
}

TextureSpec divergence_spec(int width, int height) {
    return pressure_spec(width, height);
}

TextureSpec scalar_spec(int width, int height, bool linear) {
    TextureSpec spec;
    spec.width            = width;
    spec.height           = height;
    spec.internal_format  = GL_R16F;
    spec.format           = GL_RED;
    spec.type             = GL_HALF_FLOAT;
    spec.min_filter       = linear ? GL_LINEAR : GL_NEAREST;
    spec.mag_filter       = linear ? GL_LINEAR : GL_NEAREST;
    return spec;
}

TextureSpec dye_spec(int width, int height) {
    TextureSpec spec;
    spec.width            = width;
    spec.height           = height;
    spec.internal_format  = GL_RGBA16F;
    spec.format           = GL_RGBA;
    spec.type             = GL_HALF_FLOAT;
    spec.min_filter       = GL_LINEAR;
    spec.mag_filter       = GL_LINEAR;
    return spec;
}

TextureSpec tensor_spec(int width, int height) {
    TextureSpec spec;
    spec.width            = width;
    spec.height           = height;
    spec.internal_format  = GL_RGBA32F;
    spec.format           = GL_RGBA;
    spec.type             = GL_FLOAT;
    spec.min_filter       = GL_NEAREST;
    spec.mag_filter       = GL_NEAREST;
    return spec;
}

TextureSpec mask_spec(int width, int height) {
    // Use a normalized R8 texture so it can be sampled as a float in shaders
    // and so it works as a color attachment on all GL 4.1 implementations.
    TextureSpec spec;
    spec.width            = width;
    spec.height           = height;
    spec.internal_format  = GL_R8;
    spec.format           = GL_RED;
    spec.type             = GL_UNSIGNED_BYTE;
    spec.min_filter       = GL_NEAREST;
    spec.mag_filter       = GL_NEAREST;
    return spec;
}

}  // namespace

FluidGrid::FluidGrid(int nx, int ny,
                     GLint velocity_internal_format, GLenum velocity_type)
    : nx_(nx)
    , ny_(ny)
    , u_(velocity_spec(nx + 1, ny, velocity_internal_format, velocity_type))
    , v_(velocity_spec(nx, ny + 1, velocity_internal_format, velocity_type))
    , pressure_(pressure_spec(nx, ny))
    , divergence_(divergence_spec(nx, ny))
    , smoke_(scalar_spec(nx, ny, true))
    , dye_(dye_spec(nx, ny))
    , temperature_(scalar_spec(nx, ny, true))
    , solid_mask_(mask_spec(nx, ny))
    , vorticity_(scalar_spec(nx, ny, false))
    , velocity_gradient_(tensor_spec(nx, ny))
    , stress_(tensor_spec(nx, ny))
{
    reset();
}

void FluidGrid::reset() {
    u_.clear();
    v_.clear();
    pressure_.clear();
    divergence_.clear_to_zero();
    smoke_.clear();
    dye_.clear();
    temperature_.clear();
    vorticity_.clear_to_zero();
    velocity_gradient_.clear_to_zero();
    stress_.clear_to_zero();
    initialize_solid_mask();
}

void FluidGrid::initialize_solid_mask() {
    // For the baseline solver, the domain is closed but otherwise unobstructed,
    // so the interior of the mask is 0 (fluid). Walls are handled in shaders
    // by domain-edge checks; the mask is reserved for explicit obstacles that
    // future scenes may add.
    const std::size_t count =
        static_cast<std::size_t>(nx_) * static_cast<std::size_t>(ny_);
    std::vector<unsigned char> zeros(count, 0);

    glBindTexture(GL_TEXTURE_2D, solid_mask_.id());
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
                    nx_, ny_, GL_RED, GL_UNSIGNED_BYTE, zeros.data());
    glBindTexture(GL_TEXTURE_2D, 0);
}

}  // namespace stable_fluids
