#include "flextGLWindowsDesktop.h"

#ifdef __cplusplus
extern "C" {
#endif

/* GL_ANGLE_framebuffer_blit */
FLEXTGL_EXPORT void(APIENTRY *flextglBlitFramebufferANGLE)(GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLbitfield, GLenum) = nullptr;

/* GL_ANGLE_framebuffer_multisample */
FLEXTGL_EXPORT void(APIENTRY *flextglRenderbufferStorageMultisampleANGLE)(GLenum, GLsizei, GLenum, GLsizei, GLsizei) = nullptr;

/* GL_ANGLE_instanced_arrays */
FLEXTGL_EXPORT void(APIENTRY *flextglDrawArraysInstancedANGLE)(GLenum, GLint, GLsizei, GLsizei) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglDrawElementsInstancedANGLE)(GLenum, GLsizei, GLenum, const void *, GLsizei) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglVertexAttribDivisorANGLE)(GLuint, GLuint) = nullptr;

/* GL_APPLE_framebuffer_multisample */
FLEXTGL_EXPORT void(APIENTRY *flextglRenderbufferStorageMultisampleAPPLE)(GLenum, GLsizei, GLenum, GLsizei, GLsizei) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglResolveMultisampleFramebufferAPPLE)(void) = nullptr;

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

/* GL_EXT_debug_label */
FLEXTGL_EXPORT void(APIENTRY *flextglGetObjectLabelEXT)(GLenum, GLuint, GLsizei, GLsizei *, GLchar *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglLabelObjectEXT)(GLenum, GLuint, GLsizei, const GLchar *) = nullptr;

/* GL_EXT_debug_marker */
FLEXTGL_EXPORT void(APIENTRY *flextglInsertEventMarkerEXT)(GLsizei, const GLchar *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglPopGroupMarkerEXT)(void) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglPushGroupMarkerEXT)(GLsizei, const GLchar *) = nullptr;

/* GL_EXT_discard_framebuffer */
FLEXTGL_EXPORT void(APIENTRY *flextglDiscardFramebufferEXT)(GLenum, GLsizei, const GLenum *) = nullptr;

/* GL_EXT_disjoint_timer_query */
FLEXTGL_EXPORT void(APIENTRY *flextglGetQueryObjecti64vEXT)(GLuint, GLenum, GLint64 *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGetQueryObjectivEXT)(GLuint, GLenum, GLint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGetQueryObjectui64vEXT)(GLuint, GLenum, GLuint64 *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglQueryCounterEXT)(GLuint, GLenum) = nullptr;

/* GL_EXT_draw_buffers */
FLEXTGL_EXPORT void(APIENTRY *flextglDrawBuffersEXT)(GLsizei, const GLenum *) = nullptr;

/* GL_EXT_draw_buffers_indexed */
FLEXTGL_EXPORT void(APIENTRY *flextglBlendEquationSeparateiEXT)(GLuint, GLenum, GLenum) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglBlendEquationiEXT)(GLuint, GLenum) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglBlendFuncSeparateiEXT)(GLuint, GLenum, GLenum, GLenum, GLenum) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglBlendFunciEXT)(GLuint, GLenum, GLenum) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglColorMaskiEXT)(GLuint, GLboolean, GLboolean, GLboolean, GLboolean) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglDisableiEXT)(GLenum, GLuint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglEnableiEXT)(GLenum, GLuint) = nullptr;
FLEXTGL_EXPORT GLboolean(APIENTRY *flextglIsEnablediEXT)(GLenum, GLuint) = nullptr;

/* GL_EXT_instanced_arrays */
FLEXTGL_EXPORT void(APIENTRY *flextglDrawArraysInstancedEXT)(GLenum, GLint, GLsizei, GLsizei) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglDrawElementsInstancedEXT)(GLenum, GLsizei, GLenum, const void *, GLsizei) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglVertexAttribDivisorEXT)(GLuint, GLuint) = nullptr;

/* GL_EXT_map_buffer_range */
FLEXTGL_EXPORT void(APIENTRY *flextglFlushMappedBufferRangeEXT)(GLenum, GLintptr, GLsizeiptr) = nullptr;
FLEXTGL_EXPORT void *(APIENTRY *flextglMapBufferRangeEXT)(GLenum, GLintptr, GLsizeiptr, GLbitfield) = nullptr;

/* GL_EXT_multi_draw_arrays */
FLEXTGL_EXPORT void(APIENTRY *flextglMultiDrawArraysEXT)(GLenum, const GLint *, const GLsizei *, GLsizei) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglMultiDrawElementsEXT)(GLenum, const GLsizei *, GLenum, const void *const*, GLsizei) = nullptr;

/* GL_EXT_multisampled_render_to_texture */
FLEXTGL_EXPORT void(APIENTRY *flextglFramebufferTexture2DMultisampleEXT)(GLenum, GLenum, GLenum, GLuint, GLint, GLsizei) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglRenderbufferStorageMultisampleEXT)(GLenum, GLsizei, GLenum, GLsizei, GLsizei) = nullptr;

