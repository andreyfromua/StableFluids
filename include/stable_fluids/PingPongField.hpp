#pragma once

#include "stable_fluids/Texture2D.hpp"

namespace stable_fluids {

/// Two textures with identical specifications that act as read/write buffers.
class PingPongField {
public:
    PingPongField() = default;
    explicit PingPongField(const TextureSpec& spec) { allocate(spec); }

    void allocate(const TextureSpec& spec) {
        a_.allocate(spec);
        b_.allocate(spec);
        front_ = 0;
    }

    /// Source texture for the next pass (read from).
    const Texture2D& read() const { return front_ == 0 ? a_ : b_; }
    /// Destination texture for the next pass (write to).
    Texture2D&       write()      { return front_ == 0 ? b_ : a_; }
    const Texture2D& write() const { return front_ == 0 ? b_ : a_; }

    /// Promote the destination texture to be the new source.
    void swap() { front_ = 1 - front_; }

    /// Clear both textures to zero so simulation starts from rest.
    void clear() {
        a_.clear_to_zero();
        b_.clear_to_zero();
    }

    int width()  const { return a_.width();  }
    int height() const { return a_.height(); }

private:
    Texture2D a_;
    Texture2D b_;
    int       front_ = 0;
};

}  // namespace stable_fluids
