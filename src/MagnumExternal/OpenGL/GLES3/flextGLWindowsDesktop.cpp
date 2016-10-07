#include "flextGLWindowsDesktop.h"

#ifdef __cplusplus
extern "C" {
#endif

/* GL_ES_VERSION_2_0 */
FLEXTGL_EXPORT void(APIENTRY *flextglActiveTexture)(GLenum) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglAttachShader)(GLuint, GLuint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglBindAttribLocation)(GLuint, GLuint, const GLchar *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglBindBuffer)(GLenum, GLuint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglBindFramebuffer)(GLenum, GLuint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglBindRenderbuffer)(GLenum, GLuint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglBlendColor)(GLfloat, GLfloat, GLfloat, GLfloat) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglBlendEquation)(GLenum) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglBlendEquationSeparate)(GLenum, GLenum) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglBlendFuncSeparate)(GLenum, GLenum, GLenum, GLenum) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglBufferData)(GLenum, GLsizeiptr, const void *, GLenum) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglBufferSubData)(GLenum, GLintptr, GLsizeiptr, const void *) = nullptr;
FLEXTGL_EXPORT GLenum(APIENTRY *flextglCheckFramebufferStatus)(GLenum) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglClearDepthf)(GLfloat) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglCompileShader)(GLuint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglCompressedTexImage2D)(GLenum, GLint, GLenum, GLsizei, GLsizei, GLint, GLsizei, const void *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglCompressedTexSubImage2D)(GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLsizei, const void *) = nullptr;
FLEXTGL_EXPORT GLuint(APIENTRY *flextglCreateProgram)(void) = nullptr;
FLEXTGL_EXPORT GLuint(APIENTRY *flextglCreateShader)(GLenum) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglDeleteBuffers)(GLsizei, const GLuint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglDeleteFramebuffers)(GLsizei, const GLuint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglDeleteProgram)(GLuint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglDeleteRenderbuffers)(GLsizei, const GLuint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglDeleteShader)(GLuint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglDepthRangef)(GLfloat, GLfloat) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglDetachShader)(GLuint, GLuint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglDisableVertexAttribArray)(GLuint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglEnableVertexAttribArray)(GLuint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglFramebufferRenderbuffer)(GLenum, GLenum, GLenum, GLuint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglFramebufferTexture2D)(GLenum, GLenum, GLenum, GLuint, GLint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGenBuffers)(GLsizei, GLuint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGenFramebuffers)(GLsizei, GLuint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGenRenderbuffers)(GLsizei, GLuint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGenerateMipmap)(GLenum) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGetActiveAttrib)(GLuint, GLuint, GLsizei, GLsizei *, GLint *, GLenum *, GLchar *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGetActiveUniform)(GLuint, GLuint, GLsizei, GLsizei *, GLint *, GLenum *, GLchar *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGetAttachedShaders)(GLuint, GLsizei, GLsizei *, GLuint *) = nullptr;
FLEXTGL_EXPORT GLint(APIENTRY *flextglGetAttribLocation)(GLuint, const GLchar *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGetBufferParameteriv)(GLenum, GLenum, GLint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGetFramebufferAttachmentParameteriv)(GLenum, GLenum, GLenum, GLint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGetProgramInfoLog)(GLuint, GLsizei, GLsizei *, GLchar *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGetProgramiv)(GLuint, GLenum, GLint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGetRenderbufferParameteriv)(GLenum, GLenum, GLint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGetShaderInfoLog)(GLuint, GLsizei, GLsizei *, GLchar *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGetShaderPrecisionFormat)(GLenum, GLenum, GLint *, GLint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGetShaderSource)(GLuint, GLsizei, GLsizei *, GLchar *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGetShaderiv)(GLuint, GLenum, GLint *) = nullptr;
FLEXTGL_EXPORT GLint(APIENTRY *flextglGetUniformLocation)(GLuint, const GLchar *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGetUniformfv)(GLuint, GLint, GLfloat *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGetUniformiv)(GLuint, GLint, GLint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGetVertexAttribPointerv)(GLuint, GLenum, void **) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGetVertexAttribfv)(GLuint, GLenum, GLfloat *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGetVertexAttribiv)(GLuint, GLenum, GLint *) = nullptr;
FLEXTGL_EXPORT GLboolean(APIENTRY *flextglIsBuffer)(GLuint) = nullptr;
FLEXTGL_EXPORT GLboolean(APIENTRY *flextglIsFramebuffer)(GLuint) = nullptr;
FLEXTGL_EXPORT GLboolean(APIENTRY *flextglIsProgram)(GLuint) = nullptr;
FLEXTGL_EXPORT GLboolean(APIENTRY *flextglIsRenderbuffer)(GLuint) = nullptr;
FLEXTGL_EXPORT GLboolean(APIENTRY *flextglIsShader)(GLuint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglLinkProgram)(GLuint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglReleaseShaderCompiler)(void) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglRenderbufferStorage)(GLenum, GLenum, GLsizei, GLsizei) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglSampleCoverage)(GLfloat, GLboolean) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglShaderBinary)(GLsizei, const GLuint *, GLenum, const void *, GLsizei) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglShaderSource)(GLuint, GLsizei, const GLchar *const*, const GLint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglStencilFuncSeparate)(GLenum, GLenum, GLint, GLuint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglStencilMaskSeparate)(GLenum, GLuint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglStencilOpSeparate)(GLenum, GLenum, GLenum, GLenum) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglUniform1f)(GLint, GLfloat) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglUniform1fv)(GLint, GLsizei, const GLfloat *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglUniform1i)(GLint, GLint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglUniform1iv)(GLint, GLsizei, const GLint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglUniform2f)(GLint, GLfloat, GLfloat) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglUniform2fv)(GLint, GLsizei, const GLfloat *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglUniform2i)(GLint, GLint, GLint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglUniform2iv)(GLint, GLsizei, const GLint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglUniform3f)(GLint, GLfloat, GLfloat, GLfloat) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglUniform3fv)(GLint, GLsizei, const GLfloat *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglUniform3i)(GLint, GLint, GLint, GLint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglUniform3iv)(GLint, GLsizei, const GLint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglUniform4f)(GLint, GLfloat, GLfloat, GLfloat, GLfloat) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglUniform4fv)(GLint, GLsizei, const GLfloat *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglUniform4i)(GLint, GLint, GLint, GLint, GLint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglUniform4iv)(GLint, GLsizei, const GLint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglUniformMatrix2fv)(GLint, GLsizei, GLboolean, const GLfloat *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglUniformMatrix3fv)(GLint, GLsizei, GLboolean, const GLfloat *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglUniformMatrix4fv)(GLint, GLsizei, GLboolean, const GLfloat *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglUseProgram)(GLuint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglValidateProgram)(GLuint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglVertexAttrib1f)(GLuint, GLfloat) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglVertexAttrib1fv)(GLuint, const GLfloat *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglVertexAttrib2f)(GLuint, GLfloat, GLfloat) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglVertexAttrib2fv)(GLuint, const GLfloat *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglVertexAttrib3f)(GLuint, GLfloat, GLfloat, GLfloat) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglVertexAttrib3fv)(GLuint, const GLfloat *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglVertexAttrib4f)(GLuint, GLfloat, GLfloat, GLfloat, GLfloat) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglVertexAttrib4fv)(GLuint, const GLfloat *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglVertexAttribPointer)(GLuint, GLint, GLenum, GLboolean, GLsizei, const void *) = nullptr;

