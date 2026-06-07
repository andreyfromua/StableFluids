/*
 * Minimal GLAD-compatible OpenGL 4.1 Core loader.
 *
 * This is a hand-rolled, small subset of what dav1dde/glad would generate.
 * It defines the types, enum constants, and function pointers required by
 * the Stable Fluids implementation. The function pointers are resolved at
 * runtime by `gladLoadGLLoader`, which receives a function-address loader
 * such as `glfwGetProcAddress`.
 */
#ifndef __glad_h_
#define __glad_h_

#ifdef __gl_h_
#error OpenGL header already included, remove this include, glad already provides it
#endif
#define __gl_h_

#include <KHR/khrplatform.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Basic GL typedefs (subset). */
typedef unsigned int   GLenum;
typedef unsigned char  GLboolean;
typedef unsigned int   GLbitfield;
typedef void           GLvoid;
typedef khronos_int8_t GLbyte;
typedef khronos_uint8_t GLubyte;
typedef khronos_int16_t GLshort;
typedef khronos_uint16_t GLushort;
typedef int            GLint;
typedef unsigned int   GLuint;
typedef khronos_int32_t GLclampx;
typedef int            GLsizei;
typedef khronos_float_t GLfloat;
typedef khronos_float_t GLclampf;
typedef double         GLdouble;
typedef double         GLclampd;
typedef char           GLchar;
typedef khronos_ssize_t GLsizeiptr;
typedef khronos_intptr_t GLintptr;
typedef khronos_uint64_t GLuint64;
typedef khronos_int64_t  GLint64;
typedef struct __GLsync *GLsync;

/* Loader signature. */
typedef void* (*GLADloadproc)(const char *name);

/* ------------------------------------------------------------------------- */
/* Constants                                                                  */
/* ------------------------------------------------------------------------- */
#define GL_FALSE 0
#define GL_TRUE  1

#define GL_NO_ERROR 0
#define GL_INVALID_ENUM 0x0500
#define GL_INVALID_VALUE 0x0501
#define GL_INVALID_OPERATION 0x0502
#define GL_OUT_OF_MEMORY 0x0505
#define GL_INVALID_FRAMEBUFFER_OPERATION 0x0506

#define GL_COLOR_BUFFER_BIT 0x00004000
#define GL_DEPTH_BUFFER_BIT 0x00000100

#define GL_POINTS         0x0000
#define GL_LINES          0x0001
#define GL_LINE_STRIP     0x0003
#define GL_TRIANGLES      0x0004
#define GL_TRIANGLE_STRIP 0x0005

#define GL_DEPTH_TEST     0x0B71
#define GL_BLEND          0x0BE2
#define GL_CULL_FACE      0x0B44
#define GL_SCISSOR_TEST   0x0C11
#define GL_DITHER         0x0BD0
#define GL_MULTISAMPLE    0x809D

#define GL_SRC_ALPHA            0x0302
#define GL_ONE_MINUS_SRC_ALPHA  0x0303
#define GL_ONE                  1

/* Textures */
#define GL_TEXTURE_2D            0x0DE1
#define GL_TEXTURE_MAG_FILTER    0x2800
#define GL_TEXTURE_MIN_FILTER    0x2801
#define GL_TEXTURE_WRAP_S        0x2802
#define GL_TEXTURE_WRAP_T        0x2803
#define GL_NEAREST               0x2600
#define GL_LINEAR                0x2601
#define GL_CLAMP_TO_EDGE         0x812F
#define GL_REPEAT                0x2901

/* Internal formats and pixel formats */
#define GL_RED                   0x1903
#define GL_RG                    0x8227
#define GL_RGB                   0x1907
#define GL_RGBA                  0x1908
#define GL_RED_INTEGER           0x8D94
#define GL_R8                    0x8229
#define GL_R8UI                  0x8232
#define GL_R16F                  0x822D
#define GL_R32F                  0x822E
#define GL_RG16F                 0x822F
#define GL_RG32F                 0x8230
#define GL_RGB16F                0x881B
#define GL_RGBA16F               0x881A
#define GL_RGBA32F               0x8814
#define GL_RGBA8                 0x8058
#define GL_RGB8                  0x8051

