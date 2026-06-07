#version 410 core
#include "common.glsl"

// Adds buoyancy to the v (vertical face) velocity field:
//   f_b = beta * (T - T_ambient) - alpha * s
//
// The v texture has dimensions Nx x (Ny+1). Pixel (i, j) corresponds to
// face position (i + 0.5, j). We average T and smoke from the two cells
// directly above (j) and below (j-1) the face.
in  vec2 v_uv;
out vec4 frag_color;

uniform sampler2D u_v;
uniform sampler2D u_smoke;
uniform sampler2D u_temperature;
uniform ivec2     u_grid_size;
uniform float     u_alpha;
uniform float     u_beta;
uniform float     u_ambient_t;
uniform float     u_dt;

void main() {
    float v_prev = texture(u_v, v_uv).r;

    // Face position (i+0.5, j) in cell units.
    vec2 face;
    face.x = v_uv.x * float(u_grid_size.x);
    face.y = v_uv.y * float(u_grid_size.y + 1) - 0.5;

    // Sample cell-centered fields at (face.x, face.y - 0.5) and (face.x, face.y + 0.5).
    vec2 wh = vec2(u_grid_size);
    // Linear sampling of cell-center scalar at face.y interpolates over the
    // two adjacent rows for us, which is exactly the desired average.
    float t  = sample_cell_scalar(u_temperature, face, wh);
    float s  = sample_cell_scalar(u_smoke,       face, wh);

    float fb = u_beta * (t - u_ambient_t) - u_alpha * s;

    // Skip the bottom and top walls (j = 0 or j = Ny) by zeroing the buoyancy
    // there. This avoids forcing the bottom row from outside-domain values.
    float j = v_uv.y * float(u_grid_size.y + 1) - 0.5;
    if (j <= 0.0 || j >= float(u_grid_size.y)) {
        fb = 0.0;
    }

    frag_color = vec4(v_prev + u_dt * fb, 0.0, 0.0, 0.0);
}
