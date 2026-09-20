// Browser entry point for the WebAssembly build.
//
// Mirrors src/main.cpp, but instead of argc/argv it reads configuration from
// the page URL's query string (e.g. index.html?grid=256&pressure=40). The
// shared Application drives everything else identically to the desktop build.

#include "stable_fluids/Application.hpp"
#include "stable_fluids/SimulationConfig.hpp"

#include <emscripten.h>

// Read an integer URL query parameter, or return `fallback` when it is absent
// or malformed. Implemented in JS because the query string lives in the DOM.
EM_JS(int, sf_query_int, (const char* name, int fallback), {
    const key = UTF8ToString(name);
    const search = (typeof location !== "undefined") ? location.search : "";
    const raw = new URLSearchParams(search).get(key);
    if (raw === null) return fallback;
    const value = parseInt(raw, 10);
    return Number.isFinite(value) ? value : fallback;
});

int main() {
    using namespace stable_fluids;

    SimulationConfig cfg{};
    // `grid` sets both dimensions; `grid_w`/`grid_h` override per axis.
    cfg.grid_width       = sf_query_int("grid_w",  sf_query_int("grid", cfg.grid_width));
    cfg.grid_height      = sf_query_int("grid_h",  sf_query_int("grid", cfg.grid_height));
    cfg.window_width     = sf_query_int("window_w", cfg.window_width);
    cfg.window_height    = sf_query_int("window_h", cfg.window_height);
    cfg.pressure_iterations = sf_query_int("pressure", cfg.pressure_iterations);
    cfg.substeps_per_frame  = sf_query_int("substeps", cfg.substeps_per_frame);

    // The browser MainLoop returns from run() immediately and the runtime keeps
    // invoking the frame callback (thanks to -sEXIT_RUNTIME=0). The Application
    // must therefore outlive main(): heap-allocate it and never delete — the
    // page owns it for its whole lifetime.
    auto* app = new Application(cfg);
    return app->run();
}
