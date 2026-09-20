# StableFluids — Local Plans & TODO

Local, in-repo planning document. Not published — unlike
`stable-fluids-opengl-implementation-plan.md` in the repo root (which is a
symlink into the WebSitePortfolio site), this file lives entirely inside the
StableFluids project.

---

## Plan: WebAssembly / Browser Target

**Status:** implemented; **both targets build green.** Desktop = build + CPU
tests pass. Web = compiles and links with Emscripten 6.0.3 into valid
`stable_fluids.{html,js,wasm,data}` (the `.wasm` starts with the `\0asm` magic;
shaders are preloaded into MEMFS). Build the web target with:

```sh
source ~/emsdk/emsdk_env.sh
emcmake cmake -S . -B build-web -DCMAKE_BUILD_TYPE=Release
cmake --build build-web -j
```

Tasks 1–8 are done and compile/link-verified on both targets. What remains is
**in-browser runtime validation** (tasks 9–10): serve the page and confirm the
WebGL paths that only exist at runtime — GLSL ES shader compilation, extension
detection / the `R16F` velocity fallback, the rAF loop, and touch input — plus
visual parity with desktop. That needs a browser (and, for tasks 9's DPR cap
and explicit touch→emitter mapping, a little more code). Serve locally with
`emrun build-web/stable_fluids.html` or any static server.

Implemented via a clean platform seam rather than scattered `#ifdef`s:
- `gl/GlHeaders.hpp` — one place that chooses GLAD vs GLES3 (`STABLE_FLUIDS_WEB`).
- `platform/GlslDialect` — Strategy for the `#version`/precision preamble + ImGui version string.
- `platform/GraphicsCapabilities` — Information Expert; detects float extensions, chooses velocity format.
- `platform/Platform` — Pure Fabrication; context hints, GL loader, extension enabling.
- `platform/MainLoop` — Strategy + Factory; blocking loop vs `emscripten_set_main_loop`.
- `FluidGrid` takes the velocity format by injection; `Application::frame()` is the shared unit of work.
- `web/main_web.cpp`, `web/shell.html`, and the `if(EMSCRIPTEN)` CMake branch.

**Goal:** run the existing simulator in a browser tab via WebAssembly/WebGL,
with no installation, while keeping the desktop application unchanged.

### 1. Decision: one codebase, two targets (not a separate app)

Add a browser build target to *this* project rather than forking a separate web
app. The numerical core is already written against the WebGL 2.0 subset of
OpenGL, so most of it compiles to WebAssembly unchanged.

| Layer | Portable as-is? | Reason |
|--|--|--|
| `FluidGrid`, `FluidSolver`, all 27 shaders | Yes | Pure fragment-shader passes over textures |
| `PingPongField`, `Texture2D`, `Framebuffer` | Yes | Use only the WebGL 2.0 subset (VAOs, `glDrawArraysInstanced`, `glDrawBuffers`, `glFramebufferTexture2D`) |
| `Renderer`, `InputController`, `UserInterface` | Mostly | GLFW + ImGui, both have Emscripten backends |
| `Application` (window, context, main loop) | No | Platform-specific window/context/loop |
| Build system, GLSL version, asset delivery | No | Toolchain-, dialect-, packaging-specific |

The simulation core does all numerical work in fragment shaders — no compute
shaders, no shader storage buffers, no image load/store — and draws only with
VAOs, `glDrawArraysInstanced`, `glDrawBuffers`, and `glFramebufferTexture2D`,
all of which exist in WebGL 2.0. So the great majority of the source compiles to
WebAssembly unchanged; the platform-specific surface is small and concentrated
in `Application`, the shader `#version` line, and the build files.

Forking a separate project would duplicate the solver and every shader, and
every future feature would drift across two copies. A single codebase keeps one
solver, one shader set. A separate app would only be justified if the browser
version were a *different product* (bespoke DOM UI, mobile-first redesign),
which is out of scope: the browser target is the *same* simulator in a new
place. The browser build is a thin front end (web entry point + HTML shell) over
the shared `stable_fluids_core`.

### 2. Platform mapping

