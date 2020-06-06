/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

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
#undef glGetInteger64vEXT
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
#undef glPolygonOffsetClampEXT
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
#undef glFramebufferSampleLocationsfvNV
#undef glNamedFramebufferSampleLocationsfvNV
#undef glResolveDepthValuesNV
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

#include <OpenGLES/ES2/glext.h>

void flextGLInit(Magnum::GL::Context&) {
    /* Work around missing glTexStorage3D (can't be used anyway because GLES2
       on iOS doesn't support OES_texture_3D) */
    constexpr void(*glTexStorage3DEXT)() = nullptr;

    /* GL_ANGLE_framebuffer_blit */
    #if GL_ANGLE_framebuffer_blit
    flextGL.BlitFramebufferANGLE = reinterpret_cast<void(APIENTRY*)(GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLbitfield, GLenum)>(glBlitFramebufferANGLE);
    #endif

    /* GL_ANGLE_framebuffer_multisample */
    #if GL_ANGLE_framebuffer_multisample
    flextGL.RenderbufferStorageMultisampleANGLE = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, GLsizei, GLsizei)>(glRenderbufferStorageMultisampleANGLE);
    #endif

    /* GL_ANGLE_instanced_arrays */
    #if GL_ANGLE_instanced_arrays
    flextGL.DrawArraysInstancedANGLE = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLsizei, GLsizei)>(glDrawArraysInstancedANGLE);
    flextGL.DrawElementsInstancedANGLE = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, const void *, GLsizei)>(glDrawElementsInstancedANGLE);
    flextGL.VertexAttribDivisorANGLE = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint)>(glVertexAttribDivisorANGLE);
    #endif

    /* GL_APPLE_framebuffer_multisample */
    #if GL_APPLE_framebuffer_multisample
    flextGL.RenderbufferStorageMultisampleAPPLE = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, GLsizei, GLsizei)>(glRenderbufferStorageMultisampleAPPLE);
    flextGL.ResolveMultisampleFramebufferAPPLE = reinterpret_cast<void(APIENTRY*)(void)>(glResolveMultisampleFramebufferAPPLE);
    #endif

    /* GL_EXT_debug_label */
    #if GL_EXT_debug_label
    flextGL.GetObjectLabelEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLsizei, GLsizei *, GLchar *)>(glGetObjectLabelEXT);
    flextGL.LabelObjectEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLsizei, const GLchar *)>(glLabelObjectEXT);
    #endif

    /* GL_EXT_debug_marker */
    #if GL_EXT_debug_marker
    flextGL.InsertEventMarkerEXT = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLchar *)>(glInsertEventMarkerEXT);
    flextGL.PopGroupMarkerEXT = reinterpret_cast<void(APIENTRY*)(void)>(glPopGroupMarkerEXT);
    flextGL.PushGroupMarkerEXT = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLchar *)>(glPushGroupMarkerEXT);
    #endif

    /* GL_EXT_discard_framebuffer */
    #if GL_EXT_discard_framebuffer
    flextGL.DiscardFramebufferEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, const GLenum *)>(glDiscardFramebufferEXT);
    #endif

    /* GL_EXT_disjoint_timer_query */
    #if GL_EXT_disjoint_timer_query
    flextGL.GetInteger64vEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLint64 *)>(glGetInteger64vEXT);
    flextGL.GetQueryObjecti64vEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint64 *)>(glGetQueryObjecti64vEXT);
    flextGL.GetQueryObjectivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint *)>(glGetQueryObjectivEXT);
    flextGL.GetQueryObjectui64vEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint64 *)>(glGetQueryObjectui64vEXT);
    flextGL.QueryCounterEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum)>(glQueryCounterEXT);
    #endif

    /* GL_EXT_draw_buffers */
    #if GL_EXT_draw_buffers
    flextGL.DrawBuffersEXT = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLenum *)>(glDrawBuffersEXT);
    #endif

    /* GL_EXT_draw_buffers_indexed */
    #if GL_EXT_draw_buffers_indexed
    flextGL.BlendEquationSeparateiEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLenum)>(glBlendEquationSeparateiEXT);
    flextGL.BlendEquationiEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum)>(glBlendEquationiEXT);
    flextGL.BlendFuncSeparateiEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLenum, GLenum, GLenum)>(glBlendFuncSeparateiEXT);
    flextGL.BlendFunciEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLenum)>(glBlendFunciEXT);
    flextGL.ColorMaskiEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLboolean, GLboolean, GLboolean, GLboolean)>(glColorMaskiEXT);
    flextGL.DisableiEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint)>(glDisableiEXT);
    flextGL.EnableiEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint)>(glEnableiEXT);
    flextGL.IsEnablediEXT = reinterpret_cast<GLboolean(APIENTRY*)(GLenum, GLuint)>(glIsEnablediEXT);
    #endif

    /* GL_EXT_instanced_arrays */
    #if GL_EXT_instanced_arrays
    flextGL.DrawArraysInstancedEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLsizei, GLsizei)>(glDrawArraysInstancedEXT);
    flextGL.DrawElementsInstancedEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, const void *, GLsizei)>(glDrawElementsInstancedEXT);
    flextGL.VertexAttribDivisorEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint)>(glVertexAttribDivisorEXT);
    #endif

    /* GL_EXT_map_buffer_range */
    #if GL_EXT_map_buffer_range
    flextGL.FlushMappedBufferRangeEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLintptr, GLsizeiptr)>(glFlushMappedBufferRangeEXT);
    flextGL.MapBufferRangeEXT = reinterpret_cast<void *(APIENTRY*)(GLenum, GLintptr, GLsizeiptr, GLbitfield)>(glMapBufferRangeEXT);
    #endif

    /* GL_EXT_multi_draw_arrays */
    #if GL_EXT_multi_draw_arrays
    flextGL.MultiDrawArraysEXT = reinterpret_cast<void(APIENTRY*)(GLenum, const GLint *, const GLsizei *, GLsizei)>(glMultiDrawArraysEXT);
    flextGL.MultiDrawElementsEXT = reinterpret_cast<void(APIENTRY*)(GLenum, const GLsizei *, GLenum, const void *const*, GLsizei)>(glMultiDrawElementsEXT);
    #endif

    /* GL_EXT_multisampled_render_to_texture */
    #if GL_EXT_multisampled_render_to_texture
    flextGL.FramebufferTexture2DMultisampleEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLenum, GLuint, GLint, GLsizei)>(glFramebufferTexture2DMultisampleEXT);
    flextGL.RenderbufferStorageMultisampleEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, GLsizei, GLsizei)>(glRenderbufferStorageMultisampleEXT);
    #endif

    /* GL_EXT_occlusion_query_boolean */
    #if GL_EXT_occlusion_query_boolean
    flextGL.BeginQueryEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint)>(glBeginQueryEXT);
    flextGL.DeleteQueriesEXT = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLuint *)>(glDeleteQueriesEXT);
    flextGL.EndQueryEXT = reinterpret_cast<void(APIENTRY*)(GLenum)>(glEndQueryEXT);
    flextGL.GenQueriesEXT = reinterpret_cast<void(APIENTRY*)(GLsizei, GLuint *)>(glGenQueriesEXT);
    flextGL.GetQueryObjectuivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint *)>(glGetQueryObjectuivEXT);
    flextGL.GetQueryivEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLint *)>(glGetQueryivEXT);
    flextGL.IsQueryEXT = reinterpret_cast<GLboolean(APIENTRY*)(GLuint)>(glIsQueryEXT);
    #endif

    /* GL_EXT_polygon_offset_clamp */
    #if GL_EXT_polygon_offset_clamp
    flextGL.PolygonOffsetClampEXT = reinterpret_cast<void(APIENTRY*)(GLfloat, GLfloat, GLfloat)>(glPolygonOffsetClampEXT);
    #endif

    /* GL_EXT_robustness */
    #if GL_EXT_robustness
    flextGL.GetGraphicsResetStatusEXT = reinterpret_cast<GLenum(APIENTRY*)(void)>(glGetGraphicsResetStatusEXT);
    flextGL.GetnUniformfvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLfloat *)>(glGetnUniformfvEXT);
    flextGL.GetnUniformivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLint *)>(glGetnUniformivEXT);
    flextGL.ReadnPixelsEXT = reinterpret_cast<void(APIENTRY*)(GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, GLsizei, void *)>(glReadnPixelsEXT);
    #endif

    /* GL_EXT_separate_shader_objects */
    #if GL_EXT_separate_shader_objects
    flextGL.ActiveShaderProgramEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint)>(glActiveShaderProgramEXT);
    flextGL.BindProgramPipelineEXT = reinterpret_cast<void(APIENTRY*)(GLuint)>(glBindProgramPipelineEXT);
    flextGL.CreateShaderProgramvEXT = reinterpret_cast<GLuint(APIENTRY*)(GLenum, GLsizei, const GLchar **)>(glCreateShaderProgramvEXT);
    flextGL.DeleteProgramPipelinesEXT = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLuint *)>(glDeleteProgramPipelinesEXT);
    flextGL.GenProgramPipelinesEXT = reinterpret_cast<void(APIENTRY*)(GLsizei, GLuint *)>(glGenProgramPipelinesEXT);
    flextGL.GetProgramPipelineInfoLogEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, GLsizei *, GLchar *)>(glGetProgramPipelineInfoLogEXT);
    flextGL.GetProgramPipelineivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint *)>(glGetProgramPipelineivEXT);
    flextGL.IsProgramPipelineEXT = reinterpret_cast<GLboolean(APIENTRY*)(GLuint)>(glIsProgramPipelineEXT);
    flextGL.ProgramParameteriEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint)>(glProgramParameteriEXT);
    flextGL.ProgramUniform1fEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLfloat)>(glProgramUniform1fEXT);
    flextGL.ProgramUniform1fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLfloat *)>(glProgramUniform1fvEXT);
    flextGL.ProgramUniform1iEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint)>(glProgramUniform1iEXT);
    flextGL.ProgramUniform1ivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLint *)>(glProgramUniform1ivEXT);
    flextGL.ProgramUniform2fEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLfloat, GLfloat)>(glProgramUniform2fEXT);
    flextGL.ProgramUniform2fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLfloat *)>(glProgramUniform2fvEXT);
    flextGL.ProgramUniform2iEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLint)>(glProgramUniform2iEXT);
    flextGL.ProgramUniform2ivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLint *)>(glProgramUniform2ivEXT);
    flextGL.ProgramUniform3fEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLfloat, GLfloat, GLfloat)>(glProgramUniform3fEXT);
    flextGL.ProgramUniform3fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLfloat *)>(glProgramUniform3fvEXT);
    flextGL.ProgramUniform3iEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLint, GLint)>(glProgramUniform3iEXT);
    flextGL.ProgramUniform3ivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLint *)>(glProgramUniform3ivEXT);
    flextGL.ProgramUniform4fEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLfloat, GLfloat, GLfloat, GLfloat)>(glProgramUniform4fEXT);
    flextGL.ProgramUniform4fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLfloat *)>(glProgramUniform4fvEXT);
    flextGL.ProgramUniform4iEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLint, GLint, GLint)>(glProgramUniform4iEXT);
    flextGL.ProgramUniform4ivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLint *)>(glProgramUniform4ivEXT);
    flextGL.ProgramUniformMatrix2fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(glProgramUniformMatrix2fvEXT);
    flextGL.ProgramUniformMatrix3fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(glProgramUniformMatrix3fvEXT);
    flextGL.ProgramUniformMatrix4fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(glProgramUniformMatrix4fvEXT);
    flextGL.UseProgramStagesEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLbitfield, GLuint)>(glUseProgramStagesEXT);
    flextGL.ValidateProgramPipelineEXT = reinterpret_cast<void(APIENTRY*)(GLuint)>(glValidateProgramPipelineEXT);
    #endif

    /* GL_EXT_texture_border_clamp */
    #if GL_EXT_texture_border_clamp
    flextGL.GetSamplerParameterIivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint *)>(glGetSamplerParameterIivEXT);
    flextGL.GetSamplerParameterIuivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint *)>(glGetSamplerParameterIuivEXT);
    flextGL.GetTexParameterIivEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLint *)>(glGetTexParameterIivEXT);
    flextGL.GetTexParameterIuivEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLuint *)>(glGetTexParameterIuivEXT);
    flextGL.SamplerParameterIivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, const GLint *)>(glSamplerParameterIivEXT);
    flextGL.SamplerParameterIuivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, const GLuint *)>(glSamplerParameterIuivEXT);
    flextGL.TexParameterIivEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, const GLint *)>(glTexParameterIivEXT);
    flextGL.TexParameterIuivEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, const GLuint *)>(glTexParameterIuivEXT);
    #endif

    /* GL_EXT_texture_storage */
    #if GL_EXT_texture_storage
    flextGL.TexStorage2DEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, GLsizei, GLsizei)>(glTexStorage2DEXT);
    flextGL.TexStorage3DEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLsizei)>(glTexStorage3DEXT);
    #endif

    /* GL_KHR_blend_equation_advanced */
    #if GL_KHR_blend_equation_advanced
    flextGL.BlendBarrierKHR = reinterpret_cast<void(APIENTRY*)(void)>(glBlendBarrierKHR);
    #endif

    /* GL_KHR_debug */
    #if GL_KHR_debug
    flextGL.DebugMessageCallbackKHR = reinterpret_cast<void(APIENTRY*)(GLDEBUGPROCKHR, const void *)>(glDebugMessageCallbackKHR);
    flextGL.DebugMessageControlKHR = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLenum, GLsizei, const GLuint *, GLboolean)>(glDebugMessageControlKHR);
    flextGL.DebugMessageInsertKHR = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar *)>(glDebugMessageInsertKHR);
    flextGL.GetDebugMessageLogKHR = reinterpret_cast<GLuint(APIENTRY*)(GLuint, GLsizei, GLenum *, GLenum *, GLuint *, GLenum *, GLsizei *, GLchar *)>(glGetDebugMessageLogKHR);
    flextGL.GetObjectLabelKHR = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLsizei, GLsizei *, GLchar *)>(glGetObjectLabelKHR);
    flextGL.GetObjectPtrLabelKHR = reinterpret_cast<void(APIENTRY*)(const void *, GLsizei, GLsizei *, GLchar *)>(glGetObjectPtrLabelKHR);
    flextGL.GetPointervKHR = reinterpret_cast<void(APIENTRY*)(GLenum, void **)>(glGetPointervKHR);
    flextGL.ObjectLabelKHR = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLsizei, const GLchar *)>(glObjectLabelKHR);
    flextGL.ObjectPtrLabelKHR = reinterpret_cast<void(APIENTRY*)(const void *, GLsizei, const GLchar *)>(glObjectPtrLabelKHR);
    flextGL.PopDebugGroupKHR = reinterpret_cast<void(APIENTRY*)(void)>(glPopDebugGroupKHR);
    flextGL.PushDebugGroupKHR = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLsizei, const GLchar *)>(glPushDebugGroupKHR);
    #endif

    /* GL_KHR_robustness */
    #if GL_KHR_robustness
    flextGL.GetGraphicsResetStatusKHR = reinterpret_cast<GLenum(APIENTRY*)(void)>(glGetGraphicsResetStatusKHR);
    flextGL.GetnUniformfvKHR = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLfloat *)>(glGetnUniformfvKHR);
    flextGL.GetnUniformivKHR = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLint *)>(glGetnUniformivKHR);
    flextGL.GetnUniformuivKHR = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLuint *)>(glGetnUniformuivKHR);
    flextGL.ReadnPixelsKHR = reinterpret_cast<void(APIENTRY*)(GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, GLsizei, void *)>(glReadnPixelsKHR);
    #endif

    /* GL_NV_draw_buffers */
    #if GL_NV_draw_buffers
    flextGL.DrawBuffersNV = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLenum *)>(glDrawBuffersNV);
    #endif

    /* GL_NV_draw_instanced */
    #if GL_NV_draw_instanced
    flextGL.DrawArraysInstancedNV = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLsizei, GLsizei)>(glDrawArraysInstancedNV);
    flextGL.DrawElementsInstancedNV = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, const void *, GLsizei)>(glDrawElementsInstancedNV);
    #endif

    /* GL_NV_framebuffer_blit */
    #if GL_NV_framebuffer_blit
    flextGL.BlitFramebufferNV = reinterpret_cast<void(APIENTRY*)(GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLbitfield, GLenum)>(glBlitFramebufferNV);
    #endif

    /* GL_NV_framebuffer_multisample */
    #if GL_NV_framebuffer_multisample
    flextGL.RenderbufferStorageMultisampleNV = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, GLsizei, GLsizei)>(glRenderbufferStorageMultisampleNV);
    #endif

    /* GL_NV_instanced_arrays */
    #if GL_NV_instanced_arrays
    flextGL.VertexAttribDivisorNV = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint)>(glVertexAttribDivisorNV);
    #endif

    /* GL_NV_polygon_mode */
    #if GL_NV_polygon_mode
    flextGL.PolygonModeNV = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum)>(glPolygonModeNV);
    #endif

    /* GL_NV_read_buffer */
    #if GL_NV_read_buffer
    flextGL.ReadBufferNV = reinterpret_cast<void(APIENTRY*)(GLenum)>(glReadBufferNV);
    #endif

    /* GL_NV_sample_locations */
    #if GL_NV_sample_locations
    flextGL.FramebufferSampleLocationsfvNV = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLsizei, const GLfloat *)>(glFramebufferSampleLocationsfvNV);
    flextGL.NamedFramebufferSampleLocationsfvNV = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLsizei, const GLfloat *)>(glNamedFramebufferSampleLocationsfvNV);
    flextGL.ResolveDepthValuesNV = reinterpret_cast<void(APIENTRY*)(void)>(glResolveDepthValuesNV);
    #endif

    /* GL_OES_mapbuffer */
    #if GL_OES_mapbuffer
    flextGL.GetBufferPointervOES = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, void **)>(glGetBufferPointervOES);
    flextGL.MapBufferOES = reinterpret_cast<void *(APIENTRY*)(GLenum, GLenum)>(glMapBufferOES);
    flextGL.UnmapBufferOES = reinterpret_cast<GLboolean(APIENTRY*)(GLenum)>(glUnmapBufferOES);
    #endif

    /* GL_OES_texture_3D */
    #if GL_OES_texture_3D
    flextGL.CompressedTexImage3DOES = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLenum, GLsizei, GLsizei, GLsizei, GLint, GLsizei, const void *)>(glCompressedTexImage3DOES);
    flextGL.CompressedTexSubImage3DOES = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLsizei, const void *)>(glCompressedTexSubImage3DOES);
    flextGL.CopyTexSubImage3DOES = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei)>(glCopyTexSubImage3DOES);
    flextGL.FramebufferTexture3DOES = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLenum, GLuint, GLint, GLint)>(glFramebufferTexture3DOES);
    flextGL.TexImage3DOES = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLenum, GLsizei, GLsizei, GLsizei, GLint, GLenum, GLenum, const void *)>(glTexImage3DOES);
    flextGL.TexSubImage3DOES = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, const void *)>(glTexSubImage3DOES);
    #endif

    /* GL_OES_vertex_array_object */
    #if GL_OES_vertex_array_object
    flextGL.BindVertexArrayOES = reinterpret_cast<void(APIENTRY*)(GLuint)>(glBindVertexArrayOES);
    flextGL.DeleteVertexArraysOES = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLuint *)>(glDeleteVertexArraysOES);
    flextGL.GenVertexArraysOES = reinterpret_cast<void(APIENTRY*)(GLsizei, GLuint *)>(glGenVertexArraysOES);
    flextGL.IsVertexArrayOES = reinterpret_cast<GLboolean(APIENTRY*)(GLuint)>(glIsVertexArrayOES);
    #endif
}
