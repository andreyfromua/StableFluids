#pragma once

#include "stable_fluids/PingPongField.hpp"
#include "stable_fluids/Texture2D.hpp"

namespace stable_fluids {

/// Owns all the textures that form the 2D MAC grid.
///
/// The grid has Nx*Ny cells.  Horizontal velocity `u` lives on vertical
/// faces, so it is sized (Nx+1) x Ny.  Vertical velocity `v` lives on
/// horizontal faces, so it is sized Nx x (Ny+1).  Pressure, divergence,
/// smoke, dye, temperature, mask, and vorticity share cell-centered
/// dimensions Nx x Ny.
class FluidGrid {
public:
    FluidGrid(int nx, int ny);

    int nx() const noexcept { return nx_; }
    int ny() const noexcept { return ny_; }

    // Velocity components (ping-pong).
    PingPongField& u()       { return u_; }
    PingPongField& v()       { return v_; }
    const PingPongField& u() const { return u_; }
    const PingPongField& v() const { return v_; }

    // Pressure (ping-pong for Jacobi iterations).
    PingPongField& pressure()       { return pressure_; }
    const PingPongField& pressure() const { return pressure_; }

    // Single-buffer fields.
    Texture2D& divergence()    { return divergence_; }
    Texture2D& solid_mask()    { return solid_mask_; }
    Texture2D& vorticity()     { return vorticity_; }
    const Texture2D& divergence()  const { return divergence_; }
    const Texture2D& solid_mask()  const { return solid_mask_; }
    const Texture2D& vorticity()   const { return vorticity_; }

    // Scalars (ping-pong).
    PingPongField& smoke()       { return smoke_; }
    PingPongField& dye()         { return dye_; }
    PingPongField& temperature() { return temperature_; }
    const PingPongField& smoke()       const { return smoke_; }
    const PingPongField& dye()         const { return dye_; }
    const PingPongField& temperature() const { return temperature_; }

    /// Reset all fields to zero and re-initialize the solid mask.
    void reset();

private:
    void initialize_solid_mask();

    int nx_;
    int ny_;

    PingPongField u_;
    PingPongField v_;

    PingPongField pressure_;
    Texture2D     divergence_;

    PingPongField smoke_;
    PingPongField dye_;
    PingPongField temperature_;

    Texture2D     solid_mask_;
    Texture2D     vorticity_;
};

}  // namespace stable_fluids