| Concern | Desktop (today) | Browser target |
|--|--|--|
| Toolchain | C++20, host compiler, CMake | Emscripten (`emcmake`/`emcc`) → `.wasm` + `.js` |
| Graphics API | OpenGL 4.1 Core | WebGL 2.0 (OpenGL ES 3.0) |
| Shaders | GLSL 4.10 (`#version 410 core`) | GLSL ES 3.00 (`#version 300 es` + precision) |
| GL loader | GLAD | none (Emscripten provides GLES3 symbols) |
| Window/context/input | GLFW (native) | GLFW3 Emscripten port (`-sUSE_GLFW=3`) on a canvas |
| Frame driver | blocking `while` | `emscripten_set_main_loop` (rAF) |
| Shader assets | disk (`std::ifstream`) | preloaded virtual FS (`--preload-file`) |
| UI | ImGui GLFW+GL3 backend | same backend, GLES3 init string |
| Float render targets | `R32F`/`RGBA32F`/`R16F` renderable | require `EXT_color_buffer_float` |
| Float linear filter | `R32F` velocity `GL_LINEAR` | require `OES_texture_float_linear`, else fall back to `R16F` velocity |

WebGL 2.0 is functionally OpenGL ES 3.0, so this is mostly context creation,
shader dialect, loop inversion, and asset packaging — not algorithm changes.

### 3. Task checklist (ordered roadmap)

`[x]` = code-complete and compile/link-verified on the relevant target(s).
Runtime/in-browser behaviour is validated by tasks 9–10.

- [x] **1. Toolchain bring-up.** `if(EMSCRIPTEN)` CMake branch + `STABLE_FLUIDS_WEB`
      header/loader seam; the web target links to `stable_fluids.html`. (§4.1, §4.2)
- [x] **2. Main-loop inversion.** `Application::main_loop()` → shared `frame()`;
      `MainLoop` Strategy drives it (blocking vs `emscripten_set_main_loop`). (§4.4)
- [x] **3. Context creation.** WebGL2/ES3 requested under Emscripten via the GLFW
      port (`platform::apply_gl_window_hints`). (§4.3)
- [x] **4. Shader dialect.** `GlslDialect` injects the `#version 300 es` +
      `precision highp …` prologue at load time. (GLSL ES compile happens at
      runtime — verify in-browser.) (§4.5)
- [x] **5. Float capabilities.** `GraphicsCapabilities` enables
      `EXT_color_buffer_float`, detects `OES_texture_float_linear`, and chooses
      the `R32F`→`R16F` velocity fallback. (§4.5)
- [x] **6. Assets.** `--preload-file shaders@/shaders` → `.data` bundle built;
      `LINK_DEPENDS` relinks on shader edits. (§4.6)
- [x] **7. ImGui.** GLES3 init string via `GlslDialect::imgui_glsl_version()`. (§4.7)
- [x] **8. Web entry point.** `web/main_web.cpp` (URL-param config) +
      `web/shell.html`. (§4.8)
- [~] **9. Robustness + mobile.** `shell.html` surfaces `webglcontextlost` and
      sets `touch-action:none`; still TODO: `EM_JS` rebuild-on-restore, DPR cap,
      explicit touch→emitter mapping in `InputController`, modest default web
      grid. (§4.9)
- [ ] **10. Parity + validation.** Serve the page; confirm WebGL paths and visual
      parity vs desktop; CPU stencil tests already pass (platform-independent).

### 4. Required changes (detailed)

The change is one shared library plus a platform-specific edge. Everything below
except §4.5 is confined to `Application`, the build system, and the shader loader.

#### 4.1 Build system

Add an Emscripten path to `CMakeLists.txt` guarded by `if(EMSCRIPTEN)`. Skip
`find_package(OpenGL)`, the GLAD static library, and the native GLFW fetch —
Emscripten supplies GLES3 and its own GLFW port. Emit an HTML shell via a
`.html` suffix, and pass browser options as **linker** flags with the `SHELL:`
prefix so CMake does not split or de-duplicate the space-separated arguments:

