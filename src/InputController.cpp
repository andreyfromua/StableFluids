#include "stable_fluids/InputController.hpp"
#include "stable_fluids/Application.hpp"

#include <GLFW/glfw3.h>

#include <algorithm>
#include <cmath>
#include <cstdio>

namespace stable_fluids {

InputController::InputController(GLFWwindow* window,
                                 SimulationConfig& config,
                                 Application& app)
    : window_(window)
    , config_(config)
    , app_(app)
{
    double x, y;
    glfwGetCursorPos(window_, &x, &y);
    prev_cursor_x_ = cur_cursor_x_ = x;
    prev_cursor_y_ = cur_cursor_y_ = y;
}

void InputController::cursor_to_cells(double cursor_x, double cursor_y,
                                      float& cell_x, float& cell_y) const
{
    const int framebuffer_w = app_.framebuffer_width();
    const int framebuffer_h = app_.framebuffer_height();

    int window_w = 0;
    int window_h = 0;
    glfwGetWindowSize(window_, &window_w, &window_h);

    if (window_w <= 0 || window_h <= 0 ||
        framebuffer_w <= 0 || framebuffer_h <= 0) {
        cell_x = cell_y = -1.0f;
        return;
    }

    // GLFW reports cursor positions in logical window coordinates, while
    // OpenGL viewports use framebuffer pixels. They differ on high-DPI
    // displays, so convert before comparing against the rendered viewport.
    const double framebuffer_x =
        cursor_x * double(framebuffer_w) / double(window_w);
    const double framebuffer_y =
        cursor_y * double(framebuffer_h) / double(window_h);

    // Match the renderer's aspect-preserved viewport.
    const float grid_aspect   = float(config_.grid_width)  / float(config_.grid_height);
    const float window_aspect = float(framebuffer_w) / float(framebuffer_h);

    int vp_w = framebuffer_w;
    int vp_h = framebuffer_h;
    if (window_aspect > grid_aspect) {
        vp_w = int(float(framebuffer_h) * grid_aspect);
    } else {
        vp_h = int(float(framebuffer_w) / grid_aspect);
    }
    const int vp_x = (framebuffer_w - vp_w) / 2;
    const int vp_y = (framebuffer_h - vp_h) / 2;

    // GLFW cursor origin is top-left; OpenGL viewport is bottom-left.
    const double px = framebuffer_x - vp_x;
    const double py = (double(framebuffer_h) - framebuffer_y) - vp_y;

    if (px < 0 || py < 0 || px >= vp_w || py >= vp_h) {
        cell_x = cell_y = -1.0f;
        return;
    }

    cell_x = float(px / vp_w * config_.grid_width);
    cell_y = float(py / vp_h * config_.grid_height);
}

void InputController::update(float dt) {
    // Compute drag velocity in cells/second based on cursor movement.
    if (!ui_capture_mouse_ && cursor_inside_ && !first_motion_ && dt > 1e-6f) {
        float cx0, cy0, cx1, cy1;
        cursor_to_cells(prev_cursor_x_, prev_cursor_y_, cx0, cy0);
        cursor_to_cells(cur_cursor_x_,  cur_cursor_y_,  cx1, cy1);
        if (cx0 >= 0 && cx1 >= 0) {
            drag_velocity_x_ = (cx1 - cx0) / dt;
            drag_velocity_y_ = (cy1 - cy0) / dt;
        } else {
            drag_velocity_x_ = drag_velocity_y_ = 0.0f;
        }
    } else {
        drag_velocity_x_ = drag_velocity_y_ = 0.0f;
    }
    prev_cursor_x_ = cur_cursor_x_;
    prev_cursor_y_ = cur_cursor_y_;
    first_motion_  = false;
}

EmitterPulse InputController::make_emitter() const {
    EmitterPulse e;
    if (ui_capture_mouse_ ||
        !cursor_inside_ ||
        (!left_down_ && !right_down_)) {
        return e;
    }

    float cx, cy;
    cursor_to_cells(cur_cursor_x_, cur_cursor_y_, cx, cy);
    if (cx < 0) return e;

    e.active   = true;
    e.center_x = cx;
    e.center_y = cy;
    e.radius   = config_.emitter_radius_cells;
    e.vel_x    = drag_velocity_x_;
    e.vel_y    = drag_velocity_y_;

    if (left_down_) {
        e.smoke = config_.smoke_injection_rate;
        // Cycle a colour from the cursor position so the dye isn't monotone.
        float angle = std::atan2(cy - 0.5f * float(config_.grid_height),
                                 cx - 0.5f * float(config_.grid_width));
        e.dye_r = 0.5f + 0.5f * std::cos(angle);
        e.dye_g = 0.5f + 0.5f * std::cos(angle + 2.094f);
        e.dye_b = 0.5f + 0.5f * std::cos(angle + 4.189f);
        e.dye_r *= config_.dye_injection_rate;
        e.dye_g *= config_.dye_injection_rate;
        e.dye_b *= config_.dye_injection_rate;
    }
    if (right_down_) {
        e.temperature = config_.temperature_injection_rate;
    }
    return e;
}

void InputController::on_key(int key, int action, int /*mods*/) {
    if (action != GLFW_PRESS) return;
    if (ui_capture_keyboard_) return;

    switch (key) {
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window_, GLFW_TRUE);
            break;
        case GLFW_KEY_SPACE:
            paused_ = !paused_;
            break;
        case GLFW_KEY_PERIOD:
            if (paused_) step_one_ = true;
            break;
        case GLFW_KEY_R:
            app_.request_reset();
            break;
        case GLFW_KEY_B:
            config_.buoyancy_enabled = !config_.buoyancy_enabled;
            break;
        case GLFW_KEY_V:
            config_.vorticity_confinement = !config_.vorticity_confinement;
            break;
        case GLFW_KEY_D:
            config_.diffusion_enabled = !config_.diffusion_enabled;
            break;
        case GLFW_KEY_LEFT_BRACKET:
            config_.pressure_iterations = std::max(1, config_.pressure_iterations - 4);
            break;
        case GLFW_KEY_RIGHT_BRACKET:
            config_.pressure_iterations = std::min(200, config_.pressure_iterations + 4);
            break;
        case GLFW_KEY_MINUS:
            config_.emitter_radius_cells = std::max(1.0f, config_.emitter_radius_cells - 1.0f);
            break;
        case GLFW_KEY_EQUAL:
            config_.emitter_radius_cells = std::min(64.0f, config_.emitter_radius_cells + 1.0f);
            break;
        case GLFW_KEY_F5:
            app_.request_shader_reload();
            break;
        case GLFW_KEY_1: app_.set_view_mode(ViewMode::Smoke);       break;
        case GLFW_KEY_2: app_.set_view_mode(ViewMode::Dye);         break;
        case GLFW_KEY_3: app_.set_view_mode(ViewMode::Velocity);    break;
        case GLFW_KEY_4: app_.set_view_mode(ViewMode::Pressure);    break;
        case GLFW_KEY_5: app_.set_view_mode(ViewMode::Divergence);  break;
        case GLFW_KEY_6: app_.set_view_mode(ViewMode::Temperature); break;
        case GLFW_KEY_7: app_.set_view_mode(ViewMode::Vorticity);   break;
        case GLFW_KEY_8: app_.set_view_mode(ViewMode::SolidMask);   break;
        case GLFW_KEY_9: app_.set_view_mode(ViewMode::Stress);      break;
        default: break;
    }
}

void InputController::on_mouse_button(int button, int action, int /*mods*/) {
    if (action == GLFW_RELEASE) {
        if (button == GLFW_MOUSE_BUTTON_LEFT)  left_down_  = false;
        if (button == GLFW_MOUSE_BUTTON_RIGHT) right_down_ = false;
        return;
    }

    if (ui_capture_mouse_ || action != GLFW_PRESS) {
        return;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT)  left_down_  = true;
    if (button == GLFW_MOUSE_BUTTON_RIGHT) right_down_ = true;
}

void InputController::on_cursor(double x, double y) {
    cur_cursor_x_ = x;
    cur_cursor_y_ = y;
    cursor_inside_ = true;
}

void InputController::set_ui_capture(bool mouse, bool keyboard) {
    ui_capture_mouse_ = mouse;
    ui_capture_keyboard_ = keyboard;
    if (ui_capture_mouse_) {
        drag_velocity_x_ = 0.0f;
        drag_velocity_y_ = 0.0f;
    }
}

}  // namespace stable_fluids
