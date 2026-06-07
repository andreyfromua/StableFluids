#version 410 core
#include "common.glsl"

in  vec2 v_uv;
out vec4 frag_color;

uniform sampler2D u_u;
uniform sampler2D u_v;
uniform sampler2D u_v_src;
uniform ivec2     u_grid_size;
uniform float     u_dt;

void main() {
    vec2 face;
    face.x = v_uv.x * float(u_grid_size.x);
    face.y = v_uv.y * float(u_grid_size.y + 1) - 0.5;

    vec2 vel0 = sample_velocity(u_u, u_v, face, u_grid_size);
    vec2 mid  = face - 0.5 * u_dt * vel0;
    mid       = clip_to_domain(mid, u_grid_size);
    vec2 vel  = sample_velocity(u_u, u_v, mid, u_grid_size);

    vec2 dep  = face - u_dt * vel;
    dep       = clip_to_domain(dep, u_grid_size);

    float v_dep = sample_v(u_v_src, dep, u_grid_size);
    frag_color = vec4(v_dep, 0.0, 0.0, 0.0);
}
