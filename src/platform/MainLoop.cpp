#include "stable_fluids/platform/MainLoop.hpp"

#include "stable_fluids/gl/GlHeaders.hpp"  // STABLE_FLUIDS_WEB (+ emscripten on web)

#include <utility>

namespace stable_fluids {

namespace {

/// Desktop: a classic blocking loop.
class BlockingMainLoop final : public MainLoop {
public:
    void run(FrameCallback frame) override {
        while (frame()) {
            // frame() polls input, steps, renders, and swaps.
        }
    }
};

#if STABLE_FLUIDS_WEB
/// Browser: hand the callback to the runtime and return. The instance must
/// outlive the loop (Application owns it and is kept alive by the web entry
/// point), because `simulate_infinite_loop = false` lets run() unwind.
class BrowserMainLoop final : public MainLoop {
public:
    void run(FrameCallback frame) override {
        frame_ = std::move(frame);
        // fps = 0 syncs to the display via requestAnimationFrame; the runtime
        // stays alive after this returns thanks to -sEXIT_RUNTIME=0.
        emscripten_set_main_loop_arg(&BrowserMainLoop::tick, this,
                                     /*fps=*/0, /*simulate_infinite_loop=*/false);
    }

private:
    static void tick(void* self) {
        auto* loop = static_cast<BrowserMainLoop*>(self);
        if (!loop->frame_ || !loop->frame_()) {
            emscripten_cancel_main_loop();
        }
    }

    FrameCallback frame_;
};
#endif  // STABLE_FLUIDS_WEB

}  // namespace

std::unique_ptr<MainLoop> MainLoop::create() {
#if STABLE_FLUIDS_WEB
    return std::make_unique<BrowserMainLoop>();
#else
    return std::make_unique<BlockingMainLoop>();
#endif
}

}  // namespace stable_fluids
