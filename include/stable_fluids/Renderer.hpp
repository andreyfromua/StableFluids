#pragma once

#include "stable_fluids/FluidGrid.hpp"
#include "stable_fluids/ShaderProgram.hpp"
#include "stable_fluids/SimulationConfig.hpp"

#include <string>

namespace stable_fluids {

/// Draws the selected simulation field into the default framebuffer.
class Renderer {
public:
    Renderer(const std::string& shader_dir);
    ~Renderer();

    Renderer(const Renderer&)            = delete;
    Renderer& operator=(const Renderer&) = delete;

    bool reload_shaders();

    /// Draw the selected view into the back buffer (window size in pixels).
    void render(const FluidGrid& grid,
                ViewMode mode,
                int window_w, int window_h,
                const SimulationConfig& config);

private:
    void render_smoke(const FluidGrid& grid, const SimulationConfig& cfg);
    void render_dye  (const FluidGrid& grid, const SimulationConfig& cfg);
    void render_velocity(const FluidGrid& grid);
    void render_stress(const FluidGrid& grid, const SimulationConfig& cfg);
    void render_scalar (const Texture2D& tex, float scale, float offset, bool signed_map);

    std::string shader_dir_;
    GLuint empty_vao_ = 0;

    ShaderProgram smoke_prog_;
    ShaderProgram dye_prog_;
    ShaderProgram scalar_prog_;
    ShaderProgram velocity_prog_;
    ShaderProgram stress_prog_;
};

}  // namespace stable_fluids
