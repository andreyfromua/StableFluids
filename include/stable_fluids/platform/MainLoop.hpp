#pragma once

#include <functional>
#include <memory>

namespace stable_fluids {

/// Strategy that drives the per-frame callback, isolating the one structural
/// difference between the targets: desktop owns a blocking loop, while a browser
/// tab must return control to the event loop and be called back once per
/// animation frame (Emscripten's `requestAnimationFrame` bridge).
///
/// The callback returns true to continue and false to stop.
class MainLoop {
public:
    using FrameCallback = std::function<bool()>;

    virtual ~MainLoop() = default;

    /// Begin driving `frame`. On desktop this blocks until `frame` returns
    /// false; on the web it returns immediately and the browser keeps invoking
    /// `frame` until it returns false.
    virtual void run(FrameCallback frame) = 0;

    /// Factory Method: the loop strategy for the current build target.
    static std::unique_ptr<MainLoop> create();
};

}  // namespace stable_fluids
