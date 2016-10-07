#include "flextGLWindowsDesktop.h"

#include <cstdio>
#include <cstdlib>

#include "Magnum/Platform/Implementation/OpenGLFunctionLoader.h"

void flextGLInit() {
    Magnum::Platform::Implementation::OpenGLFunctionLoader loader;

    /* GL_ES_VERSION_2_0 */
    flextglActiveTexture = reinterpret_cast<void(APIENTRY*)(GLenum)>(loader.load("glActiveTexture"));
    flextglAttachShader = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint)>(loader.load("glAttachShader"));
    flextglBindAttribLocation = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, const GLchar *)>(loader.load("glBindAttribLocation"));
    flextglBindBuffer = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint)>(loader.load("glBindBuffer"));
    flextglBindFramebuffer = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint)>(loader.load("glBindFramebuffer"));
    flextglBindRenderbuffer = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint)>(loader.load("glBindRenderbuffer"));
    flextglBlendColor = reinterpret_cast<void(APIENTRY*)(GLfloat, GLfloat, GLfloat, GLfloat)>(loader.load("glBlendColor"));
    flextglBlendEquation = reinterpret_cast<void(APIENTRY*)(GLenum)>(loader.load("glBlendEquation"));
    flextglBlendEquationSeparate = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum)>(loader.load("glBlendEquationSeparate"));
    flextglBlendFuncSeparate = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLenum, GLenum)>(loader.load("glBlendFuncSeparate"));
    flextglBufferData = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizeiptr, const void *, GLenum)>(loader.load("glBufferData"));
    flextglBufferSubData = reinterpret_cast<void(APIENTRY*)(GLenum, GLintptr, GLsizeiptr, const void *)>(loader.load("glBufferSubData"));
    flextglCheckFramebufferStatus = reinterpret_cast<GLenum(APIENTRY*)(GLenum)>(loader.load("glCheckFramebufferStatus"));
    flextglClearDepthf = reinterpret_cast<void(APIENTRY*)(GLfloat)>(loader.load("glClearDepthf"));
    flextglCompileShader = reinterpret_cast<void(APIENTRY*)(GLuint)>(loader.load("glCompileShader"));
    flextglCompressedTexImage2D = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLenum, GLsizei, GLsizei, GLint, GLsizei, const void *)>(loader.load("glCompressedTexImage2D"));
    flextglCompressedTexSubImage2D = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLsizei, const void *)>(loader.load("glCompressedTexSubImage2D"));
    flextglCreateProgram = reinterpret_cast<GLuint(APIENTRY*)(void)>(loader.load("glCreateProgram"));
    flextglCreateShader = reinterpret_cast<GLuint(APIENTRY*)(GLenum)>(loader.load("glCreateShader"));
    flextglDeleteBuffers = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLuint *)>(loader.load("glDeleteBuffers"));
    flextglDeleteFramebuffers = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLuint *)>(loader.load("glDeleteFramebuffers"));
    flextglDeleteProgram = reinterpret_cast<void(APIENTRY*)(GLuint)>(loader.load("glDeleteProgram"));
    flextglDeleteRenderbuffers = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLuint *)>(loader.load("glDeleteRenderbuffers"));
    flextglDeleteShader = reinterpret_cast<void(APIENTRY*)(GLuint)>(loader.load("glDeleteShader"));
    flextglDepthRangef = reinterpret_cast<void(APIENTRY*)(GLfloat, GLfloat)>(loader.load("glDepthRangef"));
    flextglDetachShader = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint)>(loader.load("glDetachShader"));
    flextglDisableVertexAttribArray = reinterpret_cast<void(APIENTRY*)(GLuint)>(loader.load("glDisableVertexAttribArray"));
    flextglEnableVertexAttribArray = reinterpret_cast<void(APIENTRY*)(GLuint)>(loader.load("glEnableVertexAttribArray"));
    flextglFramebufferRenderbuffer = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLenum, GLuint)>(loader.load("glFramebufferRenderbuffer"));
    flextglFramebufferTexture2D = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLenum, GLuint, GLint)>(loader.load("glFramebufferTexture2D"));
    flextglGenBuffers = reinterpret_cast<void(APIENTRY*)(GLsizei, GLuint *)>(loader.load("glGenBuffers"));
    flextglGenFramebuffers = reinterpret_cast<void(APIENTRY*)(GLsizei, GLuint *)>(loader.load("glGenFramebuffers"));
    flextglGenRenderbuffers = reinterpret_cast<void(APIENTRY*)(GLsizei, GLuint *)>(loader.load("glGenRenderbuffers"));
    flextglGenerateMipmap = reinterpret_cast<void(APIENTRY*)(GLenum)>(loader.load("glGenerateMipmap"));
    flextglGetActiveAttrib = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLsizei, GLsizei *, GLint *, GLenum *, GLchar *)>(loader.load("glGetActiveAttrib"));
    flextglGetActiveUniform = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLsizei, GLsizei *, GLint *, GLenum *, GLchar *)>(loader.load("glGetActiveUniform"));
    flextglGetAttachedShaders = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, GLsizei *, GLuint *)>(loader.load("glGetAttachedShaders"));
    flextglGetAttribLocation = reinterpret_cast<GLint(APIENTRY*)(GLuint, const GLchar *)>(loader.load("glGetAttribLocation"));
    flextglGetBufferParameteriv = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLint *)>(loader.load("glGetBufferParameteriv"));
    flextglGetFramebufferAttachmentParameteriv = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLenum, GLint *)>(loader.load("glGetFramebufferAttachmentParameteriv"));
    flextglGetProgramInfoLog = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, GLsizei *, GLchar *)>(loader.load("glGetProgramInfoLog"));
    flextglGetProgramiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint *)>(loader.load("glGetProgramiv"));
    flextglGetRenderbufferParameteriv = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLint *)>(loader.load("glGetRenderbufferParameteriv"));
    flextglGetShaderInfoLog = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, GLsizei *, GLchar *)>(loader.load("glGetShaderInfoLog"));
    flextglGetShaderPrecisionFormat = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLint *, GLint *)>(loader.load("glGetShaderPrecisionFormat"));
    flextglGetShaderSource = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, GLsizei *, GLchar *)>(loader.load("glGetShaderSource"));
    flextglGetShaderiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint *)>(loader.load("glGetShaderiv"));
    flextglGetUniformLocation = reinterpret_cast<GLint(APIENTRY*)(GLuint, const GLchar *)>(loader.load("glGetUniformLocation"));
    flextglGetUniformfv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLfloat *)>(loader.load("glGetUniformfv"));
    flextglGetUniformiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint *)>(loader.load("glGetUniformiv"));
    flextglGetVertexAttribPointerv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, void **)>(loader.load("glGetVertexAttribPointerv"));
    flextglGetVertexAttribfv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLfloat *)>(loader.load("glGetVertexAttribfv"));
    flextglGetVertexAttribiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint *)>(loader.load("glGetVertexAttribiv"));
    flextglIsBuffer = reinterpret_cast<GLboolean(APIENTRY*)(GLuint)>(loader.load("glIsBuffer"));
    flextglIsFramebuffer = reinterpret_cast<GLboolean(APIENTRY*)(GLuint)>(loader.load("glIsFramebuffer"));
    flextglIsProgram = reinterpret_cast<GLboolean(APIENTRY*)(GLuint)>(loader.load("glIsProgram"));
    flextglIsRenderbuffer = reinterpret_cast<GLboolean(APIENTRY*)(GLuint)>(loader.load("glIsRenderbuffer"));
    flextglIsShader = reinterpret_cast<GLboolean(APIENTRY*)(GLuint)>(loader.load("glIsShader"));
    flextglLinkProgram = reinterpret_cast<void(APIENTRY*)(GLuint)>(loader.load("glLinkProgram"));
    flextglReleaseShaderCompiler = reinterpret_cast<void(APIENTRY*)(void)>(loader.load("glReleaseShaderCompiler"));
    flextglRenderbufferStorage = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLsizei, GLsizei)>(loader.load("glRenderbufferStorage"));
    flextglSampleCoverage = reinterpret_cast<void(APIENTRY*)(GLfloat, GLboolean)>(loader.load("glSampleCoverage"));
    flextglShaderBinary = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLuint *, GLenum, const void *, GLsizei)>(loader.load("glShaderBinary"));
    flextglShaderSource = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, const GLchar *const*, const GLint *)>(loader.load("glShaderSource"));
    flextglStencilFuncSeparate = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLint, GLuint)>(loader.load("glStencilFuncSeparate"));
    flextglStencilMaskSeparate = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint)>(loader.load("glStencilMaskSeparate"));
    flextglStencilOpSeparate = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLenum, GLenum)>(loader.load("glStencilOpSeparate"));
    flextglUniform1f = reinterpret_cast<void(APIENTRY*)(GLint, GLfloat)>(loader.load("glUniform1f"));
    flextglUniform1fv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, const GLfloat *)>(loader.load("glUniform1fv"));
    flextglUniform1i = reinterpret_cast<void(APIENTRY*)(GLint, GLint)>(loader.load("glUniform1i"));
    flextglUniform1iv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, const GLint *)>(loader.load("glUniform1iv"));
    flextglUniform2f = reinterpret_cast<void(APIENTRY*)(GLint, GLfloat, GLfloat)>(loader.load("glUniform2f"));
    flextglUniform2fv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, const GLfloat *)>(loader.load("glUniform2fv"));
    flextglUniform2i = reinterpret_cast<void(APIENTRY*)(GLint, GLint, GLint)>(loader.load("glUniform2i"));
    flextglUniform2iv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, const GLint *)>(loader.load("glUniform2iv"));
    flextglUniform3f = reinterpret_cast<void(APIENTRY*)(GLint, GLfloat, GLfloat, GLfloat)>(loader.load("glUniform3f"));
    flextglUniform3fv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, const GLfloat *)>(loader.load("glUniform3fv"));
    flextglUniform3i = reinterpret_cast<void(APIENTRY*)(GLint, GLint, GLint, GLint)>(loader.load("glUniform3i"));
    flextglUniform3iv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, const GLint *)>(loader.load("glUniform3iv"));
    flextglUniform4f = reinterpret_cast<void(APIENTRY*)(GLint, GLfloat, GLfloat, GLfloat, GLfloat)>(loader.load("glUniform4f"));
    flextglUniform4fv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, const GLfloat *)>(loader.load("glUniform4fv"));
    flextglUniform4i = reinterpret_cast<void(APIENTRY*)(GLint, GLint, GLint, GLint, GLint)>(loader.load("glUniform4i"));
    flextglUniform4iv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, const GLint *)>(loader.load("glUniform4iv"));
    flextglUniformMatrix2fv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glUniformMatrix2fv"));
    flextglUniformMatrix3fv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glUniformMatrix3fv"));
    flextglUniformMatrix4fv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glUniformMatrix4fv"));
    flextglUseProgram = reinterpret_cast<void(APIENTRY*)(GLuint)>(loader.load("glUseProgram"));
    flextglValidateProgram = reinterpret_cast<void(APIENTRY*)(GLuint)>(loader.load("glValidateProgram"));
    flextglVertexAttrib1f = reinterpret_cast<void(APIENTRY*)(GLuint, GLfloat)>(loader.load("glVertexAttrib1f"));
    flextglVertexAttrib1fv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLfloat *)>(loader.load("glVertexAttrib1fv"));
    flextglVertexAttrib2f = reinterpret_cast<void(APIENTRY*)(GLuint, GLfloat, GLfloat)>(loader.load("glVertexAttrib2f"));
    flextglVertexAttrib2fv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLfloat *)>(loader.load("glVertexAttrib2fv"));
    flextglVertexAttrib3f = reinterpret_cast<void(APIENTRY*)(GLuint, GLfloat, GLfloat, GLfloat)>(loader.load("glVertexAttrib3f"));
    flextglVertexAttrib3fv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLfloat *)>(loader.load("glVertexAttrib3fv"));
    flextglVertexAttrib4f = reinterpret_cast<void(APIENTRY*)(GLuint, GLfloat, GLfloat, GLfloat, GLfloat)>(loader.load("glVertexAttrib4f"));
    flextglVertexAttrib4fv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLfloat *)>(loader.load("glVertexAttrib4fv"));
    flextglVertexAttribPointer = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLenum, GLboolean, GLsizei, const void *)>(loader.load("glVertexAttribPointer"));

    /* GL_ES_VERSION_3_0 */
    flextglBeginQuery = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint)>(loader.load("glBeginQuery"));
    flextglBeginTransformFeedback = reinterpret_cast<void(APIENTRY*)(GLenum)>(loader.load("glBeginTransformFeedback"));
    flextglBindBufferBase = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLuint)>(loader.load("glBindBufferBase"));
    flextglBindBufferRange = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLuint, GLintptr, GLsizeiptr)>(loader.load("glBindBufferRange"));
    flextglBindSampler = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint)>(loader.load("glBindSampler"));
    flextglBindTransformFeedback = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint)>(loader.load("glBindTransformFeedback"));
    flextglBindVertexArray = reinterpret_cast<void(APIENTRY*)(GLuint)>(loader.load("glBindVertexArray"));
    flextglBlitFramebuffer = reinterpret_cast<void(APIENTRY*)(GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLbitfield, GLenum)>(loader.load("glBlitFramebuffer"));
    flextglClearBufferfi = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLfloat, GLint)>(loader.load("glClearBufferfi"));
    flextglClearBufferfv = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, const GLfloat *)>(loader.load("glClearBufferfv"));
    flextglClearBufferiv = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, const GLint *)>(loader.load("glClearBufferiv"));
    flextglClearBufferuiv = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, const GLuint *)>(loader.load("glClearBufferuiv"));
    flextglClientWaitSync = reinterpret_cast<GLenum(APIENTRY*)(GLsync, GLbitfield, GLuint64)>(loader.load("glClientWaitSync"));
    flextglCompressedTexImage3D = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLenum, GLsizei, GLsizei, GLsizei, GLint, GLsizei, const void *)>(loader.load("glCompressedTexImage3D"));
    flextglCompressedTexSubImage3D = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLsizei, const void *)>(loader.load("glCompressedTexSubImage3D"));
    flextglCopyBufferSubData = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLintptr, GLintptr, GLsizeiptr)>(loader.load("glCopyBufferSubData"));
    flextglCopyTexSubImage3D = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei)>(loader.load("glCopyTexSubImage3D"));
    flextglDeleteQueries = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLuint *)>(loader.load("glDeleteQueries"));
    flextglDeleteSamplers = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLuint *)>(loader.load("glDeleteSamplers"));
    flextglDeleteSync = reinterpret_cast<void(APIENTRY*)(GLsync)>(loader.load("glDeleteSync"));
    flextglDeleteTransformFeedbacks = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLuint *)>(loader.load("glDeleteTransformFeedbacks"));
    flextglDeleteVertexArrays = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLuint *)>(loader.load("glDeleteVertexArrays"));
    flextglDrawArraysInstanced = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLsizei, GLsizei)>(loader.load("glDrawArraysInstanced"));
    flextglDrawBuffers = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLenum *)>(loader.load("glDrawBuffers"));
    flextglDrawElementsInstanced = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, const void *, GLsizei)>(loader.load("glDrawElementsInstanced"));
    flextglDrawRangeElements = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLuint, GLsizei, GLenum, const void *)>(loader.load("glDrawRangeElements"));
    flextglEndQuery = reinterpret_cast<void(APIENTRY*)(GLenum)>(loader.load("glEndQuery"));
    flextglEndTransformFeedback = reinterpret_cast<void(APIENTRY*)(void)>(loader.load("glEndTransformFeedback"));
    flextglFenceSync = reinterpret_cast<GLsync(APIENTRY*)(GLenum, GLbitfield)>(loader.load("glFenceSync"));
    flextglFlushMappedBufferRange = reinterpret_cast<void(APIENTRY*)(GLenum, GLintptr, GLsizeiptr)>(loader.load("glFlushMappedBufferRange"));
    flextglFramebufferTextureLayer = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLuint, GLint, GLint)>(loader.load("glFramebufferTextureLayer"));
    flextglGenQueries = reinterpret_cast<void(APIENTRY*)(GLsizei, GLuint *)>(loader.load("glGenQueries"));
    flextglGenSamplers = reinterpret_cast<void(APIENTRY*)(GLsizei, GLuint *)>(loader.load("glGenSamplers"));
    flextglGenTransformFeedbacks = reinterpret_cast<void(APIENTRY*)(GLsizei, GLuint *)>(loader.load("glGenTransformFeedbacks"));
    flextglGenVertexArrays = reinterpret_cast<void(APIENTRY*)(GLsizei, GLuint *)>(loader.load("glGenVertexArrays"));
    flextglGetActiveUniformBlockName = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLsizei, GLsizei *, GLchar *)>(loader.load("glGetActiveUniformBlockName"));
    flextglGetActiveUniformBlockiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLenum, GLint *)>(loader.load("glGetActiveUniformBlockiv"));
    flextglGetActiveUniformsiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, const GLuint *, GLenum, GLint *)>(loader.load("glGetActiveUniformsiv"));
    flextglGetBufferParameteri64v = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLint64 *)>(loader.load("glGetBufferParameteri64v"));
    flextglGetBufferPointerv = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, void **)>(loader.load("glGetBufferPointerv"));
    flextglGetFragDataLocation = reinterpret_cast<GLint(APIENTRY*)(GLuint, const GLchar *)>(loader.load("glGetFragDataLocation"));
    flextglGetInteger64i_v = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLint64 *)>(loader.load("glGetInteger64i_v"));
    flextglGetInteger64v = reinterpret_cast<void(APIENTRY*)(GLenum, GLint64 *)>(loader.load("glGetInteger64v"));
    flextglGetIntegeri_v = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLint *)>(loader.load("glGetIntegeri_v"));
    flextglGetInternalformativ = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLenum, GLsizei, GLint *)>(loader.load("glGetInternalformativ"));
    flextglGetProgramBinary = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, GLsizei *, GLenum *, void *)>(loader.load("glGetProgramBinary"));
    flextglGetQueryObjectuiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint *)>(loader.load("glGetQueryObjectuiv"));
    flextglGetQueryiv = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLint *)>(loader.load("glGetQueryiv"));
    flextglGetSamplerParameterfv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLfloat *)>(loader.load("glGetSamplerParameterfv"));
    flextglGetSamplerParameteriv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint *)>(loader.load("glGetSamplerParameteriv"));
    flextglGetStringi = reinterpret_cast<const GLubyte *(APIENTRY*)(GLenum, GLuint)>(loader.load("glGetStringi"));
    flextglGetSynciv = reinterpret_cast<void(APIENTRY*)(GLsync, GLenum, GLsizei, GLsizei *, GLint *)>(loader.load("glGetSynciv"));
    flextglGetTransformFeedbackVarying = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLsizei, GLsizei *, GLsizei *, GLenum *, GLchar *)>(loader.load("glGetTransformFeedbackVarying"));
    flextglGetUniformBlockIndex = reinterpret_cast<GLuint(APIENTRY*)(GLuint, const GLchar *)>(loader.load("glGetUniformBlockIndex"));
    flextglGetUniformIndices = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, const GLchar *const*, GLuint *)>(loader.load("glGetUniformIndices"));
    flextglGetUniformuiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLuint *)>(loader.load("glGetUniformuiv"));
    flextglGetVertexAttribIiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint *)>(loader.load("glGetVertexAttribIiv"));
    flextglGetVertexAttribIuiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint *)>(loader.load("glGetVertexAttribIuiv"));
    flextglInvalidateFramebuffer = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, const GLenum *)>(loader.load("glInvalidateFramebuffer"));
    flextglInvalidateSubFramebuffer = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, const GLenum *, GLint, GLint, GLsizei, GLsizei)>(loader.load("glInvalidateSubFramebuffer"));
    flextglIsQuery = reinterpret_cast<GLboolean(APIENTRY*)(GLuint)>(loader.load("glIsQuery"));
    flextglIsSampler = reinterpret_cast<GLboolean(APIENTRY*)(GLuint)>(loader.load("glIsSampler"));
    flextglIsSync = reinterpret_cast<GLboolean(APIENTRY*)(GLsync)>(loader.load("glIsSync"));
    flextglIsTransformFeedback = reinterpret_cast<GLboolean(APIENTRY*)(GLuint)>(loader.load("glIsTransformFeedback"));
    flextglIsVertexArray = reinterpret_cast<GLboolean(APIENTRY*)(GLuint)>(loader.load("glIsVertexArray"));
    flextglMapBufferRange = reinterpret_cast<void *(APIENTRY*)(GLenum, GLintptr, GLsizeiptr, GLbitfield)>(loader.load("glMapBufferRange"));
    flextglPauseTransformFeedback = reinterpret_cast<void(APIENTRY*)(void)>(loader.load("glPauseTransformFeedback"));
    flextglProgramBinary = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, const void *, GLsizei)>(loader.load("glProgramBinary"));
    flextglProgramParameteri = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint)>(loader.load("glProgramParameteri"));
    flextglRenderbufferStorageMultisample = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, GLsizei, GLsizei)>(loader.load("glRenderbufferStorageMultisample"));
    flextglResumeTransformFeedback = reinterpret_cast<void(APIENTRY*)(void)>(loader.load("glResumeTransformFeedback"));
    flextglSamplerParameterf = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLfloat)>(loader.load("glSamplerParameterf"));
    flextglSamplerParameterfv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, const GLfloat *)>(loader.load("glSamplerParameterfv"));
    flextglSamplerParameteri = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint)>(loader.load("glSamplerParameteri"));
    flextglSamplerParameteriv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, const GLint *)>(loader.load("glSamplerParameteriv"));
    flextglTexImage3D = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLint, GLsizei, GLsizei, GLsizei, GLint, GLenum, GLenum, const void *)>(loader.load("glTexImage3D"));
    flextglTexStorage2D = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, GLsizei, GLsizei)>(loader.load("glTexStorage2D"));
    flextglTexStorage3D = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLsizei)>(loader.load("glTexStorage3D"));
    flextglTexSubImage3D = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, const void *)>(loader.load("glTexSubImage3D"));
    flextglTransformFeedbackVaryings = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, const GLchar *const*, GLenum)>(loader.load("glTransformFeedbackVaryings"));
    flextglUniform1ui = reinterpret_cast<void(APIENTRY*)(GLint, GLuint)>(loader.load("glUniform1ui"));
    flextglUniform1uiv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, const GLuint *)>(loader.load("glUniform1uiv"));
    flextglUniform2ui = reinterpret_cast<void(APIENTRY*)(GLint, GLuint, GLuint)>(loader.load("glUniform2ui"));
    flextglUniform2uiv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, const GLuint *)>(loader.load("glUniform2uiv"));
    flextglUniform3ui = reinterpret_cast<void(APIENTRY*)(GLint, GLuint, GLuint, GLuint)>(loader.load("glUniform3ui"));
    flextglUniform3uiv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, const GLuint *)>(loader.load("glUniform3uiv"));
    flextglUniform4ui = reinterpret_cast<void(APIENTRY*)(GLint, GLuint, GLuint, GLuint, GLuint)>(loader.load("glUniform4ui"));
    flextglUniform4uiv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, const GLuint *)>(loader.load("glUniform4uiv"));
    flextglUniformBlockBinding = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLuint)>(loader.load("glUniformBlockBinding"));
    flextglUniformMatrix2x3fv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glUniformMatrix2x3fv"));
    flextglUniformMatrix2x4fv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glUniformMatrix2x4fv"));
    flextglUniformMatrix3x2fv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glUniformMatrix3x2fv"));
    flextglUniformMatrix3x4fv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glUniformMatrix3x4fv"));
    flextglUniformMatrix4x2fv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glUniformMatrix4x2fv"));
    flextglUniformMatrix4x3fv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glUniformMatrix4x3fv"));
    flextglUnmapBuffer = reinterpret_cast<GLboolean(APIENTRY*)(GLenum)>(loader.load("glUnmapBuffer"));
    flextglVertexAttribDivisor = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint)>(loader.load("glVertexAttribDivisor"));
    flextglVertexAttribI4i = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLint, GLint)>(loader.load("glVertexAttribI4i"));
    flextglVertexAttribI4iv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLint *)>(loader.load("glVertexAttribI4iv"));
    flextglVertexAttribI4ui = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLuint, GLuint, GLuint)>(loader.load("glVertexAttribI4ui"));
    flextglVertexAttribI4uiv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLuint *)>(loader.load("glVertexAttribI4uiv"));
    flextglVertexAttribIPointer = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLenum, GLsizei, const void *)>(loader.load("glVertexAttribIPointer"));
    flextglWaitSync = reinterpret_cast<void(APIENTRY*)(GLsync, GLbitfield, GLuint64)>(loader.load("glWaitSync"));

    /* GL_ES_VERSION_3_1 */
    flextglActiveShaderProgram = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint)>(loader.load("glActiveShaderProgram"));
    flextglBindImageTexture = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLint, GLboolean, GLint, GLenum, GLenum)>(loader.load("glBindImageTexture"));
    flextglBindProgramPipeline = reinterpret_cast<void(APIENTRY*)(GLuint)>(loader.load("glBindProgramPipeline"));
    flextglBindVertexBuffer = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLintptr, GLsizei)>(loader.load("glBindVertexBuffer"));
    flextglCreateShaderProgramv = reinterpret_cast<GLuint(APIENTRY*)(GLenum, GLsizei, const GLchar *const*)>(loader.load("glCreateShaderProgramv"));
    flextglDeleteProgramPipelines = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLuint *)>(loader.load("glDeleteProgramPipelines"));
    flextglDispatchCompute = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLuint)>(loader.load("glDispatchCompute"));
    flextglDispatchComputeIndirect = reinterpret_cast<void(APIENTRY*)(GLintptr)>(loader.load("glDispatchComputeIndirect"));
    flextglDrawArraysIndirect = reinterpret_cast<void(APIENTRY*)(GLenum, const void *)>(loader.load("glDrawArraysIndirect"));
    flextglDrawElementsIndirect = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, const void *)>(loader.load("glDrawElementsIndirect"));
    flextglFramebufferParameteri = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLint)>(loader.load("glFramebufferParameteri"));
    flextglGenProgramPipelines = reinterpret_cast<void(APIENTRY*)(GLsizei, GLuint *)>(loader.load("glGenProgramPipelines"));
    flextglGetBooleani_v = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLboolean *)>(loader.load("glGetBooleani_v"));
    flextglGetFramebufferParameteriv = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLint *)>(loader.load("glGetFramebufferParameteriv"));
    flextglGetMultisamplefv = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLfloat *)>(loader.load("glGetMultisamplefv"));
    flextglGetProgramInterfaceiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLenum, GLint *)>(loader.load("glGetProgramInterfaceiv"));
    flextglGetProgramPipelineInfoLog = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, GLsizei *, GLchar *)>(loader.load("glGetProgramPipelineInfoLog"));
    flextglGetProgramPipelineiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint *)>(loader.load("glGetProgramPipelineiv"));
    flextglGetProgramResourceIndex = reinterpret_cast<GLuint(APIENTRY*)(GLuint, GLenum, const GLchar *)>(loader.load("glGetProgramResourceIndex"));
    flextglGetProgramResourceLocation = reinterpret_cast<GLint(APIENTRY*)(GLuint, GLenum, const GLchar *)>(loader.load("glGetProgramResourceLocation"));
    flextglGetProgramResourceName = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint, GLsizei, GLsizei *, GLchar *)>(loader.load("glGetProgramResourceName"));
    flextglGetProgramResourceiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint, GLsizei, const GLenum *, GLsizei, GLsizei *, GLint *)>(loader.load("glGetProgramResourceiv"));
    flextglIsProgramPipeline = reinterpret_cast<GLboolean(APIENTRY*)(GLuint)>(loader.load("glIsProgramPipeline"));
    flextglMemoryBarrier = reinterpret_cast<void(APIENTRY*)(GLbitfield)>(loader.load("glMemoryBarrier"));
    flextglMemoryBarrierByRegion = reinterpret_cast<void(APIENTRY*)(GLbitfield)>(loader.load("glMemoryBarrierByRegion"));
    flextglProgramUniform1f = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLfloat)>(loader.load("glProgramUniform1f"));
    flextglProgramUniform1fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLfloat *)>(loader.load("glProgramUniform1fv"));
    flextglProgramUniform1i = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint)>(loader.load("glProgramUniform1i"));
    flextglProgramUniform1iv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLint *)>(loader.load("glProgramUniform1iv"));
    flextglProgramUniform1ui = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLuint)>(loader.load("glProgramUniform1ui"));
    flextglProgramUniform1uiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLuint *)>(loader.load("glProgramUniform1uiv"));
    flextglProgramUniform2f = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLfloat, GLfloat)>(loader.load("glProgramUniform2f"));
    flextglProgramUniform2fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLfloat *)>(loader.load("glProgramUniform2fv"));
    flextglProgramUniform2i = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLint)>(loader.load("glProgramUniform2i"));
    flextglProgramUniform2iv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLint *)>(loader.load("glProgramUniform2iv"));
    flextglProgramUniform2ui = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLuint, GLuint)>(loader.load("glProgramUniform2ui"));
    flextglProgramUniform2uiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLuint *)>(loader.load("glProgramUniform2uiv"));
    flextglProgramUniform3f = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLfloat, GLfloat, GLfloat)>(loader.load("glProgramUniform3f"));
    flextglProgramUniform3fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLfloat *)>(loader.load("glProgramUniform3fv"));
    flextglProgramUniform3i = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLint, GLint)>(loader.load("glProgramUniform3i"));
    flextglProgramUniform3iv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLint *)>(loader.load("glProgramUniform3iv"));
    flextglProgramUniform3ui = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLuint, GLuint, GLuint)>(loader.load("glProgramUniform3ui"));
    flextglProgramUniform3uiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLuint *)>(loader.load("glProgramUniform3uiv"));
    flextglProgramUniform4f = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLfloat, GLfloat, GLfloat, GLfloat)>(loader.load("glProgramUniform4f"));
    flextglProgramUniform4fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLfloat *)>(loader.load("glProgramUniform4fv"));
    flextglProgramUniform4i = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLint, GLint, GLint)>(loader.load("glProgramUniform4i"));
    flextglProgramUniform4iv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLint *)>(loader.load("glProgramUniform4iv"));
    flextglProgramUniform4ui = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLuint, GLuint, GLuint, GLuint)>(loader.load("glProgramUniform4ui"));
    flextglProgramUniform4uiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLuint *)>(loader.load("glProgramUniform4uiv"));
    flextglProgramUniformMatrix2fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix2fv"));
    flextglProgramUniformMatrix2x3fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix2x3fv"));
    flextglProgramUniformMatrix2x4fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix2x4fv"));
    flextglProgramUniformMatrix3fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix3fv"));
    flextglProgramUniformMatrix3x2fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix3x2fv"));
    flextglProgramUniformMatrix3x4fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix3x4fv"));
    flextglProgramUniformMatrix4fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix4fv"));
    flextglProgramUniformMatrix4x2fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix4x2fv"));
    flextglProgramUniformMatrix4x3fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix4x3fv"));
    flextglSampleMaski = reinterpret_cast<void(APIENTRY*)(GLuint, GLbitfield)>(loader.load("glSampleMaski"));
    flextglTexStorage2DMultisample = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLboolean)>(loader.load("glTexStorage2DMultisample"));
    flextglUseProgramStages = reinterpret_cast<void(APIENTRY*)(GLuint, GLbitfield, GLuint)>(loader.load("glUseProgramStages"));
    flextglValidateProgramPipeline = reinterpret_cast<void(APIENTRY*)(GLuint)>(loader.load("glValidateProgramPipeline"));
    flextglVertexAttribBinding = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint)>(loader.load("glVertexAttribBinding"));
    flextglVertexAttribFormat = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLenum, GLboolean, GLuint)>(loader.load("glVertexAttribFormat"));
    flextglVertexAttribIFormat = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLenum, GLuint)>(loader.load("glVertexAttribIFormat"));
    flextglVertexBindingDivisor = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint)>(loader.load("glVertexBindingDivisor"));

    /* GL_EXT_copy_image */
    flextglCopyImageSubDataEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint, GLint, GLint, GLint, GLuint, GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei)>(loader.load("glCopyImageSubDataEXT"));

    /* GL_EXT_debug_label */
    flextglGetObjectLabelEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLsizei, GLsizei *, GLchar *)>(loader.load("glGetObjectLabelEXT"));
    flextglLabelObjectEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLsizei, const GLchar *)>(loader.load("glLabelObjectEXT"));

    /* GL_EXT_debug_marker */
    flextglInsertEventMarkerEXT = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLchar *)>(loader.load("glInsertEventMarkerEXT"));
    flextglPopGroupMarkerEXT = reinterpret_cast<void(APIENTRY*)(void)>(loader.load("glPopGroupMarkerEXT"));
    flextglPushGroupMarkerEXT = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLchar *)>(loader.load("glPushGroupMarkerEXT"));

    /* GL_EXT_disjoint_timer_query */
    flextglBeginQueryEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint)>(loader.load("glBeginQueryEXT"));
    flextglDeleteQueriesEXT = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLuint *)>(loader.load("glDeleteQueriesEXT"));
    flextglEndQueryEXT = reinterpret_cast<void(APIENTRY*)(GLenum)>(loader.load("glEndQueryEXT"));
    flextglGenQueriesEXT = reinterpret_cast<void(APIENTRY*)(GLsizei, GLuint *)>(loader.load("glGenQueriesEXT"));
    flextglGetQueryObjecti64vEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint64 *)>(loader.load("glGetQueryObjecti64vEXT"));
    flextglGetQueryObjectivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint *)>(loader.load("glGetQueryObjectivEXT"));
    flextglGetQueryObjectui64vEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint64 *)>(loader.load("glGetQueryObjectui64vEXT"));
    flextglGetQueryObjectuivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint *)>(loader.load("glGetQueryObjectuivEXT"));
    flextglGetQueryivEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLint *)>(loader.load("glGetQueryivEXT"));
    flextglIsQueryEXT = reinterpret_cast<GLboolean(APIENTRY*)(GLuint)>(loader.load("glIsQueryEXT"));
    flextglQueryCounterEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum)>(loader.load("glQueryCounterEXT"));

    /* GL_EXT_draw_buffers_indexed */
    flextglBlendEquationSeparateiEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLenum)>(loader.load("glBlendEquationSeparateiEXT"));
    flextglBlendEquationiEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum)>(loader.load("glBlendEquationiEXT"));
    flextglBlendFuncSeparateiEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLenum, GLenum, GLenum)>(loader.load("glBlendFuncSeparateiEXT"));
    flextglBlendFunciEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLenum)>(loader.load("glBlendFunciEXT"));
    flextglColorMaskiEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLboolean, GLboolean, GLboolean, GLboolean)>(loader.load("glColorMaskiEXT"));
    flextglDisableiEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint)>(loader.load("glDisableiEXT"));
    flextglEnableiEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint)>(loader.load("glEnableiEXT"));
    flextglIsEnablediEXT = reinterpret_cast<GLboolean(APIENTRY*)(GLenum, GLuint)>(loader.load("glIsEnablediEXT"));

    /* GL_EXT_geometry_shader */
    flextglFramebufferTextureEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLuint, GLint)>(loader.load("glFramebufferTextureEXT"));

    /* GL_EXT_multi_draw_arrays */
    flextglMultiDrawArraysEXT = reinterpret_cast<void(APIENTRY*)(GLenum, const GLint *, const GLsizei *, GLsizei)>(loader.load("glMultiDrawArraysEXT"));
    flextglMultiDrawElementsEXT = reinterpret_cast<void(APIENTRY*)(GLenum, const GLsizei *, GLenum, const void *const*, GLsizei)>(loader.load("glMultiDrawElementsEXT"));

    /* GL_EXT_multisampled_render_to_texture */
    flextglFramebufferTexture2DMultisampleEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLenum, GLuint, GLint, GLsizei)>(loader.load("glFramebufferTexture2DMultisampleEXT"));
    flextglRenderbufferStorageMultisampleEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, GLsizei, GLsizei)>(loader.load("glRenderbufferStorageMultisampleEXT"));

    /* GL_EXT_primitive_bounding_box */
    flextglPrimitiveBoundingBoxEXT = reinterpret_cast<void(APIENTRY*)(GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat)>(loader.load("glPrimitiveBoundingBoxEXT"));

    /* GL_EXT_robustness */
    flextglGetGraphicsResetStatusEXT = reinterpret_cast<GLenum(APIENTRY*)(void)>(loader.load("glGetGraphicsResetStatusEXT"));
    flextglGetnUniformfvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLfloat *)>(loader.load("glGetnUniformfvEXT"));
    flextglGetnUniformivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLint *)>(loader.load("glGetnUniformivEXT"));
    flextglReadnPixelsEXT = reinterpret_cast<void(APIENTRY*)(GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, GLsizei, void *)>(loader.load("glReadnPixelsEXT"));

    /* GL_EXT_separate_shader_objects */
    flextglActiveShaderProgramEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint)>(loader.load("glActiveShaderProgramEXT"));
    flextglBindProgramPipelineEXT = reinterpret_cast<void(APIENTRY*)(GLuint)>(loader.load("glBindProgramPipelineEXT"));
    flextglCreateShaderProgramvEXT = reinterpret_cast<GLuint(APIENTRY*)(GLenum, GLsizei, const GLchar **)>(loader.load("glCreateShaderProgramvEXT"));
    flextglDeleteProgramPipelinesEXT = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLuint *)>(loader.load("glDeleteProgramPipelinesEXT"));
    flextglGenProgramPipelinesEXT = reinterpret_cast<void(APIENTRY*)(GLsizei, GLuint *)>(loader.load("glGenProgramPipelinesEXT"));
    flextglGetProgramPipelineInfoLogEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, GLsizei *, GLchar *)>(loader.load("glGetProgramPipelineInfoLogEXT"));
    flextglGetProgramPipelineivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint *)>(loader.load("glGetProgramPipelineivEXT"));
    flextglIsProgramPipelineEXT = reinterpret_cast<GLboolean(APIENTRY*)(GLuint)>(loader.load("glIsProgramPipelineEXT"));
    flextglProgramParameteriEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint)>(loader.load("glProgramParameteriEXT"));
    flextglProgramUniform1fEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLfloat)>(loader.load("glProgramUniform1fEXT"));
    flextglProgramUniform1fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLfloat *)>(loader.load("glProgramUniform1fvEXT"));
    flextglProgramUniform1iEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint)>(loader.load("glProgramUniform1iEXT"));
    flextglProgramUniform1ivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLint *)>(loader.load("glProgramUniform1ivEXT"));
    flextglProgramUniform1uiEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLuint)>(loader.load("glProgramUniform1uiEXT"));
    flextglProgramUniform1uivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLuint *)>(loader.load("glProgramUniform1uivEXT"));
    flextglProgramUniform2fEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLfloat, GLfloat)>(loader.load("glProgramUniform2fEXT"));
    flextglProgramUniform2fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLfloat *)>(loader.load("glProgramUniform2fvEXT"));
    flextglProgramUniform2iEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLint)>(loader.load("glProgramUniform2iEXT"));
    flextglProgramUniform2ivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLint *)>(loader.load("glProgramUniform2ivEXT"));
    flextglProgramUniform2uiEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLuint, GLuint)>(loader.load("glProgramUniform2uiEXT"));
    flextglProgramUniform2uivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLuint *)>(loader.load("glProgramUniform2uivEXT"));
    flextglProgramUniform3fEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLfloat, GLfloat, GLfloat)>(loader.load("glProgramUniform3fEXT"));
    flextglProgramUniform3fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLfloat *)>(loader.load("glProgramUniform3fvEXT"));
    flextglProgramUniform3iEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLint, GLint)>(loader.load("glProgramUniform3iEXT"));
    flextglProgramUniform3ivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLint *)>(loader.load("glProgramUniform3ivEXT"));
    flextglProgramUniform3uiEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLuint, GLuint, GLuint)>(loader.load("glProgramUniform3uiEXT"));
    flextglProgramUniform3uivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLuint *)>(loader.load("glProgramUniform3uivEXT"));
    flextglProgramUniform4fEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLfloat, GLfloat, GLfloat, GLfloat)>(loader.load("glProgramUniform4fEXT"));
    flextglProgramUniform4fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLfloat *)>(loader.load("glProgramUniform4fvEXT"));
    flextglProgramUniform4iEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLint, GLint, GLint)>(loader.load("glProgramUniform4iEXT"));
    flextglProgramUniform4ivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLint *)>(loader.load("glProgramUniform4ivEXT"));
    flextglProgramUniform4uiEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLuint, GLuint, GLuint, GLuint)>(loader.load("glProgramUniform4uiEXT"));
    flextglProgramUniform4uivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLuint *)>(loader.load("glProgramUniform4uivEXT"));
    flextglProgramUniformMatrix2fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix2fvEXT"));
    flextglProgramUniformMatrix2x3fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix2x3fvEXT"));
    flextglProgramUniformMatrix2x4fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix2x4fvEXT"));
    flextglProgramUniformMatrix3fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix3fvEXT"));
    flextglProgramUniformMatrix3x2fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix3x2fvEXT"));
    flextglProgramUniformMatrix3x4fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix3x4fvEXT"));
    flextglProgramUniformMatrix4fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix4fvEXT"));
    flextglProgramUniformMatrix4x2fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix4x2fvEXT"));
    flextglProgramUniformMatrix4x3fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix4x3fvEXT"));
    flextglUseProgramStagesEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLbitfield, GLuint)>(loader.load("glUseProgramStagesEXT"));
    flextglValidateProgramPipelineEXT = reinterpret_cast<void(APIENTRY*)(GLuint)>(loader.load("glValidateProgramPipelineEXT"));

    /* GL_EXT_tessellation_shader */
    flextglPatchParameteriEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLint)>(loader.load("glPatchParameteriEXT"));

    /* GL_EXT_texture_border_clamp */
    flextglGetSamplerParameterIivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint *)>(loader.load("glGetSamplerParameterIivEXT"));
    flextglGetSamplerParameterIuivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint *)>(loader.load("glGetSamplerParameterIuivEXT"));
    flextglGetTexParameterIivEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLint *)>(loader.load("glGetTexParameterIivEXT"));
    flextglGetTexParameterIuivEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLuint *)>(loader.load("glGetTexParameterIuivEXT"));
    flextglSamplerParameterIivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, const GLint *)>(loader.load("glSamplerParameterIivEXT"));
    flextglSamplerParameterIuivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, const GLuint *)>(loader.load("glSamplerParameterIuivEXT"));
    flextglTexParameterIivEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, const GLint *)>(loader.load("glTexParameterIivEXT"));
    flextglTexParameterIuivEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, const GLuint *)>(loader.load("glTexParameterIuivEXT"));

    /* GL_EXT_texture_buffer */
    flextglTexBufferEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLuint)>(loader.load("glTexBufferEXT"));
    flextglTexBufferRangeEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLuint, GLintptr, GLsizeiptr)>(loader.load("glTexBufferRangeEXT"));

    /* GL_KHR_blend_equation_advanced */
    flextglBlendBarrierKHR = reinterpret_cast<void(APIENTRY*)(void)>(loader.load("glBlendBarrierKHR"));

    /* GL_KHR_debug */
    flextglDebugMessageCallbackKHR = reinterpret_cast<void(APIENTRY*)(GLDEBUGPROCKHR, const void *)>(loader.load("glDebugMessageCallbackKHR"));
    flextglDebugMessageControlKHR = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLenum, GLsizei, const GLuint *, GLboolean)>(loader.load("glDebugMessageControlKHR"));
    flextglDebugMessageInsertKHR = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar *)>(loader.load("glDebugMessageInsertKHR"));
    flextglGetDebugMessageLogKHR = reinterpret_cast<GLuint(APIENTRY*)(GLuint, GLsizei, GLenum *, GLenum *, GLuint *, GLenum *, GLsizei *, GLchar *)>(loader.load("glGetDebugMessageLogKHR"));
    flextglGetObjectLabelKHR = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLsizei, GLsizei *, GLchar *)>(loader.load("glGetObjectLabelKHR"));
    flextglGetObjectPtrLabelKHR = reinterpret_cast<void(APIENTRY*)(const void *, GLsizei, GLsizei *, GLchar *)>(loader.load("glGetObjectPtrLabelKHR"));
    flextglGetPointervKHR = reinterpret_cast<void(APIENTRY*)(GLenum, void **)>(loader.load("glGetPointervKHR"));
    flextglObjectLabelKHR = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLsizei, const GLchar *)>(loader.load("glObjectLabelKHR"));
    flextglObjectPtrLabelKHR = reinterpret_cast<void(APIENTRY*)(const void *, GLsizei, const GLchar *)>(loader.load("glObjectPtrLabelKHR"));
    flextglPopDebugGroupKHR = reinterpret_cast<void(APIENTRY*)(void)>(loader.load("glPopDebugGroupKHR"));
    flextglPushDebugGroupKHR = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLsizei, const GLchar *)>(loader.load("glPushDebugGroupKHR"));

    /* GL_KHR_robustness */
    flextglGetGraphicsResetStatusKHR = reinterpret_cast<GLenum(APIENTRY*)(void)>(loader.load("glGetGraphicsResetStatusKHR"));
    flextglGetnUniformfvKHR = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLfloat *)>(loader.load("glGetnUniformfvKHR"));
    flextglGetnUniformivKHR = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLint *)>(loader.load("glGetnUniformivKHR"));
    flextglGetnUniformuivKHR = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLuint *)>(loader.load("glGetnUniformuivKHR"));
    flextglReadnPixelsKHR = reinterpret_cast<void(APIENTRY*)(GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, GLsizei, void *)>(loader.load("glReadnPixelsKHR"));

    /* GL_NV_polygon_mode */
    flextglPolygonModeNV = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum)>(loader.load("glPolygonModeNV"));

    /* GL_OES_mapbuffer */
    flextglGetBufferPointervOES = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, void **)>(loader.load("glGetBufferPointervOES"));
    flextglMapBufferOES = reinterpret_cast<void *(APIENTRY*)(GLenum, GLenum)>(loader.load("glMapBufferOES"));
    flextglUnmapBufferOES = reinterpret_cast<GLboolean(APIENTRY*)(GLenum)>(loader.load("glUnmapBufferOES"));

    /* GL_OES_sample_shading */
    flextglMinSampleShadingOES = reinterpret_cast<void(APIENTRY*)(GLfloat)>(loader.load("glMinSampleShadingOES"));

    /* GL_OES_texture_storage_multisample_2d_array */
    flextglTexStorage3DMultisampleOES = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLsizei, GLboolean)>(loader.load("glTexStorage3DMultisampleOES"));
}
