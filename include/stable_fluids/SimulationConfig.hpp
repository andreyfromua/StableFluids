#pragma once

#include <cstdint>

namespace stable_fluids {

/// Visualization modes selectable through the 1..9 keys.
enum class ViewMode : int {
    Smoke      = 0,
    Dye        = 1,
    Velocity   = 2,
    Pressure   = 3,
    Divergence = 4,
    Temperature = 5,
    Vorticity  = 6,
    SolidMask  = 7,
    Count
};

/// Aggregated configuration that the C++ layer hands to the solver and renderer.
struct SimulationConfig {
    // Grid dimensions in cells. Power-of-two values are recommended.
    int grid_width  = 256;
    int grid_height = 256;

    // Initial window dimensions in screen-space pixels.
    int window_width  = 768;
    int window_height = 768;

    // Physical parameters.
    float viscosity                  = 0.0f;  // disabled by default
    float buoyancy_alpha             = 0.0f;  // smoke gravitational pull
    float buoyancy_beta              = 0.8f;  // temperature buoyant rise
    float ambient_temperature        = 0.0f;
    float smoke_dissipation_rate     = 0.20f; // per second
    float temperature_dissipation    = 0.40f; // per second toward ambient

    // Solver knobs.
    int   pressure_iterations        = 40;
    int   diffusion_iterations       = 20;
    int   substeps_per_frame         = 1;
    float fixed_timestep             = 1.0f / 60.0f;
    float target_courant             = 1.0f;
    bool  adaptive_substeps          = false;

    // Feature toggles.
    bool  buoyancy_enabled           = true;
    bool  diffusion_enabled          = false;
    bool  vorticity_confinement      = false;
    float vorticity_strength         = 0.3f;

    // Source emitter behaviour.
    float emitter_radius_cells       = 6.0f;
    float smoke_injection_rate       = 4.0f;
    float dye_injection_rate         = 4.0f;
    float temperature_injection_rate = 3.0f;
    float momentum_injection_scale   = 1.5f;

    // Initial view.
    ViewMode view_mode               = ViewMode::Smoke;
};

}  // namespace stable_fluids
