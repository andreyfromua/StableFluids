#pragma once

#include "stable_fluids/SimulationConfig.hpp"

struct GLFWwindow;

namespace stable_fluids {

struct UiActions {
    bool reset          = false;
    bool reload_shaders = false;
    bool single_step    = false;
    bool pause_changed  = false;
    bool paused         = false;
};

/// Dear ImGui control panel rendered over the simulation.
class UserInterface {
public:
    explicit UserInterface(GLFWwindow* window);
    ~UserInterface();

    UserInterface(const UserInterface&)            = delete;
    UserInterface& operator=(const UserInterface&) = delete;

    void begin_frame();
    UiActions draw_controls(SimulationConfig& config,
                            bool paused,
                            float frame_dt,
                            int framebuffer_width,
                            int framebuffer_height);

    /// Draw a centered, click-through "how to start" overlay. Shown until the
    /// user first interacts (emits), then suppressed by passing visible=false.
    void draw_hint(bool visible);

    void render();

    bool wants_capture_mouse() const;
    bool wants_capture_keyboard() const;

private:
    bool initialized_ = false;
};

}  // namespace stable_fluids