#define GL_UNSIGNED_BYTE  0x1401
#define GL_BYTE           0x1400
#define GL_UNSIGNED_SHORT 0x1403
#define GL_SHORT          0x1402
#define GL_UNSIGNED_INT   0x1405
#define GL_INT            0x1404
#define GL_FLOAT          0x1406
#define GL_HALF_FLOAT     0x140B

/* Texture units */
#define GL_TEXTURE0       0x84C0

/* Framebuffer */
#define GL_FRAMEBUFFER              0x8D40
#define GL_DRAW_FRAMEBUFFER         0x8CA9
#define GL_READ_FRAMEBUFFER         0x8CA8
#define GL_COLOR_ATTACHMENT0        0x8CE0
#define GL_DEPTH_ATTACHMENT         0x8D00
#define GL_FRAMEBUFFER_COMPLETE     0x8CD5

/* Shaders */
#define GL_VERTEX_SHADER     0x8B31
#define GL_FRAGMENT_SHADER   0x8B30
#define GL_COMPILE_STATUS    0x8B81
#define GL_LINK_STATUS       0x8B82
#define GL_INFO_LOG_LENGTH   0x8B84

/* Pixel store */
#define GL_UNPACK_ALIGNMENT  0x0CF5
#define GL_PACK_ALIGNMENT    0x0D05

/* Debug output (KHR_debug, core in 4.3 but available as extension elsewhere). */
#define GL_DEBUG_OUTPUT             0x92E0
#define GL_DEBUG_OUTPUT_SYNCHRONOUS 0x8242

/* ------------------------------------------------------------------------- */
/* Function pointer declarations                                              */
/* ------------------------------------------------------------------------- */
#define GLAPIENTRY KHRONOS_APIENTRY

typedef void (GLAPIENTRY *PFNGLCLEARPROC)(GLbitfield mask);
typedef void (GLAPIENTRY *PFNGLCLEARCOLORPROC)(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
typedef void (GLAPIENTRY *PFNGLVIEWPORTPROC)(GLint x, GLint y, GLsizei w, GLsizei h);
typedef void (GLAPIENTRY *PFNGLENABLEPROC)(GLenum cap);
typedef void (GLAPIENTRY *PFNGLDISABLEPROC)(GLenum cap);
typedef void (GLAPIENTRY *PFNGLBLENDFUNCPROC)(GLenum sfactor, GLenum dfactor);
typedef GLenum (GLAPIENTRY *PFNGLGETERRORPROC)(void);
typedef const GLubyte * (GLAPIENTRY *PFNGLGETSTRINGPROC)(GLenum name);
typedef void (GLAPIENTRY *PFNGLDRAWARRAYSPROC)(GLenum mode, GLint first, GLsizei count);
typedef void (GLAPIENTRY *PFNGLDRAWARRAYSINSTANCEDPROC)(GLenum mode, GLint first, GLsizei count, GLsizei instancecount);
typedef void (GLAPIENTRY *PFNGLPIXELSTOREIPROC)(GLenum pname, GLint param);
typedef void (GLAPIENTRY *PFNGLREADPIXELSPROC)(GLint x, GLint y, GLsizei w, GLsizei h, GLenum format, GLenum type, void *data);
typedef void (GLAPIENTRY *PFNGLFINISHPROC)(void);
typedef void (GLAPIENTRY *PFNGLFLUSHPROC)(void);

/* Textures */
typedef void (GLAPIENTRY *PFNGLGENTEXTURESPROC)(GLsizei n, GLuint *textures);
typedef void (GLAPIENTRY *PFNGLDELETETEXTURESPROC)(GLsizei n, const GLuint *textures);
typedef void (GLAPIENTRY *PFNGLBINDTEXTUREPROC)(GLenum target, GLuint texture);
typedef void (GLAPIENTRY *PFNGLACTIVETEXTUREPROC)(GLenum texture);
typedef void (GLAPIENTRY *PFNGLTEXIMAGE2DPROC)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels);
typedef void (GLAPIENTRY *PFNGLTEXPARAMETERIPROC)(GLenum target, GLenum pname, GLint param);
typedef void (GLAPIENTRY *PFNGLTEXSUBIMAGE2DPROC)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels);