/* GL_ES_VERSION_3_0 */
FLEXTGL_EXPORT void(APIENTRY *flextglBeginQuery)(GLenum, GLuint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglBeginTransformFeedback)(GLenum) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglBindBufferBase)(GLenum, GLuint, GLuint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglBindBufferRange)(GLenum, GLuint, GLuint, GLintptr, GLsizeiptr) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglBindSampler)(GLuint, GLuint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglBindTransformFeedback)(GLenum, GLuint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglBindVertexArray)(GLuint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglBlitFramebuffer)(GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLbitfield, GLenum) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglClearBufferfi)(GLenum, GLint, GLfloat, GLint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglClearBufferfv)(GLenum, GLint, const GLfloat *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglClearBufferiv)(GLenum, GLint, const GLint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglClearBufferuiv)(GLenum, GLint, const GLuint *) = nullptr;
FLEXTGL_EXPORT GLenum(APIENTRY *flextglClientWaitSync)(GLsync, GLbitfield, GLuint64) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglCompressedTexImage3D)(GLenum, GLint, GLenum, GLsizei, GLsizei, GLsizei, GLint, GLsizei, const void *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglCompressedTexSubImage3D)(GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLsizei, const void *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglCopyBufferSubData)(GLenum, GLenum, GLintptr, GLintptr, GLsizeiptr) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglCopyTexSubImage3D)(GLenum, GLint, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglDeleteQueries)(GLsizei, const GLuint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglDeleteSamplers)(GLsizei, const GLuint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglDeleteSync)(GLsync) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglDeleteTransformFeedbacks)(GLsizei, const GLuint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglDeleteVertexArrays)(GLsizei, const GLuint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglDrawArraysInstanced)(GLenum, GLint, GLsizei, GLsizei) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglDrawBuffers)(GLsizei, const GLenum *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglDrawElementsInstanced)(GLenum, GLsizei, GLenum, const void *, GLsizei) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglDrawRangeElements)(GLenum, GLuint, GLuint, GLsizei, GLenum, const void *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglEndQuery)(GLenum) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglEndTransformFeedback)(void) = nullptr;
FLEXTGL_EXPORT GLsync(APIENTRY *flextglFenceSync)(GLenum, GLbitfield) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglFlushMappedBufferRange)(GLenum, GLintptr, GLsizeiptr) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglFramebufferTextureLayer)(GLenum, GLenum, GLuint, GLint, GLint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGenQueries)(GLsizei, GLuint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGenSamplers)(GLsizei, GLuint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGenTransformFeedbacks)(GLsizei, GLuint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGenVertexArrays)(GLsizei, GLuint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGetActiveUniformBlockName)(GLuint, GLuint, GLsizei, GLsizei *, GLchar *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGetActiveUniformBlockiv)(GLuint, GLuint, GLenum, GLint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGetActiveUniformsiv)(GLuint, GLsizei, const GLuint *, GLenum, GLint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGetBufferParameteri64v)(GLenum, GLenum, GLint64 *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGetBufferPointerv)(GLenum, GLenum, void **) = nullptr;
FLEXTGL_EXPORT GLint(APIENTRY *flextglGetFragDataLocation)(GLuint, const GLchar *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGetInteger64i_v)(GLenum, GLuint, GLint64 *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGetInteger64v)(GLenum, GLint64 *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGetIntegeri_v)(GLenum, GLuint, GLint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGetInternalformativ)(GLenum, GLenum, GLenum, GLsizei, GLint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGetProgramBinary)(GLuint, GLsizei, GLsizei *, GLenum *, void *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGetQueryObjectuiv)(GLuint, GLenum, GLuint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGetQueryiv)(GLenum, GLenum, GLint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGetSamplerParameterfv)(GLuint, GLenum, GLfloat *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGetSamplerParameteriv)(GLuint, GLenum, GLint *) = nullptr;
FLEXTGL_EXPORT const GLubyte *(APIENTRY *flextglGetStringi)(GLenum, GLuint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGetSynciv)(GLsync, GLenum, GLsizei, GLsizei *, GLint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGetTransformFeedbackVarying)(GLuint, GLuint, GLsizei, GLsizei *, GLsizei *, GLenum *, GLchar *) = nullptr;
FLEXTGL_EXPORT GLuint(APIENTRY *flextglGetUniformBlockIndex)(GLuint, const GLchar *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGetUniformIndices)(GLuint, GLsizei, const GLchar *const*, GLuint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGetUniformuiv)(GLuint, GLint, GLuint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGetVertexAttribIiv)(GLuint, GLenum, GLint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGetVertexAttribIuiv)(GLuint, GLenum, GLuint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglInvalidateFramebuffer)(GLenum, GLsizei, const GLenum *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglInvalidateSubFramebuffer)(GLenum, GLsizei, const GLenum *, GLint, GLint, GLsizei, GLsizei) = nullptr;
FLEXTGL_EXPORT GLboolean(APIENTRY *flextglIsQuery)(GLuint) = nullptr;
FLEXTGL_EXPORT GLboolean(APIENTRY *flextglIsSampler)(GLuint) = nullptr;
FLEXTGL_EXPORT GLboolean(APIENTRY *flextglIsSync)(GLsync) = nullptr;
FLEXTGL_EXPORT GLboolean(APIENTRY *flextglIsTransformFeedback)(GLuint) = nullptr;
FLEXTGL_EXPORT GLboolean(APIENTRY *flextglIsVertexArray)(GLuint) = nullptr;
FLEXTGL_EXPORT void *(APIENTRY *flextglMapBufferRange)(GLenum, GLintptr, GLsizeiptr, GLbitfield) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglPauseTransformFeedback)(void) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramBinary)(GLuint, GLenum, const void *, GLsizei) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramParameteri)(GLuint, GLenum, GLint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglRenderbufferStorageMultisample)(GLenum, GLsizei, GLenum, GLsizei, GLsizei) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglResumeTransformFeedback)(void) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglSamplerParameterf)(GLuint, GLenum, GLfloat) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglSamplerParameterfv)(GLuint, GLenum, const GLfloat *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglSamplerParameteri)(GLuint, GLenum, GLint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglSamplerParameteriv)(GLuint, GLenum, const GLint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglTexImage3D)(GLenum, GLint, GLint, GLsizei, GLsizei, GLsizei, GLint, GLenum, GLenum, const void *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglTexStorage2D)(GLenum, GLsizei, GLenum, GLsizei, GLsizei) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglTexStorage3D)(GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLsizei) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglTexSubImage3D)(GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, const void *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglTransformFeedbackVaryings)(GLuint, GLsizei, const GLchar *const*, GLenum) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglUniform1ui)(GLint, GLuint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglUniform1uiv)(GLint, GLsizei, const GLuint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglUniform2ui)(GLint, GLuint, GLuint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglUniform2uiv)(GLint, GLsizei, const GLuint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglUniform3ui)(GLint, GLuint, GLuint, GLuint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglUniform3uiv)(GLint, GLsizei, const GLuint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglUniform4ui)(GLint, GLuint, GLuint, GLuint, GLuint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglUniform4uiv)(GLint, GLsizei, const GLuint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglUniformBlockBinding)(GLuint, GLuint, GLuint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglUniformMatrix2x3fv)(GLint, GLsizei, GLboolean, const GLfloat *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglUniformMatrix2x4fv)(GLint, GLsizei, GLboolean, const GLfloat *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglUniformMatrix3x2fv)(GLint, GLsizei, GLboolean, const GLfloat *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglUniformMatrix3x4fv)(GLint, GLsizei, GLboolean, const GLfloat *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglUniformMatrix4x2fv)(GLint, GLsizei, GLboolean, const GLfloat *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglUniformMatrix4x3fv)(GLint, GLsizei, GLboolean, const GLfloat *) = nullptr;
FLEXTGL_EXPORT GLboolean(APIENTRY *flextglUnmapBuffer)(GLenum) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglVertexAttribDivisor)(GLuint, GLuint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglVertexAttribI4i)(GLuint, GLint, GLint, GLint, GLint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglVertexAttribI4iv)(GLuint, const GLint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglVertexAttribI4ui)(GLuint, GLuint, GLuint, GLuint, GLuint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglVertexAttribI4uiv)(GLuint, const GLuint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglVertexAttribIPointer)(GLuint, GLint, GLenum, GLsizei, const void *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglWaitSync)(GLsync, GLbitfield, GLuint64) = nullptr;

