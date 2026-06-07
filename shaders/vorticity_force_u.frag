#version 410 core
#include "common.glsl"

// Vorticity confinement: f = epsilon * (N x omega) where
//   N = grad(|omega|) / |grad(|omega|)|.
// In 2D, omega is a scalar; f.x = epsilon * dN_y/dx * |omega|... use the
// classical formulation:
//   f_x = epsilon * h * (|omega|_{i,j+1} - |omega|_{i,j-1}) / (2|N|)
// Concretely, we form eta = grad(|omega|), then N = eta / max(|eta|, eps),
// and the body force is (N.y, -N.x) * omega multiplied by epsilon.
in  vec2 v_uv;
out vec4 frag_color;

uniform sampler2D u_u;
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
    int i = int(floor(v_uv.x * float(u_grid_size.x + 1)));
    int j = int(floor(v_uv.y * float(u_grid_size.y)));

    float u_old = texelFetch(u_u, ivec2(i, j), 0).r;
    if (i <= 0 || i >= u_grid_size.x) { frag_color = vec4(u_old, 0, 0, 0); return; }

    // Cell-center samples on the two adjacent cells of this face.
    int ci = i - 1;        // left cell
    int cj = j;

    // grad(|omega|) at the left cell center.
    float eta_l_x = 0.5 * (abs_omega(ci + 1, cj) - abs_omega(ci - 1, cj));
    float eta_l_y = 0.5 * (abs_omega(ci, cj + 1) - abs_omega(ci, cj - 1));
    float eta_r_x = 0.5 * (abs_omega(ci + 2, cj) - abs_omega(ci,     cj));
    float eta_r_y = 0.5 * (abs_omega(ci + 1, cj + 1) - abs_omega(ci + 1, cj - 1));

    // Average to face.
    float eta_x = 0.5 * (eta_l_x + eta_r_x);
    float eta_y = 0.5 * (eta_l_y + eta_r_y);
    float len   = max(length(vec2(eta_x, eta_y)), 1e-5);
    vec2  N     = vec2(eta_x, eta_y) / len;

    // Average omega to face.
    float w = 0.5 * (texelFetch(u_vort, ivec2(ci, cj), 0).r +
                     texelFetch(u_vort, ivec2(ci + 1, cj), 0).r);
    float fx = u_epsilon * (N.y * w);
    frag_color = vec4(u_old + u_dt * fx, 0.0, 0.0, 0.0);
}
