// Shared GLSL helpers for the Stable Fluids passes.
//
// Conventions:
//   - Cell centers are at integer index (i + 0.5, j + 0.5) in grid units.
//   - The horizontal velocity u_{i+1/2, j} lives at grid position (i, j+0.5),
//     so the u texture has size (Nx+1) x Ny and pixel (i, j) holds u at
//     grid position (i, j + 0.5).
//   - The vertical velocity v_{i, j+1/2} lives at grid position (i+0.5, j),
//     so the v texture has size Nx x (Ny+1) and pixel (i, j) holds v at
//     grid position (i + 0.5, j).
//   - Pressure, divergence, smoke, dye, temperature, mask, vorticity are
//     cell-centered and sized Nx x Ny.
//
// All shaders receive the uniforms:
//   uniform ivec2 u_grid_size;     // (Nx, Ny)
//   uniform vec2  u_inv_grid_size; // (1/Nx, 1/Ny)
// when they need to know the domain dimensions.

// Convert pixel (i, j) of a texture sized (W, H) to a UV coordinate at the
// pixel center.
vec2 pixel_uv(vec2 ij, vec2 wh) {
    return (ij + vec2(0.5)) / wh;
}

// Sample a cell-centered scalar at an arbitrary grid-unit position with
// CLAMP_TO_EDGE bilinear interpolation. `wh` is the cell-grid dimensions
// (Nx, Ny). Position is in cell-center units, i.e. position (0.5, 0.5)
// corresponds to cell (0,0).
float sample_cell_scalar(sampler2D tex, vec2 pos, vec2 wh) {
    // Translate from "position-in-cells" to pixel-center coordinates.
    vec2 uv = pos / wh;
    return texture(tex, uv).r;
}

vec4 sample_cell_rgba(sampler2D tex, vec2 pos, vec2 wh) {
    vec2 uv = pos / wh;
    return texture(tex, uv);
}

// Reconstruct horizontal velocity u at an arbitrary grid-unit position.
//   u texture has dimensions (Nx+1, Ny).
//   pixel (i, j) of u stores the value of u at grid position (i, j+0.5).
//   To bilinearly interpolate u(pos), we want the UV coordinate that lands
//   on the right pixel positions. Pixel center j in v-direction maps to
//   grid position j+0.5, so v_uv = (pos.y) / Ny. In x-direction pixel
//   center i maps to grid position i, so u_uv = pos.x / (Nx+1) is wrong;
//   instead we use (pos.x + 0.5) / (Nx+1).
float sample_u(sampler2D u_tex, vec2 pos, ivec2 grid) {
    vec2 wh = vec2(float(grid.x + 1), float(grid.y));
    vec2 uv = vec2(pos.x + 0.5, pos.y) / wh;
    return texture(u_tex, uv).r;
}

float sample_v(sampler2D v_tex, vec2 pos, ivec2 grid) {
    vec2 wh = vec2(float(grid.x), float(grid.y + 1));
    vec2 uv = vec2(pos.x, pos.y + 0.5) / wh;
    return texture(v_tex, uv).r;
}

// Reconstruct the full velocity vector at a grid-unit position.
vec2 sample_velocity(sampler2D u_tex, sampler2D v_tex, vec2 pos, ivec2 grid) {
    return vec2(sample_u(u_tex, pos, grid), sample_v(v_tex, pos, grid));
}

// Clip a backtraced departure position to the closed-domain fluid region.
vec2 clip_to_domain(vec2 pos, ivec2 grid) {
    return clamp(pos, vec2(0.5), vec2(float(grid.x) - 0.5, float(grid.y) - 0.5));
}

// Lookup the solid mask. Returns >= 0.5 if the cell is solid.
float sample_solid_mask(sampler2D mask, vec2 pos, ivec2 grid) {
    vec2 uv = clamp(pos, vec2(0.5), vec2(float(grid.x) - 0.5, float(grid.y) - 0.5))
              / vec2(grid);
    return texture(mask, uv).r;
}
