#include "flextGL.h"

#undef glActiveShaderProgram
#undef glBindImageTexture
#undef glBindProgramPipeline
#undef glBindVertexBuffer
#undef glCreateShaderProgramv
#undef glDeleteProgramPipelines
#undef glDispatchCompute
#undef glDispatchComputeIndirect
#undef glDrawArraysIndirect
#undef glDrawElementsIndirect
#undef glFramebufferParameteri
#undef glGenProgramPipelines
#undef glGetBooleani_v
#undef glGetFramebufferParameteriv
#undef glGetMultisamplefv
#undef glGetProgramInterfaceiv
#undef glGetProgramPipelineInfoLog
#undef glGetProgramPipelineiv
#undef glGetProgramResourceIndex
#undef glGetProgramResourceLocation
#undef glGetProgramResourceName
#undef glGetProgramResourceiv
#undef glGetTexLevelParameterfv
#undef glGetTexLevelParameteriv
#undef glIsProgramPipeline
#undef glMemoryBarrier
#undef glMemoryBarrierByRegion
#undef glProgramUniform1f
#undef glProgramUniform1fv
#undef glProgramUniform1i
#undef glProgramUniform1iv
#undef glProgramUniform1ui
#undef glProgramUniform1uiv
#undef glProgramUniform2f
#undef glProgramUniform2fv
#undef glProgramUniform2i
#undef glProgramUniform2iv
#undef glProgramUniform2ui
#undef glProgramUniform2uiv
#undef glProgramUniform3f
#undef glProgramUniform3fv
#undef glProgramUniform3i
#undef glProgramUniform3iv
#undef glProgramUniform3ui
#undef glProgramUniform3uiv
#undef glProgramUniform4f
#undef glProgramUniform4fv
#undef glProgramUniform4i
#undef glProgramUniform4iv
#undef glProgramUniform4ui
#undef glProgramUniform4uiv
#undef glProgramUniformMatrix2fv
#undef glProgramUniformMatrix2x3fv
#undef glProgramUniformMatrix2x4fv
#undef glProgramUniformMatrix3fv
#undef glProgramUniformMatrix3x2fv
#undef glProgramUniformMatrix3x4fv
#undef glProgramUniformMatrix4fv
#undef glProgramUniformMatrix4x2fv
#undef glProgramUniformMatrix4x3fv
#undef glSampleMaski
#undef glTexStorage2DMultisample
#undef glUseProgramStages
#undef glValidateProgramPipeline
#undef glVertexAttribBinding
#undef glVertexAttribFormat
#undef glVertexAttribIFormat
#undef glVertexBindingDivisor
#undef glCopyImageSubDataEXT
#undef glGetObjectLabelEXT
#undef glLabelObjectEXT
#undef glInsertEventMarkerEXT
#undef glPopGroupMarkerEXT
#undef glPushGroupMarkerEXT
#undef glBeginQueryEXT
#undef glDeleteQueriesEXT
#undef glEndQueryEXT
#undef glGenQueriesEXT
#undef glGetQueryObjecti64vEXT
#undef glGetQueryObjectivEXT
#undef glGetQueryObjectui64vEXT
#undef glGetQueryObjectuivEXT
#undef glGetQueryivEXT
#undef glIsQueryEXT
#undef glQueryCounterEXT
#undef glBlendEquationSeparateiEXT
#undef glBlendEquationiEXT
#undef glBlendFuncSeparateiEXT
#undef glBlendFunciEXT
#undef glColorMaskiEXT
#undef glDisableiEXT
#undef glEnableiEXT
#undef glIsEnablediEXT
#undef glFramebufferTextureEXT
#undef glMultiDrawArraysEXT
#undef glMultiDrawElementsEXT
#undef glFramebufferTexture2DMultisampleEXT
#undef glRenderbufferStorageMultisampleEXT
#undef glPrimitiveBoundingBoxEXT
#undef glGetGraphicsResetStatusEXT
#undef glGetnUniformfvEXT
#undef glGetnUniformivEXT
#undef glReadnPixelsEXT
#undef glActiveShaderProgramEXT
#undef glBindProgramPipelineEXT
#undef glCreateShaderProgramvEXT
#undef glDeleteProgramPipelinesEXT
#undef glGenProgramPipelinesEXT
#undef glGetProgramPipelineInfoLogEXT
#undef glGetProgramPipelineivEXT
#undef glIsProgramPipelineEXT
#undef glProgramParameteriEXT
#undef glProgramUniform1fEXT
#undef glProgramUniform1fvEXT
#undef glProgramUniform1iEXT
#undef glProgramUniform1ivEXT
#undef glProgramUniform1uiEXT
#undef glProgramUniform1uivEXT
#undef glProgramUniform2fEXT
#undef glProgramUniform2fvEXT
#undef glProgramUniform2iEXT
#undef glProgramUniform2ivEXT
#undef glProgramUniform2uiEXT
#undef glProgramUniform2uivEXT
#undef glProgramUniform3fEXT
#undef glProgramUniform3fvEXT
#undef glProgramUniform3iEXT
#undef glProgramUniform3ivEXT
#undef glProgramUniform3uiEXT
#undef glProgramUniform3uivEXT
#undef glProgramUniform4fEXT
#undef glProgramUniform4fvEXT
#undef glProgramUniform4iEXT
#undef glProgramUniform4ivEXT
#undef glProgramUniform4uiEXT
#undef glProgramUniform4uivEXT
#undef glProgramUniformMatrix2fvEXT
#undef glProgramUniformMatrix2x3fvEXT
#undef glProgramUniformMatrix2x4fvEXT
#undef glProgramUniformMatrix3fvEXT
#undef glProgramUniformMatrix3x2fvEXT
#undef glProgramUniformMatrix3x4fvEXT
#undef glProgramUniformMatrix4fvEXT
#undef glProgramUniformMatrix4x2fvEXT
#undef glProgramUniformMatrix4x3fvEXT
#undef glUseProgramStagesEXT
#undef glValidateProgramPipelineEXT
#undef glPatchParameteriEXT
#undef glGetSamplerParameterIivEXT
#undef glGetSamplerParameterIuivEXT
#undef glGetTexParameterIivEXT
#undef glGetTexParameterIuivEXT
#undef glSamplerParameterIivEXT
#undef glSamplerParameterIuivEXT
#undef glTexParameterIivEXT
#undef glTexParameterIuivEXT
#undef glTexBufferEXT
#undef glTexBufferRangeEXT
#undef glBlendBarrierKHR
#undef glDebugMessageCallbackKHR
#undef glDebugMessageControlKHR
#undef glDebugMessageInsertKHR
#undef glGetDebugMessageLogKHR
#undef glGetObjectLabelKHR
#undef glGetObjectPtrLabelKHR
#undef glGetPointervKHR
#undef glObjectLabelKHR
#undef glObjectPtrLabelKHR
#undef glPopDebugGroupKHR
#undef glPushDebugGroupKHR
#undef glGetGraphicsResetStatusKHR
#undef glGetnUniformfvKHR
#undef glGetnUniformivKHR
#undef glGetnUniformuivKHR
#undef glReadnPixelsKHR
#undef glPolygonModeNV
#undef glGetBufferPointervOES
#undef glMapBufferOES
#undef glUnmapBufferOES
#undef glMinSampleShadingOES
#undef glTexStorage3DMultisampleOES

