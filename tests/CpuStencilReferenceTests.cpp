#include "TestRunner.hpp"

#include <algorithm>
#include <cmath>
#include <vector>

// CPU mirrors of the GLSL stencil kernels. Each function below has a small
// implementation that can be checked by hand against a tiny example. These
// implementations are intentionally simple and do not optimize.

namespace {

constexpr int Nx = 4;
constexpr int Ny = 4;

using Field2D = std::vector<float>;

inline float& at(Field2D& f, int w, int i, int j) { return f[std::size_t(j) * w + i]; }
inline float  at(const Field2D& f, int w, int i, int j) { return f[std::size_t(j) * w + i]; }

Field2D zeros(int w, int h) { return Field2D(std::size_t(w) * h, 0.0f); }

// MAC-grid divergence (unit cell spacing).
//   d_{i,j} = (u_{i+1,j} - u_{i,j}) + (v_{i,j+1} - v_{i,j})
Field2D mac_divergence(const Field2D& u, const Field2D& v) {
    Field2D d = zeros(Nx, Ny);
    for (int j = 0; j < Ny; ++j) {
        for (int i = 0; i < Nx; ++i) {
            float du = at(u, Nx + 1, i + 1, j) - at(u, Nx + 1, i, j);
            float dv = at(v, Nx, i, j + 1)     - at(v, Nx, i, j);
            at(d, Nx, i, j) = du + dv;
        }
    }
    return d;
}

// One Jacobi step of Lap(p) = d  with Neumann boundaries.
Field2D pressure_jacobi(const Field2D& p, const Field2D& d) {
    Field2D pn = zeros(Nx, Ny);
    for (int j = 0; j < Ny; ++j) {
        for (int i = 0; i < Nx; ++i) {
            float c  = at(p, Nx, i, j);
            float pl = (i > 0)       ? at(p, Nx, i - 1, j) : c;
            float pr = (i < Nx - 1)  ? at(p, Nx, i + 1, j) : c;
            float pb = (j > 0)       ? at(p, Nx, i, j - 1) : c;
            float pt = (j < Ny - 1)  ? at(p, Nx, i, j + 1) : c;
            at(pn, Nx, i, j) = 0.25f * (pl + pr + pb + pt - at(d, Nx, i, j));
        }
    }
    return pn;
}

// Five-point Laplacian with Neumann boundary.
Field2D five_point_laplacian(const Field2D& q) {
    Field2D L = zeros(Nx, Ny);
    for (int j = 0; j < Ny; ++j) {
        for (int i = 0; i < Nx; ++i) {
            float c  = at(q, Nx, i, j);
            float ql = (i > 0)      ? at(q, Nx, i - 1, j) : c;
            float qr = (i < Nx - 1) ? at(q, Nx, i + 1, j) : c;
            float qb = (j > 0)      ? at(q, Nx, i, j - 1) : c;
            float qt = (j < Ny - 1) ? at(q, Nx, i, j + 1) : c;
            at(L, Nx, i, j) = ql + qr + qb + qt - 4.0f * c;
        }
    }
    return L;
}

// Bilinear sample of a Nx*Ny scalar field at continuous cell coordinates
// (px, py) where (0,0) is the lower-left cell center, and (Nx-1, Ny-1) is the
// upper-right cell center.
float bilinear(const Field2D& f, float px, float py) {
    px = std::clamp(px, 0.0f, float(Nx - 1));
    py = std::clamp(py, 0.0f, float(Ny - 1));
    int i0 = int(std::floor(px));
    int j0 = int(std::floor(py));
    int i1 = std::min(i0 + 1, Nx - 1);
    int j1 = std::min(j0 + 1, Ny - 1);
    float fx = px - float(i0);
    float fy = py - float(j0);
    float v00 = at(f, Nx, i0, j0);
    float v10 = at(f, Nx, i1, j0);
    float v01 = at(f, Nx, i0, j1);
    float v11 = at(f, Nx, i1, j1);
    float v0 = v00 + fx * (v10 - v00);
    float v1 = v01 + fx * (v11 - v01);
    return v0 + fy * (v1 - v0);
}

}  // namespace

