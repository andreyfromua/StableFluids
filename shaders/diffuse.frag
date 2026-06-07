#version 410 core
#include "common.glsl"

// One Jacobi iteration of (I - nu*dt*Lap) q^D = q^0.
//   q^D_{i,j} = (q^0_{i,j} + a * (q^D_{i-1,j} + q^D_{i+1,j}
//                                 + q^D_{i,j-1} + q^D_{i,j+1}))
//              / (1 + 4*a)
// where a = nu * dt / h^2 (we use h = 1).
// The same shader can process u, v, smoke, etc. - dimensions and boundary
// classification are baked into the bound textures and the simulated layout.
in  vec2 v_uv;
out vec4 frag_color;

uniform sampler2D u_field;     // current iterate
uniform sampler2D u_source;    // fixed q^0 (RHS)
uniform ivec2     u_tex_size;  // dimensions of THIS texture (not grid)
uniform float     u_alpha;     // a = nu*dt/h^2

void main() {
    int i = int(floor(v_uv.x * float(u_tex_size.x)));
    int j = int(floor(v_uv.y * float(u_tex_size.y)));

    float c = texelFetch(u_field, ivec2(i, j), 0).r;
    int il = max(i - 1, 0);
    int ir = min(i + 1, u_tex_size.x - 1);
    int jb = max(j - 1, 0);
    int jt = min(j + 1, u_tex_size.y - 1);

    float l = texelFetch(u_field, ivec2(il, j), 0).r;
    float r = texelFetch(u_field, ivec2(ir, j), 0).r;
    float b = texelFetch(u_field, ivec2(i, jb), 0).r;
    float t = texelFetch(u_field, ivec2(i, jt), 0).r;

    // Neumann at boundaries (zero gradient).
    if (i == 0)                 l = c;
    if (i == u_tex_size.x - 1)  r = c;
    if (j == 0)                 b = c;
    if (j == u_tex_size.y - 1)  t = c;

    float q0 = texelFetch(u_source, ivec2(i, j), 0).r;

    float q  = (q0 + u_alpha * (l + r + b + t)) / (1.0 + 4.0 * u_alpha);
    frag_color = vec4(q, 0.0, 0.0, 0.0);
}
