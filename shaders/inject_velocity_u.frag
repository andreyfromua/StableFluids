#version 410 core
#include "common.glsl"

// Adds an impulse to the horizontal-face velocity u.
// The u texture has dimensions (Nx+1) x Ny. Pixel (i, j) stores u at face
// position (i, j + 0.5) in cell units, so from a fragment's UV (v_uv in
// [0,1]) the face position is:
//     face.x = v_uv.x * (Nx+1) - 0.5
//     face.y = v_uv.y * Ny
in  vec2 v_uv;
out vec4 frag_color;

uniform sampler2D u_u;             // previous u
uniform ivec2     u_grid_size;     // (Nx, Ny) cell-grid size
uniform vec2      u_center_cells;
uniform float     u_radius_cells;
uniform float     u_amount;        // dt-scaled force scale
uniform vec2      u_direction;     // mouse drag direction (cells/sec)

void main() {
    vec2 face;
    face.x = v_uv.x * float(u_grid_size.x + 1) - 0.5;
    face.y = v_uv.y * float(u_grid_size.y);

    vec2 d = face - u_center_cells;
    float r2 = dot(d, d) / max(u_radius_cells * u_radius_cells, 1e-6);
    float w  = exp(-r2);

    float prev    = texture(u_u, v_uv).r;
    float impulse = u_amount * w * u_direction.x;
    frag_color    = vec4(prev + impulse, 0.0, 0.0, 0.0);
}
