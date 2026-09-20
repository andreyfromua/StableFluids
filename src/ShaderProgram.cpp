#include "stable_fluids/ShaderProgram.hpp"

#include "stable_fluids/platform/GlslDialect.hpp"

#include <cstdio>
#include <fstream>
#include <set>
#include <sstream>
#include <utility>
#include <vector>

namespace stable_fluids {

namespace {

// Read a file in its entirety.
bool read_file(const std::string& path, std::string& out) {
    std::ifstream is(path);
    if (!is.is_open()) {
        std::fprintf(stderr, "Could not open shader file: %s\n", path.c_str());
        return false;
    }
    std::ostringstream ss;
    ss << is.rdbuf();
    out = ss.str();
    return true;
}

// Expand `#include "name"` directives recursively.  Cycles are broken by
// remembering which files have already been pulled in.
bool expand_includes(const std::string& shader_dir,
                     const std::string& current_file,
                     std::set<std::string>& visited,
                     std::string& out)
{
    const std::string path = shader_dir + "/" + current_file;
    if (!visited.insert(path).second) {
        return true;  // already included
    }

    std::string source;
    if (!read_file(path, source)) {
        return false;
    }

    std::istringstream is(source);
    std::string line;
    while (std::getline(is, line)) {
        // Skip whitespace.
        std::size_t i = 0;
        while (i < line.size() && (line[i] == ' ' || line[i] == '\t')) ++i;
        if (i + 8 <= line.size() && line.compare(i, 8, "#include") == 0) {
            // find quote
            const auto qopen = line.find_first_of("\"<", i + 8);
            if (qopen == std::string::npos) { out += line; out += '\n'; continue; }
            const char qend = (line[qopen] == '"') ? '"' : '>';
            const auto qclose = line.find(qend, qopen + 1);
            if (qclose == std::string::npos) { out += line; out += '\n'; continue; }
            const std::string include_name = line.substr(qopen + 1, qclose - qopen - 1);
            out += "// === include " + include_name + " ===\n";
            if (!expand_includes(shader_dir, include_name, visited, out)) {
                return false;
            }
            out += "// === end include " + include_name + " ===\n";
        } else {
            out += line;
            out += '\n';
        }
    }
    return true;
}

GLuint compile_shader(GLenum stage, const std::string& source, const std::string& label) {
    GLuint shader = glCreateShader(stage);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint ok = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (ok == GL_FALSE) {
        GLint log_len = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_len);
        std::vector<char> log(static_cast<std::size_t>(log_len > 1 ? log_len : 1), '\0');
        glGetShaderInfoLog(shader, log_len, nullptr, log.data());
        std::fprintf(stderr,
                     "Shader compilation failed (%s):\n%s\n",
                     label.c_str(), log.data());
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

}  // namespace

ShaderProgram::~ShaderProgram() {
    release();
}

ShaderProgram::ShaderProgram(ShaderProgram&& other) noexcept
    : id_(other.id_), uniform_cache_(std::move(other.uniform_cache_)) {
    other.id_ = 0;
}

ShaderProgram& ShaderProgram::operator=(ShaderProgram&& other) noexcept {
    if (this != &other) {
        release();
        id_              = other.id_;
        uniform_cache_   = std::move(other.uniform_cache_);
        other.id_        = 0;
    }
    return *this;
}

void ShaderProgram::release() noexcept {
    if (id_ != 0) {
        glDeleteProgram(id_);
        id_ = 0;
    }
    uniform_cache_.clear();
}

bool ShaderProgram::load(const std::string& shader_dir,
                         const std::string& vertex_file,
                         const std::string& fragment_file)
{
    std::set<std::string> visited_v, visited_f;
    std::string vs_src, fs_src;
    if (!expand_includes(shader_dir, vertex_file, visited_v, vs_src)) return false;
    if (!expand_includes(shader_dir, fragment_file, visited_f, fs_src)) return false;

    // Rewrite the authored `#version 410 core` line into the current target's
    // dialect (GLSL 4.10 on desktop, GLSL ES 3.00 on the web) before compiling.
    const GlslDialect& dialect = GlslDialect::target();
    vs_src = dialect.apply(vs_src);
    fs_src = dialect.apply(fs_src);

    GLuint vs = compile_shader(GL_VERTEX_SHADER,   vs_src, vertex_file);
    if (vs == 0) return false;
    GLuint fs = compile_shader(GL_FRAGMENT_SHADER, fs_src, fragment_file);
    if (fs == 0) { glDeleteShader(vs); return false; }

    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    GLint ok = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &ok);
    if (ok == GL_FALSE) {
        GLint log_len = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_len);
        std::vector<char> log(static_cast<std::size_t>(log_len > 1 ? log_len : 1), '\0');
        glGetProgramInfoLog(program, log_len, nullptr, log.data());
        std::fprintf(stderr,
                     "Program link failed (%s / %s):\n%s\n",
                     vertex_file.c_str(), fragment_file.c_str(), log.data());
        glDeleteShader(vs);
        glDeleteShader(fs);
        glDeleteProgram(program);
        return false;
    }

    glDetachShader(program, vs);
    glDetachShader(program, fs);
    glDeleteShader(vs);
    glDeleteShader(fs);

    release();
    id_ = program;
    return true;
}

void ShaderProgram::use() const {
    glUseProgram(id_);
}

GLint ShaderProgram::uniform(std::string_view name) const {
    auto key = std::string(name);
    auto it = uniform_cache_.find(key);
    if (it != uniform_cache_.end()) {
        return it->second;
    }
    const GLint loc = glGetUniformLocation(id_, key.c_str());
    uniform_cache_.emplace(std::move(key), loc);
    return loc;
}

void ShaderProgram::set_int(std::string_view name, int value) const {
    const GLint loc = uniform(name);
    if (loc >= 0) glUniform1i(loc, value);
}
void ShaderProgram::set_float(std::string_view name, float value) const {
    const GLint loc = uniform(name);
    if (loc >= 0) glUniform1f(loc, value);
}
void ShaderProgram::set_vec2(std::string_view name, float x, float y) const {
    const GLint loc = uniform(name);
    if (loc >= 0) glUniform2f(loc, x, y);
}
void ShaderProgram::set_ivec2(std::string_view name, int x, int y) const {
    const GLint loc = uniform(name);
    if (loc >= 0) glUniform2i(loc, x, y);
}
void ShaderProgram::set_vec3(std::string_view name, float x, float y, float z) const {
    const GLint loc = uniform(name);
    if (loc >= 0) glUniform3f(loc, x, y, z);
}
void ShaderProgram::set_vec4(std::string_view name, float x, float y, float z, float w) const {
    const GLint loc = uniform(name);
    if (loc >= 0) glUniform4f(loc, x, y, z, w);
}

}  // namespace stable_fluids
