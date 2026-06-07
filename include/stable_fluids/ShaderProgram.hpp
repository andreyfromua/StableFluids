#pragma once

#include <glad/glad.h>

#include <string>
#include <string_view>
#include <unordered_map>

namespace stable_fluids {

/// A minimal shader-program wrapper that supports project-local
/// `#include "name.glsl"` preprocessing for the shader loader.
class ShaderProgram {
public:
    ShaderProgram() = default;
    ~ShaderProgram();

    ShaderProgram(const ShaderProgram&)            = delete;
    ShaderProgram& operator=(const ShaderProgram&) = delete;
    ShaderProgram(ShaderProgram&& other) noexcept;
    ShaderProgram& operator=(ShaderProgram&& other) noexcept;

    /// Load and compile a vertex and fragment shader pair from disk.
    /// `shader_dir` is the directory under which the file names are resolved.
    /// Returns true on success; on failure, the program is left in its
    /// previous state and the error log is written to stderr.
    bool load(const std::string& shader_dir,
              const std::string& vertex_file,
              const std::string& fragment_file);

    /// Bind the program. Becomes the active program for subsequent draw calls.
    void use() const;

    /// Look up (and cache) a uniform location by name. Returns -1 if unknown.
    GLint uniform(std::string_view name) const;

    /// Convenience setters; -1 locations are silently ignored.
    void set_int  (std::string_view name, int   value)  const;
    void set_float(std::string_view name, float value)  const;
    void set_vec2 (std::string_view name, float x, float y) const;
    void set_ivec2(std::string_view name, int x, int y) const;
    void set_vec3 (std::string_view name, float x, float y, float z) const;
    void set_vec4 (std::string_view name, float x, float y, float z, float w) const;

    GLuint id() const noexcept { return id_; }
    bool   is_valid() const noexcept { return id_ != 0; }

private:
    void release() noexcept;

    GLuint id_ = 0;
    // Cache uniform locations to avoid repeated glGetUniformLocation lookups.
    mutable std::unordered_map<std::string, GLint> uniform_cache_;
};

}  // namespace stable_fluids