/* GL_ES_VERSION_3_1 */
FLEXTGL_EXPORT void(APIENTRY *flextglActiveShaderProgram)(GLuint, GLuint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglBindImageTexture)(GLuint, GLuint, GLint, GLboolean, GLint, GLenum, GLenum) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglBindProgramPipeline)(GLuint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglBindVertexBuffer)(GLuint, GLuint, GLintptr, GLsizei) = nullptr;
FLEXTGL_EXPORT GLuint(APIENTRY *flextglCreateShaderProgramv)(GLenum, GLsizei, const GLchar *const*) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglDeleteProgramPipelines)(GLsizei, const GLuint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglDispatchCompute)(GLuint, GLuint, GLuint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglDispatchComputeIndirect)(GLintptr) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglDrawArraysIndirect)(GLenum, const void *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglDrawElementsIndirect)(GLenum, GLenum, const void *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglFramebufferParameteri)(GLenum, GLenum, GLint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGenProgramPipelines)(GLsizei, GLuint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGetBooleani_v)(GLenum, GLuint, GLboolean *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGetFramebufferParameteriv)(GLenum, GLenum, GLint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGetMultisamplefv)(GLenum, GLuint, GLfloat *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGetProgramInterfaceiv)(GLuint, GLenum, GLenum, GLint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGetProgramPipelineInfoLog)(GLuint, GLsizei, GLsizei *, GLchar *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGetProgramPipelineiv)(GLuint, GLenum, GLint *) = nullptr;
FLEXTGL_EXPORT GLuint(APIENTRY *flextglGetProgramResourceIndex)(GLuint, GLenum, const GLchar *) = nullptr;
FLEXTGL_EXPORT GLint(APIENTRY *flextglGetProgramResourceLocation)(GLuint, GLenum, const GLchar *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGetProgramResourceName)(GLuint, GLenum, GLuint, GLsizei, GLsizei *, GLchar *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGetProgramResourceiv)(GLuint, GLenum, GLuint, GLsizei, const GLenum *, GLsizei, GLsizei *, GLint *) = nullptr;
FLEXTGL_EXPORT GLboolean(APIENTRY *flextglIsProgramPipeline)(GLuint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglMemoryBarrier)(GLbitfield) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglMemoryBarrierByRegion)(GLbitfield) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniform1f)(GLuint, GLint, GLfloat) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniform1fv)(GLuint, GLint, GLsizei, const GLfloat *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniform1i)(GLuint, GLint, GLint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniform1iv)(GLuint, GLint, GLsizei, const GLint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniform1ui)(GLuint, GLint, GLuint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniform1uiv)(GLuint, GLint, GLsizei, const GLuint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniform2f)(GLuint, GLint, GLfloat, GLfloat) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniform2fv)(GLuint, GLint, GLsizei, const GLfloat *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniform2i)(GLuint, GLint, GLint, GLint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniform2iv)(GLuint, GLint, GLsizei, const GLint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniform2ui)(GLuint, GLint, GLuint, GLuint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniform2uiv)(GLuint, GLint, GLsizei, const GLuint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniform3f)(GLuint, GLint, GLfloat, GLfloat, GLfloat) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniform3fv)(GLuint, GLint, GLsizei, const GLfloat *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniform3i)(GLuint, GLint, GLint, GLint, GLint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniform3iv)(GLuint, GLint, GLsizei, const GLint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniform3ui)(GLuint, GLint, GLuint, GLuint, GLuint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniform3uiv)(GLuint, GLint, GLsizei, const GLuint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniform4f)(GLuint, GLint, GLfloat, GLfloat, GLfloat, GLfloat) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniform4fv)(GLuint, GLint, GLsizei, const GLfloat *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniform4i)(GLuint, GLint, GLint, GLint, GLint, GLint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniform4iv)(GLuint, GLint, GLsizei, const GLint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniform4ui)(GLuint, GLint, GLuint, GLuint, GLuint, GLuint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniform4uiv)(GLuint, GLint, GLsizei, const GLuint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniformMatrix2fv)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniformMatrix2x3fv)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniformMatrix2x4fv)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniformMatrix3fv)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniformMatrix3x2fv)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniformMatrix3x4fv)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniformMatrix4fv)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniformMatrix4x2fv)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniformMatrix4x3fv)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglSampleMaski)(GLuint, GLbitfield) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglTexStorage2DMultisample)(GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLboolean) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglUseProgramStages)(GLuint, GLbitfield, GLuint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglValidateProgramPipeline)(GLuint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglVertexAttribBinding)(GLuint, GLuint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglVertexAttribFormat)(GLuint, GLint, GLenum, GLboolean, GLuint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglVertexAttribIFormat)(GLuint, GLint, GLenum, GLuint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglVertexBindingDivisor)(GLuint, GLuint) = nullptr;

