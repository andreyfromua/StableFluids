#pragma once

#include <glad/glad.h>

namespace stable_fluids {

class Texture2D;

/// A single-color-attachment framebuffer object that is reattached on each pass.
class Framebuffer {
public:
    Framebuffer();
    ~Framebuffer();

    Framebuffer(const Framebuffer&)            = delete;
    Framebuffer& operator=(const Framebuffer&) = delete;
    Framebuffer(Framebuffer&& other) noexcept;
    Framebuffer& operator=(Framebuffer&& other) noexcept;

    /// Attach a texture as the sole color attachment. Validates completeness.
    void attach(const Texture2D& texture);

    /// Bind the framebuffer for drawing and configure the viewport to the
    /// supplied dimensions in pixels.
    void bind_for_draw(int width, int height) const;

    /// Bind the default framebuffer (the on-screen window).
    static void bind_default(int width, int height);

    GLuint id() const noexcept { return id_; }

private:
    void release() noexcept;

    GLuint id_ = 0;
};

}  // namespace stable_fluids