#include <ES3/glext.h>

void flextGLInit() {

    /* GL_ES_VERSION_3_1 */
    #if GL_ES_VERSION_3_1
    flextglActiveShaderProgram = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint)>(glActiveShaderProgram);
    flextglBindImageTexture = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLint, GLboolean, GLint, GLenum, GLenum)>(glBindImageTexture);
    flextglBindProgramPipeline = reinterpret_cast<void(APIENTRY*)(GLuint)>(glBindProgramPipeline);
    flextglBindVertexBuffer = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLintptr, GLsizei)>(glBindVertexBuffer);
    flextglCreateShaderProgramv = reinterpret_cast<GLuint(APIENTRY*)(GLenum, GLsizei, const GLchar *const*)>(glCreateShaderProgramv);
    flextglDeleteProgramPipelines = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLuint *)>(glDeleteProgramPipelines);
    flextglDispatchCompute = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLuint)>(glDispatchCompute);
    flextglDispatchComputeIndirect = reinterpret_cast<void(APIENTRY*)(GLintptr)>(glDispatchComputeIndirect);
    flextglDrawArraysIndirect = reinterpret_cast<void(APIENTRY*)(GLenum, const void *)>(glDrawArraysIndirect);
    flextglDrawElementsIndirect = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, const void *)>(glDrawElementsIndirect);
    flextglFramebufferParameteri = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLint)>(glFramebufferParameteri);
    flextglGenProgramPipelines = reinterpret_cast<void(APIENTRY*)(GLsizei, GLuint *)>(glGenProgramPipelines);
    flextglGetBooleani_v = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLboolean *)>(glGetBooleani_v);
    flextglGetFramebufferParameteriv = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLint *)>(glGetFramebufferParameteriv);
    flextglGetMultisamplefv = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLfloat *)>(glGetMultisamplefv);
    flextglGetProgramInterfaceiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLenum, GLint *)>(glGetProgramInterfaceiv);
    flextglGetProgramPipelineInfoLog = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, GLsizei *, GLchar *)>(glGetProgramPipelineInfoLog);
    flextglGetProgramPipelineiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint *)>(glGetProgramPipelineiv);
    flextglGetProgramResourceIndex = reinterpret_cast<GLuint(APIENTRY*)(GLuint, GLenum, const GLchar *)>(glGetProgramResourceIndex);
    flextglGetProgramResourceLocation = reinterpret_cast<GLint(APIENTRY*)(GLuint, GLenum, const GLchar *)>(glGetProgramResourceLocation);
    flextglGetProgramResourceName = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint, GLsizei, GLsizei *, GLchar *)>(glGetProgramResourceName);
    flextglGetProgramResourceiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint, GLsizei, const GLenum *, GLsizei, GLsizei *, GLint *)>(glGetProgramResourceiv);
    flextglGetTexLevelParameterfv = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLenum, GLfloat *)>(glGetTexLevelParameterfv);
    flextglGetTexLevelParameteriv = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLenum, GLint *)>(glGetTexLevelParameteriv);
    flextglIsProgramPipeline = reinterpret_cast<GLboolean(APIENTRY*)(GLuint)>(glIsProgramPipeline);
    flextglMemoryBarrier = reinterpret_cast<void(APIENTRY*)(GLbitfield)>(glMemoryBarrier);
    flextglMemoryBarrierByRegion = reinterpret_cast<void(APIENTRY*)(GLbitfield)>(glMemoryBarrierByRegion);
    flextglProgramUniform1f = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLfloat)>(glProgramUniform1f);
    flextglProgramUniform1fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLfloat *)>(glProgramUniform1fv);
    flextglProgramUniform1i = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint)>(glProgramUniform1i);
    flextglProgramUniform1iv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLint *)>(glProgramUniform1iv);
    flextglProgramUniform1ui = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLuint)>(glProgramUniform1ui);
    flextglProgramUniform1uiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLuint *)>(glProgramUniform1uiv);
    flextglProgramUniform2f = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLfloat, GLfloat)>(glProgramUniform2f);
    flextglProgramUniform2fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLfloat *)>(glProgramUniform2fv);
    flextglProgramUniform2i = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLint)>(glProgramUniform2i);
    flextglProgramUniform2iv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLint *)>(glProgramUniform2iv);
    flextglProgramUniform2ui = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLuint, GLuint)>(glProgramUniform2ui);
    flextglProgramUniform2uiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLuint *)>(glProgramUniform2uiv);
    flextglProgramUniform3f = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLfloat, GLfloat, GLfloat)>(glProgramUniform3f);
    flextglProgramUniform3fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLfloat *)>(glProgramUniform3fv);
    flextglProgramUniform3i = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLint, GLint)>(glProgramUniform3i);
    flextglProgramUniform3iv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLint *)>(glProgramUniform3iv);
    flextglProgramUniform3ui = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLuint, GLuint, GLuint)>(glProgramUniform3ui);
    flextglProgramUniform3uiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLuint *)>(glProgramUniform3uiv);
    flextglProgramUniform4f = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLfloat, GLfloat, GLfloat, GLfloat)>(glProgramUniform4f);
    flextglProgramUniform4fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLfloat *)>(glProgramUniform4fv);
    flextglProgramUniform4i = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLint, GLint, GLint)>(glProgramUniform4i);
    flextglProgramUniform4iv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLint *)>(glProgramUniform4iv);
    flextglProgramUniform4ui = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLuint, GLuint, GLuint, GLuint)>(glProgramUniform4ui);
    flextglProgramUniform4uiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLuint *)>(glProgramUniform4uiv);
    flextglProgramUniformMatrix2fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(glProgramUniformMatrix2fv);
    flextglProgramUniformMatrix2x3fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(glProgramUniformMatrix2x3fv);
    flextglProgramUniformMatrix2x4fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(glProgramUniformMatrix2x4fv);
    flextglProgramUniformMatrix3fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(glProgramUniformMatrix3fv);
    flextglProgramUniformMatrix3x2fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(glProgramUniformMatrix3x2fv);
    flextglProgramUniformMatrix3x4fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(glProgramUniformMatrix3x4fv);
    flextglProgramUniformMatrix4fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(glProgramUniformMatrix4fv);
    flextglProgramUniformMatrix4x2fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(glProgramUniformMatrix4x2fv);
    flextglProgramUniformMatrix4x3fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(glProgramUniformMatrix4x3fv);
    flextglSampleMaski = reinterpret_cast<void(APIENTRY*)(GLuint, GLbitfield)>(glSampleMaski);
    flextglTexStorage2DMultisample = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLboolean)>(glTexStorage2DMultisample);
    flextglUseProgramStages = reinterpret_cast<void(APIENTRY*)(GLuint, GLbitfield, GLuint)>(glUseProgramStages);
    flextglValidateProgramPipeline = reinterpret_cast<void(APIENTRY*)(GLuint)>(glValidateProgramPipeline);
    flextglVertexAttribBinding = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint)>(glVertexAttribBinding);
    flextglVertexAttribFormat = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLenum, GLboolean, GLuint)>(glVertexAttribFormat);
    flextglVertexAttribIFormat = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLenum, GLuint)>(glVertexAttribIFormat);
    flextglVertexBindingDivisor = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint)>(glVertexBindingDivisor);
    #endif

    /* GL_EXT_copy_image */
    #if GL_EXT_copy_image
    flextglCopyImageSubDataEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint, GLint, GLint, GLint, GLuint, GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei)>(glCopyImageSubDataEXT);
    #endif

    /* GL_EXT_debug_label */
    #if GL_EXT_debug_label
    flextglGetObjectLabelEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLsizei, GLsizei *, GLchar *)>(glGetObjectLabelEXT);
    flextglLabelObjectEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLsizei, const GLchar *)>(glLabelObjectEXT);
    #endif

    /* GL_EXT_debug_marker */
    #if GL_EXT_debug_marker
    flextglInsertEventMarkerEXT = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLchar *)>(glInsertEventMarkerEXT);
    flextglPopGroupMarkerEXT = reinterpret_cast<void(APIENTRY*)(void)>(glPopGroupMarkerEXT);
    flextglPushGroupMarkerEXT = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLchar *)>(glPushGroupMarkerEXT);
    #endif

    /* GL_EXT_disjoint_timer_query */
    #if GL_EXT_disjoint_timer_query
    flextglBeginQueryEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint)>(glBeginQueryEXT);
    flextglDeleteQueriesEXT = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLuint *)>(glDeleteQueriesEXT);
    flextglEndQueryEXT = reinterpret_cast<void(APIENTRY*)(GLenum)>(glEndQueryEXT);
    flextglGenQueriesEXT = reinterpret_cast<void(APIENTRY*)(GLsizei, GLuint *)>(glGenQueriesEXT);
    flextglGetQueryObjecti64vEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint64 *)>(glGetQueryObjecti64vEXT);
    flextglGetQueryObjectivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint *)>(glGetQueryObjectivEXT);
    flextglGetQueryObjectui64vEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint64 *)>(glGetQueryObjectui64vEXT);
    flextglGetQueryObjectuivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint *)>(glGetQueryObjectuivEXT);
    flextglGetQueryivEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLint *)>(glGetQueryivEXT);
    flextglIsQueryEXT = reinterpret_cast<GLboolean(APIENTRY*)(GLuint)>(glIsQueryEXT);
    flextglQueryCounterEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum)>(glQueryCounterEXT);
    #endif

    /* GL_EXT_draw_buffers_indexed */
    #if GL_EXT_draw_buffers_indexed
    flextglBlendEquationSeparateiEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLenum)>(glBlendEquationSeparateiEXT);
    flextglBlendEquationiEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum)>(glBlendEquationiEXT);
    flextglBlendFuncSeparateiEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLenum, GLenum, GLenum)>(glBlendFuncSeparateiEXT);
    flextglBlendFunciEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLenum)>(glBlendFunciEXT);
    flextglColorMaskiEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLboolean, GLboolean, GLboolean, GLboolean)>(glColorMaskiEXT);
    flextglDisableiEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint)>(glDisableiEXT);
    flextglEnableiEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint)>(glEnableiEXT);
    flextglIsEnablediEXT = reinterpret_cast<GLboolean(APIENTRY*)(GLenum, GLuint)>(glIsEnablediEXT);
    #endif

    /* GL_EXT_geometry_shader */
    #if GL_EXT_geometry_shader
    flextglFramebufferTextureEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLuint, GLint)>(glFramebufferTextureEXT);
    #endif

    /* GL_EXT_multi_draw_arrays */
    #if GL_EXT_multi_draw_arrays
    flextglMultiDrawArraysEXT = reinterpret_cast<void(APIENTRY*)(GLenum, const GLint *, const GLsizei *, GLsizei)>(glMultiDrawArraysEXT);
    flextglMultiDrawElementsEXT = reinterpret_cast<void(APIENTRY*)(GLenum, const GLsizei *, GLenum, const void *const*, GLsizei)>(glMultiDrawElementsEXT);
    #endif

    /* GL_EXT_multisampled_render_to_texture */
    #if GL_EXT_multisampled_render_to_texture
    flextglFramebufferTexture2DMultisampleEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLenum, GLuint, GLint, GLsizei)>(glFramebufferTexture2DMultisampleEXT);
    flextglRenderbufferStorageMultisampleEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, GLsizei, GLsizei)>(glRenderbufferStorageMultisampleEXT);
    #endif

    /* GL_EXT_primitive_bounding_box */
    #if GL_EXT_primitive_bounding_box
    flextglPrimitiveBoundingBoxEXT = reinterpret_cast<void(APIENTRY*)(GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat)>(glPrimitiveBoundingBoxEXT);
    #endif

    /* GL_EXT_robustness */
    #if GL_EXT_robustness
    flextglGetGraphicsResetStatusEXT = reinterpret_cast<GLenum(APIENTRY*)(void)>(glGetGraphicsResetStatusEXT);
    flextglGetnUniformfvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLfloat *)>(glGetnUniformfvEXT);
    flextglGetnUniformivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLint *)>(glGetnUniformivEXT);
    flextglReadnPixelsEXT = reinterpret_cast<void(APIENTRY*)(GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, GLsizei, void *)>(glReadnPixelsEXT);
    #endif

    /* GL_EXT_separate_shader_objects */
    #if GL_EXT_separate_shader_objects
    flextglActiveShaderProgramEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint)>(glActiveShaderProgramEXT);
    flextglBindProgramPipelineEXT = reinterpret_cast<void(APIENTRY*)(GLuint)>(glBindProgramPipelineEXT);
    flextglCreateShaderProgramvEXT = reinterpret_cast<GLuint(APIENTRY*)(GLenum, GLsizei, const GLchar **)>(glCreateShaderProgramvEXT);
    flextglDeleteProgramPipelinesEXT = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLuint *)>(glDeleteProgramPipelinesEXT);
    flextglGenProgramPipelinesEXT = reinterpret_cast<void(APIENTRY*)(GLsizei, GLuint *)>(glGenProgramPipelinesEXT);
    flextglGetProgramPipelineInfoLogEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, GLsizei *, GLchar *)>(glGetProgramPipelineInfoLogEXT);
    flextglGetProgramPipelineivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint *)>(glGetProgramPipelineivEXT);
    flextglIsProgramPipelineEXT = reinterpret_cast<GLboolean(APIENTRY*)(GLuint)>(glIsProgramPipelineEXT);
    flextglProgramParameteriEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint)>(glProgramParameteriEXT);
    flextglProgramUniform1fEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLfloat)>(glProgramUniform1fEXT);
    flextglProgramUniform1fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLfloat *)>(glProgramUniform1fvEXT);
    flextglProgramUniform1iEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint)>(glProgramUniform1iEXT);
    flextglProgramUniform1ivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLint *)>(glProgramUniform1ivEXT);
    flextglProgramUniform1uiEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLuint)>(glProgramUniform1uiEXT);
    flextglProgramUniform1uivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLuint *)>(glProgramUniform1uivEXT);
    flextglProgramUniform2fEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLfloat, GLfloat)>(glProgramUniform2fEXT);
    flextglProgramUniform2fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLfloat *)>(glProgramUniform2fvEXT);
    flextglProgramUniform2iEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLint)>(glProgramUniform2iEXT);
    flextglProgramUniform2ivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLint *)>(glProgramUniform2ivEXT);
    flextglProgramUniform2uiEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLuint, GLuint)>(glProgramUniform2uiEXT);
    flextglProgramUniform2uivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLuint *)>(glProgramUniform2uivEXT);
    flextglProgramUniform3fEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLfloat, GLfloat, GLfloat)>(glProgramUniform3fEXT);
    flextglProgramUniform3fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLfloat *)>(glProgramUniform3fvEXT);
    flextglProgramUniform3iEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLint, GLint)>(glProgramUniform3iEXT);
    flextglProgramUniform3ivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLint *)>(glProgramUniform3ivEXT);
    flextglProgramUniform3uiEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLuint, GLuint, GLuint)>(glProgramUniform3uiEXT);
    flextglProgramUniform3uivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLuint *)>(glProgramUniform3uivEXT);
    flextglProgramUniform4fEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLfloat, GLfloat, GLfloat, GLfloat)>(glProgramUniform4fEXT);
    flextglProgramUniform4fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLfloat *)>(glProgramUniform4fvEXT);
    flextglProgramUniform4iEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLint, GLint, GLint)>(glProgramUniform4iEXT);
    flextglProgramUniform4ivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLint *)>(glProgramUniform4ivEXT);
    flextglProgramUniform4uiEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLuint, GLuint, GLuint, GLuint)>(glProgramUniform4uiEXT);
    flextglProgramUniform4uivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLuint *)>(glProgramUniform4uivEXT);
    flextglProgramUniformMatrix2fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(glProgramUniformMatrix2fvEXT);
    flextglProgramUniformMatrix2x3fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(glProgramUniformMatrix2x3fvEXT);
    flextglProgramUniformMatrix2x4fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(glProgramUniformMatrix2x4fvEXT);
    flextglProgramUniformMatrix3fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(glProgramUniformMatrix3fvEXT);
    flextglProgramUniformMatrix3x2fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(glProgramUniformMatrix3x2fvEXT);
    flextglProgramUniformMatrix3x4fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(glProgramUniformMatrix3x4fvEXT);
    flextglProgramUniformMatrix4fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(glProgramUniformMatrix4fvEXT);
    flextglProgramUniformMatrix4x2fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(glProgramUniformMatrix4x2fvEXT);
    flextglProgramUniformMatrix4x3fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(glProgramUniformMatrix4x3fvEXT);
    flextglUseProgramStagesEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLbitfield, GLuint)>(glUseProgramStagesEXT);
    flextglValidateProgramPipelineEXT = reinterpret_cast<void(APIENTRY*)(GLuint)>(glValidateProgramPipelineEXT);
    #endif

    /* GL_EXT_tessellation_shader */
    #if GL_EXT_tessellation_shader
    flextglPatchParameteriEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLint)>(glPatchParameteriEXT);
    #endif

    /* GL_EXT_texture_border_clamp */
    #if GL_EXT_texture_border_clamp
    flextglGetSamplerParameterIivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint *)>(glGetSamplerParameterIivEXT);
    flextglGetSamplerParameterIuivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint *)>(glGetSamplerParameterIuivEXT);
    flextglGetTexParameterIivEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLint *)>(glGetTexParameterIivEXT);
    flextglGetTexParameterIuivEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLuint *)>(glGetTexParameterIuivEXT);
    flextglSamplerParameterIivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, const GLint *)>(glSamplerParameterIivEXT);
    flextglSamplerParameterIuivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, const GLuint *)>(glSamplerParameterIuivEXT);
    flextglTexParameterIivEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, const GLint *)>(glTexParameterIivEXT);
    flextglTexParameterIuivEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, const GLuint *)>(glTexParameterIuivEXT);
    #endif

    /* GL_EXT_texture_buffer */
    #if GL_EXT_texture_buffer
    flextglTexBufferEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLuint)>(glTexBufferEXT);
    flextglTexBufferRangeEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLuint, GLintptr, GLsizeiptr)>(glTexBufferRangeEXT);
    #endif

    /* GL_KHR_blend_equation_advanced */
    #if GL_KHR_blend_equation_advanced
    flextglBlendBarrierKHR = reinterpret_cast<void(APIENTRY*)(void)>(glBlendBarrierKHR);
    #endif

    /* GL_KHR_debug */
    #if GL_KHR_debug
    flextglDebugMessageCallbackKHR = reinterpret_cast<void(APIENTRY*)(GLDEBUGPROCKHR, const void *)>(glDebugMessageCallbackKHR);
    flextglDebugMessageControlKHR = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLenum, GLsizei, const GLuint *, GLboolean)>(glDebugMessageControlKHR);
    flextglDebugMessageInsertKHR = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar *)>(glDebugMessageInsertKHR);
    flextglGetDebugMessageLogKHR = reinterpret_cast<GLuint(APIENTRY*)(GLuint, GLsizei, GLenum *, GLenum *, GLuint *, GLenum *, GLsizei *, GLchar *)>(glGetDebugMessageLogKHR);
    flextglGetObjectLabelKHR = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLsizei, GLsizei *, GLchar *)>(glGetObjectLabelKHR);
    flextglGetObjectPtrLabelKHR = reinterpret_cast<void(APIENTRY*)(const void *, GLsizei, GLsizei *, GLchar *)>(glGetObjectPtrLabelKHR);
    flextglGetPointervKHR = reinterpret_cast<void(APIENTRY*)(GLenum, void **)>(glGetPointervKHR);
    flextglObjectLabelKHR = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLsizei, const GLchar *)>(glObjectLabelKHR);
    flextglObjectPtrLabelKHR = reinterpret_cast<void(APIENTRY*)(const void *, GLsizei, const GLchar *)>(glObjectPtrLabelKHR);
    flextglPopDebugGroupKHR = reinterpret_cast<void(APIENTRY*)(void)>(glPopDebugGroupKHR);
    flextglPushDebugGroupKHR = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLsizei, const GLchar *)>(glPushDebugGroupKHR);
    #endif

    /* GL_KHR_robustness */
    #if GL_KHR_robustness
    flextglGetGraphicsResetStatusKHR = reinterpret_cast<GLenum(APIENTRY*)(void)>(glGetGraphicsResetStatusKHR);
    flextglGetnUniformfvKHR = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLfloat *)>(glGetnUniformfvKHR);
    flextglGetnUniformivKHR = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLint *)>(glGetnUniformivKHR);
    flextglGetnUniformuivKHR = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLuint *)>(glGetnUniformuivKHR);
    flextglReadnPixelsKHR = reinterpret_cast<void(APIENTRY*)(GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, GLsizei, void *)>(glReadnPixelsKHR);
    #endif

    /* GL_NV_polygon_mode */
    #if GL_NV_polygon_mode
    flextglPolygonModeNV = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum)>(glPolygonModeNV);
    #endif

    /* GL_OES_mapbuffer */
    #if GL_OES_mapbuffer
    flextglGetBufferPointervOES = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, void **)>(glGetBufferPointervOES);
    flextglMapBufferOES = reinterpret_cast<void *(APIENTRY*)(GLenum, GLenum)>(glMapBufferOES);
    flextglUnmapBufferOES = reinterpret_cast<GLboolean(APIENTRY*)(GLenum)>(glUnmapBufferOES);
    #endif

    /* GL_OES_sample_shading */
    #if GL_OES_sample_shading
    flextglMinSampleShadingOES = reinterpret_cast<void(APIENTRY*)(GLfloat)>(glMinSampleShadingOES);
    #endif

    /* GL_OES_texture_storage_multisample_2d_array */
    #if GL_OES_texture_storage_multisample_2d_array
    flextglTexStorage3DMultisampleOES = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLsizei, GLboolean)>(glTexStorage3DMultisampleOES);
    #endif
}
