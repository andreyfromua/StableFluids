#version 410 core
#include "common.glsl"

// Closed walls: v = 0 at the bottom (j = 0) and top (j = Ny) domain edges.
in  vec2 v_uv;
out vec4 frag_color;

uniform sampler2D u_v;
uniform ivec2     u_grid_size;

void main() {
    float v_val = texture(u_v, v_uv).r;

    int j = int(floor(v_uv.y * float(u_grid_size.y + 1)));
    if (j <= 0 || j >= u_grid_size.y) {
        v_val = 0.0;
    }

    frag_color = vec4(v_val, 0.0, 0.0, 0.0);
}
