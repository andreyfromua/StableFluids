#version 410 core
#include "common.glsl"

// Adds an impulse to the vertical-face velocity v.
// The v texture has dimensions Nx x (Ny+1). Pixel (i, j) stores v at face
// position (i + 0.5, j). From a fragment's UV:
//     face.x = v_uv.x * Nx
//     face.y = v_uv.y * (Ny+1) - 0.5
in  vec2 v_uv;
out vec4 frag_color;

uniform sampler2D u_v;
uniform ivec2     u_grid_size;
uniform vec2      u_center_cells;
uniform float     u_radius_cells;
uniform float     u_amount;
uniform vec2      u_direction;

void main() {
    vec2 face;
    face.x = v_uv.x * float(u_grid_size.x);
    face.y = v_uv.y * float(u_grid_size.y + 1) - 0.5;

    vec2 d = face - u_center_cells;
    float r2 = dot(d, d) / max(u_radius_cells * u_radius_cells, 1e-6);
    float w  = exp(-r2);

    float prev    = texture(u_v, v_uv).r;
    float impulse = u_amount * w * u_direction.y;
    frag_color    = vec4(prev + impulse, 0.0, 0.0, 0.0);
}
