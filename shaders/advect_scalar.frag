#version 410 core
#include "common.glsl"

// Cell-centered scalar advection. Works for either a scalar (R16F) or
// RGBA texture; controlled by u_is_rgba.
in  vec2 v_uv;
out vec4 frag_color;

uniform sampler2D u_u;
uniform sampler2D u_v;
uniform sampler2D u_field;        // field to advect
uniform ivec2     u_grid_size;
uniform float     u_dt;
uniform float     u_dissipation;  // exp(-rate * dt) - precomputed by C++
uniform float     u_ambient;      // for relaxation (temperature)
uniform float     u_relaxation;   // exp(-rate * dt); 1 = no relaxation
uniform int       u_is_rgba;      // 1 = treat field as RGBA

void main() {
    vec2 pos;
    pos.x = v_uv.x * float(u_grid_size.x);
    pos.y = v_uv.y * float(u_grid_size.y);

    vec2 vel0 = sample_velocity(u_u, u_v, pos, u_grid_size);
    vec2 mid  = pos - 0.5 * u_dt * vel0;
    mid       = clip_to_domain(mid, u_grid_size);
    vec2 vel  = sample_velocity(u_u, u_v, mid, u_grid_size);

    vec2 dep  = pos - u_dt * vel;
    dep       = clip_to_domain(dep, u_grid_size);

    vec2 wh = vec2(u_grid_size);
    if (u_is_rgba == 1) {
        vec4 q = sample_cell_rgba(u_field, dep, wh);
        q     *= u_dissipation;
        frag_color = q;
    } else {
        float q = sample_cell_scalar(u_field, dep, wh);
        q       = u_ambient + (q - u_ambient) * u_relaxation;
        q      *= u_dissipation;
        frag_color = vec4(q, 0.0, 0.0, 0.0);
    }
}
