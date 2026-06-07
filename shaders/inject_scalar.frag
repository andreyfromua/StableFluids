#version 410 core
#include "common.glsl"

in  vec2 v_uv;
out vec4 frag_color;

uniform sampler2D u_scalar;        // previous value
uniform ivec2     u_grid_size;     // (Nx, Ny)
uniform vec2      u_center_cells;  // emitter center in cell units
uniform float     u_radius_cells;  // emitter radius in cell units
uniform float     u_amount;        // dt-scaled injection strength
uniform vec4      u_color;         // for RGBA fields (smoke uses (1,0,0,0))
uniform int       u_is_rgba;       // 1 = treat texture as RGBA

void main() {
    // Cell-center position of this destination pixel in grid units.
    vec2 pos = v_uv * vec2(u_grid_size);

    // Smooth radial falloff (Gaussian-like).
    vec2 d = pos - u_center_cells;
    float r2 = dot(d, d) / max(u_radius_cells * u_radius_cells, 1e-6);
    float w  = exp(-r2);

    if (u_is_rgba == 1) {
        vec4 prev = texture(u_scalar, v_uv);
        vec4 inj  = u_color * (u_amount * w);
        frag_color = max(vec4(0.0), prev + inj);
    } else {
        float prev = texture(u_scalar, v_uv).r;
        float inj  = u_amount * w;
        frag_color = vec4(max(0.0, prev + inj), 0.0, 0.0, 0.0);
    }
}
