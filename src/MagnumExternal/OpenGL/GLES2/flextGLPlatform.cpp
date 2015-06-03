#include "flextGL.h"

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

    /* GL_ANGLE_depth_texture */

    /* GL_APPLE_framebuffer_multisample */
    flextglRenderbufferStorageMultisampleAPPLE = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, GLsizei, GLsizei)>(loader.load("glRenderbufferStorageMultisampleAPPLE"));
    flextglResolveMultisampleFramebufferAPPLE = reinterpret_cast<void(APIENTRY*)(void)>(loader.load("glResolveMultisampleFramebufferAPPLE"));

    /* GL_APPLE_texture_max_level */

    /* GL_ARM_rgba8 */

    /* GL_EXT_texture_type_2_10_10_10_REV */

    /* GL_EXT_discard_framebuffer */
    flextglDiscardFramebufferEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, const GLenum *)>(loader.load("glDiscardFramebufferEXT"));

    /* GL_EXT_blend_minmax */
    flextglBlendEquationEXT = reinterpret_cast<void(APIENTRY*)(GLenum)>(loader.load("glBlendEquationEXT"));

    /* GL_EXT_shader_texture_lod */

    /* GL_EXT_occlusion_query_boolean */
    flextglGenQueriesEXT = reinterpret_cast<void(APIENTRY*)(GLsizei, GLuint *)>(loader.load("glGenQueriesEXT"));
    flextglDeleteQueriesEXT = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLuint *)>(loader.load("glDeleteQueriesEXT"));
    flextglIsQueryEXT = reinterpret_cast<GLboolean(APIENTRY*)(GLuint)>(loader.load("glIsQueryEXT"));
    flextglBeginQueryEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint)>(loader.load("glBeginQueryEXT"));
    flextglEndQueryEXT = reinterpret_cast<void(APIENTRY*)(GLenum)>(loader.load("glEndQueryEXT"));
    flextglGetQueryivEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLint *)>(loader.load("glGetQueryivEXT"));
    flextglGetQueryObjectuivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint *)>(loader.load("glGetQueryObjectuivEXT"));

    /* GL_EXT_shadow_samplers */

    /* GL_EXT_texture_rg */

    /* GL_EXT_sRGB */

    /* GL_EXT_texture_storage */
    flextglTexStorage1DEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, GLsizei)>(loader.load("glTexStorage1DEXT"));
    flextglTexStorage2DEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, GLsizei, GLsizei)>(loader.load("glTexStorage2DEXT"));
    flextglTexStorage3DEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLsizei)>(loader.load("glTexStorage3DEXT"));
    flextglTextureStorage1DEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLsizei, GLenum, GLsizei)>(loader.load("glTextureStorage1DEXT"));
    flextglTextureStorage2DEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLsizei, GLenum, GLsizei, GLsizei)>(loader.load("glTextureStorage2DEXT"));
    flextglTextureStorage3DEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLsizei)>(loader.load("glTextureStorage3DEXT"));

    /* GL_EXT_map_buffer_range */
    flextglMapBufferRangeEXT = reinterpret_cast<void *(APIENTRY*)(GLenum, GLintptr, GLsizeiptr, GLbitfield)>(loader.load("glMapBufferRangeEXT"));
    flextglFlushMappedBufferRangeEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLintptr, GLsizeiptr)>(loader.load("glFlushMappedBufferRangeEXT"));

    /* GL_EXT_draw_buffers */
    flextglDrawBuffersEXT = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLenum *)>(loader.load("glDrawBuffersEXT"));

    /* GL_EXT_instanced_arrays */
    flextglDrawArraysInstancedEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLsizei, GLsizei)>(loader.load("glDrawArraysInstancedEXT"));
    flextglDrawElementsInstancedEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, const void *, GLsizei)>(loader.load("glDrawElementsInstancedEXT"));
    flextglVertexAttribDivisorEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint)>(loader.load("glVertexAttribDivisorEXT"));

    /* GL_EXT_draw_instanced */

    /* GL_NV_draw_buffers */
    flextglDrawBuffersNV = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLenum *)>(loader.load("glDrawBuffersNV"));

    /* GL_NV_fbo_color_attachments */

    /* GL_NV_read_buffer */
    flextglReadBufferNV = reinterpret_cast<void(APIENTRY*)(GLenum)>(loader.load("glReadBufferNV"));

    /* GL_NV_draw_instanced */
    flextglDrawArraysInstancedNV = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLsizei, GLsizei)>(loader.load("glDrawArraysInstancedNV"));
    flextglDrawElementsInstancedNV = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, const void *, GLsizei)>(loader.load("glDrawElementsInstancedNV"));

    /* GL_NV_framebuffer_blit */
    flextglBlitFramebufferNV = reinterpret_cast<void(APIENTRY*)(GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLbitfield, GLenum)>(loader.load("glBlitFramebufferNV"));

    /* GL_NV_framebuffer_multisample */
    flextglRenderbufferStorageMultisampleNV = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, GLsizei, GLsizei)>(loader.load("glRenderbufferStorageMultisampleNV"));

    /* GL_NV_instanced_arrays */
    flextglVertexAttribDivisorNV = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint)>(loader.load("glVertexAttribDivisorNV"));

    /* GL_NV_shadow_samplers_array */

    /* GL_NV_shadow_samplers_cube */

    /* GL_OES_depth24 */

    /* GL_OES_element_index_uint */

    /* GL_OES_rgb8_rgba8 */

    /* GL_OES_texture_3D */
    flextglTexImage3DOES = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLenum, GLsizei, GLsizei, GLsizei, GLint, GLenum, GLenum, const void *)>(loader.load("glTexImage3DOES"));
    flextglTexSubImage3DOES = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, const void *)>(loader.load("glTexSubImage3DOES"));
    flextglCopyTexSubImage3DOES = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei)>(loader.load("glCopyTexSubImage3DOES"));
    flextglCompressedTexImage3DOES = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLenum, GLsizei, GLsizei, GLsizei, GLint, GLsizei, const void *)>(loader.load("glCompressedTexImage3DOES"));
    flextglCompressedTexSubImage3DOES = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLsizei, const void *)>(loader.load("glCompressedTexSubImage3DOES"));
    flextglFramebufferTexture3DOES = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLenum, GLuint, GLint, GLint)>(loader.load("glFramebufferTexture3DOES"));

    /* GL_OES_texture_half_float_linear */

    /* GL_OES_texture_float_linear */

    /* GL_OES_texture_half_float */

    /* GL_OES_texture_float */

    /* GL_OES_vertex_half_float */

    /* GL_OES_packed_depth_stencil */

    /* GL_OES_depth_texture */

    /* GL_OES_standard_derivatives */

    /* GL_OES_vertex_array_object */
    flextglBindVertexArrayOES = reinterpret_cast<void(APIENTRY*)(GLuint)>(loader.load("glBindVertexArrayOES"));
    flextglDeleteVertexArraysOES = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLuint *)>(loader.load("glDeleteVertexArraysOES"));
    flextglGenVertexArraysOES = reinterpret_cast<void(APIENTRY*)(GLsizei, GLuint *)>(loader.load("glGenVertexArraysOES"));
    flextglIsVertexArrayOES = reinterpret_cast<GLboolean(APIENTRY*)(GLuint)>(loader.load("glIsVertexArrayOES"));

    /* GL_OES_required_internalformat */

    /* GL_OES_surfaceless_context */

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
    flextglQueryCounterEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum)>(loader.load("glQueryCounterEXT"));
    flextglGetQueryObjectivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint *)>(loader.load("glGetQueryObjectivEXT"));
    flextglGetQueryObjecti64vEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint64 *)>(loader.load("glGetQueryObjecti64vEXT"));
    flextglGetQueryObjectui64vEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint64 *)>(loader.load("glGetQueryObjectui64vEXT"));

    /* GL_EXT_texture_sRGB_decode */

    /* GL_EXT_sRGB_write_control */

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
