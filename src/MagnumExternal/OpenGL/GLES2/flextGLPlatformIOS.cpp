#include "flextGL.h"

#undef glBlitFramebufferANGLE
#undef glRenderbufferStorageMultisampleANGLE
#undef glDrawArraysInstancedANGLE
#undef glDrawElementsInstancedANGLE
#undef glVertexAttribDivisorANGLE
#undef glRenderbufferStorageMultisampleAPPLE
#undef glResolveMultisampleFramebufferAPPLE
#undef glGetObjectLabelEXT
#undef glLabelObjectEXT
#undef glInsertEventMarkerEXT
#undef glPopGroupMarkerEXT
#undef glPushGroupMarkerEXT
#undef glDiscardFramebufferEXT
#undef glGetQueryObjecti64vEXT
#undef glGetQueryObjectivEXT
#undef glGetQueryObjectui64vEXT
#undef glQueryCounterEXT
#undef glDrawBuffersEXT
#undef glBlendEquationSeparateiEXT
#undef glBlendEquationiEXT
#undef glBlendFuncSeparateiEXT
#undef glBlendFunciEXT
#undef glColorMaskiEXT
#undef glDisableiEXT
#undef glEnableiEXT
#undef glIsEnablediEXT
#undef glDrawArraysInstancedEXT
#undef glDrawElementsInstancedEXT
#undef glVertexAttribDivisorEXT
#undef glFlushMappedBufferRangeEXT
#undef glMapBufferRangeEXT
#undef glMultiDrawArraysEXT
#undef glMultiDrawElementsEXT
#undef glFramebufferTexture2DMultisampleEXT
#undef glRenderbufferStorageMultisampleEXT
#undef glBeginQueryEXT
#undef glDeleteQueriesEXT
#undef glEndQueryEXT
#undef glGenQueriesEXT
#undef glGetQueryObjectuivEXT
#undef glGetQueryivEXT
#undef glIsQueryEXT
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
#undef glProgramUniform2fEXT
#undef glProgramUniform2fvEXT
#undef glProgramUniform2iEXT
#undef glProgramUniform2ivEXT
#undef glProgramUniform3fEXT
#undef glProgramUniform3fvEXT
#undef glProgramUniform3iEXT
#undef glProgramUniform3ivEXT
#undef glProgramUniform4fEXT
#undef glProgramUniform4fvEXT
#undef glProgramUniform4iEXT
#undef glProgramUniform4ivEXT
#undef glProgramUniformMatrix2fvEXT
#undef glProgramUniformMatrix3fvEXT
#undef glProgramUniformMatrix4fvEXT
#undef glUseProgramStagesEXT
#undef glValidateProgramPipelineEXT
#undef glGetSamplerParameterIivEXT
#undef glGetSamplerParameterIuivEXT
#undef glGetTexParameterIivEXT
#undef glGetTexParameterIuivEXT
#undef glSamplerParameterIivEXT
#undef glSamplerParameterIuivEXT
#undef glTexParameterIivEXT
#undef glTexParameterIuivEXT
#undef glTexStorage2DEXT
#undef glTexStorage3DEXT
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
#undef glDrawBuffersNV
#undef glDrawArraysInstancedNV
#undef glDrawElementsInstancedNV
#undef glBlitFramebufferNV
#undef glRenderbufferStorageMultisampleNV
#undef glVertexAttribDivisorNV
#undef glPolygonModeNV
#undef glReadBufferNV
#undef glGetBufferPointervOES
#undef glMapBufferOES
#undef glUnmapBufferOES
#undef glCompressedTexImage3DOES
#undef glCompressedTexSubImage3DOES
#undef glCopyTexSubImage3DOES
#undef glFramebufferTexture3DOES
#undef glTexImage3DOES
#undef glTexSubImage3DOES
#undef glBindVertexArrayOES
#undef glDeleteVertexArraysOES
#undef glGenVertexArraysOES
#undef glIsVertexArrayOES

#include <ES2/glext.h>