/* GL_EXT_copy_image */
FLEXTGL_EXPORT void(APIENTRY *flextglCopyImageSubDataEXT)(GLuint, GLenum, GLint, GLint, GLint, GLint, GLuint, GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei) = nullptr;

/* GL_EXT_debug_label */
FLEXTGL_EXPORT void(APIENTRY *flextglGetObjectLabelEXT)(GLenum, GLuint, GLsizei, GLsizei *, GLchar *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglLabelObjectEXT)(GLenum, GLuint, GLsizei, const GLchar *) = nullptr;

/* GL_EXT_debug_marker */
FLEXTGL_EXPORT void(APIENTRY *flextglInsertEventMarkerEXT)(GLsizei, const GLchar *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglPopGroupMarkerEXT)(void) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglPushGroupMarkerEXT)(GLsizei, const GLchar *) = nullptr;

/* GL_EXT_disjoint_timer_query */
FLEXTGL_EXPORT void(APIENTRY *flextglBeginQueryEXT)(GLenum, GLuint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglDeleteQueriesEXT)(GLsizei, const GLuint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglEndQueryEXT)(GLenum) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGenQueriesEXT)(GLsizei, GLuint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGetQueryObjecti64vEXT)(GLuint, GLenum, GLint64 *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGetQueryObjectivEXT)(GLuint, GLenum, GLint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGetQueryObjectui64vEXT)(GLuint, GLenum, GLuint64 *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGetQueryObjectuivEXT)(GLuint, GLenum, GLuint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGetQueryivEXT)(GLenum, GLenum, GLint *) = nullptr;
FLEXTGL_EXPORT GLboolean(APIENTRY *flextglIsQueryEXT)(GLuint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglQueryCounterEXT)(GLuint, GLenum) = nullptr;

