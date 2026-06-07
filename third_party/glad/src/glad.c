/*
 * Minimal GLAD-compatible OpenGL 4.1 Core loader implementation.
 */
#include <glad/glad.h>
#include <stdio.h>
#include <string.h>

PFNGLCLEARPROC glad_glClear = 0;
PFNGLCLEARCOLORPROC glad_glClearColor = 0;
PFNGLVIEWPORTPROC glad_glViewport = 0;
PFNGLENABLEPROC glad_glEnable = 0;
PFNGLDISABLEPROC glad_glDisable = 0;
PFNGLBLENDFUNCPROC glad_glBlendFunc = 0;
PFNGLGETERRORPROC glad_glGetError = 0;
PFNGLGETSTRINGPROC glad_glGetString = 0;
PFNGLDRAWARRAYSPROC glad_glDrawArrays = 0;
PFNGLDRAWARRAYSINSTANCEDPROC glad_glDrawArraysInstanced = 0;
PFNGLPIXELSTOREIPROC glad_glPixelStorei = 0;
PFNGLREADPIXELSPROC glad_glReadPixels = 0;
PFNGLFINISHPROC glad_glFinish = 0;
PFNGLFLUSHPROC glad_glFlush = 0;

PFNGLGENTEXTURESPROC glad_glGenTextures = 0;
PFNGLDELETETEXTURESPROC glad_glDeleteTextures = 0;
PFNGLBINDTEXTUREPROC glad_glBindTexture = 0;
PFNGLACTIVETEXTUREPROC glad_glActiveTexture = 0;
PFNGLTEXIMAGE2DPROC glad_glTexImage2D = 0;
PFNGLTEXPARAMETERIPROC glad_glTexParameteri = 0;
PFNGLTEXSUBIMAGE2DPROC glad_glTexSubImage2D = 0;

PFNGLGENFRAMEBUFFERSPROC glad_glGenFramebuffers = 0;
PFNGLDELETEFRAMEBUFFERSPROC glad_glDeleteFramebuffers = 0;
PFNGLBINDFRAMEBUFFERPROC glad_glBindFramebuffer = 0;
PFNGLFRAMEBUFFERTEXTURE2DPROC glad_glFramebufferTexture2D = 0;
PFNGLCHECKFRAMEBUFFERSTATUSPROC glad_glCheckFramebufferStatus = 0;
PFNGLDRAWBUFFERSPROC glad_glDrawBuffers = 0;

PFNGLGENVERTEXARRAYSPROC glad_glGenVertexArrays = 0;
PFNGLDELETEVERTEXARRAYSPROC glad_glDeleteVertexArrays = 0;
PFNGLBINDVERTEXARRAYPROC glad_glBindVertexArray = 0;

PFNGLCREATESHADERPROC glad_glCreateShader = 0;
PFNGLDELETESHADERPROC glad_glDeleteShader = 0;
PFNGLSHADERSOURCEPROC glad_glShaderSource = 0;
PFNGLCOMPILESHADERPROC glad_glCompileShader = 0;
PFNGLGETSHADERIVPROC glad_glGetShaderiv = 0;
PFNGLGETSHADERINFOLOGPROC glad_glGetShaderInfoLog = 0;

PFNGLCREATEPROGRAMPROC glad_glCreateProgram = 0;
PFNGLDELETEPROGRAMPROC glad_glDeleteProgram = 0;
PFNGLATTACHSHADERPROC glad_glAttachShader = 0;
PFNGLDETACHSHADERPROC glad_glDetachShader = 0;
PFNGLLINKPROGRAMPROC glad_glLinkProgram = 0;
PFNGLUSEPROGRAMPROC glad_glUseProgram = 0;
PFNGLGETPROGRAMIVPROC glad_glGetProgramiv = 0;
PFNGLGETPROGRAMINFOLOGPROC glad_glGetProgramInfoLog = 0;
PFNGLGETUNIFORMLOCATIONPROC glad_glGetUniformLocation = 0;
PFNGLUNIFORM1IPROC glad_glUniform1i = 0;
PFNGLUNIFORM1FPROC glad_glUniform1f = 0;
PFNGLUNIFORM2FPROC glad_glUniform2f = 0;
PFNGLUNIFORM2IPROC glad_glUniform2i = 0;
PFNGLUNIFORM3FPROC glad_glUniform3f = 0;
PFNGLUNIFORM4FPROC glad_glUniform4f = 0;

PFNGLDEBUGMESSAGECALLBACKPROC glad_glDebugMessageCallback = 0;

#define LOAD(fn, type, name)                                        \
    do {                                                            \
        fn = (type)loader(name);                                    \
        if (!fn) {                                                  \
            fprintf(stderr, "glad: failed to load %s\n", name);     \
            missing++;                                              \
        }                                                           \
    } while (0)

