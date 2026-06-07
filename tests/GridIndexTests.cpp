#include "TestRunner.hpp"

// These tests check the indexing conventions documented in the design plan:
//   - cell-center (i, j) sits at continuous position (i + 0.5, j + 0.5)
//   - u face (i+1/2, j) sits at position (i + 1.0, j + 0.5) in cell units
//   - v face (i, j+1/2) sits at position (i + 0.5, j + 1.0) in cell units
//   - u texture is (Nx+1) x Ny, v texture is Nx x (Ny+1)

namespace {

constexpr int Nx = 8;
constexpr int Ny = 6;

float cell_center_x(int i) { return float(i) + 0.5f; }
float cell_center_y(int j) { return float(j) + 0.5f; }

float u_face_x(int i) { return float(i); }
float u_face_y(int j) { return float(j) + 0.5f; }

float v_face_x(int i) { return float(i) + 0.5f; }
float v_face_y(int j) { return float(j); }

}  // namespace

TEST(grid_cell_centers_are_half_offsets) {
    EXPECT_NEAR(cell_center_x(0), 0.5f, 1e-6f);
    EXPECT_NEAR(cell_center_y(Ny - 1), Ny - 0.5f, 1e-6f);
}

TEST(u_faces_lie_on_vertical_grid_lines) {
    // u face (i+1/2, j) -> x in cell units is i+1 for face index i+1
    EXPECT_NEAR(u_face_x(0),   0.0f, 1e-6f);
    EXPECT_NEAR(u_face_x(Nx),  float(Nx), 1e-6f);
    EXPECT_NEAR(u_face_y(0),   0.5f, 1e-6f);
}

TEST(v_faces_lie_on_horizontal_grid_lines) {
    EXPECT_NEAR(v_face_x(0), 0.5f, 1e-6f);
    EXPECT_NEAR(v_face_y(0), 0.0f, 1e-6f);
    EXPECT_NEAR(v_face_y(Ny), float(Ny), 1e-6f);
}
