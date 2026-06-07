#include "stable_fluids/Renderer.hpp"

#include <cstdio>

namespace stable_fluids {

Renderer::Renderer(const std::string& shader_dir)
    : shader_dir_(shader_dir)
{
    glGenVertexArrays(1, &empty_vao_);
    if (!reload_shaders()) {
        std::fprintf(stderr, "Renderer: failed to load presentation shaders.\n");
    }
}

Renderer::~Renderer() {
    if (empty_vao_) {
        glDeleteVertexArrays(1, &empty_vao_);
    }
}

bool Renderer::reload_shaders() {
    bool ok = true;
    ok &= smoke_prog_   .load(shader_dir_, "fullscreen.vert", "render_smoke.frag");
    ok &= dye_prog_     .load(shader_dir_, "fullscreen.vert", "render_dye.frag");
    ok &= scalar_prog_  .load(shader_dir_, "fullscreen.vert", "render_scalar.frag");
    ok &= velocity_prog_.load(shader_dir_, "fullscreen.vert", "render_velocity.frag");
    return ok;
}

void Renderer::render(const FluidGrid& grid,
                      ViewMode mode,
                      int window_w, int window_h,
                      const SimulationConfig& config)
{
    // Aspect-preserving viewport in the default framebuffer.
    const float grid_aspect   = float(grid.nx()) / float(grid.ny());
    const float window_aspect = float(window_w)  / float(window_h);

    int vp_w = window_w;
    int vp_h = window_h;
    if (window_aspect > grid_aspect) {
        vp_w = int(window_h * grid_aspect);
    } else {
        vp_h = int(window_w / grid_aspect);
    }
    int vp_x = (window_w - vp_w) / 2;
    int vp_y = (window_h - vp_h) / 2;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, window_w, window_h);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);
    glClearColor(0.05f, 0.05f, 0.07f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glViewport(vp_x, vp_y, vp_w, vp_h);
    glBindVertexArray(empty_vao_);

    switch (mode) {
        case ViewMode::Smoke:        render_smoke(grid, config);                                    break;
        case ViewMode::Dye:          render_dye  (grid, config);                                    break;
        case ViewMode::Velocity:     render_velocity(grid);                                         break;
        case ViewMode::Pressure:     render_scalar(grid.pressure().read(),  0.5f, 0.0f, true);      break;
        case ViewMode::Divergence:   render_scalar(grid.divergence(),       4.0f, 0.0f, true);      break;
        case ViewMode::Temperature:  render_scalar(grid.temperature().read(),0.5f,0.0f, true);      break;
        case ViewMode::Vorticity:    render_scalar(grid.vorticity(),        2.0f, 0.0f, true);      break;
        case ViewMode::SolidMask:    render_scalar(grid.solid_mask(),       1.0f, 0.0f, false);     break;
        default: break;
    }
}

void Renderer::render_smoke(const FluidGrid& grid, const SimulationConfig& /*cfg*/) {
    smoke_prog_.use();
    grid.smoke().read().bind(0);
    smoke_prog_.set_int  ("u_smoke", 0);
    smoke_prog_.set_float("u_k", 1.5f);
    smoke_prog_.set_vec3 ("u_smoke_color", 0.95f, 0.95f, 0.95f);
    smoke_prog_.set_vec3 ("u_background",  0.04f, 0.05f, 0.08f);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void Renderer::render_dye(const FluidGrid& grid, const SimulationConfig& /*cfg*/) {
    dye_prog_.use();
    grid.dye().read().bind(0);
    dye_prog_.set_int ("u_dye", 0);
    dye_prog_.set_vec3("u_background", 0.04f, 0.05f, 0.08f);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void Renderer::render_velocity(const FluidGrid& grid) {
    velocity_prog_.use();
    grid.u().read().bind(0);
    grid.v().read().bind(1);
    velocity_prog_.set_int  ("u_u", 0);
    velocity_prog_.set_int  ("u_v", 1);
    velocity_prog_.set_ivec2("u_grid_size", grid.nx(), grid.ny());
    velocity_prog_.set_float("u_scale", 3.0f);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void Renderer::render_scalar(const Texture2D& tex, float scale, float offset, bool signed_map) {
    scalar_prog_.use();
    tex.bind(0);
    scalar_prog_.set_int  ("u_field",  0);
    scalar_prog_.set_float("u_scale",  scale);
    scalar_prog_.set_float("u_offset", offset);
    scalar_prog_.set_int  ("u_signed", signed_map ? 1 : 0);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

}  // namespace stable_fluids
