#include "stable_fluids/Framebuffer.hpp"

#include "stable_fluids/Texture2D.hpp"

#include <cstdio>
#include <utility>

namespace stable_fluids {

Framebuffer::Framebuffer() {
    glGenFramebuffers(1, &id_);
}

Framebuffer::~Framebuffer() {
    release();
}

Framebuffer::Framebuffer(Framebuffer&& other) noexcept
    : id_(other.id_) {
    other.id_ = 0;
}

Framebuffer& Framebuffer::operator=(Framebuffer&& other) noexcept {
    if (this != &other) {
        release();
        id_       = other.id_;
        other.id_ = 0;
    }
    return *this;
}

void Framebuffer::release() noexcept {
    if (id_ != 0) {
        glDeleteFramebuffers(1, &id_);
        id_ = 0;
    }
}

void Framebuffer::attach(const Texture2D& texture) {
    glBindFramebuffer(GL_FRAMEBUFFER, id_);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, texture.id(), 0);

    GLenum draw_buffers[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, draw_buffers);

    const GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        std::fprintf(stderr,
                     "Framebuffer incomplete (status=0x%X) "
                     "when attaching texture %u (%dx%d)\n",
                     status, texture.id(),
                     texture.width(), texture.height());
    }
}

void Framebuffer::bind_for_draw(int width, int height) const {
    glBindFramebuffer(GL_FRAMEBUFFER, id_);
    glViewport(0, 0, width, height);
}

void Framebuffer::bind_default(int width, int height) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, width, height);
}

}  // namespace stable_fluids
