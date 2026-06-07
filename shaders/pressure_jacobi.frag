#version 410 core
#include "common.glsl"

// One Jacobi iteration of the discrete pressure Poisson equation
//   Lap(p) = div(u*)
// With unit cell spacing this reduces to
//   p_new_{i,j} = (p_{i+1,j} + p_{i-1,j} + p_{i,j+1} + p_{i,j-1} - d_{i,j}) / 4
// Neumann boundary condition at closed walls: neighbor outside the domain is
// equal to the current cell (i.e. dp/dn = 0).
in  vec2 v_uv;
out vec4 frag_color;

uniform sampler2D u_pressure;
uniform sampler2D u_divergence;
uniform ivec2     u_grid_size;

void main() {
    int i = int(floor(v_uv.x * float(u_grid_size.x)));
    int j = int(floor(v_uv.y * float(u_grid_size.y)));

    float p_c = texelFetch(u_pressure, ivec2(i, j), 0).r;

    int il = max(i - 1, 0);
    int ir = min(i + 1, u_grid_size.x - 1);
    int jb = max(j - 1, 0);
    int jt = min(j + 1, u_grid_size.y - 1);

    float p_l = texelFetch(u_pressure, ivec2(il, j), 0).r;
    float p_r = texelFetch(u_pressure, ivec2(ir, j), 0).r;
    float p_b = texelFetch(u_pressure, ivec2(i, jb), 0).r;
    float p_t = texelFetch(u_pressure, ivec2(i, jt), 0).r;

    // At walls, neighbor outside the domain contributes p_c (Neumann).
    if (i == 0)                p_l = p_c;
    if (i == u_grid_size.x - 1) p_r = p_c;
    if (j == 0)                p_b = p_c;
    if (j == u_grid_size.y - 1) p_t = p_c;

    float d = texelFetch(u_divergence, ivec2(i, j), 0).r;

    float p_new = 0.25 * (p_l + p_r + p_b + p_t - d);
    frag_color = vec4(p_new, 0.0, 0.0, 0.0);
}
