#pragma once

#include "stable_fluids/FluidGrid.hpp"
#include "stable_fluids/FluidSolver.hpp"
#include "stable_fluids/InputController.hpp"
#include "stable_fluids/Renderer.hpp"
#include "stable_fluids/SimulationConfig.hpp"
#include "stable_fluids/UserInterface.hpp"

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
    void main_loop();

    void on_framebuffer_size(int width, int height);

    SimulationConfig            config_;
    GLFWwindow*                 window_   = nullptr;
    int                         fb_width_ = 0;
    int                         fb_height_ = 0;

    std::unique_ptr<FluidGrid>       grid_;
    std::unique_ptr<FluidSolver>     solver_;
    std::unique_ptr<Renderer>        renderer_;
    std::unique_ptr<InputController> input_;
    std::unique_ptr<UserInterface>   ui_;

    bool request_reset_         = false;
    bool request_shader_reload_ = false;
};

}  // namespace stable_fluids