```cmake
if(EMSCRIPTEN)
    set_target_properties(stable_fluids PROPERTIES SUFFIX ".html")
    target_link_options(stable_fluids PRIVATE
        "SHELL:-sUSE_GLFW=3"
        "SHELL:-sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2"
        "SHELL:-sALLOW_MEMORY_GROWTH=1"
        "SHELL:-sEXIT_RUNTIME=0"          # keep the runtime alive after run() returns
        "SHELL:--shell-file ${CMAKE_CURRENT_SOURCE_DIR}/web/shell.html"
        "SHELL:--preload-file ${CMAKE_CURRENT_SOURCE_DIR}/shaders@/shaders")
    # --preload-file bakes the bundle at LINK time; register the shaders as link
    # inputs so a shader edit forces a relink and the .data never goes stale.
    file(GLOB SF_PRELOAD CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*")
    set_target_properties(stable_fluids PROPERTIES
        LINK_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/web/shell.html;${SF_PRELOAD}")
endif()
```

- `-sEXIT_RUNTIME=0` is required because the frame callback keeps firing after
  `run()` returns (see §4.4).
- `-sFULL_ES3=1` is deliberately omitted: it only matters for client-side vertex
  arrays, which this app does not use (it draws from VAOs).
- Debug web builds add `-g -gsource-map` and `-sASSERTIONS=2` for DWARF
  breakpoints and readable API-misuse messages.
- Keep the desktop executable, the GLAD library, and the shader-copy
  `add_custom_command` behind `if(NOT EMSCRIPTEN)`.
- `emcmake cmake -S . -B build-web` selects the browser path into a separate
  build directory; the desktop build is unaffected.

#### 4.2 GL headers and function loading

GLAD is desktop-only. Wrap the include and the `gladLoadGLLoader` call:

```cpp
#ifdef __EMSCRIPTEN__
#  include <GLES3/gl3.h>
#  include <emscripten.h>
#  include <emscripten/html5.h>
#else
#  include <glad/glad.h>
#endif
```

`Application::initialize_gl()` becomes a no-op loader under Emscripten (symbols
are already resolved) but keeps the extension checks from §4.5. The existing
`#ifdef GL_KHR_debug` debug-callback block is naturally excluded in the browser
because that macro is not defined there.

#### 4.3 Context creation and the GLFW-vs-HTML5 choice

We deliberately keep **GLFW** on the web (via `-sUSE_GLFW=3`) rather than porting
to `emscripten_webgl_create_context` + the HTML5 event API. Reason: the existing
`InputController` and the Dear ImGui `imgui_impl_glfw` backend are both written
against GLFW, and the Emscripten port implements exactly the `glfwCreateWindow`,
cursor, mouse, key, and framebuffer-size callbacks they already use. Rewriting to
raw HTML5 would re-plumb input and force a different ImGui backend for no gain.
The GLFW3 + OpenGL3 + Emscripten combination is also Dear ImGui's official
example, so it is well travelled.

In `initialize_glfw()`, request a WebGL 2.0 / ES 3.0 context under Emscripten:

```cpp
#ifdef __EMSCRIPTEN__
glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
glfwWindowHint(GLFW_SAMPLES, 0);   // antialias:false — see §4.9
#else
// existing 4.1 Core hints
#endif
```

The `-sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2` link flags are what actually
force a WebGL 2.0 context under the GLFW port; the hints only document intent.
`glfwSwapInterval`, the sRGB hint, the debug-context hint, and
`glfwSetWindowTitle` are ignored in the browser and can stay as harmless no-ops.
Canvas resizing arrives through the same `glfwSetFramebufferSizeCallback`
already wired up.

#### 4.4 Main-loop inversion (the one structural refactor)

A browser cannot run a blocking `while (!glfwWindowShouldClose)` loop; the page
would freeze. Extract the body of `Application::main_loop()` into a single
`void frame()` that advances exactly one frame. Desktop keeps a `while` loop that
calls `frame()`; the browser registers it with the runtime:

```cpp
#ifdef __EMSCRIPTEN__
static void frame_trampoline(void* self) {
    static_cast<Application*>(self)->frame();
}
// in run(): fps=0 syncs to the display via requestAnimationFrame (do not
// hard-code 60); simulate_infinite_loop=false so run() returns and the browser
// drives the callback. Pair with -sEXIT_RUNTIME=0 (§4.1) so the runtime is not
// torn down while the callback still needs it.
emscripten_set_main_loop_arg(frame_trampoline, this, /*fps=*/0,
                             /*simulate_infinite_loop=*/false);
#else
while (!glfwWindowShouldClose(window_)) frame();
#endif
```

