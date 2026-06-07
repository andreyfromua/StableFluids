#version 410 core
#include "common.glsl"

// Subtract the pressure gradient from u:
//   u_new_{i+1/2, j} = u_{i+1/2, j} - (p_{i+1, j} - p_{i, j})
// at internal vertical faces (1 <= i < Nx). Domain-edge faces are left at 0
// (the boundary pass enforces it again before/after).
in  vec2 v_uv;
out vec4 frag_color;

uniform sampler2D u_u;
uniform sampler2D u_pressure;
uniform ivec2     u_grid_size;

void main() {
    int i = int(floor(v_uv.x * float(u_grid_size.x + 1)));
    int j = int(floor(v_uv.y * float(u_grid_size.y)));

    if (i <= 0 || i >= u_grid_size.x) {
        // Domain-edge face: keep velocity at zero.
        frag_color = vec4(0.0);
        return;
    }

    float u_old = texelFetch(u_u, ivec2(i, j), 0).r;
    float p_l   = texelFetch(u_pressure, ivec2(i - 1, j), 0).r;
    float p_r   = texelFetch(u_pressure, ivec2(i,     j), 0).r;
    float u_new = u_old - (p_r - p_l);

    frag_color = vec4(u_new, 0.0, 0.0, 0.0);
}
