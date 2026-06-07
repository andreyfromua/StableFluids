#pragma once

#include <glad/glad.h>

namespace stable_fluids {

/// Specification used to allocate a Texture2D.
struct TextureSpec {
    int    width            = 0;
    int    height           = 0;
    GLint  internal_format  = GL_R32F;
    GLenum format           = GL_RED;
    GLenum type             = GL_FLOAT;
    GLint  min_filter       = GL_LINEAR;
    GLint  mag_filter       = GL_LINEAR;
    GLint  wrap_s           = GL_CLAMP_TO_EDGE;
    GLint  wrap_t           = GL_CLAMP_TO_EDGE;
};

/// A non-mipmapped 2D texture allocated through glTexImage2D.
///
/// The class is move-only and assumes a current OpenGL context.
class Texture2D {
public:
    Texture2D() = default;
    explicit Texture2D(const TextureSpec& spec);
    ~Texture2D();

    Texture2D(const Texture2D&)            = delete;
    Texture2D& operator=(const Texture2D&) = delete;
    Texture2D(Texture2D&& other) noexcept;
    Texture2D& operator=(Texture2D&& other) noexcept;

    /// Re-allocates the texture with the given specification.
    void allocate(const TextureSpec& spec);

    /// Clear the texture by uploading zeroed pixel data.
    void clear_to_zero() const;

    /// Bind to the given texture unit (GL_TEXTURE0 + unit).
    void bind(int unit) const;

    GLuint           id()              const noexcept { return id_; }
    int              width()           const noexcept { return spec_.width; }
    int              height()          const noexcept { return spec_.height; }
    const TextureSpec& spec()          const noexcept { return spec_; }
    bool             is_valid()        const noexcept { return id_ != 0; }

private:
    void release() noexcept;

    GLuint      id_   = 0;
    TextureSpec spec_{};
};

}  // namespace stable_fluids