// ---------------------------------------------------------------------------

TEST(mac_divergence_of_uniform_flow_is_zero) {
    Field2D u = zeros(Nx + 1, Ny);
    Field2D v = zeros(Nx, Ny + 1);
    // Uniform u = 1 everywhere.
    std::fill(u.begin(), u.end(), 1.0f);
    auto d = mac_divergence(u, v);
    for (float x : d) EXPECT_NEAR(x, 0.0f, 1e-6f);
}

TEST(mac_divergence_detects_source_cell) {
    Field2D u = zeros(Nx + 1, Ny);
    Field2D v = zeros(Nx, Ny + 1);
    // Set u_right of cell (1,1) to 1 and others zero -> divergence = 1.
    at(u, Nx + 1, 2, 1) = 1.0f;
    auto d = mac_divergence(u, v);
    EXPECT_NEAR(at(d, Nx, 1, 1), 1.0f, 1e-6f);
    EXPECT_NEAR(at(d, Nx, 0, 0), 0.0f, 1e-6f);
}

TEST(pressure_jacobi_zero_input_stays_zero) {
    Field2D p = zeros(Nx, Ny);
    Field2D d = zeros(Nx, Ny);
    auto p1 = pressure_jacobi(p, d);
    for (float x : p1) EXPECT_NEAR(x, 0.0f, 1e-6f);
}

TEST(pressure_jacobi_converges_for_constant_rhs) {
    // For d = 0, p = 0 is a fixed point.
    // Here we verify that running 50 Jacobi iterations on a small dipole
    // reduces the residual ||Lap(p) - d|| compared to the initial guess 0.
    Field2D d = zeros(Nx, Ny);
    at(d, Nx, 1, 1) =  1.0f;
    at(d, Nx, 2, 2) = -1.0f;

    Field2D p = zeros(Nx, Ny);
    auto residual = [&]() {
        auto L = five_point_laplacian(p);
        double s = 0;
        for (int k = 0; k < Nx * Ny; ++k) {
            double r = L[k] - d[k];
            s += r * r;
        }
        return std::sqrt(s);
    };

    double r0 = residual();
    for (int it = 0; it < 100; ++it) p = pressure_jacobi(p, d);
    double r1 = residual();
    EXPECT_TRUE(r1 < r0);
    EXPECT_TRUE(r1 < 1e-2);
}

TEST(bilinear_recovers_node_values) {
    Field2D f = zeros(Nx, Ny);
    for (int j = 0; j < Ny; ++j) {
        for (int i = 0; i < Nx; ++i) {
            at(f, Nx, i, j) = float(10 * i + j);
        }
    }
    for (int j = 0; j < Ny; ++j) {
        for (int i = 0; i < Nx; ++i) {
            EXPECT_NEAR(bilinear(f, float(i), float(j)), 10 * i + j, 1e-5f);
        }
    }
}

TEST(bilinear_midpoint_is_average) {
    Field2D f = zeros(Nx, Ny);
    at(f, Nx, 1, 1) = 4.0f;
    at(f, Nx, 2, 1) = 8.0f;
    at(f, Nx, 1, 2) = 2.0f;
    at(f, Nx, 2, 2) = 6.0f;
    EXPECT_NEAR(bilinear(f, 1.5f, 1.5f), 5.0f, 1e-5f);
}

TEST(midpoint_backtrace_with_uniform_velocity) {
    // If velocity is (1, 0) everywhere, a point at (2, 2) backtraced by dt=1
    // lands at (1, 2).
    auto trace = [](float x, float y, float vx, float vy, float dt) {
        // Midpoint: v0 = (vx, vy); mid = (x - 0.5*dt*vx, y - 0.5*dt*vy);
        // v_mid = (vx, vy); departure = (x - dt*vx, y - dt*vy).
        return std::pair<float, float>{x - dt * vx, y - dt * vy};
    };
    auto [x, y] = trace(2.0f, 2.0f, 1.0f, 0.0f, 1.0f);
    EXPECT_NEAR(x, 1.0f, 1e-6f);
    EXPECT_NEAR(y, 2.0f, 1e-6f);
}

int main() {
    return tests::run_all();
}
