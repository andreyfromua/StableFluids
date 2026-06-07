#version 410 core
#include "common.glsl"

// Semi-Lagrangian advection of the u face velocity using midpoint backtrace.
in  vec2 v_uv;
out vec4 frag_color;

uniform sampler2D u_u;
uniform sampler2D u_v;
uniform sampler2D u_u_src;     // field to be advected (same texture for self-advection)
uniform ivec2     u_grid_size;
uniform float     u_dt;

void main() {
    // Compute face position (i, j+0.5) in cell units.
    vec2 face;
    face.x = v_uv.x * float(u_grid_size.x + 1) - 0.5;
    face.y = v_uv.y * float(u_grid_size.y);

    // Sample full velocity at face position.
    vec2 vel0 = sample_velocity(u_u, u_v, face, u_grid_size);

    // Midpoint backtrace.
    vec2 mid  = face - 0.5 * u_dt * vel0;
    mid       = clip_to_domain(mid, u_grid_size);
    vec2 vel  = sample_velocity(u_u, u_v, mid, u_grid_size);

    vec2 dep  = face - u_dt * vel;
    dep       = clip_to_domain(dep, u_grid_size);

    // Sample u at the departure position.
    float u_dep = sample_u(u_u_src, dep, u_grid_size);

    frag_color = vec4(u_dep, 0.0, 0.0, 0.0);
}
