#include "stable_fluids/Application.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>

static void print_usage(const char* prog) {
    std::printf(
        "Usage: %s [options]\n"
        "  --grid WxH        Simulation grid resolution (default 256x256)\n"
        "  --window WxH      Window resolution (default 768x768)\n"
        "  --pressure N      Pressure Jacobi iterations (default 40)\n"
        "  --substeps N      Substeps per displayed frame (default 1)\n"
        "  --no-buoyancy     Disable buoyancy at start\n"
        "  --viscosity F     Set kinematic viscosity (default 0)\n"
        "  -h, --help        Show this message\n", prog);
}

static bool parse_pair(const char* s, int& a, int& b) {
    return std::sscanf(s, "%dx%d", &a, &b) == 2;
}

int main(int argc, char** argv) {
    stable_fluids::SimulationConfig cfg{};

    for (int i = 1; i < argc; ++i) {
        const char* arg = argv[i];
        if (std::strcmp(arg, "-h") == 0 || std::strcmp(arg, "--help") == 0) {
            print_usage(argv[0]);
            return 0;
        }
        if (std::strcmp(arg, "--grid") == 0 && i + 1 < argc) {
            int w, h;
            if (parse_pair(argv[++i], w, h)) { cfg.grid_width = w; cfg.grid_height = h; }
        } else if (std::strcmp(arg, "--window") == 0 && i + 1 < argc) {
            int w, h;
            if (parse_pair(argv[++i], w, h)) { cfg.window_width = w; cfg.window_height = h; }
        } else if (std::strcmp(arg, "--pressure") == 0 && i + 1 < argc) {
            cfg.pressure_iterations = std::atoi(argv[++i]);
        } else if (std::strcmp(arg, "--substeps") == 0 && i + 1 < argc) {
            cfg.substeps_per_frame = std::atoi(argv[++i]);
        } else if (std::strcmp(arg, "--no-buoyancy") == 0) {
            cfg.buoyancy_enabled = false;
        } else if (std::strcmp(arg, "--viscosity") == 0 && i + 1 < argc) {
            cfg.viscosity = float(std::atof(argv[++i]));
            cfg.diffusion_enabled = cfg.viscosity > 0.0f;
        } else {
            std::fprintf(stderr, "Unknown argument: %s\n", arg);
            print_usage(argv[0]);
            return 1;
        }
    }

    stable_fluids::Application app(cfg);
    return app.run();
}
