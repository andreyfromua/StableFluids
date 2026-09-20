#include "stable_fluids/platform/GlslDialect.hpp"

#include "stable_fluids/gl/GlHeaders.hpp"  // STABLE_FLUIDS_WEB

namespace stable_fluids {

const GlslDialect& GlslDialect::target() {
#if STABLE_FLUIDS_WEB
    static const GlslDialect dialect(
        "#version 300 es\n"
        "precision highp float;\n"
        "precision highp int;\n"
        "precision highp sampler2D;\n",
        "#version 300 es");
#else
    static const GlslDialect dialect(
        "#version 410 core\n",
        "#version 410 core");
#endif
    return dialect;
}

std::string GlslDialect::apply(std::string_view source) const {
    std::string body(source);

    // Skip a leading run of blank/whitespace characters, then remove a single
    // `#version ...` directive so our own `#version` lands on physical line 1.
    std::size_t i = 0;
    while (i < body.size() &&
           (body[i] == '\n' || body[i] == '\r' ||
            body[i] == ' '  || body[i] == '\t')) {
        ++i;
    }
    if (i + 8 <= body.size() && body.compare(i, 8, "#version") == 0) {
        const std::size_t eol = body.find('\n', i);
        if (eol == std::string::npos) {
            body.clear();
        } else {
            body.erase(0, eol + 1);
        }
    }

    return preamble_ + body;
}

}  // namespace stable_fluids
