#include "flextGL.h"

#include <cstdio>
#include <cstdlib>

#include "Magnum/Platform/Implementation/OpenGLFunctionLoader.h"

void flextGLInit() {
    Magnum::Platform::Implementation::OpenGLFunctionLoader loader;

    /* GL_ES_VERSION_3_1 */
    flextglDispatchCompute = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLuint)>(loader.load("glDispatchCompute"));
    flextglDispatchComputeIndirect = reinterpret_cast<void(APIENTRY*)(GLintptr)>(loader.load("glDispatchComputeIndirect"));
    flextglDrawArraysIndirect = reinterpret_cast<void(APIENTRY*)(GLenum, const void *)>(loader.load("glDrawArraysIndirect"));
    flextglDrawElementsIndirect = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, const void *)>(loader.load("glDrawElementsIndirect"));
    flextglFramebufferParameteri = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLint)>(loader.load("glFramebufferParameteri"));
    flextglGetFramebufferParameteriv = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLint *)>(loader.load("glGetFramebufferParameteriv"));
    flextglGetProgramInterfaceiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLenum, GLint *)>(loader.load("glGetProgramInterfaceiv"));
    flextglGetProgramResourceIndex = reinterpret_cast<GLuint(APIENTRY*)(GLuint, GLenum, const GLchar *)>(loader.load("glGetProgramResourceIndex"));
    flextglGetProgramResourceName = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint, GLsizei, GLsizei *, GLchar *)>(loader.load("glGetProgramResourceName"));
    flextglGetProgramResourceiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint, GLsizei, const GLenum *, GLsizei, GLsizei *, GLint *)>(loader.load("glGetProgramResourceiv"));
    flextglGetProgramResourceLocation = reinterpret_cast<GLint(APIENTRY*)(GLuint, GLenum, const GLchar *)>(loader.load("glGetProgramResourceLocation"));
    flextglUseProgramStages = reinterpret_cast<void(APIENTRY*)(GLuint, GLbitfield, GLuint)>(loader.load("glUseProgramStages"));
    flextglActiveShaderProgram = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint)>(loader.load("glActiveShaderProgram"));
    flextglCreateShaderProgramv = reinterpret_cast<GLuint(APIENTRY*)(GLenum, GLsizei, const GLchar *const*)>(loader.load("glCreateShaderProgramv"));
    flextglBindProgramPipeline = reinterpret_cast<void(APIENTRY*)(GLuint)>(loader.load("glBindProgramPipeline"));
    flextglDeleteProgramPipelines = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLuint *)>(loader.load("glDeleteProgramPipelines"));
    flextglGenProgramPipelines = reinterpret_cast<void(APIENTRY*)(GLsizei, GLuint *)>(loader.load("glGenProgramPipelines"));
    flextglIsProgramPipeline = reinterpret_cast<GLboolean(APIENTRY*)(GLuint)>(loader.load("glIsProgramPipeline"));
    flextglGetProgramPipelineiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint *)>(loader.load("glGetProgramPipelineiv"));
    flextglProgramUniform1i = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint)>(loader.load("glProgramUniform1i"));
    flextglProgramUniform2i = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLint)>(loader.load("glProgramUniform2i"));
    flextglProgramUniform3i = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLint, GLint)>(loader.load("glProgramUniform3i"));
    flextglProgramUniform4i = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLint, GLint, GLint)>(loader.load("glProgramUniform4i"));
    flextglProgramUniform1ui = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLuint)>(loader.load("glProgramUniform1ui"));
    flextglProgramUniform2ui = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLuint, GLuint)>(loader.load("glProgramUniform2ui"));
    flextglProgramUniform3ui = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLuint, GLuint, GLuint)>(loader.load("glProgramUniform3ui"));
    flextglProgramUniform4ui = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLuint, GLuint, GLuint, GLuint)>(loader.load("glProgramUniform4ui"));
    flextglProgramUniform1f = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLfloat)>(loader.load("glProgramUniform1f"));
    flextglProgramUniform2f = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLfloat, GLfloat)>(loader.load("glProgramUniform2f"));
    flextglProgramUniform3f = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLfloat, GLfloat, GLfloat)>(loader.load("glProgramUniform3f"));
    flextglProgramUniform4f = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLfloat, GLfloat, GLfloat, GLfloat)>(loader.load("glProgramUniform4f"));
    flextglProgramUniform1iv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLint *)>(loader.load("glProgramUniform1iv"));
    flextglProgramUniform2iv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLint *)>(loader.load("glProgramUniform2iv"));
    flextglProgramUniform3iv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLint *)>(loader.load("glProgramUniform3iv"));
    flextglProgramUniform4iv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLint *)>(loader.load("glProgramUniform4iv"));
    flextglProgramUniform1uiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLuint *)>(loader.load("glProgramUniform1uiv"));
    flextglProgramUniform2uiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLuint *)>(loader.load("glProgramUniform2uiv"));
    flextglProgramUniform3uiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLuint *)>(loader.load("glProgramUniform3uiv"));
    flextglProgramUniform4uiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLuint *)>(loader.load("glProgramUniform4uiv"));
    flextglProgramUniform1fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLfloat *)>(loader.load("glProgramUniform1fv"));
    flextglProgramUniform2fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLfloat *)>(loader.load("glProgramUniform2fv"));
    flextglProgramUniform3fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLfloat *)>(loader.load("glProgramUniform3fv"));
    flextglProgramUniform4fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLfloat *)>(loader.load("glProgramUniform4fv"));
    flextglProgramUniformMatrix2fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix2fv"));
    flextglProgramUniformMatrix3fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix3fv"));
    flextglProgramUniformMatrix4fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix4fv"));
    flextglProgramUniformMatrix2x3fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix2x3fv"));
    flextglProgramUniformMatrix3x2fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix3x2fv"));
    flextglProgramUniformMatrix2x4fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix2x4fv"));
    flextglProgramUniformMatrix4x2fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix4x2fv"));
    flextglProgramUniformMatrix3x4fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix3x4fv"));
    flextglProgramUniformMatrix4x3fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix4x3fv"));
    flextglValidateProgramPipeline = reinterpret_cast<void(APIENTRY*)(GLuint)>(loader.load("glValidateProgramPipeline"));
    flextglGetProgramPipelineInfoLog = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, GLsizei *, GLchar *)>(loader.load("glGetProgramPipelineInfoLog"));
    flextglBindImageTexture = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLint, GLboolean, GLint, GLenum, GLenum)>(loader.load("glBindImageTexture"));
    flextglGetBooleani_v = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLboolean *)>(loader.load("glGetBooleani_v"));
    flextglMemoryBarrier = reinterpret_cast<void(APIENTRY*)(GLbitfield)>(loader.load("glMemoryBarrier"));
    flextglMemoryBarrierByRegion = reinterpret_cast<void(APIENTRY*)(GLbitfield)>(loader.load("glMemoryBarrierByRegion"));
    flextglTexStorage2DMultisample = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLboolean)>(loader.load("glTexStorage2DMultisample"));
    flextglGetMultisamplefv = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLfloat *)>(loader.load("glGetMultisamplefv"));
    flextglSampleMaski = reinterpret_cast<void(APIENTRY*)(GLuint, GLbitfield)>(loader.load("glSampleMaski"));
    flextglGetTexLevelParameteriv = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLenum, GLint *)>(loader.load("glGetTexLevelParameteriv"));
    flextglGetTexLevelParameterfv = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLenum, GLfloat *)>(loader.load("glGetTexLevelParameterfv"));
    flextglBindVertexBuffer = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLintptr, GLsizei)>(loader.load("glBindVertexBuffer"));
    flextglVertexAttribFormat = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLenum, GLboolean, GLuint)>(loader.load("glVertexAttribFormat"));
    flextglVertexAttribIFormat = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLenum, GLuint)>(loader.load("glVertexAttribIFormat"));
    flextglVertexAttribBinding = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint)>(loader.load("glVertexAttribBinding"));
    flextglVertexBindingDivisor = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint)>(loader.load("glVertexBindingDivisor"));

    /* GL_APPLE_texture_format_BGRA8888 */

    /* GL_EXT_texture_filter_anisotropic */

    /* GL_EXT_texture_format_BGRA8888 */

    /* GL_EXT_read_format_bgra */

    /* GL_EXT_multi_draw_arrays */
    flextglMultiDrawArraysEXT = reinterpret_cast<void(APIENTRY*)(GLenum, const GLint *, const GLsizei *, GLsizei)>(loader.load("glMultiDrawArraysEXT"));
    flextglMultiDrawElementsEXT = reinterpret_cast<void(APIENTRY*)(GLenum, const GLsizei *, GLenum, const void *const*, GLsizei)>(loader.load("glMultiDrawElementsEXT"));

    /* GL_EXT_debug_label */
    flextglLabelObjectEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLsizei, const GLchar *)>(loader.load("glLabelObjectEXT"));
    flextglGetObjectLabelEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLsizei, GLsizei *, GLchar *)>(loader.load("glGetObjectLabelEXT"));

    /* GL_EXT_debug_marker */
    flextglInsertEventMarkerEXT = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLchar *)>(loader.load("glInsertEventMarkerEXT"));
    flextglPushGroupMarkerEXT = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLchar *)>(loader.load("glPushGroupMarkerEXT"));
    flextglPopGroupMarkerEXT = reinterpret_cast<void(APIENTRY*)(void)>(loader.load("glPopGroupMarkerEXT"));

    /* GL_EXT_disjoint_timer_query */
    flextglGenQueriesEXT = reinterpret_cast<void(APIENTRY*)(GLsizei, GLuint *)>(loader.load("glGenQueriesEXT"));
    flextglDeleteQueriesEXT = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLuint *)>(loader.load("glDeleteQueriesEXT"));
    flextglIsQueryEXT = reinterpret_cast<GLboolean(APIENTRY*)(GLuint)>(loader.load("glIsQueryEXT"));
    flextglBeginQueryEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint)>(loader.load("glBeginQueryEXT"));
    flextglEndQueryEXT = reinterpret_cast<void(APIENTRY*)(GLenum)>(loader.load("glEndQueryEXT"));
    flextglQueryCounterEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum)>(loader.load("glQueryCounterEXT"));
    flextglGetQueryivEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLint *)>(loader.load("glGetQueryivEXT"));
    flextglGetQueryObjectivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint *)>(loader.load("glGetQueryObjectivEXT"));
    flextglGetQueryObjectuivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint *)>(loader.load("glGetQueryObjectuivEXT"));
    flextglGetQueryObjecti64vEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint64 *)>(loader.load("glGetQueryObjecti64vEXT"));
    flextglGetQueryObjectui64vEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint64 *)>(loader.load("glGetQueryObjectui64vEXT"));

    /* GL_EXT_texture_sRGB_decode */

    /* GL_EXT_separate_shader_objects */
    flextglUseShaderProgramEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint)>(loader.load("glUseShaderProgramEXT"));
    flextglActiveProgramEXT = reinterpret_cast<void(APIENTRY*)(GLuint)>(loader.load("glActiveProgramEXT"));
    flextglCreateShaderProgramEXT = reinterpret_cast<GLuint(APIENTRY*)(GLenum, const GLchar *)>(loader.load("glCreateShaderProgramEXT"));
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
    flextglProgramUniform1uiEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLuint)>(loader.load("glProgramUniform1uiEXT"));
    flextglProgramUniform2uiEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLuint, GLuint)>(loader.load("glProgramUniform2uiEXT"));
    flextglProgramUniform3uiEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLuint, GLuint, GLuint)>(loader.load("glProgramUniform3uiEXT"));
    flextglProgramUniform4uiEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLuint, GLuint, GLuint, GLuint)>(loader.load("glProgramUniform4uiEXT"));
    flextglProgramUniform1uivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLuint *)>(loader.load("glProgramUniform1uivEXT"));
    flextglProgramUniform2uivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLuint *)>(loader.load("glProgramUniform2uivEXT"));
    flextglProgramUniform3uivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLuint *)>(loader.load("glProgramUniform3uivEXT"));
    flextglProgramUniform4uivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLuint *)>(loader.load("glProgramUniform4uivEXT"));
    flextglProgramUniformMatrix2x3fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix2x3fvEXT"));
    flextglProgramUniformMatrix3x2fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix3x2fvEXT"));
    flextglProgramUniformMatrix2x4fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix2x4fvEXT"));
    flextglProgramUniformMatrix4x2fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix4x2fvEXT"));
    flextglProgramUniformMatrix3x4fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix3x4fvEXT"));
    flextglProgramUniformMatrix4x3fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix4x3fvEXT"));

    /* GL_EXT_sRGB */

    /* GL_EXT_multisampled_render_to_texture */
    flextglRenderbufferStorageMultisampleEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, GLsizei, GLsizei)>(loader.load("glRenderbufferStorageMultisampleEXT"));
    flextglFramebufferTexture2DMultisampleEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLenum, GLuint, GLint, GLsizei)>(loader.load("glFramebufferTexture2DMultisampleEXT"));

    /* GL_EXT_robustness */
    flextglGetGraphicsResetStatusEXT = reinterpret_cast<GLenum(APIENTRY*)(void)>(loader.load("glGetGraphicsResetStatusEXT"));
    flextglReadnPixelsEXT = reinterpret_cast<void(APIENTRY*)(GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, GLsizei, void *)>(loader.load("glReadnPixelsEXT"));
    flextglGetnUniformfvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLfloat *)>(loader.load("glGetnUniformfvEXT"));
    flextglGetnUniformivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLint *)>(loader.load("glGetnUniformivEXT"));

    /* GL_KHR_debug */
    flextglDebugMessageControl = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLenum, GLsizei, const GLuint *, GLboolean)>(loader.load("glDebugMessageControl"));
    flextglDebugMessageInsert = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar *)>(loader.load("glDebugMessageInsert"));
    flextglDebugMessageCallback = reinterpret_cast<void(APIENTRY*)(GLDEBUGPROC, const void *)>(loader.load("glDebugMessageCallback"));
    flextglGetDebugMessageLog = reinterpret_cast<GLuint(APIENTRY*)(GLuint, GLsizei, GLenum *, GLenum *, GLuint *, GLenum *, GLsizei *, GLchar *)>(loader.load("glGetDebugMessageLog"));
    flextglPushDebugGroup = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLsizei, const GLchar *)>(loader.load("glPushDebugGroup"));
    flextglPopDebugGroup = reinterpret_cast<void(APIENTRY*)(void)>(loader.load("glPopDebugGroup"));
    flextglObjectLabel = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLsizei, const GLchar *)>(loader.load("glObjectLabel"));
    flextglGetObjectLabel = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLsizei, GLsizei *, GLchar *)>(loader.load("glGetObjectLabel"));
    flextglObjectPtrLabel = reinterpret_cast<void(APIENTRY*)(const void *, GLsizei, const GLchar *)>(loader.load("glObjectPtrLabel"));
    flextglGetObjectPtrLabel = reinterpret_cast<void(APIENTRY*)(const void *, GLsizei, GLsizei *, GLchar *)>(loader.load("glGetObjectPtrLabel"));
    flextglGetPointerv = reinterpret_cast<void(APIENTRY*)(GLenum, void **)>(loader.load("glGetPointerv"));
    flextglDebugMessageControlKHR = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLenum, GLsizei, const GLuint *, GLboolean)>(loader.load("glDebugMessageControlKHR"));
    flextglDebugMessageInsertKHR = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar *)>(loader.load("glDebugMessageInsertKHR"));
    flextglDebugMessageCallbackKHR = reinterpret_cast<void(APIENTRY*)(GLDEBUGPROCKHR, const void *)>(loader.load("glDebugMessageCallbackKHR"));
    flextglGetDebugMessageLogKHR = reinterpret_cast<GLuint(APIENTRY*)(GLuint, GLsizei, GLenum *, GLenum *, GLuint *, GLenum *, GLsizei *, GLchar *)>(loader.load("glGetDebugMessageLogKHR"));
    flextglPushDebugGroupKHR = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLsizei, const GLchar *)>(loader.load("glPushDebugGroupKHR"));
    flextglPopDebugGroupKHR = reinterpret_cast<void(APIENTRY*)(void)>(loader.load("glPopDebugGroupKHR"));
    flextglObjectLabelKHR = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLsizei, const GLchar *)>(loader.load("glObjectLabelKHR"));
    flextglGetObjectLabelKHR = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLsizei, GLsizei *, GLchar *)>(loader.load("glGetObjectLabelKHR"));
    flextglObjectPtrLabelKHR = reinterpret_cast<void(APIENTRY*)(const void *, GLsizei, const GLchar *)>(loader.load("glObjectPtrLabelKHR"));
    flextglGetObjectPtrLabelKHR = reinterpret_cast<void(APIENTRY*)(const void *, GLsizei, GLsizei *, GLchar *)>(loader.load("glGetObjectPtrLabelKHR"));
    flextglGetPointervKHR = reinterpret_cast<void(APIENTRY*)(GLenum, void **)>(loader.load("glGetPointervKHR"));

    /* GL_NV_read_buffer_front */

    /* GL_NV_read_depth */

    /* GL_NV_read_stencil */

    /* GL_NV_read_depth_stencil */

    /* GL_NV_texture_border_clamp */

    /* GL_OES_depth32 */

    /* GL_OES_mapbuffer */
    flextglMapBufferOES = reinterpret_cast<void *(APIENTRY*)(GLenum, GLenum)>(loader.load("glMapBufferOES"));
    flextglUnmapBufferOES = reinterpret_cast<GLboolean(APIENTRY*)(GLenum)>(loader.load("glUnmapBufferOES"));
    flextglGetBufferPointervOES = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, void **)>(loader.load("glGetBufferPointervOES"));

    /* GL_OES_stencil1 */

    /* GL_OES_stencil4 */
}