/* GL_EXT_draw_buffers_indexed */
FLEXTGL_EXPORT void(APIENTRY *flextglBlendEquationSeparateiEXT)(GLuint, GLenum, GLenum) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglBlendEquationiEXT)(GLuint, GLenum) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglBlendFuncSeparateiEXT)(GLuint, GLenum, GLenum, GLenum, GLenum) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglBlendFunciEXT)(GLuint, GLenum, GLenum) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglColorMaskiEXT)(GLuint, GLboolean, GLboolean, GLboolean, GLboolean) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglDisableiEXT)(GLenum, GLuint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglEnableiEXT)(GLenum, GLuint) = nullptr;
FLEXTGL_EXPORT GLboolean(APIENTRY *flextglIsEnablediEXT)(GLenum, GLuint) = nullptr;

/* GL_EXT_geometry_shader */
FLEXTGL_EXPORT void(APIENTRY *flextglFramebufferTextureEXT)(GLenum, GLenum, GLuint, GLint) = nullptr;

/* GL_EXT_multi_draw_arrays */
FLEXTGL_EXPORT void(APIENTRY *flextglMultiDrawArraysEXT)(GLenum, const GLint *, const GLsizei *, GLsizei) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglMultiDrawElementsEXT)(GLenum, const GLsizei *, GLenum, const void *const*, GLsizei) = nullptr;

