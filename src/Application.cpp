#include "stable_fluids/Application.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <chrono>
#include <cstdio>
#include <cstring>

#ifndef STABLE_FLUIDS_SHADER_DIR
#  define STABLE_FLUIDS_SHADER_DIR "shaders"
#endif

namespace stable_fluids {

namespace {

void glfw_error_callback(int code, const char* description) {
    std::fprintf(stderr, "GLFW error %d: %s\n", code, description);
}

#ifdef GL_KHR_debug
void APIENTRY gl_debug_callback(GLenum, GLenum type, GLuint id, GLenum severity,
                                GLsizei, const GLchar* message, const void*) {
    if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) return;
    std::fprintf(stderr, "[GL %u/%u/%u] %s\n", type, id, severity, message);
}
#endif

}  // namespace

Application::Application(SimulationConfig config) : config_(std::move(config)) {}

Application::~Application() {
    // Subsystems are destroyed before the GL context.
    input_.reset();
    renderer_.reset();
    solver_.reset();
    grid_.reset();

    if (window_) {
        glfwDestroyWindow(window_);
        window_ = nullptr;
    }
    glfwTerminate();
}

bool Application::initialize_glfw() {
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) {
        std::fprintf(stderr, "Failed to initialize GLFW.\n");
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE,        GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#ifndef NDEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_FALSE);

    window_ = glfwCreateWindow(config_.window_width, config_.window_height,
                               "Stable Fluids (OpenGL 4.1)", nullptr, nullptr);
    if (!window_) {
        std::fprintf(stderr, "Failed to create GLFW window.\n");
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(window_);
    glfwSwapInterval(1);  // vsync

    glfwSetWindowUserPointer(window_, this);

    glfwSetFramebufferSizeCallback(window_, [](GLFWwindow* w, int width, int height) {
        auto* app = static_cast<Application*>(glfwGetWindowUserPointer(w));
        if (app) app->on_framebuffer_size(width, height);
    });
    glfwSetKeyCallback(window_, [](GLFWwindow* w, int key, int sc, int action, int mods) {
        (void)sc;
        auto* app = static_cast<Application*>(glfwGetWindowUserPointer(w));
        if (app && app->input_) app->input_->on_key(key, action, mods);
    });
    glfwSetMouseButtonCallback(window_, [](GLFWwindow* w, int button, int action, int mods) {
        auto* app = static_cast<Application*>(glfwGetWindowUserPointer(w));
        if (app && app->input_) app->input_->on_mouse_button(button, action, mods);
    });
    glfwSetCursorPosCallback(window_, [](GLFWwindow* w, double x, double y) {
        auto* app = static_cast<Application*>(glfwGetWindowUserPointer(w));
        if (app && app->input_) app->input_->on_cursor(x, y);
    });

    glfwGetFramebufferSize(window_, &fb_width_, &fb_height_);
    return true;
}

bool Application::initialize_gl() {
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::fprintf(stderr, "Failed to load OpenGL functions through GLAD.\n");
        return false;
    }

#ifdef GL_KHR_debug
    if (glDebugMessageCallback) {
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(gl_debug_callback, nullptr);
    }
#endif
    return true;
}

void Application::create_subsystems() {
    grid_     = std::make_unique<FluidGrid>(config_.grid_width, config_.grid_height);
    solver_   = std::make_unique<FluidSolver>(*grid_, config_, STABLE_FLUIDS_SHADER_DIR);
    renderer_ = std::make_unique<Renderer>(STABLE_FLUIDS_SHADER_DIR);
    input_    = std::make_unique<InputController>(window_, config_, *this);
}

void Application::on_framebuffer_size(int width, int height) {
    fb_width_  = width;
    fb_height_ = height;
}

void Application::main_loop() {
    using clock = std::chrono::steady_clock;
    auto t_prev = clock::now();

    int    frames_since_title = 0;
    double seconds_since_title = 0.0;

    while (!glfwWindowShouldClose(window_)) {
        glfwPollEvents();

        if (request_shader_reload_) {
            std::fputs("Reloading shaders...\n", stdout);
            solver_->reload_shaders();
            renderer_->reload_shaders();
            request_shader_reload_ = false;
        }
        if (request_reset_) {
            grid_->reset();
            request_reset_ = false;
        }

        auto   t_now = clock::now();
        float  frame_dt = std::chrono::duration<float>(t_now - t_prev).count();
        t_prev = t_now;

        if (frame_dt > 0.1f) frame_dt = 0.1f;  // clamp after pauses

        input_->update(frame_dt);

        const bool should_step = !input_->paused() || input_->step_one();
        if (should_step) {
            input_->consume_step_one();
            // Solver substep used by the plan: fixed dt for stability of the
            // baseline. Adaptive Courant is an explicit later milestone.
            float dt   = (config_.fixed_timestep > 0.0f)
                           ? config_.fixed_timestep
                           : frame_dt;
            int   subs = std::max(1, config_.substeps_per_frame);
            float h    = dt / float(subs);

            // Solver picks up the latest config every step (cheap).
            solver_->config() = config_;

            EmitterPulse pulse = input_->make_emitter();
            for (int s = 0; s < subs; ++s) {
                solver_->step(h, pulse);
            }
        }

        // Diagnostic computations only for currently-visible views.
        switch (config_.view_mode) {
            case ViewMode::Vorticity:   solver_->compute_vorticity();           break;
            case ViewMode::Divergence:  solver_->compute_divergence_into_grid(); break;
            default: break;
        }

        renderer_->render(*grid_, config_.view_mode,
                          fb_width_, fb_height_, config_);
        glfwSwapBuffers(window_);

        ++frames_since_title;
        seconds_since_title += frame_dt;
        if (seconds_since_title >= 0.5) {
            char title[256];
            double fps = frames_since_title / seconds_since_title;
            std::snprintf(title, sizeof(title),
                          "Stable Fluids %dx%d  |  %.1f fps  |  P=%d  |  %s%s%s",
                          config_.grid_width, config_.grid_height,
                          fps, config_.pressure_iterations,
                          config_.buoyancy_enabled       ? "B " : "",
                          config_.diffusion_enabled      ? "D " : "",
                          config_.vorticity_confinement  ? "V " : "");
            glfwSetWindowTitle(window_, title);
            frames_since_title  = 0;
            seconds_since_title = 0.0;
        }
    }
}

int Application::run() {
    if (!initialize_glfw()) return 1;
    if (!initialize_gl())   return 2;
    create_subsystems();
    main_loop();
    return 0;
}

}  // namespace stable_fluids
