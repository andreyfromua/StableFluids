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
    Stress     = 8,
    Count
};

/// Scalar quantity extracted from the per-cell 2x2 Cauchy stress tensor.
enum class StressDisplayMode : int {
    VonMises    = 0,
    SigmaXX     = 1,
    SigmaYY     = 2,
    SigmaXY     = 3,
    Hydrostatic = 4,
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
    float fluid_density              = 1.0f;  // used to reconstruct stress

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
    StressDisplayMode stress_display_mode = StressDisplayMode::VonMises;
    float stress_display_scale       = 0.02f;

    // MAC-grid debugging overlay.
    bool  velocity_overlay_enabled   = false;
    bool  velocity_overlay_grid      = true;
    int   velocity_overlay_stride    = 16;
    float velocity_overlay_scale     = 0.25f;
};

}  // namespace stable_fluids