/* GL_EXT_multisampled_render_to_texture */
FLEXTGL_EXPORT void(APIENTRY *flextglFramebufferTexture2DMultisampleEXT)(GLenum, GLenum, GLenum, GLuint, GLint, GLsizei) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglRenderbufferStorageMultisampleEXT)(GLenum, GLsizei, GLenum, GLsizei, GLsizei) = nullptr;

/* GL_EXT_primitive_bounding_box */
FLEXTGL_EXPORT void(APIENTRY *flextglPrimitiveBoundingBoxEXT)(GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat) = nullptr;

/* GL_EXT_robustness */
FLEXTGL_EXPORT GLenum(APIENTRY *flextglGetGraphicsResetStatusEXT)(void) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGetnUniformfvEXT)(GLuint, GLint, GLsizei, GLfloat *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGetnUniformivEXT)(GLuint, GLint, GLsizei, GLint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglReadnPixelsEXT)(GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, GLsizei, void *) = nullptr;

/* GL_EXT_separate_shader_objects */
FLEXTGL_EXPORT void(APIENTRY *flextglActiveShaderProgramEXT)(GLuint, GLuint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglBindProgramPipelineEXT)(GLuint) = nullptr;
FLEXTGL_EXPORT GLuint(APIENTRY *flextglCreateShaderProgramvEXT)(GLenum, GLsizei, const GLchar **) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglDeleteProgramPipelinesEXT)(GLsizei, const GLuint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGenProgramPipelinesEXT)(GLsizei, GLuint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGetProgramPipelineInfoLogEXT)(GLuint, GLsizei, GLsizei *, GLchar *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGetProgramPipelineivEXT)(GLuint, GLenum, GLint *) = nullptr;
FLEXTGL_EXPORT GLboolean(APIENTRY *flextglIsProgramPipelineEXT)(GLuint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramParameteriEXT)(GLuint, GLenum, GLint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniform1fEXT)(GLuint, GLint, GLfloat) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniform1fvEXT)(GLuint, GLint, GLsizei, const GLfloat *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniform1iEXT)(GLuint, GLint, GLint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniform1ivEXT)(GLuint, GLint, GLsizei, const GLint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniform1uiEXT)(GLuint, GLint, GLuint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniform1uivEXT)(GLuint, GLint, GLsizei, const GLuint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniform2fEXT)(GLuint, GLint, GLfloat, GLfloat) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniform2fvEXT)(GLuint, GLint, GLsizei, const GLfloat *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniform2iEXT)(GLuint, GLint, GLint, GLint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniform2ivEXT)(GLuint, GLint, GLsizei, const GLint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniform2uiEXT)(GLuint, GLint, GLuint, GLuint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniform2uivEXT)(GLuint, GLint, GLsizei, const GLuint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniform3fEXT)(GLuint, GLint, GLfloat, GLfloat, GLfloat) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniform3fvEXT)(GLuint, GLint, GLsizei, const GLfloat *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniform3iEXT)(GLuint, GLint, GLint, GLint, GLint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniform3ivEXT)(GLuint, GLint, GLsizei, const GLint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniform3uiEXT)(GLuint, GLint, GLuint, GLuint, GLuint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniform3uivEXT)(GLuint, GLint, GLsizei, const GLuint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniform4fEXT)(GLuint, GLint, GLfloat, GLfloat, GLfloat, GLfloat) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniform4fvEXT)(GLuint, GLint, GLsizei, const GLfloat *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniform4iEXT)(GLuint, GLint, GLint, GLint, GLint, GLint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniform4ivEXT)(GLuint, GLint, GLsizei, const GLint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniform4uiEXT)(GLuint, GLint, GLuint, GLuint, GLuint, GLuint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniform4uivEXT)(GLuint, GLint, GLsizei, const GLuint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniformMatrix2fvEXT)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniformMatrix2x3fvEXT)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniformMatrix2x4fvEXT)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniformMatrix3fvEXT)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniformMatrix3x2fvEXT)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniformMatrix3x4fvEXT)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniformMatrix4fvEXT)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniformMatrix4x2fvEXT)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniformMatrix4x3fvEXT)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglUseProgramStagesEXT)(GLuint, GLbitfield, GLuint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglValidateProgramPipelineEXT)(GLuint) = nullptr;