/* Framebuffers */
typedef void (GLAPIENTRY *PFNGLGENFRAMEBUFFERSPROC)(GLsizei n, GLuint *fbos);
typedef void (GLAPIENTRY *PFNGLDELETEFRAMEBUFFERSPROC)(GLsizei n, const GLuint *fbos);
typedef void (GLAPIENTRY *PFNGLBINDFRAMEBUFFERPROC)(GLenum target, GLuint fbo);
typedef void (GLAPIENTRY *PFNGLFRAMEBUFFERTEXTURE2DPROC)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
typedef GLenum (GLAPIENTRY *PFNGLCHECKFRAMEBUFFERSTATUSPROC)(GLenum target);
typedef void (GLAPIENTRY *PFNGLDRAWBUFFERSPROC)(GLsizei n, const GLenum *bufs);

/* Vertex arrays / buffers */
typedef void (GLAPIENTRY *PFNGLGENVERTEXARRAYSPROC)(GLsizei n, GLuint *arrays);
typedef void (GLAPIENTRY *PFNGLDELETEVERTEXARRAYSPROC)(GLsizei n, const GLuint *arrays);
typedef void (GLAPIENTRY *PFNGLBINDVERTEXARRAYPROC)(GLuint array);

/* Shaders */
typedef GLuint (GLAPIENTRY *PFNGLCREATESHADERPROC)(GLenum type);
typedef void (GLAPIENTRY *PFNGLDELETESHADERPROC)(GLuint shader);
typedef void (GLAPIENTRY *PFNGLSHADERSOURCEPROC)(GLuint shader, GLsizei count, const GLchar *const *string, const GLint *length);
typedef void (GLAPIENTRY *PFNGLCOMPILESHADERPROC)(GLuint shader);
typedef void (GLAPIENTRY *PFNGLGETSHADERIVPROC)(GLuint shader, GLenum pname, GLint *params);
typedef void (GLAPIENTRY *PFNGLGETSHADERINFOLOGPROC)(GLuint shader, GLsizei maxLength, GLsizei *length, GLchar *log);

/* Programs */
typedef GLuint (GLAPIENTRY *PFNGLCREATEPROGRAMPROC)(void);
typedef void (GLAPIENTRY *PFNGLDELETEPROGRAMPROC)(GLuint program);
typedef void (GLAPIENTRY *PFNGLATTACHSHADERPROC)(GLuint program, GLuint shader);
typedef void (GLAPIENTRY *PFNGLDETACHSHADERPROC)(GLuint program, GLuint shader);
typedef void (GLAPIENTRY *PFNGLLINKPROGRAMPROC)(GLuint program);
typedef void (GLAPIENTRY *PFNGLUSEPROGRAMPROC)(GLuint program);
typedef void (GLAPIENTRY *PFNGLGETPROGRAMIVPROC)(GLuint program, GLenum pname, GLint *params);
typedef void (GLAPIENTRY *PFNGLGETPROGRAMINFOLOGPROC)(GLuint program, GLsizei maxLength, GLsizei *length, GLchar *log);
typedef GLint (GLAPIENTRY *PFNGLGETUNIFORMLOCATIONPROC)(GLuint program, const GLchar *name);
typedef void (GLAPIENTRY *PFNGLUNIFORM1IPROC)(GLint location, GLint v0);
typedef void (GLAPIENTRY *PFNGLUNIFORM1FPROC)(GLint location, GLfloat v0);
typedef void (GLAPIENTRY *PFNGLUNIFORM2FPROC)(GLint location, GLfloat v0, GLfloat v1);
typedef void (GLAPIENTRY *PFNGLUNIFORM2IPROC)(GLint location, GLint v0, GLint v1);
typedef void (GLAPIENTRY *PFNGLUNIFORM3FPROC)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
typedef void (GLAPIENTRY *PFNGLUNIFORM4FPROC)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);