The per-frame timing that uses `std::chrono::steady_clock` works unchanged under
Emscripten (it maps to the browser high-resolution clock), so the `frame_dt`
clamp and substep scheduling are preserved.

#### 4.5 GLSL dialect and float-texture capabilities

**Dialect.** Every shader begins with `#version 410 core`. GLSL ES 3.00 needs
`#version 300 es` followed by default precision qualifiers
(`precision highp float;`, `precision highp int;`, and `highp` sampler
precision). Rather than maintaining two copies of 27 shaders, inject the
version/precision preamble **at load time**: `expand_includes()` in
`ShaderProgram.cpp` is the natural injection point — strip the leading
`#version` line and prepend the target-appropriate header. The injected header's
very first characters must be `#version 300 es` with **no leading blank line** —
GLES requires `#version` on physical line 1, and a stray newline before it makes
WebGL 2.0 reject every shader (a black-screen failure that desktop GL tolerates).
The built-ins the shaders already use — `texelFetch`, `texture`, `gl_VertexID`,
`gl_InstanceID`, instanced drawing — all exist in GLSL ES 3.00, so the bodies
need no rewrite. `highp` (not `mediump`) is required for the finite-difference
stencils and back-trace coordinates to stay accurate.

**Renderable float targets.** WebGL 2.0 does not make `R32F`, `RGBA32F`, `R16F`,
or `RGBA16F` color-renderable by default. Enable `EXT_color_buffer_float` once at
startup (query it in `initialize_gl()`); without it every ping-pong framebuffer
is incomplete.

**Linear filtering of full-float textures — the sharpest risk.** The velocity
fields `u` and `v` are `R32F` with `GL_LINEAR` and are sampled with `texture()`
during advection and buoyancy. In WebGL 2.0, linear filtering of a **32-bit**
float texture requires `OES_texture_float_linear`, widely available on desktop
but not guaranteed on all mobile GPUs. Half-float linear is core in WebGL 2.0,
so smoke/dye/temperature (`R16F`/`RGBA16F`, `GL_LINEAR`) are safe; pressure,
divergence, and tensor fields use `GL_NEAREST` and are also safe. Mitigation, in
order:

1. Detect `OES_texture_float_linear`; if present, keep `R32F` velocity —
   bit-for-bit the desktop layout.
2. If absent, allocate velocity as `R16F` (half-float), whose linear filtering
   is guaranteed. Localized change in `FluidGrid::velocity_spec`; acceptable for
   interactive smoke, at some precision loss in long-running projection.

This single capability gate is the main portability question — validate early on
target hardware.

#### 4.6 Shader asset delivery

Shaders are read at runtime with `std::ifstream` from `STABLE_FLUIDS_SHADER_DIR`.
Packaging `shaders/` with `--preload-file shaders@/shaders` mounts it into
Emscripten's in-memory filesystem, so the existing `ifstream` include-expansion
path works unchanged against the virtual path `/shaders`. The `F5` reload
shortcut keeps working against the preloaded copy; it just cannot pick up host
edits without a rebuild.

#### 4.7 Dear ImGui

The GLFW and OpenGL 3 ImGui backends already support Emscripten. Adjustments:

- Initialize with `ImGui_ImplOpenGL3_Init("#version 300 es")` under Emscripten
  instead of `"#version 410 core"`.
- Disable `.ini` persistence (`io.IniFilename = nullptr`) or route it to browser
  storage — there is no writable local file.
- Touch/pointer events arrive through the same GLFW callbacks; no separate input
  path is needed for the panel.

#### 4.8 Web entry point and configuration

`main.cpp` parses `argc`/`argv`, which is not meaningful in a browser. Add a
small `web/main_web.cpp` that builds a `SimulationConfig` from URL query
parameters (grid size, pressure iterations, substeps) via a short JS glue
snippet, then calls the same `Application::run()`. The desktop `main.cpp` is
untouched. `web/shell.html` provides the canvas, layout, and a loading indicator.

#### 4.9 Robustness and mobile GPU budget

The simulator is lighter than a full 3D engine — its shaders are small fragment
passes with a handful of uniforms, so the WebGL 2.0 vertex-uniform ceiling (256
vectors) is not a concern here — but three points still apply:

