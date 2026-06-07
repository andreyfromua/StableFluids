#version 410 core
#include "common.glsl"

// Closed walls: u = 0 at the left (i = 0) and right (i = Nx) domain edges.
in  vec2 v_uv;
out vec4 frag_color;

uniform sampler2D u_u;
uniform ivec2     u_grid_size;

void main() {
    float u_val = texture(u_u, v_uv).r;

    int i = int(floor(v_uv.x * float(u_grid_size.x + 1)));
    if (i <= 0 || i >= u_grid_size.x) {
        u_val = 0.0;
    }

    frag_color = vec4(u_val, 0.0, 0.0, 0.0);
}
