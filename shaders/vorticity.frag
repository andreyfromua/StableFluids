#version 410 core
#include "common.glsl"

// omega = dv/dx - du/dy at cell centers, using central differences from
// surrounding faces (unit cell spacing).
in  vec2 v_uv;
out vec4 frag_color;

uniform sampler2D u_u;
uniform sampler2D u_v;
uniform ivec2     u_grid_size;

void main() {
    int i = int(floor(v_uv.x * float(u_grid_size.x)));
    int j = int(floor(v_uv.y * float(u_grid_size.y)));

    // dv/dx: difference of v at horizontal cell centers.
    // v_right at (i+0.5, j+0.5) and v_left at (i-0.5, j+0.5). We average v's
    // top+bottom face for each.
    float v_iL = 0.5 * (texelFetch(u_v, ivec2(max(i - 1, 0), j),     0).r +
                        texelFetch(u_v, ivec2(max(i - 1, 0), j + 1), 0).r);
    float v_iR = 0.5 * (texelFetch(u_v, ivec2(min(i + 1, u_grid_size.x - 1), j),     0).r +
                        texelFetch(u_v, ivec2(min(i + 1, u_grid_size.x - 1), j + 1), 0).r);
    float dvdx = 0.5 * (v_iR - v_iL);

    float u_jB = 0.5 * (texelFetch(u_u, ivec2(i,     max(j - 1, 0)), 0).r +
                        texelFetch(u_u, ivec2(i + 1, max(j - 1, 0)), 0).r);
    float u_jT = 0.5 * (texelFetch(u_u, ivec2(i,     min(j + 1, u_grid_size.y - 1)), 0).r +
                        texelFetch(u_u, ivec2(i + 1, min(j + 1, u_grid_size.y - 1)), 0).r);
    float dudy = 0.5 * (u_jT - u_jB);

    float omega = dvdx - dudy;
    frag_color = vec4(omega, 0.0, 0.0, 0.0);
}
