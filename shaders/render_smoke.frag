#version 410 core

// Smoke presentation: opacity from concentration; composited over a soft
// background color.
in  vec2 v_uv;
out vec4 frag_color;

uniform sampler2D u_smoke;
uniform float     u_k;            // extinction coefficient
uniform vec3      u_smoke_color;
uniform vec3      u_background;

void main() {
    float s = max(texture(u_smoke, v_uv).r, 0.0);
    float alpha = 1.0 - exp(-u_k * s);
    vec3 color  = mix(u_background, u_smoke_color, alpha);
    frag_color  = vec4(color, 1.0);
}
