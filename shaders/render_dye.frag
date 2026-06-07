#version 410 core

in  vec2 v_uv;
out vec4 frag_color;

uniform sampler2D u_dye;
uniform vec3      u_background;

void main() {
    vec4 dye = texture(u_dye, v_uv);
    float a  = clamp(dye.a, 0.0, 1.0);
    vec3 col = mix(u_background, dye.rgb, a);
    frag_color = vec4(col, 1.0);
}
