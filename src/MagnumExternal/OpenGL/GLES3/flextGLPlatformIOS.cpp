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
#undef glGetInteger64vEXT
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
#undef glPolygonOffsetClampEXT
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
#undef glFramebufferSampleLocationsfvNV
#undef glNamedFramebufferSampleLocationsfvNV
#undef glResolveDepthValuesNV
#undef glGetBufferPointervOES
#undef glMapBufferOES
#undef glUnmapBufferOES
#undef glMinSampleShadingOES
#undef glTexStorage3DMultisampleOES
#undef glFramebufferTextureMultiviewOVR

#include <OpenGLES/ES3/glext.h>

void flextGLInit(Magnum::GL::Context&) {

    /* GL_EXT_copy_image */
    #if GL_EXT_copy_image
    flextGL.CopyImageSubDataEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint, GLint, GLint, GLint, GLuint, GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei)>(glCopyImageSubDataEXT);
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

    /* GL_EXT_disjoint_timer_query */
    #if GL_EXT_disjoint_timer_query
    flextGL.BeginQueryEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint)>(glBeginQueryEXT);
    flextGL.DeleteQueriesEXT = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLuint *)>(glDeleteQueriesEXT);
    flextGL.EndQueryEXT = reinterpret_cast<void(APIENTRY*)(GLenum)>(glEndQueryEXT);
    flextGL.GenQueriesEXT = reinterpret_cast<void(APIENTRY*)(GLsizei, GLuint *)>(glGenQueriesEXT);
    flextGL.GetInteger64vEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLint64 *)>(glGetInteger64vEXT);
    flextGL.GetQueryObjecti64vEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint64 *)>(glGetQueryObjecti64vEXT);
    flextGL.GetQueryObjectivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint *)>(glGetQueryObjectivEXT);
    flextGL.GetQueryObjectui64vEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint64 *)>(glGetQueryObjectui64vEXT);
    flextGL.GetQueryObjectuivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint *)>(glGetQueryObjectuivEXT);
    flextGL.GetQueryivEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLint *)>(glGetQueryivEXT);
    flextGL.IsQueryEXT = reinterpret_cast<GLboolean(APIENTRY*)(GLuint)>(glIsQueryEXT);
    flextGL.QueryCounterEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum)>(glQueryCounterEXT);
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

    /* GL_EXT_geometry_shader */
    #if GL_EXT_geometry_shader
    flextGL.FramebufferTextureEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLuint, GLint)>(glFramebufferTextureEXT);
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

    /* GL_EXT_polygon_offset_clamp */
    #if GL_EXT_polygon_offset_clamp
    flextGL.PolygonOffsetClampEXT = reinterpret_cast<void(APIENTRY*)(GLfloat, GLfloat, GLfloat)>(glPolygonOffsetClampEXT);
    #endif

    /* GL_EXT_primitive_bounding_box */
    #if GL_EXT_primitive_bounding_box
    flextGL.PrimitiveBoundingBoxEXT = reinterpret_cast<void(APIENTRY*)(GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat)>(glPrimitiveBoundingBoxEXT);
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
    flextGL.ProgramUniform1uiEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLuint)>(glProgramUniform1uiEXT);
    flextGL.ProgramUniform1uivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLuint *)>(glProgramUniform1uivEXT);
    flextGL.ProgramUniform2fEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLfloat, GLfloat)>(glProgramUniform2fEXT);
    flextGL.ProgramUniform2fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLfloat *)>(glProgramUniform2fvEXT);
    flextGL.ProgramUniform2iEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLint)>(glProgramUniform2iEXT);
    flextGL.ProgramUniform2ivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLint *)>(glProgramUniform2ivEXT);
    flextGL.ProgramUniform2uiEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLuint, GLuint)>(glProgramUniform2uiEXT);
    flextGL.ProgramUniform2uivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLuint *)>(glProgramUniform2uivEXT);
    flextGL.ProgramUniform3fEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLfloat, GLfloat, GLfloat)>(glProgramUniform3fEXT);
    flextGL.ProgramUniform3fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLfloat *)>(glProgramUniform3fvEXT);
    flextGL.ProgramUniform3iEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLint, GLint)>(glProgramUniform3iEXT);
    flextGL.ProgramUniform3ivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLint *)>(glProgramUniform3ivEXT);
    flextGL.ProgramUniform3uiEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLuint, GLuint, GLuint)>(glProgramUniform3uiEXT);
    flextGL.ProgramUniform3uivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLuint *)>(glProgramUniform3uivEXT);
    flextGL.ProgramUniform4fEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLfloat, GLfloat, GLfloat, GLfloat)>(glProgramUniform4fEXT);
    flextGL.ProgramUniform4fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLfloat *)>(glProgramUniform4fvEXT);
    flextGL.ProgramUniform4iEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLint, GLint, GLint)>(glProgramUniform4iEXT);
    flextGL.ProgramUniform4ivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLint *)>(glProgramUniform4ivEXT);
    flextGL.ProgramUniform4uiEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLuint, GLuint, GLuint, GLuint)>(glProgramUniform4uiEXT);
    flextGL.ProgramUniform4uivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLuint *)>(glProgramUniform4uivEXT);
    flextGL.ProgramUniformMatrix2fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(glProgramUniformMatrix2fvEXT);
    flextGL.ProgramUniformMatrix2x3fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(glProgramUniformMatrix2x3fvEXT);
    flextGL.ProgramUniformMatrix2x4fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(glProgramUniformMatrix2x4fvEXT);
    flextGL.ProgramUniformMatrix3fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(glProgramUniformMatrix3fvEXT);
    flextGL.ProgramUniformMatrix3x2fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(glProgramUniformMatrix3x2fvEXT);
    flextGL.ProgramUniformMatrix3x4fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(glProgramUniformMatrix3x4fvEXT);
    flextGL.ProgramUniformMatrix4fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(glProgramUniformMatrix4fvEXT);
    flextGL.ProgramUniformMatrix4x2fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(glProgramUniformMatrix4x2fvEXT);
    flextGL.ProgramUniformMatrix4x3fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(glProgramUniformMatrix4x3fvEXT);
    flextGL.UseProgramStagesEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLbitfield, GLuint)>(glUseProgramStagesEXT);
    flextGL.ValidateProgramPipelineEXT = reinterpret_cast<void(APIENTRY*)(GLuint)>(glValidateProgramPipelineEXT);
    #endif

    /* GL_EXT_tessellation_shader */
    #if GL_EXT_tessellation_shader
    flextGL.PatchParameteriEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLint)>(glPatchParameteriEXT);
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

    /* GL_EXT_texture_buffer */
    #if GL_EXT_texture_buffer
    flextGL.TexBufferEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLuint)>(glTexBufferEXT);
    flextGL.TexBufferRangeEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLuint, GLintptr, GLsizeiptr)>(glTexBufferRangeEXT);
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

    /* GL_NV_polygon_mode */
    #if GL_NV_polygon_mode
    flextGL.PolygonModeNV = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum)>(glPolygonModeNV);
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

    /* GL_OES_sample_shading */
    #if GL_OES_sample_shading
    flextGL.MinSampleShadingOES = reinterpret_cast<void(APIENTRY*)(GLfloat)>(glMinSampleShadingOES);
    #endif

    /* GL_OES_texture_storage_multisample_2d_array */
    #if GL_OES_texture_storage_multisample_2d_array
    flextGL.TexStorage3DMultisampleOES = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLsizei, GLboolean)>(glTexStorage3DMultisampleOES);
    #endif

    /* GL_OVR_multiview */
    #if GL_OVR_multiview
    flextGL.FramebufferTextureMultiviewOVR = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLuint, GLint, GLint, GLsizei)>(glFramebufferTextureMultiviewOVR);
    #endif
}
