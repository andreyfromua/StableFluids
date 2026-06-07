#version 410 core
#include "common.glsl"

// Cell-centered velocity magnitude visualization.
in  vec2 v_uv;
out vec4 frag_color;

uniform sampler2D u_u;
uniform sampler2D u_v;
uniform ivec2     u_grid_size;
uniform float     u_scale;

vec3 mag_ramp(float t) {
    t = clamp(t, 0.0, 1.0);
    return vec3(0.05 + 0.95 * t, 0.10 + 0.60 * t, 0.30 + 0.20 * t);
}

void main() {
    int i = int(floor(v_uv.x * float(u_grid_size.x)));
    int j = int(floor(v_uv.y * float(u_grid_size.y)));

    float u_l = texelFetch(u_u, ivec2(i,     j), 0).r;
    float u_r = texelFetch(u_u, ivec2(i + 1, j), 0).r;
    float v_b = texelFetch(u_v, ivec2(i, j    ), 0).r;
    float v_t = texelFetch(u_v, ivec2(i, j + 1), 0).r;

    vec2 vel = vec2(0.5 * (u_l + u_r), 0.5 * (v_b + v_t));
    float m  = length(vel) * u_scale;
    frag_color = vec4(mag_ramp(m), 1.0);
}