/* Debug callback */
typedef void (GLAPIENTRY *GLDEBUGPROC)(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam);
typedef void (GLAPIENTRY *PFNGLDEBUGMESSAGECALLBACKPROC)(GLDEBUGPROC callback, const void *userParam);

/* ------------------------------------------------------------------------- */
/* Externally visible function pointers                                       */
/* ------------------------------------------------------------------------- */
extern PFNGLCLEARPROC glad_glClear;
extern PFNGLCLEARCOLORPROC glad_glClearColor;
extern PFNGLVIEWPORTPROC glad_glViewport;
extern PFNGLENABLEPROC glad_glEnable;
extern PFNGLDISABLEPROC glad_glDisable;
extern PFNGLBLENDFUNCPROC glad_glBlendFunc;
extern PFNGLGETERRORPROC glad_glGetError;
extern PFNGLGETSTRINGPROC glad_glGetString;
extern PFNGLDRAWARRAYSPROC glad_glDrawArrays;
extern PFNGLDRAWARRAYSINSTANCEDPROC glad_glDrawArraysInstanced;
extern PFNGLPIXELSTOREIPROC glad_glPixelStorei;
extern PFNGLREADPIXELSPROC glad_glReadPixels;
extern PFNGLFINISHPROC glad_glFinish;
extern PFNGLFLUSHPROC glad_glFlush;

extern PFNGLGENTEXTURESPROC glad_glGenTextures;
extern PFNGLDELETETEXTURESPROC glad_glDeleteTextures;
extern PFNGLBINDTEXTUREPROC glad_glBindTexture;
extern PFNGLACTIVETEXTUREPROC glad_glActiveTexture;
extern PFNGLTEXIMAGE2DPROC glad_glTexImage2D;
extern PFNGLTEXPARAMETERIPROC glad_glTexParameteri;
extern PFNGLTEXSUBIMAGE2DPROC glad_glTexSubImage2D;

extern PFNGLGENFRAMEBUFFERSPROC glad_glGenFramebuffers;
extern PFNGLDELETEFRAMEBUFFERSPROC glad_glDeleteFramebuffers;
extern PFNGLBINDFRAMEBUFFERPROC glad_glBindFramebuffer;
extern PFNGLFRAMEBUFFERTEXTURE2DPROC glad_glFramebufferTexture2D;
extern PFNGLCHECKFRAMEBUFFERSTATUSPROC glad_glCheckFramebufferStatus;
extern PFNGLDRAWBUFFERSPROC glad_glDrawBuffers;

extern PFNGLGENVERTEXARRAYSPROC glad_glGenVertexArrays;
extern PFNGLDELETEVERTEXARRAYSPROC glad_glDeleteVertexArrays;
extern PFNGLBINDVERTEXARRAYPROC glad_glBindVertexArray;

extern PFNGLCREATESHADERPROC glad_glCreateShader;
extern PFNGLDELETESHADERPROC glad_glDeleteShader;
extern PFNGLSHADERSOURCEPROC glad_glShaderSource;
extern PFNGLCOMPILESHADERPROC glad_glCompileShader;
extern PFNGLGETSHADERIVPROC glad_glGetShaderiv;
extern PFNGLGETSHADERINFOLOGPROC glad_glGetShaderInfoLog;

