#version 410 core
#include "common.glsl"

// Computes cell-centered divergence d_{i,j} = (u_{i+1/2,j} - u_{i-1/2,j})
//                                            + (v_{i,j+1/2} - v_{i,j-1/2})
// assuming unit cell spacing (h = 1).
in  vec2 v_uv;
out vec4 frag_color;

uniform sampler2D u_u;
uniform sampler2D u_v;
uniform ivec2     u_grid_size;

void main() {
    // Cell index (i, j). v_uv corresponds to cell-center pixel.
    int i = int(floor(v_uv.x * float(u_grid_size.x)));
    int j = int(floor(v_uv.y * float(u_grid_size.y)));

    // u_left  is at pixel (i, j) of u-texture (size (Nx+1)*Ny).
    // u_right is at pixel (i+1, j).
    float u_left  = texelFetch(u_u, ivec2(i,     j), 0).r;
    float u_right = texelFetch(u_u, ivec2(i + 1, j), 0).r;

    // v_bottom is at pixel (i, j), v_top at (i, j+1) of v-texture (size Nx*(Ny+1)).
    float v_bot = texelFetch(u_v, ivec2(i, j),     0).r;
    float v_top = texelFetch(u_v, ivec2(i, j + 1), 0).r;

    float div = (u_right - u_left) + (v_top - v_bot);
    frag_color = vec4(div, 0.0, 0.0, 0.0);
}
