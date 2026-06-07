#version 410 core

// Reconstruct the cell-centered Newtonian Cauchy stress tensor:
//   sigma = -p I + 2 mu D
// where D = 0.5 * (grad(u) + grad(u)^T), mu = rho * nu, and the pressure
// equivalent is recovered from projection potential phi = dt * p / rho.
//
// Output channels:
//   R = sigma_xx, G = sigma_xy, B = sigma_yx, A = sigma_yy
out vec4 frag_color;

uniform sampler2D u_velocity_gradient;
uniform sampler2D u_projection_potential;
uniform float     u_density;
uniform float     u_kinematic_viscosity;
uniform float     u_dt;

void main() {
    ivec2 cell = ivec2(gl_FragCoord.xy);
    vec4 gradient = texelFetch(u_velocity_gradient, cell, 0);
    float dudx = gradient.r;
    float dudy = gradient.g;
    float dvdx = gradient.b;
    float dvdy = gradient.a;

    float phi = texelFetch(u_projection_potential, cell, 0).r;
    float pressure = u_density * phi / max(u_dt, 1e-6);
    float dynamic_viscosity = u_density * u_kinematic_viscosity;

    float sigma_xx = -pressure + 2.0 * dynamic_viscosity * dudx;
    float sigma_xy = dynamic_viscosity * (dudy + dvdx);
    float sigma_yy = -pressure + 2.0 * dynamic_viscosity * dvdy;

    frag_color = vec4(sigma_xx, sigma_xy, sigma_xy, sigma_yy);
}