- **Handle context loss.** A mobile browser can drop the WebGL context under
  memory pressure; the first `glCreateShader`/`glCreateProgram` afterward returns
  `0`, which otherwise surfaces as an opaque `TypeError`. The creation sites in
  `ShaderProgram` already check for `0`; add `webglcontextlost`/
  `webglcontextrestored` handlers via a short `EM_JS` snippet that
  `preventDefault()`s the loss and rebuilds GL state on restore (recreate the
  grid textures and reload shaders).
- **Budget GPU memory against the grid, not asset textures.** This app has no
  image assets; its GPU footprint is the ping-pong float fields, which scale with
  Nx×Ny. At the default 256×256 the total is a few megabytes and safe. A large
  web grid (512×512 or 1024×1024) multiplies both memory and per-substep cost, so
  the browser build should default to a modest grid and expose size as an opt-in.
- **Cap the presentation cost.** The material pass runs one fragment per output
  pixel at cssSize × devicePixelRatio, and cost scales with DPR squared. Cap the
  drawing-buffer DPR (e.g. 1.5) on high-density displays, and request
  `antialias:false` (`GLFW_SAMPLES = 0`); a multisampled HiDPI backbuffer is a
  leading cause of mobile context loss.

**Touch input.** The emitter is driven by mouse drag. On a phone there is no
mouse, so the web build must map touch to the same drag pulse. The Emscripten
GLFW port forwards primary touch as cursor/button events, but the handler must
treat touch-move as "continue drag" and only touch-end as "stop" — never treat
move as release, or a jittering held finger cancels the stroke on the next event.

### 5. Proposed layout

```text
src/            unchanged shared library (Application gains frame()/ifdefs)
shaders/        unchanged; preloaded into the virtual FS for the web build
web/
    main_web.cpp   browser entry point (config from URL, calls run())
    shell.html     canvas + layout + loader
docs/TODO.md    this plan
CMakeLists.txt  gains an if(EMSCRIPTEN) branch
```

### 6. Risks and validation

| Risk | Mitigation |
|--|--|
| `OES_texture_float_linear` missing on some GPUs | Fall back to `R16F` velocity (§4.5) — the sharpest question, validate early |
| `EXT_color_buffer_float` missing | Clear unsupported-browser message; WebGL2 desktop coverage is broad |
| `highp` vs desktop `float` precision drift | Require `highp`; validate divergence/projection residual vs desktop |
| Main-thread stalls at high grid sizes | Keep substep/fixed-timestep controls; small default web grid |
| Context loss on mobile (opaque `TypeError`) | Handle `webglcontextlost`/`restored`; defend null shader/program returns (§4.9) |
| HiDPI/MSAA exhausting mobile GPU memory | Cap DPR; `antialias:false` (§4.9) |
| No mouse on phones | Map touch → emitter drag; move = continue, not release (§4.9) |
| Stale preloaded shaders after an edit | `LINK_DEPENDS` forces a relink (§4.1) |
| Shader-reload expectations | Browser reload uses the preloaded copy; needs a rebuild |

The desktop build remains the reference implementation. The browser target is
validated by visual parity on identical inputs and by the existing
platform-independent CPU stencil tests, which continue to exercise the shared
solver math.

### 7. Distribution / shipping

The four output files (`.wasm`, `.js`, `.data`, `.html`) are static assets.
Serve over HTTPS with the `application/wasm` MIME type (which also unlocks
streaming compilation) and versioned paths to avoid stale caches. Embed into a
page with a self-sized `<iframe>`. Because the build is single-threaded, it needs
none of the cross-origin-isolation headers (`COOP`/`COEP`) that a pthreads build
would require.

### 8. Reference and alignment

Background recipe: the companion article
`WebSitePortfolio/content/articles/cpp-to-webassembly-native-code-in-the-browser.md`
(read-only — do not edit). This plan is aligned with it: one codebase behind an
`if(EMSCRIPTEN)`/`__EMSCRIPTEN__` seam, main-loop inversion with
`-sEXIT_RUNTIME=0`, `#version 300 es` + `highp` prologue, `--preload-file`
assets, explicit mobile GPU budget. One intentional deviation: the guide ports
input/context to `emscripten_webgl_create_context` + the HTML5 event API, whereas
here we keep GLFW via `-sUSE_GLFW=3` to reuse the existing GLFW-based input and
the Dear ImGui GLFW backend (§4.3).
