#include "stable_fluids/Texture2D.hpp"

#include <cstring>
#include <utility>
#include <vector>

namespace stable_fluids {

namespace {

// Estimate the byte size of one pixel given a format/type pair.
// Conservative: returns 16 (RGBA32F) for unfamiliar combinations, which is
// only used for clearing and is always >= the real footprint.
std::size_t pixel_byte_size(GLenum format, GLenum type) {
    int channels = 4;
    switch (format) {
        case GL_RED:
        case GL_RED_INTEGER: channels = 1; break;
        case GL_RG:          channels = 2; break;
        case GL_RGB:         channels = 3; break;
        case GL_RGBA:        channels = 4; break;
        default:             channels = 4; break;
    }

    std::size_t per_channel = 4;
    switch (type) {
        case GL_BYTE:
        case GL_UNSIGNED_BYTE:  per_channel = 1; break;
        case GL_SHORT:
        case GL_UNSIGNED_SHORT:
        case GL_HALF_FLOAT:     per_channel = 2; break;
        case GL_INT:
        case GL_UNSIGNED_INT:
        case GL_FLOAT:          per_channel = 4; break;
        default:                per_channel = 4; break;
    }

    return static_cast<std::size_t>(channels) * per_channel;
}

}  // namespace

Texture2D::Texture2D(const TextureSpec& spec) {
    allocate(spec);
}

Texture2D::~Texture2D() {
    release();
}

Texture2D::Texture2D(Texture2D&& other) noexcept
    : id_(other.id_), spec_(other.spec_) {
    other.id_ = 0;
}

Texture2D& Texture2D::operator=(Texture2D&& other) noexcept {
    if (this != &other) {
        release();
        id_       = other.id_;
        spec_     = other.spec_;
        other.id_ = 0;
    }
    return *this;
}

void Texture2D::release() noexcept {
    if (id_ != 0) {
        glDeleteTextures(1, &id_);
        id_ = 0;
    }
}

void Texture2D::allocate(const TextureSpec& spec) {
    release();
    spec_ = spec;

    glGenTextures(1, &id_);
    glBindTexture(GL_TEXTURE_2D, id_);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, spec_.internal_format,
                 spec_.width, spec_.height, 0,
                 spec_.format, spec_.type, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, spec_.min_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, spec_.mag_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     spec_.wrap_s);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     spec_.wrap_t);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::clear_to_zero() const {
    if (!is_valid()) {
        return;
    }
    const std::size_t pixels = static_cast<std::size_t>(spec_.width) *
                               static_cast<std::size_t>(spec_.height);
    const std::size_t bytes  = pixels * pixel_byte_size(spec_.format, spec_.type);
    std::vector<unsigned char> zeros(bytes, 0);

    glBindTexture(GL_TEXTURE_2D, id_);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
                    spec_.width, spec_.height,
                    spec_.format, spec_.type, zeros.data());
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::bind(int unit) const {
    glActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + unit));
    glBindTexture(GL_TEXTURE_2D, id_);
}

}  // namespace stable_fluids
