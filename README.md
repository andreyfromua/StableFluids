# Stable Fluids (OpenGL 4.1 Core)

A real-time, two-dimensional Stable Fluids simulator implemented in C++20 and
GLSL 4.10 following the design described in
[`stable-fluids-opengl-implementation-plan.md`](./stable-fluids-opengl-implementation-plan.md).

The numerical scheme runs entirely on the GPU as a sequence of fullscreen
fragment-shader passes, while the C++ layer manages the window, OpenGL
resources, input, timing, and pass scheduling.

## Features (baseline)

- Dear ImGui control panel for live solver, source, and display settings
- MAC grid with face-centered velocity components
- Mouse-driven injection of smoke, dye, temperature, and momentum
- Buoyancy and optional vorticity confinement
- Optional implicit velocity diffusion
- Two pressure projections per substep (Jacobi solver)
- Midpoint semi-Lagrangian advection on staggered and cell-centered fields
- Closed free-slip walls
- Multiple diagnostic views (smoke, dye, velocity, pressure, divergence,
  temperature, vorticity, solid mask, Cauchy stress)

## Build

```bash
make configure      # configure debug build in ./build
make build          # compile
make run            # build and run the simulator
make test           # build and run CPU tests
make release        # configure and build optimized release in ./build-release
make clean          # remove build directories
```

Underneath, CMake is the source of truth:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j
./build/stable_fluids
```

OpenGL 4.1 Core is requested for cross-platform compatibility (Linux, macOS,
and Windows).  GLFW is discovered with `find_package` first and fetched from
GitHub if no installed copy is found. Dear ImGui is fetched at the pinned
`v1.92.8` release, or a local source tree can be supplied with
`-DSTABLE_FLUIDS_IMGUI_SOURCE_DIR=/path/to/imgui`. GLAD is committed in
`third_party/glad/` so no code-generation step is required.

## Control panel

The on-screen Dear ImGui panel provides live controls for:

- Display mode
- Stress quantity, color scale, and fluid density
- Pause, single-step, reset, and shader reload
- Pressure iterations, substeps, and timestep
- Buoyancy parameters
- Velocity diffusion and viscosity
- Vorticity confinement
- Emitter radius and source strengths
- Smoke and temperature dissipation

Mouse and keyboard input captured by the panel is not forwarded to fluid
injection or simulator shortcuts.

## Controls

| Input | Action |
|--|--|
| Left mouse drag | Add smoke / dye and momentum |
| Right mouse drag | Add heat and momentum |
| `Space` | Pause / resume |
| `.` | Single-step (while paused) |
| `R` | Reset |
| `1`..`9` | Smoke / Dye / Velocity / Pressure / Divergence / Temperature / Vorticity / Solid mask / Stress |
| `B` | Toggle buoyancy |
| `V` | Toggle vorticity confinement |
| `D` | Toggle velocity diffusion |
| `[` / `]` | Decrease / increase pressure iterations |
| `-` / `=` | Decrease / increase emitter radius |
| `F5` | Reload shaders |
| `Esc` | Quit |

## Command-line options

```text
--grid WxH        Simulation grid resolution (default 256x256)
--window WxH      Window resolution (default 768x768)
--pressure N      Pressure Jacobi iterations
--substeps N      Substeps per displayed frame
--no-buoyancy     Disable buoyancy at start
--viscosity F     Kinematic viscosity (0 disables diffusion)
```

## Project layout

```
include/stable_fluids/   public headers for the core library
src/                     C++20 implementation
shaders/                 GLSL 4.10 fragment shaders + a fullscreen vertex stage
third_party/glad/        committed OpenGL 4.1 Core loader
tests/                   small CPU stencil reference checks
cmake/                   warning / sanitizer helpers
```

## Notes on the algorithm

The simulator implements the substep pipeline described in the plan:

1. Inject scalar sources (smoke, dye, temperature).
2. Inject momentum onto the `u` and `v` faces.
3. Apply buoyancy on the vertical-face grid.
4. Apply velocity boundary conditions (free-slip walls).
5. Diffuse velocity with a Jacobi-iterated implicit solve (optional).
6. First pressure projection (divergence → Jacobi pressure → gradient subtract).
7. Advect each face-velocity component (midpoint semi-Lagrangian).
8. Second pressure projection.
9. Advect smoke, dye, and temperature with optional dissipation / relaxation.

The stress view is generated on demand after the final projection. A
cell-centered gradient pass computes
`(du/dx, du/dy, dv/dx, dv/dy)`, then a second pass constructs the Newtonian
Cauchy tensor `sigma = -p I + 2 mu D`. The UI can display von Mises magnitude,
individual normal or shear components, or hydrostatic pressure. This tensor is
a diagnostic reconstruction; pressure projection and velocity diffusion
already apply its effects to the simulated velocity.

The diffusion pass is intentionally simplified to a "rolling-RHS" Jacobi: the
right-hand side is sampled from the most recent iterate. The fixed point is
identical to the standard implicit-diffusion solver and the convergence is
adequate for the visual baseline; a separate RHS texture is the obvious next
step if higher accuracy is desired.