int gladLoadGLLoader(GLADloadproc loader)
{
    int missing = 0;

    LOAD(glad_glClear, PFNGLCLEARPROC, "glClear");
    LOAD(glad_glClearColor, PFNGLCLEARCOLORPROC, "glClearColor");
    LOAD(glad_glViewport, PFNGLVIEWPORTPROC, "glViewport");
    LOAD(glad_glEnable, PFNGLENABLEPROC, "glEnable");
    LOAD(glad_glDisable, PFNGLDISABLEPROC, "glDisable");
    LOAD(glad_glBlendFunc, PFNGLBLENDFUNCPROC, "glBlendFunc");
    LOAD(glad_glGetError, PFNGLGETERRORPROC, "glGetError");
    LOAD(glad_glGetString, PFNGLGETSTRINGPROC, "glGetString");
    LOAD(glad_glDrawArrays, PFNGLDRAWARRAYSPROC, "glDrawArrays");
    LOAD(glad_glDrawArraysInstanced, PFNGLDRAWARRAYSINSTANCEDPROC, "glDrawArraysInstanced");
    LOAD(glad_glPixelStorei, PFNGLPIXELSTOREIPROC, "glPixelStorei");
    LOAD(glad_glReadPixels, PFNGLREADPIXELSPROC, "glReadPixels");
    LOAD(glad_glFinish, PFNGLFINISHPROC, "glFinish");
    LOAD(glad_glFlush, PFNGLFLUSHPROC, "glFlush");

    LOAD(glad_glGenTextures, PFNGLGENTEXTURESPROC, "glGenTextures");
    LOAD(glad_glDeleteTextures, PFNGLDELETETEXTURESPROC, "glDeleteTextures");
    LOAD(glad_glBindTexture, PFNGLBINDTEXTUREPROC, "glBindTexture");
    LOAD(glad_glActiveTexture, PFNGLACTIVETEXTUREPROC, "glActiveTexture");
    LOAD(glad_glTexImage2D, PFNGLTEXIMAGE2DPROC, "glTexImage2D");
    LOAD(glad_glTexParameteri, PFNGLTEXPARAMETERIPROC, "glTexParameteri");
    LOAD(glad_glTexSubImage2D, PFNGLTEXSUBIMAGE2DPROC, "glTexSubImage2D");

    LOAD(glad_glGenFramebuffers, PFNGLGENFRAMEBUFFERSPROC, "glGenFramebuffers");
    LOAD(glad_glDeleteFramebuffers, PFNGLDELETEFRAMEBUFFERSPROC, "glDeleteFramebuffers");
    LOAD(glad_glBindFramebuffer, PFNGLBINDFRAMEBUFFERPROC, "glBindFramebuffer");
    LOAD(glad_glFramebufferTexture2D, PFNGLFRAMEBUFFERTEXTURE2DPROC, "glFramebufferTexture2D");
    LOAD(glad_glCheckFramebufferStatus, PFNGLCHECKFRAMEBUFFERSTATUSPROC, "glCheckFramebufferStatus");
    LOAD(glad_glDrawBuffers, PFNGLDRAWBUFFERSPROC, "glDrawBuffers");

    LOAD(glad_glGenVertexArrays, PFNGLGENVERTEXARRAYSPROC, "glGenVertexArrays");
    LOAD(glad_glDeleteVertexArrays, PFNGLDELETEVERTEXARRAYSPROC, "glDeleteVertexArrays");
    LOAD(glad_glBindVertexArray, PFNGLBINDVERTEXARRAYPROC, "glBindVertexArray");

    LOAD(glad_glCreateShader, PFNGLCREATESHADERPROC, "glCreateShader");
    LOAD(glad_glDeleteShader, PFNGLDELETESHADERPROC, "glDeleteShader");
    LOAD(glad_glShaderSource, PFNGLSHADERSOURCEPROC, "glShaderSource");
    LOAD(glad_glCompileShader, PFNGLCOMPILESHADERPROC, "glCompileShader");
    LOAD(glad_glGetShaderiv, PFNGLGETSHADERIVPROC, "glGetShaderiv");
    LOAD(glad_glGetShaderInfoLog, PFNGLGETSHADERINFOLOGPROC, "glGetShaderInfoLog");

    LOAD(glad_glCreateProgram, PFNGLCREATEPROGRAMPROC, "glCreateProgram");
    LOAD(glad_glDeleteProgram, PFNGLDELETEPROGRAMPROC, "glDeleteProgram");
    LOAD(glad_glAttachShader, PFNGLATTACHSHADERPROC, "glAttachShader");
    LOAD(glad_glDetachShader, PFNGLDETACHSHADERPROC, "glDetachShader");
    LOAD(glad_glLinkProgram, PFNGLLINKPROGRAMPROC, "glLinkProgram");
    LOAD(glad_glUseProgram, PFNGLUSEPROGRAMPROC, "glUseProgram");
    LOAD(glad_glGetProgramiv, PFNGLGETPROGRAMIVPROC, "glGetProgramiv");
    LOAD(glad_glGetProgramInfoLog, PFNGLGETPROGRAMINFOLOGPROC, "glGetProgramInfoLog");
    LOAD(glad_glGetUniformLocation, PFNGLGETUNIFORMLOCATIONPROC, "glGetUniformLocation");
    LOAD(glad_glUniform1i, PFNGLUNIFORM1IPROC, "glUniform1i");
    LOAD(glad_glUniform1f, PFNGLUNIFORM1FPROC, "glUniform1f");
    LOAD(glad_glUniform2f, PFNGLUNIFORM2FPROC, "glUniform2f");
    LOAD(glad_glUniform2i, PFNGLUNIFORM2IPROC, "glUniform2i");
    LOAD(glad_glUniform3f, PFNGLUNIFORM3FPROC, "glUniform3f");
    LOAD(glad_glUniform4f, PFNGLUNIFORM4FPROC, "glUniform4f");

    /* Optional: ARB or KHR_debug. May be null on macOS 4.1 contexts. */
    glad_glDebugMessageCallback = (PFNGLDEBUGMESSAGECALLBACKPROC)loader("glDebugMessageCallback");
    if (!glad_glDebugMessageCallback) {
        glad_glDebugMessageCallback = (PFNGLDEBUGMESSAGECALLBACKPROC)loader("glDebugMessageCallbackARB");
    }

    return missing == 0;
}
