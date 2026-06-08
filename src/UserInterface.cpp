#include "stable_fluids/UserInterface.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <GLFW/glfw3.h>

#include <cstdio>

namespace stable_fluids {

namespace {

constexpr const char* view_mode_names[] = {
    "Smoke",
    "Dye",
    "Velocity",
    "Projection potential",
    "Divergence",
    "Temperature",
    "Vorticity",
    "Solid mask",
    "Stress",
};

constexpr const char* stress_mode_names[] = {
    "Von Mises magnitude",
    "Normal stress sigma xx",
    "Normal stress sigma yy",
    "Shear stress sigma xy",
    "Hydrostatic pressure",
};

}  // namespace

UserInterface::UserInterface(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 6.0f;
    style.FrameRounding  = 4.0f;

    if (!ImGui_ImplGlfw_InitForOpenGL(window, true)) {
        std::fprintf(stderr, "Dear ImGui: failed to initialize GLFW backend.\n");
        ImGui::DestroyContext();
        return;
    }
    if (!ImGui_ImplOpenGL3_Init("#version 410 core")) {
        std::fprintf(stderr, "Dear ImGui: failed to initialize OpenGL backend.\n");
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        return;
    }

    initialized_ = true;
}

UserInterface::~UserInterface() {
    if (!initialized_) {
        return;
    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void UserInterface::begin_frame() {
    if (!initialized_) {
        return;
    }
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

UiActions UserInterface::draw_controls(SimulationConfig& config,
                                       bool paused,
                                       float frame_dt,
                                       int framebuffer_width,
                                       int framebuffer_height)
{
    UiActions actions;
    actions.paused = paused;
    if (!initialized_) {
        return actions;
    }

    ImGui::SetNextWindowPos(ImVec2(12.0f, 12.0f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(365.0f, 700.0f), ImGuiCond_FirstUseEver);

    ImGui::Begin("Stable Fluids Controls");

    const ImGuiIO& io = ImGui::GetIO();
    ImGui::Text("Grid: %d x %d", config.grid_width, config.grid_height);
    ImGui::Text("Framebuffer: %d x %d",
                framebuffer_width, framebuffer_height);
    ImGui::Text("Frame: %.2f ms  (%.1f FPS)",
                static_cast<double>(frame_dt * 1000.0f),
                static_cast<double>(io.Framerate));

    bool paused_value = paused;
    if (ImGui::Checkbox("Paused", &paused_value)) {
        actions.pause_changed = true;
        actions.paused = paused_value;
    }
    ImGui::SameLine();
    if (ImGui::Button("Single step")) {
        actions.single_step = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset")) {
        actions.reset = true;
    }
    if (ImGui::Button("Reload shaders")) {
        actions.reload_shaders = true;
    }

    ImGui::SeparatorText("Display");
    int view_mode = static_cast<int>(config.view_mode);
    if (ImGui::Combo("View", &view_mode, view_mode_names,
                     static_cast<int>(ViewMode::Count))) {
        config.view_mode = static_cast<ViewMode>(view_mode);
    }
    if (config.view_mode == ViewMode::Stress) {
        int stress_mode = static_cast<int>(config.stress_display_mode);
        if (ImGui::Combo("Stress quantity", &stress_mode, stress_mode_names,
                         static_cast<int>(StressDisplayMode::Count))) {
            config.stress_display_mode =
                static_cast<StressDisplayMode>(stress_mode);
        }
        ImGui::SliderFloat("Stress display scale",
                           &config.stress_display_scale,
                           0.0001f, 10.0f, "%.4f",
                           ImGuiSliderFlags_Logarithmic);
        ImGui::SliderFloat("Fluid density",
                           &config.fluid_density, 0.1f, 10.0f, "%.2f");
        ImGui::TextDisabled(
            "Signed values: blue = negative, red = positive.\n"
            "Von Mises: dark = low, orange = high.");
        if (!config.diffusion_enabled) {
            ImGui::TextDisabled(
                "Velocity diffusion is off, so viscous stress is zero.");
        }
    }
    ImGui::Checkbox("MAC velocity overlay",
                    &config.velocity_overlay_enabled);
    ImGui::BeginDisabled(!config.velocity_overlay_enabled);
    ImGui::Checkbox("Show sample grid",
                    &config.velocity_overlay_grid);
    ImGui::SliderInt("Velocity sample stride",
                     &config.velocity_overlay_stride, 1, 64,
                     "%d cells");
    ImGui::SliderFloat("Velocity arrow scale",
                       &config.velocity_overlay_scale,
                       0.005f, 2.0f, "%.3f",
                       ImGuiSliderFlags_Logarithmic);
    ImGui::TextDisabled(
        "Cyan: u on left/right faces.\n"
        "Orange: v on bottom/top faces.");
    ImGui::EndDisabled();

    ImGui::SeparatorText("Solver");
    ImGui::SliderInt("Pressure iterations",
                     &config.pressure_iterations, 1, 200);
    ImGui::SliderInt("Substeps per frame",
                     &config.substeps_per_frame, 1, 16);

    float timestep_ms = config.fixed_timestep * 1000.0f;
    if (ImGui::SliderFloat("Fixed timestep",
                           &timestep_ms, 1.0f, 33.333f, "%.2f ms")) {
        config.fixed_timestep = timestep_ms / 1000.0f;
    }

    ImGui::SeparatorText("Buoyancy");
    ImGui::Checkbox("Enable buoyancy", &config.buoyancy_enabled);
    ImGui::BeginDisabled(!config.buoyancy_enabled);
    ImGui::SliderFloat("Temperature lift",
                       &config.buoyancy_beta, 0.0f, 5.0f, "%.3f");
    ImGui::SliderFloat("Smoke weight",
                       &config.buoyancy_alpha, 0.0f, 5.0f, "%.3f");
    ImGui::SliderFloat("Ambient temperature",
                       &config.ambient_temperature, -5.0f, 5.0f, "%.2f");
    ImGui::EndDisabled();

    ImGui::SeparatorText("Diffusion");
    ImGui::Checkbox("Enable velocity diffusion", &config.diffusion_enabled);
    ImGui::BeginDisabled(!config.diffusion_enabled);
    ImGui::DragFloat("Viscosity", &config.viscosity,
                     0.0005f, 0.0f, 1.0f, "%.4f");
    ImGui::SliderInt("Diffusion iterations",
                     &config.diffusion_iterations, 1, 100);
    ImGui::EndDisabled();

    ImGui::SeparatorText("Vorticity");
    ImGui::Checkbox("Enable confinement",
                    &config.vorticity_confinement);
    ImGui::BeginDisabled(!config.vorticity_confinement);
    ImGui::SliderFloat("Confinement strength",
                       &config.vorticity_strength, 0.0f, 5.0f, "%.3f");
    ImGui::EndDisabled();

    ImGui::SeparatorText("Sources");
    ImGui::SliderFloat("Emitter radius",
                       &config.emitter_radius_cells, 1.0f, 64.0f, "%.1f cells");
    ImGui::SliderFloat("Smoke rate",
                       &config.smoke_injection_rate, 0.0f, 20.0f, "%.2f");
    ImGui::SliderFloat("Dye rate",
                       &config.dye_injection_rate, 0.0f, 20.0f, "%.2f");
    ImGui::SliderFloat("Temperature rate",
                       &config.temperature_injection_rate, 0.0f, 20.0f, "%.2f");
    ImGui::SliderFloat("Momentum scale",
                       &config.momentum_injection_scale, 0.0f, 10.0f, "%.2f");

    ImGui::SeparatorText("Scalar decay");
    ImGui::SliderFloat("Smoke dissipation",
                       &config.smoke_dissipation_rate, 0.0f, 5.0f, "%.3f / s");
    ImGui::SliderFloat("Temperature relaxation",
                       &config.temperature_dissipation, 0.0f, 5.0f, "%.3f / s");

    ImGui::Separator();
    ImGui::TextDisabled(
        "Mouse: left emits smoke/dye, right emits heat.\n"
        "Keyboard shortcuts work while the UI is not focused.");

    ImGui::End();
    return actions;
}

void UserInterface::render() {
    if (!initialized_) {
        return;
    }
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

bool UserInterface::wants_capture_mouse() const {
    return initialized_ && ImGui::GetIO().WantCaptureMouse;
}

bool UserInterface::wants_capture_keyboard() const {
    return initialized_ && ImGui::GetIO().WantCaptureKeyboard;
}

}  // namespace stable_fluids
