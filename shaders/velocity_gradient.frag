#version 410 core
#include "common.glsl"

// Stores L = grad(u_center) packed as (du/dx, du/dy, dv/dx, dv/dy) using
// central differences on the staggered grid (unit cell spacing).
in  vec2 v_uv;
out vec4 frag_color;

uniform sampler2D u_u;
uniform sampler2D u_v;
uniform ivec2     u_grid_size;

void main() {
    int i = int(floor(v_uv.x * float(u_grid_size.x)));
    int j = int(floor(v_uv.y * float(u_grid_size.y)));

    float u_l = texelFetch(u_u, ivec2(i,     j), 0).r;
    float u_r = texelFetch(u_u, ivec2(i + 1, j), 0).r;
    float v_b = texelFetch(u_v, ivec2(i, j    ), 0).r;
    float v_t = texelFetch(u_v, ivec2(i, j + 1), 0).r;

    // du/dx exactly: (u_r - u_l) / h with h = 1.
    float dudx = u_r - u_l;
    float dvdy = v_t - v_b;

    // du/dy and dv/dx by cell-center central differences on the averaged
    // velocity in neighbouring cells.
    int il = max(i - 1, 0);
    int ir = min(i + 1, u_grid_size.x - 1);
    int jb = max(j - 1, 0);
    int jt = min(j + 1, u_grid_size.y - 1);

    float uc_b = 0.5 * (texelFetch(u_u, ivec2(i,     jb), 0).r +
                        texelFetch(u_u, ivec2(i + 1, jb), 0).r);
    float uc_t = 0.5 * (texelFetch(u_u, ivec2(i,     jt), 0).r +
                        texelFetch(u_u, ivec2(i + 1, jt), 0).r);
    float vc_l = 0.5 * (texelFetch(u_v, ivec2(il, j    ), 0).r +
                        texelFetch(u_v, ivec2(il, j + 1), 0).r);
    float vc_r = 0.5 * (texelFetch(u_v, ivec2(ir, j    ), 0).r +
                        texelFetch(u_v, ivec2(ir, j + 1), 0).r);

    // The clamped neighbor span is two cells in the interior and one cell at
    // a domain edge, naturally switching to a one-sided boundary derivative.
    float dy = float(max(jt - jb, 1));
    float dx = float(max(ir - il, 1));
    float dudy = (uc_t - uc_b) / dy;
    float dvdx = (vc_r - vc_l) / dx;

    frag_color = vec4(dudx, dudy, dvdx, dvdy);
}
