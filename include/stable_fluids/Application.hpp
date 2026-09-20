#pragma once

#include "stable_fluids/FluidGrid.hpp"
#include "stable_fluids/FluidSolver.hpp"
#include "stable_fluids/InputController.hpp"
#include "stable_fluids/Renderer.hpp"
#include "stable_fluids/SimulationConfig.hpp"
#include "stable_fluids/UserInterface.hpp"
#include "stable_fluids/platform/GraphicsCapabilities.hpp"
#include "stable_fluids/platform/MainLoop.hpp"

#include <chrono>
#include <memory>
#include <string>

struct GLFWwindow;

namespace stable_fluids {

/// Owns the GLFW window, the OpenGL context, the solver, the renderer,
/// and the input controller; drives the main loop.
class Application {
public:
    explicit Application(SimulationConfig config);
    ~Application();

    Application(const Application&)            = delete;
    Application& operator=(const Application&) = delete;

    /// Run the main loop until the window is closed. Returns the exit code.
    int run();

    // Used by the InputController to request behavioural changes.
    void request_reset()         { request_reset_         = true; }
    void request_shader_reload() { request_shader_reload_ = true; }
    void set_view_mode(ViewMode m) { config_.view_mode = m; }

    // Window size in pixels.
    int  framebuffer_width()  const { return fb_width_;  }
    int  framebuffer_height() const { return fb_height_; }

    const SimulationConfig& config() const { return config_; }
    SimulationConfig&       config()       { return config_; }

private:
    bool initialize_glfw();
    bool initialize_gl();
    void create_subsystems();

    /// Advance exactly one frame. Returns true to keep running, false to stop.
    /// The MainLoop strategy calls this; it is the shared body across targets.
    bool frame();

    void on_framebuffer_size(int width, int height);

    SimulationConfig            config_;
    GraphicsCapabilities        caps_{};
    GLFWwindow*                 window_   = nullptr;
    int                         fb_width_ = 0;
    int                         fb_height_ = 0;

    std::unique_ptr<FluidGrid>       grid_;
    std::unique_ptr<FluidSolver>     solver_;
    std::unique_ptr<Renderer>        renderer_;
    std::unique_ptr<InputController> input_;
    std::unique_ptr<UserInterface>   ui_;
    std::unique_ptr<MainLoop>        loop_;

    bool request_reset_         = false;
    bool request_shader_reload_ = false;
    bool user_interacted_       = false;  // true after the first active emit

    // Per-frame timing state (persists across frame() calls under the browser
    // loop, where each frame is a separate callback invocation).
    std::chrono::steady_clock::time_point t_prev_{};
    int    frames_since_title_  = 0;
    double seconds_since_title_ = 0.0;
    bool   timing_started_      = false;
};

}  // namespace stable_fluids