extern PFNGLCREATEPROGRAMPROC glad_glCreateProgram;
extern PFNGLDELETEPROGRAMPROC glad_glDeleteProgram;
extern PFNGLATTACHSHADERPROC glad_glAttachShader;
extern PFNGLDETACHSHADERPROC glad_glDetachShader;
extern PFNGLLINKPROGRAMPROC glad_glLinkProgram;
extern PFNGLUSEPROGRAMPROC glad_glUseProgram;
extern PFNGLGETPROGRAMIVPROC glad_glGetProgramiv;
extern PFNGLGETPROGRAMINFOLOGPROC glad_glGetProgramInfoLog;
extern PFNGLGETUNIFORMLOCATIONPROC glad_glGetUniformLocation;
extern PFNGLUNIFORM1IPROC glad_glUniform1i;
extern PFNGLUNIFORM1FPROC glad_glUniform1f;
extern PFNGLUNIFORM2FPROC glad_glUniform2f;
extern PFNGLUNIFORM2IPROC glad_glUniform2i;
extern PFNGLUNIFORM3FPROC glad_glUniform3f;
extern PFNGLUNIFORM4FPROC glad_glUniform4f;

extern PFNGLDEBUGMESSAGECALLBACKPROC glad_glDebugMessageCallback;

/* Convenience macros so user code reads like real GL. */
#define glClear glad_glClear
#define glClearColor glad_glClearColor
#define glViewport glad_glViewport
#define glEnable glad_glEnable
#define glDisable glad_glDisable
#define glBlendFunc glad_glBlendFunc
#define glGetError glad_glGetError
#define glGetString glad_glGetString
#define glDrawArrays glad_glDrawArrays
#define glDrawArraysInstanced glad_glDrawArraysInstanced
#define glPixelStorei glad_glPixelStorei
#define glReadPixels glad_glReadPixels
#define glFinish glad_glFinish
#define glFlush glad_glFlush

#define glGenTextures glad_glGenTextures
#define glDeleteTextures glad_glDeleteTextures
#define glBindTexture glad_glBindTexture
#define glActiveTexture glad_glActiveTexture
#define glTexImage2D glad_glTexImage2D
#define glTexParameteri glad_glTexParameteri
#define glTexSubImage2D glad_glTexSubImage2D

#define glGenFramebuffers glad_glGenFramebuffers
#define glDeleteFramebuffers glad_glDeleteFramebuffers
#define glBindFramebuffer glad_glBindFramebuffer
#define glFramebufferTexture2D glad_glFramebufferTexture2D
#define glCheckFramebufferStatus glad_glCheckFramebufferStatus
#define glDrawBuffers glad_glDrawBuffers

#define glGenVertexArrays glad_glGenVertexArrays
#define glDeleteVertexArrays glad_glDeleteVertexArrays
#define glBindVertexArray glad_glBindVertexArray

#define glCreateShader glad_glCreateShader
#define glDeleteShader glad_glDeleteShader
#define glShaderSource glad_glShaderSource
#define glCompileShader glad_glCompileShader
#define glGetShaderiv glad_glGetShaderiv
#define glGetShaderInfoLog glad_glGetShaderInfoLog

#define glCreateProgram glad_glCreateProgram
#define glDeleteProgram glad_glDeleteProgram
#define glAttachShader glad_glAttachShader
#define glDetachShader glad_glDetachShader
#define glLinkProgram glad_glLinkProgram
#define glUseProgram glad_glUseProgram
#define glGetProgramiv glad_glGetProgramiv
#define glGetProgramInfoLog glad_glGetProgramInfoLog
#define glGetUniformLocation glad_glGetUniformLocation
#define glUniform1i glad_glUniform1i
#define glUniform1f glad_glUniform1f
#define glUniform2f glad_glUniform2f
#define glUniform2i glad_glUniform2i
#define glUniform3f glad_glUniform3f
#define glUniform4f glad_glUniform4f

#define glDebugMessageCallback glad_glDebugMessageCallback

/* Loader function: returns 1 on success, 0 on failure (mandatory functions missing). */
int gladLoadGLLoader(GLADloadproc loader);

#ifdef __cplusplus
}
#endif

#endif /* __glad_h_ */
