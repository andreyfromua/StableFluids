#version 410 core

// Generic scalar visualization. The pixel value is mapped through a
// signed colormap so positive values are warm and negative values are cool.
in  vec2 v_uv;
out vec4 frag_color;

uniform sampler2D u_field;
uniform float     u_scale;     // multiply the field value
uniform float     u_offset;    // added afterward
uniform int       u_signed;    // 1 = signed colormap, 0 = magnitude only

vec3 cool_to_warm(float t) {
    // -1..1 -> blue-white-red
    t = clamp(t, -1.0, 1.0);
    vec3 neg = vec3(0.20, 0.40, 1.00);
    vec3 pos = vec3(1.00, 0.40, 0.20);
    vec3 mid = vec3(1.0);
    if (t >= 0.0) return mix(mid, pos, t);
    return mix(mid, neg, -t);
}

vec3 mag_ramp(float t) {
    t = clamp(t, 0.0, 1.0);
    return vec3(t * 0.8 + 0.05, t * 0.85 + 0.05, t);
}

void main() {
    float v = texture(u_field, v_uv).r * u_scale + u_offset;
    if (u_signed == 1) {
        frag_color = vec4(cool_to_warm(v), 1.0);
    } else {
        frag_color = vec4(mag_ramp(v), 1.0);
    }
}
