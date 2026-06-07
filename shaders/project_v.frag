#version 410 core
#include "common.glsl"

// v_new_{i, j+1/2} = v_{i, j+1/2} - (p_{i, j+1} - p_{i, j})
in  vec2 v_uv;
out vec4 frag_color;

uniform sampler2D u_v;
uniform sampler2D u_pressure;
uniform ivec2     u_grid_size;

void main() {
    int i = int(floor(v_uv.x * float(u_grid_size.x)));
    int j = int(floor(v_uv.y * float(u_grid_size.y + 1)));

    if (j <= 0 || j >= u_grid_size.y) {
        frag_color = vec4(0.0);
        return;
    }

    float v_old = texelFetch(u_v, ivec2(i, j), 0).r;
    float p_b   = texelFetch(u_pressure, ivec2(i, j - 1), 0).r;
    float p_t   = texelFetch(u_pressure, ivec2(i, j    ), 0).r;
    float v_new = v_old - (p_t - p_b);

    frag_color = vec4(v_new, 0.0, 0.0, 0.0);
}