/* GL_EXT_occlusion_query_boolean */
FLEXTGL_EXPORT void(APIENTRY *flextglBeginQueryEXT)(GLenum, GLuint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglDeleteQueriesEXT)(GLsizei, const GLuint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglEndQueryEXT)(GLenum) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGenQueriesEXT)(GLsizei, GLuint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGetQueryObjectuivEXT)(GLuint, GLenum, GLuint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGetQueryivEXT)(GLenum, GLenum, GLint *) = nullptr;
FLEXTGL_EXPORT GLboolean(APIENTRY *flextglIsQueryEXT)(GLuint) = nullptr;

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
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniform2fEXT)(GLuint, GLint, GLfloat, GLfloat) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniform2fvEXT)(GLuint, GLint, GLsizei, const GLfloat *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniform2iEXT)(GLuint, GLint, GLint, GLint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniform2ivEXT)(GLuint, GLint, GLsizei, const GLint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniform3fEXT)(GLuint, GLint, GLfloat, GLfloat, GLfloat) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniform3fvEXT)(GLuint, GLint, GLsizei, const GLfloat *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniform3iEXT)(GLuint, GLint, GLint, GLint, GLint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniform3ivEXT)(GLuint, GLint, GLsizei, const GLint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniform4fEXT)(GLuint, GLint, GLfloat, GLfloat, GLfloat, GLfloat) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniform4fvEXT)(GLuint, GLint, GLsizei, const GLfloat *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniform4iEXT)(GLuint, GLint, GLint, GLint, GLint, GLint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniform4ivEXT)(GLuint, GLint, GLsizei, const GLint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniformMatrix2fvEXT)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniformMatrix3fvEXT)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglProgramUniformMatrix4fvEXT)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglUseProgramStagesEXT)(GLuint, GLbitfield, GLuint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglValidateProgramPipelineEXT)(GLuint) = nullptr;

/* GL_EXT_texture_border_clamp */
FLEXTGL_EXPORT void(APIENTRY *flextglGetSamplerParameterIivEXT)(GLuint, GLenum, GLint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGetSamplerParameterIuivEXT)(GLuint, GLenum, GLuint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGetTexParameterIivEXT)(GLenum, GLenum, GLint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGetTexParameterIuivEXT)(GLenum, GLenum, GLuint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglSamplerParameterIivEXT)(GLuint, GLenum, const GLint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglSamplerParameterIuivEXT)(GLuint, GLenum, const GLuint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglTexParameterIivEXT)(GLenum, GLenum, const GLint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglTexParameterIuivEXT)(GLenum, GLenum, const GLuint *) = nullptr;

/* GL_EXT_texture_storage */
FLEXTGL_EXPORT void(APIENTRY *flextglTexStorage2DEXT)(GLenum, GLsizei, GLenum, GLsizei, GLsizei) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglTexStorage3DEXT)(GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLsizei) = nullptr;

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

/* GL_NV_draw_buffers */
FLEXTGL_EXPORT void(APIENTRY *flextglDrawBuffersNV)(GLsizei, const GLenum *) = nullptr;

/* GL_NV_draw_instanced */
FLEXTGL_EXPORT void(APIENTRY *flextglDrawArraysInstancedNV)(GLenum, GLint, GLsizei, GLsizei) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglDrawElementsInstancedNV)(GLenum, GLsizei, GLenum, const void *, GLsizei) = nullptr;

/* GL_NV_framebuffer_blit */
FLEXTGL_EXPORT void(APIENTRY *flextglBlitFramebufferNV)(GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLbitfield, GLenum) = nullptr;

/* GL_NV_framebuffer_multisample */
FLEXTGL_EXPORT void(APIENTRY *flextglRenderbufferStorageMultisampleNV)(GLenum, GLsizei, GLenum, GLsizei, GLsizei) = nullptr;

/* GL_NV_instanced_arrays */
FLEXTGL_EXPORT void(APIENTRY *flextglVertexAttribDivisorNV)(GLuint, GLuint) = nullptr;

/* GL_NV_polygon_mode */
FLEXTGL_EXPORT void(APIENTRY *flextglPolygonModeNV)(GLenum, GLenum) = nullptr;

/* GL_NV_read_buffer */
FLEXTGL_EXPORT void(APIENTRY *flextglReadBufferNV)(GLenum) = nullptr;

/* GL_OES_mapbuffer */
FLEXTGL_EXPORT void(APIENTRY *flextglGetBufferPointervOES)(GLenum, GLenum, void **) = nullptr;
FLEXTGL_EXPORT void *(APIENTRY *flextglMapBufferOES)(GLenum, GLenum) = nullptr;
FLEXTGL_EXPORT GLboolean(APIENTRY *flextglUnmapBufferOES)(GLenum) = nullptr;

/* GL_OES_texture_3D */
FLEXTGL_EXPORT void(APIENTRY *flextglCompressedTexImage3DOES)(GLenum, GLint, GLenum, GLsizei, GLsizei, GLsizei, GLint, GLsizei, const void *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglCompressedTexSubImage3DOES)(GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLsizei, const void *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglCopyTexSubImage3DOES)(GLenum, GLint, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglFramebufferTexture3DOES)(GLenum, GLenum, GLenum, GLuint, GLint, GLint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglTexImage3DOES)(GLenum, GLint, GLenum, GLsizei, GLsizei, GLsizei, GLint, GLenum, GLenum, const void *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglTexSubImage3DOES)(GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, const void *) = nullptr;

/* GL_OES_vertex_array_object */
FLEXTGL_EXPORT void(APIENTRY *flextglBindVertexArrayOES)(GLuint) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglDeleteVertexArraysOES)(GLsizei, const GLuint *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGenVertexArraysOES)(GLsizei, GLuint *) = nullptr;
FLEXTGL_EXPORT GLboolean(APIENTRY *flextglIsVertexArrayOES)(GLuint) = nullptr;

#ifdef __cplusplus
}
#endif