/* GL_EXT_tessellation_shader */
FLEXTGL_EXPORT void(APIENTRY *flextglPatchParameteriEXT)(GLenum, GLint) = nullptr;

/* GL_EXT_texture_border_clamp */
FLEXTGL_EXPORT void(APIENTRY *flextglGetSamplerParameterIivEXT)(GLuint, GLenum, GLint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGetSamplerParameterIuivEXT)(GLuint, GLenum, GLuint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGetTexParameterIivEXT)(GLenum, GLenum, GLint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGetTexParameterIuivEXT)(GLenum, GLenum, GLuint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglSamplerParameterIivEXT)(GLuint, GLenum, const GLint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglSamplerParameterIuivEXT)(GLuint, GLenum, const GLuint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglTexParameterIivEXT)(GLenum, GLenum, const GLint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglTexParameterIuivEXT)(GLenum, GLenum, const GLuint *) = nullptr;

/* GL_EXT_texture_buffer */
FLEXTGL_EXPORT void(APIENTRY *flextglTexBufferEXT)(GLenum, GLenum, GLuint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglTexBufferRangeEXT)(GLenum, GLenum, GLuint, GLintptr, GLsizeiptr) = nullptr;

/* GL_KHR_blend_equation_advanced */
FLEXTGL_EXPORT void(APIENTRY *flextglBlendBarrierKHR)(void) = nullptr;

