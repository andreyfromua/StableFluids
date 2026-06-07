#version 410 core

// Visualizes a scalar quantity extracted from the per-cell stress tensor.
in  vec2 v_uv;
out vec4 frag_color;

uniform sampler2D u_stress;
uniform int       u_mode;
uniform float     u_scale;

vec3 signed_map(float value) {
    float t = clamp(value, -1.0, 1.0);
    vec3 negative = vec3(0.16, 0.38, 1.00);
    vec3 positive = vec3(1.00, 0.30, 0.12);
    vec3 neutral  = vec3(0.95);
    return t < 0.0
        ? mix(neutral, negative, -t)
        : mix(neutral, positive, t);
}

vec3 magnitude_map(float value) {
    float t = clamp(value, 0.0, 1.0);
    vec3 low  = vec3(0.03, 0.05, 0.16);
    vec3 mid  = vec3(0.10, 0.78, 0.92);
    vec3 high = vec3(1.00, 0.30, 0.05);
    return t < 0.5
        ? mix(low, mid, 2.0 * t)
        : mix(mid, high, 2.0 * t - 1.0);
}

void main() {
    vec4 stored = texture(u_stress, v_uv);
    float sigma_xx = stored.r;
    float sigma_xy = 0.5 * (stored.g + stored.b);
    float sigma_yy = stored.a;

    float value;
    bool signed_value = true;

    if (u_mode == 0) {
        // Two-dimensional von Mises equivalent stress.
        value = sqrt(max(
            sigma_xx * sigma_xx
          - sigma_xx * sigma_yy
          + sigma_yy * sigma_yy
          + 3.0 * sigma_xy * sigma_xy,
            0.0));
        signed_value = false;
    } else if (u_mode == 1) {
        value = sigma_xx;
    } else if (u_mode == 2) {
        value = sigma_yy;
    } else if (u_mode == 3) {
        value = sigma_xy;
    } else {
        // Positive means compression.
        value = -0.5 * (sigma_xx + sigma_yy);
    }

    float mapped = value * u_scale;
    vec3 color = signed_value ? signed_map(mapped)
                              : magnitude_map(mapped);
    frag_color = vec4(color, 1.0);
}
