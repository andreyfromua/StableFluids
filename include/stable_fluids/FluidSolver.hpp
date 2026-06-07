#pragma once

#include "stable_fluids/FluidGrid.hpp"
#include "stable_fluids/Framebuffer.hpp"
#include "stable_fluids/ShaderProgram.hpp"
#include "stable_fluids/SimulationConfig.hpp"

#include <string>

namespace stable_fluids {

/// Description of a continuous mouse injection event in cell-grid units.
struct EmitterPulse {
    bool   active        = false;  // any of the emit_* / momentum components nonzero
    bool   use_rgba_dye  = false;  // if true, emits dye RGBA instead of smoke
    float  center_x      = 0.0f;   // cells
    float  center_y      = 0.0f;
    float  radius        = 6.0f;
    float  smoke         = 0.0f;
    float  temperature   = 0.0f;
    float  dye_r         = 0.0f;
    float  dye_g         = 0.0f;
    float  dye_b         = 0.0f;
    float  vel_x         = 0.0f;   // cells per second
    float  vel_y         = 0.0f;
};

/// The GPU-side solver. Holds shader programs, a reusable framebuffer, and
/// orchestrates the per-substep pass sequence described in the design plan.
class FluidSolver {
public:
    FluidSolver(FluidGrid& grid,
                const SimulationConfig& config,
                const std::string& shader_dir);

    /// Reload all shader programs. Returns true if every program compiled.
    bool reload_shaders();

    /// Run one simulation substep with the given timestep `dt`.
    void step(float dt, const EmitterPulse& emitter);

    /// Recompute the cell-centered vorticity texture (for diagnostics).
    void compute_vorticity();

    /// Recompute the velocity gradient diagnostic.
    void compute_velocity_gradient(Texture2D& target);

    /// Compute divergence into grid_.divergence() (for diagnostic display).
    void compute_divergence_into_grid();

    /// Tell the solver about a parameter change (re-read from config).
    void refresh_config() {}

    const SimulationConfig& config() const noexcept { return config_; }
    SimulationConfig&       config()       noexcept { return config_; }

private:
    // --- Pass helpers ------------------------------------------------------
    void draw_pass(const Texture2D& target) const;

    void inject_scalar_field(PingPongField& field,
                             const EmitterPulse& emitter,
                             float amount_per_unit,
                             bool  is_rgba,
                             float r, float g, float b, float a,
                             float dt);

    void inject_velocity(const EmitterPulse& emitter, float dt);
    void apply_buoyancy(float dt);

    void apply_boundary_u();
    void apply_boundary_v();

    void diffuse_component(PingPongField& field, float dt);
    void diffuse_velocity(float dt);

    void compute_divergence();
    void solve_pressure();
    void subtract_pressure_gradient();
    void project();

    void advect_velocity(float dt);
    void advect_scalars(float dt);

    void vorticity_confinement(float dt);

    // --- Data --------------------------------------------------------------
    FluidGrid&        grid_;
    SimulationConfig  config_;
    std::string       shader_dir_;
    Framebuffer       fbo_;
    GLuint            empty_vao_ = 0;

    ShaderProgram inject_scalar_;
    ShaderProgram inject_u_;
    ShaderProgram inject_v_;
    ShaderProgram add_buoyancy_;
    ShaderProgram diffuse_;
    ShaderProgram divergence_;
    ShaderProgram pressure_jacobi_;
    ShaderProgram project_u_;
    ShaderProgram project_v_;
    ShaderProgram advect_u_;
    ShaderProgram advect_v_;
    ShaderProgram advect_scalar_;
    ShaderProgram apply_boundary_u_;
    ShaderProgram apply_boundary_v_;
    ShaderProgram vorticity_;
    ShaderProgram vorticity_force_u_;
    ShaderProgram vorticity_force_v_;
    ShaderProgram velocity_gradient_;
};

}  // namespace stable_fluids
