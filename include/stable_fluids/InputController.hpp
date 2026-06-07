#pragma once

#include "stable_fluids/FluidSolver.hpp"
#include "stable_fluids/SimulationConfig.hpp"

struct GLFWwindow;

namespace stable_fluids {

class Application;

/// Translates raw GLFW input into solver-friendly state.
class InputController {
public:
    InputController(GLFWwindow* window,
                    SimulationConfig& config,
                    Application& app);

    /// Polled once per frame after glfwPollEvents().
    void update(float dt);

    /// Build a per-substep emitter from the polled mouse state.
    EmitterPulse make_emitter() const;

    // ----- Key edge events ---------------------------------------------------
    void on_key(int key, int action, int mods);
    void on_mouse_button(int button, int action, int mods);
    void on_cursor(double x, double y);

    bool paused()   const { return paused_; }
    bool step_one() const { return step_one_; }
    void consume_step_one() { step_one_ = false; }

private:
    GLFWwindow*       window_;
    SimulationConfig& config_;
    Application&      app_;

    // Mouse state in *cell* coordinates, computed from the cursor position.
    double prev_cursor_x_ = 0.0;
    double prev_cursor_y_ = 0.0;
    double cur_cursor_x_  = 0.0;
    double cur_cursor_y_  = 0.0;
    bool   cursor_inside_ = false;
    bool   first_motion_  = true;

    bool   left_down_  = false;
    bool   right_down_ = false;

    // Velocity averaged over the most recent input frame, in cells/second.
    float  drag_velocity_x_ = 0.0f;
    float  drag_velocity_y_ = 0.0f;

    // Toggle states
    bool   paused_   = false;
    bool   step_one_ = false;

    void cursor_to_cells(double cursor_x, double cursor_y,
                         float& cell_x, float& cell_y) const;
};

}  // namespace stable_fluids