void flextGLInit() {
    /* Work around missing glTexStorage3D (can't be used anyway because GLES2
       on iOS doesn't support OES_texture_3D) */
    constexpr void(*glTexStorage3DEXT)() = nullptr;

    /* GL_ANGLE_framebuffer_blit */
    #if GL_ANGLE_framebuffer_blit
    flextglBlitFramebufferANGLE = reinterpret_cast<void(APIENTRY*)(GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLbitfield, GLenum)>(glBlitFramebufferANGLE);
    #endif

    /* GL_ANGLE_framebuffer_multisample */
    #if GL_ANGLE_framebuffer_multisample
    flextglRenderbufferStorageMultisampleANGLE = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, GLsizei, GLsizei)>(glRenderbufferStorageMultisampleANGLE);
    #endif

    /* GL_ANGLE_instanced_arrays */
    #if GL_ANGLE_instanced_arrays
    flextglDrawArraysInstancedANGLE = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLsizei, GLsizei)>(glDrawArraysInstancedANGLE);
    flextglDrawElementsInstancedANGLE = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, const void *, GLsizei)>(glDrawElementsInstancedANGLE);
    flextglVertexAttribDivisorANGLE = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint)>(glVertexAttribDivisorANGLE);
    #endif

    /* GL_APPLE_framebuffer_multisample */
    #if GL_APPLE_framebuffer_multisample
    flextglRenderbufferStorageMultisampleAPPLE = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, GLsizei, GLsizei)>(glRenderbufferStorageMultisampleAPPLE);
    flextglResolveMultisampleFramebufferAPPLE = reinterpret_cast<void(APIENTRY*)(void)>(glResolveMultisampleFramebufferAPPLE);
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

    /* GL_EXT_discard_framebuffer */
    #if GL_EXT_discard_framebuffer
    flextglDiscardFramebufferEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, const GLenum *)>(glDiscardFramebufferEXT);
    #endif

    /* GL_EXT_disjoint_timer_query */
    #if GL_EXT_disjoint_timer_query
    flextglGetQueryObjecti64vEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint64 *)>(glGetQueryObjecti64vEXT);
    flextglGetQueryObjectivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint *)>(glGetQueryObjectivEXT);
    flextglGetQueryObjectui64vEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint64 *)>(glGetQueryObjectui64vEXT);
    flextglQueryCounterEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum)>(glQueryCounterEXT);
    #endif

    /* GL_EXT_draw_buffers */
    #if GL_EXT_draw_buffers
    flextglDrawBuffersEXT = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLenum *)>(glDrawBuffersEXT);
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

    /* GL_EXT_instanced_arrays */
    #if GL_EXT_instanced_arrays
    flextglDrawArraysInstancedEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLsizei, GLsizei)>(glDrawArraysInstancedEXT);
    flextglDrawElementsInstancedEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, const void *, GLsizei)>(glDrawElementsInstancedEXT);
    flextglVertexAttribDivisorEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint)>(glVertexAttribDivisorEXT);
    #endif

    /* GL_EXT_map_buffer_range */
    #if GL_EXT_map_buffer_range
    flextglFlushMappedBufferRangeEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLintptr, GLsizeiptr)>(glFlushMappedBufferRangeEXT);
    flextglMapBufferRangeEXT = reinterpret_cast<void *(APIENTRY*)(GLenum, GLintptr, GLsizeiptr, GLbitfield)>(glMapBufferRangeEXT);
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

    /* GL_EXT_occlusion_query_boolean */
    #if GL_EXT_occlusion_query_boolean
    flextglBeginQueryEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint)>(glBeginQueryEXT);
    flextglDeleteQueriesEXT = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLuint *)>(glDeleteQueriesEXT);
    flextglEndQueryEXT = reinterpret_cast<void(APIENTRY*)(GLenum)>(glEndQueryEXT);
    flextglGenQueriesEXT = reinterpret_cast<void(APIENTRY*)(GLsizei, GLuint *)>(glGenQueriesEXT);
    flextglGetQueryObjectuivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint *)>(glGetQueryObjectuivEXT);
    flextglGetQueryivEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLint *)>(glGetQueryivEXT);
    flextglIsQueryEXT = reinterpret_cast<GLboolean(APIENTRY*)(GLuint)>(glIsQueryEXT);
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
    flextglProgramUniform2fEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLfloat, GLfloat)>(glProgramUniform2fEXT);
    flextglProgramUniform2fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLfloat *)>(glProgramUniform2fvEXT);
    flextglProgramUniform2iEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLint)>(glProgramUniform2iEXT);
    flextglProgramUniform2ivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLint *)>(glProgramUniform2ivEXT);
    flextglProgramUniform3fEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLfloat, GLfloat, GLfloat)>(glProgramUniform3fEXT);
    flextglProgramUniform3fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLfloat *)>(glProgramUniform3fvEXT);
    flextglProgramUniform3iEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLint, GLint)>(glProgramUniform3iEXT);
    flextglProgramUniform3ivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLint *)>(glProgramUniform3ivEXT);
    flextglProgramUniform4fEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLfloat, GLfloat, GLfloat, GLfloat)>(glProgramUniform4fEXT);
    flextglProgramUniform4fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLfloat *)>(glProgramUniform4fvEXT);
    flextglProgramUniform4iEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLint, GLint, GLint)>(glProgramUniform4iEXT);
    flextglProgramUniform4ivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLint *)>(glProgramUniform4ivEXT);
    flextglProgramUniformMatrix2fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(glProgramUniformMatrix2fvEXT);
    flextglProgramUniformMatrix3fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(glProgramUniformMatrix3fvEXT);
    flextglProgramUniformMatrix4fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(glProgramUniformMatrix4fvEXT);
    flextglUseProgramStagesEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLbitfield, GLuint)>(glUseProgramStagesEXT);
    flextglValidateProgramPipelineEXT = reinterpret_cast<void(APIENTRY*)(GLuint)>(glValidateProgramPipelineEXT);
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

    /* GL_EXT_texture_storage */
    #if GL_EXT_texture_storage
    flextglTexStorage2DEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, GLsizei, GLsizei)>(glTexStorage2DEXT);
    flextglTexStorage3DEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLsizei)>(glTexStorage3DEXT);
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

    /* GL_NV_draw_buffers */
    #if GL_NV_draw_buffers
    flextglDrawBuffersNV = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLenum *)>(glDrawBuffersNV);
    #endif

    /* GL_NV_draw_instanced */
    #if GL_NV_draw_instanced
    flextglDrawArraysInstancedNV = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLsizei, GLsizei)>(glDrawArraysInstancedNV);
    flextglDrawElementsInstancedNV = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, const void *, GLsizei)>(glDrawElementsInstancedNV);
    #endif

    /* GL_NV_framebuffer_blit */
    #if GL_NV_framebuffer_blit
    flextglBlitFramebufferNV = reinterpret_cast<void(APIENTRY*)(GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLbitfield, GLenum)>(glBlitFramebufferNV);
    #endif

    /* GL_NV_framebuffer_multisample */
    #if GL_NV_framebuffer_multisample
    flextglRenderbufferStorageMultisampleNV = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, GLsizei, GLsizei)>(glRenderbufferStorageMultisampleNV);
    #endif

    /* GL_NV_instanced_arrays */
    #if GL_NV_instanced_arrays
    flextglVertexAttribDivisorNV = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint)>(glVertexAttribDivisorNV);
    #endif

    /* GL_NV_polygon_mode */
    #if GL_NV_polygon_mode
    flextglPolygonModeNV = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum)>(glPolygonModeNV);
    #endif

    /* GL_NV_read_buffer */
    #if GL_NV_read_buffer
    flextglReadBufferNV = reinterpret_cast<void(APIENTRY*)(GLenum)>(glReadBufferNV);
    #endif

    /* GL_OES_mapbuffer */
    #if GL_OES_mapbuffer
    flextglGetBufferPointervOES = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, void **)>(glGetBufferPointervOES);
    flextglMapBufferOES = reinterpret_cast<void *(APIENTRY*)(GLenum, GLenum)>(glMapBufferOES);
    flextglUnmapBufferOES = reinterpret_cast<GLboolean(APIENTRY*)(GLenum)>(glUnmapBufferOES);
    #endif

    /* GL_OES_texture_3D */
    #if GL_OES_texture_3D
    flextglCompressedTexImage3DOES = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLenum, GLsizei, GLsizei, GLsizei, GLint, GLsizei, const void *)>(glCompressedTexImage3DOES);
    flextglCompressedTexSubImage3DOES = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLsizei, const void *)>(glCompressedTexSubImage3DOES);
    flextglCopyTexSubImage3DOES = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei)>(glCopyTexSubImage3DOES);
    flextglFramebufferTexture3DOES = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLenum, GLuint, GLint, GLint)>(glFramebufferTexture3DOES);
    flextglTexImage3DOES = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLenum, GLsizei, GLsizei, GLsizei, GLint, GLenum, GLenum, const void *)>(glTexImage3DOES);
    flextglTexSubImage3DOES = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, const void *)>(glTexSubImage3DOES);
    #endif

    /* GL_OES_vertex_array_object */
    #if GL_OES_vertex_array_object
    flextglBindVertexArrayOES = reinterpret_cast<void(APIENTRY*)(GLuint)>(glBindVertexArrayOES);
    flextglDeleteVertexArraysOES = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLuint *)>(glDeleteVertexArraysOES);
    flextglGenVertexArraysOES = reinterpret_cast<void(APIENTRY*)(GLsizei, GLuint *)>(glGenVertexArraysOES);
    flextglIsVertexArrayOES = reinterpret_cast<GLboolean(APIENTRY*)(GLuint)>(glIsVertexArrayOES);
    #endif
}
