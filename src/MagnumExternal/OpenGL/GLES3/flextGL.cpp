#include "flextGL.h"

#ifdef __cplusplus
extern "C" {
#endif

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
FLEXTGL_EXPORT void(APIENTRY *flextglGetTexLevelParameterfv)(GLenum, GLint, GLenum, GLfloat *) = nullptr;
FLEXTGL_EXPORT void(APIENTRY *flextglGetTexLevelParameteriv)(GLenum, GLint, GLenum, GLint *) = nullptr;
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
