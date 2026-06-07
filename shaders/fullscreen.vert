#version 410 core

// Emit a single large triangle that covers the entire framebuffer.
// Output: v_uv has the range [0, 1] over the covered viewport.
out vec2 v_uv;

void main() {
    // Three corners of an oversized triangle.
    vec2 positions[3] = vec2[3](
        vec2(-1.0, -1.0),
        vec2( 3.0, -1.0),
        vec2(-1.0,  3.0)
    );
    vec2 uvs[3] = vec2[3](
        vec2(0.0, 0.0),
        vec2(2.0, 0.0),
        vec2(0.0, 2.0)
    );
    gl_Position = vec4(positions[gl_VertexID], 0.0, 1.0);
    v_uv        = uvs[gl_VertexID];
}
