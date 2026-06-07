#version 410 core
#include "common.glsl"

// Vorticity confinement force on v faces.  f_y = -epsilon * N.x * omega.
in  vec2 v_uv;
out vec4 frag_color;

uniform sampler2D u_v;
uniform sampler2D u_vort;
uniform ivec2     u_grid_size;
uniform float     u_epsilon;
uniform float     u_dt;

float abs_omega(int i, int j) {
    i = clamp(i, 0, u_grid_size.x - 1);
    j = clamp(j, 0, u_grid_size.y - 1);
    return abs(texelFetch(u_vort, ivec2(i, j), 0).r);
}

void main() {
    int i = int(floor(v_uv.x * float(u_grid_size.x)));
    int j = int(floor(v_uv.y * float(u_grid_size.y + 1)));

    float v_old = texelFetch(u_v, ivec2(i, j), 0).r;
    if (j <= 0 || j >= u_grid_size.y) { frag_color = vec4(v_old, 0, 0, 0); return; }

    int ci = i, cj = j - 1;     // bottom cell
    float eta_b_x = 0.5 * (abs_omega(ci + 1, cj) - abs_omega(ci - 1, cj));
    float eta_b_y = 0.5 * (abs_omega(ci, cj + 1) - abs_omega(ci, cj - 1));
    float eta_t_x = 0.5 * (abs_omega(ci + 1, cj + 1) - abs_omega(ci - 1, cj + 1));
    float eta_t_y = 0.5 * (abs_omega(ci, cj + 2) - abs_omega(ci, cj));

    float eta_x = 0.5 * (eta_b_x + eta_t_x);
    float eta_y = 0.5 * (eta_b_y + eta_t_y);
    float len   = max(length(vec2(eta_x, eta_y)), 1e-5);
    vec2  N     = vec2(eta_x, eta_y) / len;

    float w = 0.5 * (texelFetch(u_vort, ivec2(ci, cj), 0).r +
                     texelFetch(u_vort, ivec2(ci, cj + 1), 0).r);
    float fy = -u_epsilon * (N.x * w);
    frag_color = vec4(v_old + u_dt * fy, 0.0, 0.0, 0.0);
}
