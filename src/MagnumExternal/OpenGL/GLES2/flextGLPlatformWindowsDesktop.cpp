#include "flextGLWindowsDesktop.h"

#include <cstdio>
#include <cstdlib>

#include "Magnum/Platform/Implementation/OpenGLFunctionLoader.h"

void flextGLInit() {
    Magnum::Platform::Implementation::OpenGLFunctionLoader loader;

    /* GL_ANGLE_framebuffer_blit */
    flextglBlitFramebufferANGLE = reinterpret_cast<void(APIENTRY*)(GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLbitfield, GLenum)>(loader.load("glBlitFramebufferANGLE"));

    /* GL_ANGLE_framebuffer_multisample */
    flextglRenderbufferStorageMultisampleANGLE = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, GLsizei, GLsizei)>(loader.load("glRenderbufferStorageMultisampleANGLE"));

    /* GL_ANGLE_instanced_arrays */
    flextglDrawArraysInstancedANGLE = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLsizei, GLsizei)>(loader.load("glDrawArraysInstancedANGLE"));
    flextglDrawElementsInstancedANGLE = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, const void *, GLsizei)>(loader.load("glDrawElementsInstancedANGLE"));
    flextglVertexAttribDivisorANGLE = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint)>(loader.load("glVertexAttribDivisorANGLE"));

    /* GL_APPLE_framebuffer_multisample */
    flextglRenderbufferStorageMultisampleAPPLE = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, GLsizei, GLsizei)>(loader.load("glRenderbufferStorageMultisampleAPPLE"));
    flextglResolveMultisampleFramebufferAPPLE = reinterpret_cast<void(APIENTRY*)(void)>(loader.load("glResolveMultisampleFramebufferAPPLE"));

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

    /* GL_EXT_debug_label */
    flextglGetObjectLabelEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLsizei, GLsizei *, GLchar *)>(loader.load("glGetObjectLabelEXT"));
    flextglLabelObjectEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLsizei, const GLchar *)>(loader.load("glLabelObjectEXT"));

    /* GL_EXT_debug_marker */
    flextglInsertEventMarkerEXT = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLchar *)>(loader.load("glInsertEventMarkerEXT"));
    flextglPopGroupMarkerEXT = reinterpret_cast<void(APIENTRY*)(void)>(loader.load("glPopGroupMarkerEXT"));
    flextglPushGroupMarkerEXT = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLchar *)>(loader.load("glPushGroupMarkerEXT"));

    /* GL_EXT_discard_framebuffer */
    flextglDiscardFramebufferEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, const GLenum *)>(loader.load("glDiscardFramebufferEXT"));

    /* GL_EXT_disjoint_timer_query */
    flextglGetQueryObjecti64vEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint64 *)>(loader.load("glGetQueryObjecti64vEXT"));
    flextglGetQueryObjectivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint *)>(loader.load("glGetQueryObjectivEXT"));
    flextglGetQueryObjectui64vEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint64 *)>(loader.load("glGetQueryObjectui64vEXT"));
    flextglQueryCounterEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum)>(loader.load("glQueryCounterEXT"));

    /* GL_EXT_draw_buffers */
    flextglDrawBuffersEXT = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLenum *)>(loader.load("glDrawBuffersEXT"));

    /* GL_EXT_draw_buffers_indexed */
    flextglBlendEquationSeparateiEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLenum)>(loader.load("glBlendEquationSeparateiEXT"));
    flextglBlendEquationiEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum)>(loader.load("glBlendEquationiEXT"));
    flextglBlendFuncSeparateiEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLenum, GLenum, GLenum)>(loader.load("glBlendFuncSeparateiEXT"));
    flextglBlendFunciEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLenum)>(loader.load("glBlendFunciEXT"));
    flextglColorMaskiEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLboolean, GLboolean, GLboolean, GLboolean)>(loader.load("glColorMaskiEXT"));
    flextglDisableiEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint)>(loader.load("glDisableiEXT"));
    flextglEnableiEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint)>(loader.load("glEnableiEXT"));
    flextglIsEnablediEXT = reinterpret_cast<GLboolean(APIENTRY*)(GLenum, GLuint)>(loader.load("glIsEnablediEXT"));

    /* GL_EXT_instanced_arrays */
    flextglDrawArraysInstancedEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLsizei, GLsizei)>(loader.load("glDrawArraysInstancedEXT"));
    flextglDrawElementsInstancedEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, const void *, GLsizei)>(loader.load("glDrawElementsInstancedEXT"));
    flextglVertexAttribDivisorEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint)>(loader.load("glVertexAttribDivisorEXT"));

    /* GL_EXT_map_buffer_range */
    flextglFlushMappedBufferRangeEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLintptr, GLsizeiptr)>(loader.load("glFlushMappedBufferRangeEXT"));
    flextglMapBufferRangeEXT = reinterpret_cast<void *(APIENTRY*)(GLenum, GLintptr, GLsizeiptr, GLbitfield)>(loader.load("glMapBufferRangeEXT"));

    /* GL_EXT_multi_draw_arrays */
    flextglMultiDrawArraysEXT = reinterpret_cast<void(APIENTRY*)(GLenum, const GLint *, const GLsizei *, GLsizei)>(loader.load("glMultiDrawArraysEXT"));
    flextglMultiDrawElementsEXT = reinterpret_cast<void(APIENTRY*)(GLenum, const GLsizei *, GLenum, const void *const*, GLsizei)>(loader.load("glMultiDrawElementsEXT"));

    /* GL_EXT_multisampled_render_to_texture */
    flextglFramebufferTexture2DMultisampleEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLenum, GLuint, GLint, GLsizei)>(loader.load("glFramebufferTexture2DMultisampleEXT"));
    flextglRenderbufferStorageMultisampleEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, GLsizei, GLsizei)>(loader.load("glRenderbufferStorageMultisampleEXT"));

    /* GL_EXT_occlusion_query_boolean */
    flextglBeginQueryEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint)>(loader.load("glBeginQueryEXT"));
    flextglDeleteQueriesEXT = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLuint *)>(loader.load("glDeleteQueriesEXT"));
    flextglEndQueryEXT = reinterpret_cast<void(APIENTRY*)(GLenum)>(loader.load("glEndQueryEXT"));
    flextglGenQueriesEXT = reinterpret_cast<void(APIENTRY*)(GLsizei, GLuint *)>(loader.load("glGenQueriesEXT"));
    flextglGetQueryObjectuivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint *)>(loader.load("glGetQueryObjectuivEXT"));
    flextglGetQueryivEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLint *)>(loader.load("glGetQueryivEXT"));
    flextglIsQueryEXT = reinterpret_cast<GLboolean(APIENTRY*)(GLuint)>(loader.load("glIsQueryEXT"));

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
    flextglProgramUniform2fEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLfloat, GLfloat)>(loader.load("glProgramUniform2fEXT"));
    flextglProgramUniform2fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLfloat *)>(loader.load("glProgramUniform2fvEXT"));
    flextglProgramUniform2iEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLint)>(loader.load("glProgramUniform2iEXT"));
    flextglProgramUniform2ivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLint *)>(loader.load("glProgramUniform2ivEXT"));
    flextglProgramUniform3fEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLfloat, GLfloat, GLfloat)>(loader.load("glProgramUniform3fEXT"));
    flextglProgramUniform3fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLfloat *)>(loader.load("glProgramUniform3fvEXT"));
    flextglProgramUniform3iEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLint, GLint)>(loader.load("glProgramUniform3iEXT"));
    flextglProgramUniform3ivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLint *)>(loader.load("glProgramUniform3ivEXT"));
    flextglProgramUniform4fEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLfloat, GLfloat, GLfloat, GLfloat)>(loader.load("glProgramUniform4fEXT"));
    flextglProgramUniform4fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLfloat *)>(loader.load("glProgramUniform4fvEXT"));
    flextglProgramUniform4iEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLint, GLint, GLint)>(loader.load("glProgramUniform4iEXT"));
    flextglProgramUniform4ivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLint *)>(loader.load("glProgramUniform4ivEXT"));
    flextglProgramUniformMatrix2fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix2fvEXT"));
    flextglProgramUniformMatrix3fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix3fvEXT"));
    flextglProgramUniformMatrix4fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix4fvEXT"));
    flextglUseProgramStagesEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLbitfield, GLuint)>(loader.load("glUseProgramStagesEXT"));
    flextglValidateProgramPipelineEXT = reinterpret_cast<void(APIENTRY*)(GLuint)>(loader.load("glValidateProgramPipelineEXT"));

    /* GL_EXT_texture_border_clamp */
    flextglGetSamplerParameterIivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint *)>(loader.load("glGetSamplerParameterIivEXT"));
    flextglGetSamplerParameterIuivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint *)>(loader.load("glGetSamplerParameterIuivEXT"));
    flextglGetTexParameterIivEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLint *)>(loader.load("glGetTexParameterIivEXT"));
    flextglGetTexParameterIuivEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLuint *)>(loader.load("glGetTexParameterIuivEXT"));
    flextglSamplerParameterIivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, const GLint *)>(loader.load("glSamplerParameterIivEXT"));
    flextglSamplerParameterIuivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, const GLuint *)>(loader.load("glSamplerParameterIuivEXT"));
    flextglTexParameterIivEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, const GLint *)>(loader.load("glTexParameterIivEXT"));
    flextglTexParameterIuivEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, const GLuint *)>(loader.load("glTexParameterIuivEXT"));

    /* GL_EXT_texture_storage */
    flextglTexStorage2DEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, GLsizei, GLsizei)>(loader.load("glTexStorage2DEXT"));
    flextglTexStorage3DEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLsizei)>(loader.load("glTexStorage3DEXT"));

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

    /* GL_NV_draw_buffers */
    flextglDrawBuffersNV = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLenum *)>(loader.load("glDrawBuffersNV"));

    /* GL_NV_draw_instanced */
    flextglDrawArraysInstancedNV = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLsizei, GLsizei)>(loader.load("glDrawArraysInstancedNV"));
    flextglDrawElementsInstancedNV = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, const void *, GLsizei)>(loader.load("glDrawElementsInstancedNV"));

    /* GL_NV_framebuffer_blit */
    flextglBlitFramebufferNV = reinterpret_cast<void(APIENTRY*)(GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLbitfield, GLenum)>(loader.load("glBlitFramebufferNV"));

    /* GL_NV_framebuffer_multisample */
    flextglRenderbufferStorageMultisampleNV = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, GLsizei, GLsizei)>(loader.load("glRenderbufferStorageMultisampleNV"));

    /* GL_NV_instanced_arrays */
    flextglVertexAttribDivisorNV = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint)>(loader.load("glVertexAttribDivisorNV"));

    /* GL_NV_polygon_mode */
    flextglPolygonModeNV = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum)>(loader.load("glPolygonModeNV"));

    /* GL_NV_read_buffer */
    flextglReadBufferNV = reinterpret_cast<void(APIENTRY*)(GLenum)>(loader.load("glReadBufferNV"));

    /* GL_OES_mapbuffer */
    flextglGetBufferPointervOES = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, void **)>(loader.load("glGetBufferPointervOES"));
    flextglMapBufferOES = reinterpret_cast<void *(APIENTRY*)(GLenum, GLenum)>(loader.load("glMapBufferOES"));
    flextglUnmapBufferOES = reinterpret_cast<GLboolean(APIENTRY*)(GLenum)>(loader.load("glUnmapBufferOES"));

    /* GL_OES_texture_3D */
    flextglCompressedTexImage3DOES = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLenum, GLsizei, GLsizei, GLsizei, GLint, GLsizei, const void *)>(loader.load("glCompressedTexImage3DOES"));
    flextglCompressedTexSubImage3DOES = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLsizei, const void *)>(loader.load("glCompressedTexSubImage3DOES"));
    flextglCopyTexSubImage3DOES = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei)>(loader.load("glCopyTexSubImage3DOES"));
    flextglFramebufferTexture3DOES = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLenum, GLuint, GLint, GLint)>(loader.load("glFramebufferTexture3DOES"));
    flextglTexImage3DOES = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLenum, GLsizei, GLsizei, GLsizei, GLint, GLenum, GLenum, const void *)>(loader.load("glTexImage3DOES"));
    flextglTexSubImage3DOES = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, const void *)>(loader.load("glTexSubImage3DOES"));

    /* GL_OES_vertex_array_object */
    flextglBindVertexArrayOES = reinterpret_cast<void(APIENTRY*)(GLuint)>(loader.load("glBindVertexArrayOES"));
    flextglDeleteVertexArraysOES = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLuint *)>(loader.load("glDeleteVertexArraysOES"));
    flextglGenVertexArraysOES = reinterpret_cast<void(APIENTRY*)(GLsizei, GLuint *)>(loader.load("glGenVertexArraysOES"));
    flextglIsVertexArrayOES = reinterpret_cast<GLboolean(APIENTRY*)(GLuint)>(loader.load("glIsVertexArrayOES"));
}
