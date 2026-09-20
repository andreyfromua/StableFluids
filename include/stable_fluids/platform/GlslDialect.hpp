#pragma once

#include <string>
#include <string_view>

namespace stable_fluids {

/// Encapsulates the GLSL dialect differences between the desktop target
/// (OpenGL 4.1 Core, GLSL 4.10) and the browser target (WebGL 2.0, GLSL ES
/// 3.00). A Strategy selected once per build target.
///
/// Shaders in the repository are authored with a single `#version 410 core`
/// line. At load time the dialect rewrites that line into the header the
/// current target needs: on the web that is `#version 300 es` followed by the
/// mandatory default precision qualifiers. GLES requires `#version` on physical
/// line 1, so the rewrite strips the original directive and prepends its own —
/// never leaving a blank line in front of `#version`.
class GlslDialect {
public:
    /// The dialect selected for the current build target.
    static const GlslDialect& target();

    /// Header prepended to every shader. Ends with a newline and begins with
    /// `#version` so it is valid as physical line 1.
    const std::string& preamble() const { return preamble_; }

    /// Version string Dear ImGui's OpenGL3 backend must be initialized with.
    const char* imgui_glsl_version() const { return imgui_version_; }

    /// Rewrite one (already include-expanded) shader source: drop a leading
    /// `#version ...` line if present, then prepend `preamble()`.
    std::string apply(std::string_view source) const;

private:
    GlslDialect(std::string preamble, const char* imgui_version)
        : preamble_(std::move(preamble)), imgui_version_(imgui_version) {}

    std::string preamble_;
    const char* imgui_version_;
};

}  // namespace stable_fluids