/* GL_KHR_debug */
FLEXTGL_EXPORT void(APIENTRY *flextglDebugMessageCallbackKHR)(GLDEBUGPROCKHR, const void *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglDebugMessageControlKHR)(GLenum, GLenum, GLenum, GLsizei, const GLuint *, GLboolean) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglDebugMessageInsertKHR)(GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar *) = nullptr;
FLEXTGL_EXPORT GLuint(APIENTRY *flextglGetDebugMessageLogKHR)(GLuint, GLsizei, GLenum *, GLenum *, GLuint *, GLenum *, GLsizei *, GLchar *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGetObjectLabelKHR)(GLenum, GLuint, GLsizei, GLsizei *, GLchar *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGetObjectPtrLabelKHR)(const void *, GLsizei, GLsizei *, GLchar *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGetPointervKHR)(GLenum, void **) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglObjectLabelKHR)(GLenum, GLuint, GLsizei, const GLchar *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglObjectPtrLabelKHR)(const void *, GLsizei, const GLchar *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglPopDebugGroupKHR)(void) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglPushDebugGroupKHR)(GLenum, GLuint, GLsizei, const GLchar *) = nullptr;

/* GL_KHR_robustness */
FLEXTGL_EXPORT GLenum(APIENTRY *flextglGetGraphicsResetStatusKHR)(void) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGetnUniformfvKHR)(GLuint, GLint, GLsizei, GLfloat *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGetnUniformivKHR)(GLuint, GLint, GLsizei, GLint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGetnUniformuivKHR)(GLuint, GLint, GLsizei, GLuint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglReadnPixelsKHR)(GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, GLsizei, void *) = nullptr;

/* GL_NV_polygon_mode */
FLEXTGL_EXPORT void(APIENTRY *flextglPolygonModeNV)(GLenum, GLenum) = nullptr;

/* GL_OES_mapbuffer */
FLEXTGL_EXPORT void(APIENTRY *flextglGetBufferPointervOES)(GLenum, GLenum, void **) = nullptr;
FLEXTGL_EXPORT void *(APIENTRY *flextglMapBufferOES)(GLenum, GLenum) = nullptr;
FLEXTGL_EXPORT GLboolean(APIENTRY *flextglUnmapBufferOES)(GLenum) = nullptr;

/* GL_OES_sample_shading */
FLEXTGL_EXPORT void(APIENTRY *flextglMinSampleShadingOES)(GLfloat) = nullptr;

/* GL_OES_texture_storage_multisample_2d_array */
FLEXTGL_EXPORT void(APIENTRY *flextglTexStorage3DMultisampleOES)(GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLsizei, GLboolean) = nullptr;

#ifdef __cplusplus
}
#endif
