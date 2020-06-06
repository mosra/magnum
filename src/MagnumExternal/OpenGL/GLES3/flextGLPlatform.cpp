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

#include <cstdio>
#include <cstdlib>

#include "Magnum/Platform/Implementation/OpenGLFunctionLoader.h"

void flextGLInit(Magnum::GL::Context&) {
    Magnum::Platform::Implementation::OpenGLFunctionLoader loader;

    /* GL_ES_VERSION_3_1 */
    flextGL.ActiveShaderProgram = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint)>(loader.load("glActiveShaderProgram"));
    flextGL.BindImageTexture = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLint, GLboolean, GLint, GLenum, GLenum)>(loader.load("glBindImageTexture"));
    flextGL.BindProgramPipeline = reinterpret_cast<void(APIENTRY*)(GLuint)>(loader.load("glBindProgramPipeline"));
    flextGL.BindVertexBuffer = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLintptr, GLsizei)>(loader.load("glBindVertexBuffer"));
    flextGL.CreateShaderProgramv = reinterpret_cast<GLuint(APIENTRY*)(GLenum, GLsizei, const GLchar *const*)>(loader.load("glCreateShaderProgramv"));
    flextGL.DeleteProgramPipelines = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLuint *)>(loader.load("glDeleteProgramPipelines"));
    flextGL.DispatchCompute = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLuint)>(loader.load("glDispatchCompute"));
    flextGL.DispatchComputeIndirect = reinterpret_cast<void(APIENTRY*)(GLintptr)>(loader.load("glDispatchComputeIndirect"));
    flextGL.DrawArraysIndirect = reinterpret_cast<void(APIENTRY*)(GLenum, const void *)>(loader.load("glDrawArraysIndirect"));
    flextGL.DrawElementsIndirect = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, const void *)>(loader.load("glDrawElementsIndirect"));
    flextGL.FramebufferParameteri = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLint)>(loader.load("glFramebufferParameteri"));
    flextGL.GenProgramPipelines = reinterpret_cast<void(APIENTRY*)(GLsizei, GLuint *)>(loader.load("glGenProgramPipelines"));
    flextGL.GetBooleani_v = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLboolean *)>(loader.load("glGetBooleani_v"));
    flextGL.GetFramebufferParameteriv = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLint *)>(loader.load("glGetFramebufferParameteriv"));
    flextGL.GetMultisamplefv = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLfloat *)>(loader.load("glGetMultisamplefv"));
    flextGL.GetProgramInterfaceiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLenum, GLint *)>(loader.load("glGetProgramInterfaceiv"));
    flextGL.GetProgramPipelineInfoLog = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, GLsizei *, GLchar *)>(loader.load("glGetProgramPipelineInfoLog"));
    flextGL.GetProgramPipelineiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint *)>(loader.load("glGetProgramPipelineiv"));
    flextGL.GetProgramResourceIndex = reinterpret_cast<GLuint(APIENTRY*)(GLuint, GLenum, const GLchar *)>(loader.load("glGetProgramResourceIndex"));
    flextGL.GetProgramResourceLocation = reinterpret_cast<GLint(APIENTRY*)(GLuint, GLenum, const GLchar *)>(loader.load("glGetProgramResourceLocation"));
    flextGL.GetProgramResourceName = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint, GLsizei, GLsizei *, GLchar *)>(loader.load("glGetProgramResourceName"));
    flextGL.GetProgramResourceiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint, GLsizei, const GLenum *, GLsizei, GLsizei *, GLint *)>(loader.load("glGetProgramResourceiv"));
    flextGL.GetTexLevelParameterfv = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLenum, GLfloat *)>(loader.load("glGetTexLevelParameterfv"));
    flextGL.GetTexLevelParameteriv = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLenum, GLint *)>(loader.load("glGetTexLevelParameteriv"));
    flextGL.IsProgramPipeline = reinterpret_cast<GLboolean(APIENTRY*)(GLuint)>(loader.load("glIsProgramPipeline"));
    flextGL.MemoryBarrier = reinterpret_cast<void(APIENTRY*)(GLbitfield)>(loader.load("glMemoryBarrier"));
    flextGL.MemoryBarrierByRegion = reinterpret_cast<void(APIENTRY*)(GLbitfield)>(loader.load("glMemoryBarrierByRegion"));
    flextGL.ProgramUniform1f = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLfloat)>(loader.load("glProgramUniform1f"));
    flextGL.ProgramUniform1fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLfloat *)>(loader.load("glProgramUniform1fv"));
    flextGL.ProgramUniform1i = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint)>(loader.load("glProgramUniform1i"));
    flextGL.ProgramUniform1iv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLint *)>(loader.load("glProgramUniform1iv"));
    flextGL.ProgramUniform1ui = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLuint)>(loader.load("glProgramUniform1ui"));
    flextGL.ProgramUniform1uiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLuint *)>(loader.load("glProgramUniform1uiv"));
    flextGL.ProgramUniform2f = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLfloat, GLfloat)>(loader.load("glProgramUniform2f"));
    flextGL.ProgramUniform2fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLfloat *)>(loader.load("glProgramUniform2fv"));
    flextGL.ProgramUniform2i = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLint)>(loader.load("glProgramUniform2i"));
    flextGL.ProgramUniform2iv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLint *)>(loader.load("glProgramUniform2iv"));
    flextGL.ProgramUniform2ui = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLuint, GLuint)>(loader.load("glProgramUniform2ui"));
    flextGL.ProgramUniform2uiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLuint *)>(loader.load("glProgramUniform2uiv"));
    flextGL.ProgramUniform3f = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLfloat, GLfloat, GLfloat)>(loader.load("glProgramUniform3f"));
    flextGL.ProgramUniform3fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLfloat *)>(loader.load("glProgramUniform3fv"));
    flextGL.ProgramUniform3i = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLint, GLint)>(loader.load("glProgramUniform3i"));
    flextGL.ProgramUniform3iv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLint *)>(loader.load("glProgramUniform3iv"));
    flextGL.ProgramUniform3ui = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLuint, GLuint, GLuint)>(loader.load("glProgramUniform3ui"));
    flextGL.ProgramUniform3uiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLuint *)>(loader.load("glProgramUniform3uiv"));
    flextGL.ProgramUniform4f = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLfloat, GLfloat, GLfloat, GLfloat)>(loader.load("glProgramUniform4f"));
    flextGL.ProgramUniform4fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLfloat *)>(loader.load("glProgramUniform4fv"));
    flextGL.ProgramUniform4i = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLint, GLint, GLint)>(loader.load("glProgramUniform4i"));
    flextGL.ProgramUniform4iv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLint *)>(loader.load("glProgramUniform4iv"));
    flextGL.ProgramUniform4ui = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLuint, GLuint, GLuint, GLuint)>(loader.load("glProgramUniform4ui"));
    flextGL.ProgramUniform4uiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLuint *)>(loader.load("glProgramUniform4uiv"));
    flextGL.ProgramUniformMatrix2fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix2fv"));
    flextGL.ProgramUniformMatrix2x3fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix2x3fv"));
    flextGL.ProgramUniformMatrix2x4fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix2x4fv"));
    flextGL.ProgramUniformMatrix3fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix3fv"));
    flextGL.ProgramUniformMatrix3x2fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix3x2fv"));
    flextGL.ProgramUniformMatrix3x4fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix3x4fv"));
    flextGL.ProgramUniformMatrix4fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix4fv"));
    flextGL.ProgramUniformMatrix4x2fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix4x2fv"));
    flextGL.ProgramUniformMatrix4x3fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix4x3fv"));
    flextGL.SampleMaski = reinterpret_cast<void(APIENTRY*)(GLuint, GLbitfield)>(loader.load("glSampleMaski"));
    flextGL.TexStorage2DMultisample = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLboolean)>(loader.load("glTexStorage2DMultisample"));
    flextGL.UseProgramStages = reinterpret_cast<void(APIENTRY*)(GLuint, GLbitfield, GLuint)>(loader.load("glUseProgramStages"));
    flextGL.ValidateProgramPipeline = reinterpret_cast<void(APIENTRY*)(GLuint)>(loader.load("glValidateProgramPipeline"));
    flextGL.VertexAttribBinding = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint)>(loader.load("glVertexAttribBinding"));
    flextGL.VertexAttribFormat = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLenum, GLboolean, GLuint)>(loader.load("glVertexAttribFormat"));
    flextGL.VertexAttribIFormat = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLenum, GLuint)>(loader.load("glVertexAttribIFormat"));
    flextGL.VertexBindingDivisor = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint)>(loader.load("glVertexBindingDivisor"));

    /* GL_ES_VERSION_3_2 */
    flextGL.BlendBarrier = reinterpret_cast<void(APIENTRY*)(void)>(loader.load("glBlendBarrier"));
    flextGL.BlendEquationSeparatei = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLenum)>(loader.load("glBlendEquationSeparatei"));
    flextGL.BlendEquationi = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum)>(loader.load("glBlendEquationi"));
    flextGL.BlendFuncSeparatei = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLenum, GLenum, GLenum)>(loader.load("glBlendFuncSeparatei"));
    flextGL.BlendFunci = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLenum)>(loader.load("glBlendFunci"));
    flextGL.ColorMaski = reinterpret_cast<void(APIENTRY*)(GLuint, GLboolean, GLboolean, GLboolean, GLboolean)>(loader.load("glColorMaski"));
    flextGL.CopyImageSubData = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint, GLint, GLint, GLint, GLuint, GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei)>(loader.load("glCopyImageSubData"));
    flextGL.DebugMessageCallback = reinterpret_cast<void(APIENTRY*)(GLDEBUGPROC, const void *)>(loader.load("glDebugMessageCallback"));
    flextGL.DebugMessageControl = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLenum, GLsizei, const GLuint *, GLboolean)>(loader.load("glDebugMessageControl"));
    flextGL.DebugMessageInsert = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar *)>(loader.load("glDebugMessageInsert"));
    flextGL.Disablei = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint)>(loader.load("glDisablei"));
    flextGL.DrawElementsBaseVertex = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, const void *, GLint)>(loader.load("glDrawElementsBaseVertex"));
    flextGL.DrawElementsInstancedBaseVertex = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, const void *, GLsizei, GLint)>(loader.load("glDrawElementsInstancedBaseVertex"));
    flextGL.DrawRangeElementsBaseVertex = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLuint, GLsizei, GLenum, const void *, GLint)>(loader.load("glDrawRangeElementsBaseVertex"));
    flextGL.Enablei = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint)>(loader.load("glEnablei"));
    flextGL.FramebufferTexture = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLuint, GLint)>(loader.load("glFramebufferTexture"));
    flextGL.GetDebugMessageLog = reinterpret_cast<GLuint(APIENTRY*)(GLuint, GLsizei, GLenum *, GLenum *, GLuint *, GLenum *, GLsizei *, GLchar *)>(loader.load("glGetDebugMessageLog"));
    flextGL.GetGraphicsResetStatus = reinterpret_cast<GLenum(APIENTRY*)(void)>(loader.load("glGetGraphicsResetStatus"));
    flextGL.GetObjectLabel = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLsizei, GLsizei *, GLchar *)>(loader.load("glGetObjectLabel"));
    flextGL.GetObjectPtrLabel = reinterpret_cast<void(APIENTRY*)(const void *, GLsizei, GLsizei *, GLchar *)>(loader.load("glGetObjectPtrLabel"));
    flextGL.GetPointerv = reinterpret_cast<void(APIENTRY*)(GLenum, void **)>(loader.load("glGetPointerv"));
    flextGL.GetSamplerParameterIiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint *)>(loader.load("glGetSamplerParameterIiv"));
    flextGL.GetSamplerParameterIuiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint *)>(loader.load("glGetSamplerParameterIuiv"));
    flextGL.GetTexParameterIiv = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLint *)>(loader.load("glGetTexParameterIiv"));
    flextGL.GetTexParameterIuiv = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLuint *)>(loader.load("glGetTexParameterIuiv"));
    flextGL.GetnUniformfv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLfloat *)>(loader.load("glGetnUniformfv"));
    flextGL.GetnUniformiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLint *)>(loader.load("glGetnUniformiv"));
    flextGL.GetnUniformuiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLuint *)>(loader.load("glGetnUniformuiv"));
    flextGL.IsEnabledi = reinterpret_cast<GLboolean(APIENTRY*)(GLenum, GLuint)>(loader.load("glIsEnabledi"));
    flextGL.MinSampleShading = reinterpret_cast<void(APIENTRY*)(GLfloat)>(loader.load("glMinSampleShading"));
    flextGL.ObjectLabel = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLsizei, const GLchar *)>(loader.load("glObjectLabel"));
    flextGL.ObjectPtrLabel = reinterpret_cast<void(APIENTRY*)(const void *, GLsizei, const GLchar *)>(loader.load("glObjectPtrLabel"));
    flextGL.PatchParameteri = reinterpret_cast<void(APIENTRY*)(GLenum, GLint)>(loader.load("glPatchParameteri"));
    flextGL.PopDebugGroup = reinterpret_cast<void(APIENTRY*)(void)>(loader.load("glPopDebugGroup"));
    flextGL.PrimitiveBoundingBox = reinterpret_cast<void(APIENTRY*)(GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat)>(loader.load("glPrimitiveBoundingBox"));
    flextGL.PushDebugGroup = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLsizei, const GLchar *)>(loader.load("glPushDebugGroup"));
    flextGL.ReadnPixels = reinterpret_cast<void(APIENTRY*)(GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, GLsizei, void *)>(loader.load("glReadnPixels"));
    flextGL.SamplerParameterIiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, const GLint *)>(loader.load("glSamplerParameterIiv"));
    flextGL.SamplerParameterIuiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, const GLuint *)>(loader.load("glSamplerParameterIuiv"));
    flextGL.TexBuffer = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLuint)>(loader.load("glTexBuffer"));
    flextGL.TexBufferRange = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLuint, GLintptr, GLsizeiptr)>(loader.load("glTexBufferRange"));
    flextGL.TexParameterIiv = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, const GLint *)>(loader.load("glTexParameterIiv"));
    flextGL.TexParameterIuiv = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, const GLuint *)>(loader.load("glTexParameterIuiv"));
    flextGL.TexStorage3DMultisample = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLsizei, GLboolean)>(loader.load("glTexStorage3DMultisample"));

    /* GL_EXT_copy_image */
    flextGL.CopyImageSubDataEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint, GLint, GLint, GLint, GLuint, GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei)>(loader.load("glCopyImageSubDataEXT"));

    /* GL_EXT_debug_label */
    flextGL.GetObjectLabelEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLsizei, GLsizei *, GLchar *)>(loader.load("glGetObjectLabelEXT"));
    flextGL.LabelObjectEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLsizei, const GLchar *)>(loader.load("glLabelObjectEXT"));

    /* GL_EXT_debug_marker */
    flextGL.InsertEventMarkerEXT = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLchar *)>(loader.load("glInsertEventMarkerEXT"));
    flextGL.PopGroupMarkerEXT = reinterpret_cast<void(APIENTRY*)(void)>(loader.load("glPopGroupMarkerEXT"));
    flextGL.PushGroupMarkerEXT = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLchar *)>(loader.load("glPushGroupMarkerEXT"));

    /* GL_EXT_disjoint_timer_query */
    flextGL.BeginQueryEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint)>(loader.load("glBeginQueryEXT"));
    flextGL.DeleteQueriesEXT = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLuint *)>(loader.load("glDeleteQueriesEXT"));
    flextGL.EndQueryEXT = reinterpret_cast<void(APIENTRY*)(GLenum)>(loader.load("glEndQueryEXT"));
    flextGL.GenQueriesEXT = reinterpret_cast<void(APIENTRY*)(GLsizei, GLuint *)>(loader.load("glGenQueriesEXT"));
    flextGL.GetInteger64vEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLint64 *)>(loader.load("glGetInteger64vEXT"));
    flextGL.GetQueryObjecti64vEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint64 *)>(loader.load("glGetQueryObjecti64vEXT"));
    flextGL.GetQueryObjectivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint *)>(loader.load("glGetQueryObjectivEXT"));
    flextGL.GetQueryObjectui64vEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint64 *)>(loader.load("glGetQueryObjectui64vEXT"));
    flextGL.GetQueryObjectuivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint *)>(loader.load("glGetQueryObjectuivEXT"));
    flextGL.GetQueryivEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLint *)>(loader.load("glGetQueryivEXT"));
    flextGL.IsQueryEXT = reinterpret_cast<GLboolean(APIENTRY*)(GLuint)>(loader.load("glIsQueryEXT"));
    flextGL.QueryCounterEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum)>(loader.load("glQueryCounterEXT"));

    /* GL_EXT_draw_buffers_indexed */
    flextGL.BlendEquationSeparateiEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLenum)>(loader.load("glBlendEquationSeparateiEXT"));
    flextGL.BlendEquationiEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum)>(loader.load("glBlendEquationiEXT"));
    flextGL.BlendFuncSeparateiEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLenum, GLenum, GLenum)>(loader.load("glBlendFuncSeparateiEXT"));
    flextGL.BlendFunciEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLenum)>(loader.load("glBlendFunciEXT"));
    flextGL.ColorMaskiEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLboolean, GLboolean, GLboolean, GLboolean)>(loader.load("glColorMaskiEXT"));
    flextGL.DisableiEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint)>(loader.load("glDisableiEXT"));
    flextGL.EnableiEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint)>(loader.load("glEnableiEXT"));
    flextGL.IsEnablediEXT = reinterpret_cast<GLboolean(APIENTRY*)(GLenum, GLuint)>(loader.load("glIsEnablediEXT"));

    /* GL_EXT_geometry_shader */
    flextGL.FramebufferTextureEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLuint, GLint)>(loader.load("glFramebufferTextureEXT"));

    /* GL_EXT_multi_draw_arrays */
    flextGL.MultiDrawArraysEXT = reinterpret_cast<void(APIENTRY*)(GLenum, const GLint *, const GLsizei *, GLsizei)>(loader.load("glMultiDrawArraysEXT"));
    flextGL.MultiDrawElementsEXT = reinterpret_cast<void(APIENTRY*)(GLenum, const GLsizei *, GLenum, const void *const*, GLsizei)>(loader.load("glMultiDrawElementsEXT"));

    /* GL_EXT_multisampled_render_to_texture */
    flextGL.FramebufferTexture2DMultisampleEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLenum, GLuint, GLint, GLsizei)>(loader.load("glFramebufferTexture2DMultisampleEXT"));
    flextGL.RenderbufferStorageMultisampleEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, GLsizei, GLsizei)>(loader.load("glRenderbufferStorageMultisampleEXT"));

    /* GL_EXT_polygon_offset_clamp */
    flextGL.PolygonOffsetClampEXT = reinterpret_cast<void(APIENTRY*)(GLfloat, GLfloat, GLfloat)>(loader.load("glPolygonOffsetClampEXT"));

    /* GL_EXT_primitive_bounding_box */
    flextGL.PrimitiveBoundingBoxEXT = reinterpret_cast<void(APIENTRY*)(GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat)>(loader.load("glPrimitiveBoundingBoxEXT"));

    /* GL_EXT_robustness */
    flextGL.GetGraphicsResetStatusEXT = reinterpret_cast<GLenum(APIENTRY*)(void)>(loader.load("glGetGraphicsResetStatusEXT"));
    flextGL.GetnUniformfvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLfloat *)>(loader.load("glGetnUniformfvEXT"));
    flextGL.GetnUniformivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLint *)>(loader.load("glGetnUniformivEXT"));
    flextGL.ReadnPixelsEXT = reinterpret_cast<void(APIENTRY*)(GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, GLsizei, void *)>(loader.load("glReadnPixelsEXT"));

    /* GL_EXT_separate_shader_objects */
    flextGL.ActiveShaderProgramEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint)>(loader.load("glActiveShaderProgramEXT"));
    flextGL.BindProgramPipelineEXT = reinterpret_cast<void(APIENTRY*)(GLuint)>(loader.load("glBindProgramPipelineEXT"));
    flextGL.CreateShaderProgramvEXT = reinterpret_cast<GLuint(APIENTRY*)(GLenum, GLsizei, const GLchar **)>(loader.load("glCreateShaderProgramvEXT"));
    flextGL.DeleteProgramPipelinesEXT = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLuint *)>(loader.load("glDeleteProgramPipelinesEXT"));
    flextGL.GenProgramPipelinesEXT = reinterpret_cast<void(APIENTRY*)(GLsizei, GLuint *)>(loader.load("glGenProgramPipelinesEXT"));
    flextGL.GetProgramPipelineInfoLogEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, GLsizei *, GLchar *)>(loader.load("glGetProgramPipelineInfoLogEXT"));
    flextGL.GetProgramPipelineivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint *)>(loader.load("glGetProgramPipelineivEXT"));
    flextGL.IsProgramPipelineEXT = reinterpret_cast<GLboolean(APIENTRY*)(GLuint)>(loader.load("glIsProgramPipelineEXT"));
    flextGL.ProgramParameteriEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint)>(loader.load("glProgramParameteriEXT"));
    flextGL.ProgramUniform1fEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLfloat)>(loader.load("glProgramUniform1fEXT"));
    flextGL.ProgramUniform1fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLfloat *)>(loader.load("glProgramUniform1fvEXT"));
    flextGL.ProgramUniform1iEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint)>(loader.load("glProgramUniform1iEXT"));
    flextGL.ProgramUniform1ivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLint *)>(loader.load("glProgramUniform1ivEXT"));
    flextGL.ProgramUniform1uiEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLuint)>(loader.load("glProgramUniform1uiEXT"));
    flextGL.ProgramUniform1uivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLuint *)>(loader.load("glProgramUniform1uivEXT"));
    flextGL.ProgramUniform2fEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLfloat, GLfloat)>(loader.load("glProgramUniform2fEXT"));
    flextGL.ProgramUniform2fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLfloat *)>(loader.load("glProgramUniform2fvEXT"));
    flextGL.ProgramUniform2iEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLint)>(loader.load("glProgramUniform2iEXT"));
    flextGL.ProgramUniform2ivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLint *)>(loader.load("glProgramUniform2ivEXT"));
    flextGL.ProgramUniform2uiEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLuint, GLuint)>(loader.load("glProgramUniform2uiEXT"));
    flextGL.ProgramUniform2uivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLuint *)>(loader.load("glProgramUniform2uivEXT"));
    flextGL.ProgramUniform3fEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLfloat, GLfloat, GLfloat)>(loader.load("glProgramUniform3fEXT"));
    flextGL.ProgramUniform3fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLfloat *)>(loader.load("glProgramUniform3fvEXT"));
    flextGL.ProgramUniform3iEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLint, GLint)>(loader.load("glProgramUniform3iEXT"));
    flextGL.ProgramUniform3ivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLint *)>(loader.load("glProgramUniform3ivEXT"));
    flextGL.ProgramUniform3uiEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLuint, GLuint, GLuint)>(loader.load("glProgramUniform3uiEXT"));
    flextGL.ProgramUniform3uivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLuint *)>(loader.load("glProgramUniform3uivEXT"));
    flextGL.ProgramUniform4fEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLfloat, GLfloat, GLfloat, GLfloat)>(loader.load("glProgramUniform4fEXT"));
    flextGL.ProgramUniform4fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLfloat *)>(loader.load("glProgramUniform4fvEXT"));
    flextGL.ProgramUniform4iEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLint, GLint, GLint)>(loader.load("glProgramUniform4iEXT"));
    flextGL.ProgramUniform4ivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLint *)>(loader.load("glProgramUniform4ivEXT"));
    flextGL.ProgramUniform4uiEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLuint, GLuint, GLuint, GLuint)>(loader.load("glProgramUniform4uiEXT"));
    flextGL.ProgramUniform4uivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLuint *)>(loader.load("glProgramUniform4uivEXT"));
    flextGL.ProgramUniformMatrix2fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix2fvEXT"));
    flextGL.ProgramUniformMatrix2x3fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix2x3fvEXT"));
    flextGL.ProgramUniformMatrix2x4fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix2x4fvEXT"));
    flextGL.ProgramUniformMatrix3fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix3fvEXT"));
    flextGL.ProgramUniformMatrix3x2fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix3x2fvEXT"));
    flextGL.ProgramUniformMatrix3x4fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix3x4fvEXT"));
    flextGL.ProgramUniformMatrix4fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix4fvEXT"));
    flextGL.ProgramUniformMatrix4x2fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix4x2fvEXT"));
    flextGL.ProgramUniformMatrix4x3fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix4x3fvEXT"));
    flextGL.UseProgramStagesEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLbitfield, GLuint)>(loader.load("glUseProgramStagesEXT"));
    flextGL.ValidateProgramPipelineEXT = reinterpret_cast<void(APIENTRY*)(GLuint)>(loader.load("glValidateProgramPipelineEXT"));

    /* GL_EXT_tessellation_shader */
    flextGL.PatchParameteriEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLint)>(loader.load("glPatchParameteriEXT"));

    /* GL_EXT_texture_border_clamp */
    flextGL.GetSamplerParameterIivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint *)>(loader.load("glGetSamplerParameterIivEXT"));
    flextGL.GetSamplerParameterIuivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint *)>(loader.load("glGetSamplerParameterIuivEXT"));
    flextGL.GetTexParameterIivEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLint *)>(loader.load("glGetTexParameterIivEXT"));
    flextGL.GetTexParameterIuivEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLuint *)>(loader.load("glGetTexParameterIuivEXT"));
    flextGL.SamplerParameterIivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, const GLint *)>(loader.load("glSamplerParameterIivEXT"));
    flextGL.SamplerParameterIuivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, const GLuint *)>(loader.load("glSamplerParameterIuivEXT"));
    flextGL.TexParameterIivEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, const GLint *)>(loader.load("glTexParameterIivEXT"));
    flextGL.TexParameterIuivEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, const GLuint *)>(loader.load("glTexParameterIuivEXT"));

    /* GL_EXT_texture_buffer */
    flextGL.TexBufferEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLuint)>(loader.load("glTexBufferEXT"));
    flextGL.TexBufferRangeEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLuint, GLintptr, GLsizeiptr)>(loader.load("glTexBufferRangeEXT"));

    /* GL_KHR_blend_equation_advanced */
    flextGL.BlendBarrierKHR = reinterpret_cast<void(APIENTRY*)(void)>(loader.load("glBlendBarrierKHR"));

    /* GL_KHR_debug */
    flextGL.DebugMessageCallbackKHR = reinterpret_cast<void(APIENTRY*)(GLDEBUGPROCKHR, const void *)>(loader.load("glDebugMessageCallbackKHR"));
    flextGL.DebugMessageControlKHR = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLenum, GLsizei, const GLuint *, GLboolean)>(loader.load("glDebugMessageControlKHR"));
    flextGL.DebugMessageInsertKHR = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar *)>(loader.load("glDebugMessageInsertKHR"));
    flextGL.GetDebugMessageLogKHR = reinterpret_cast<GLuint(APIENTRY*)(GLuint, GLsizei, GLenum *, GLenum *, GLuint *, GLenum *, GLsizei *, GLchar *)>(loader.load("glGetDebugMessageLogKHR"));
    flextGL.GetObjectLabelKHR = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLsizei, GLsizei *, GLchar *)>(loader.load("glGetObjectLabelKHR"));
    flextGL.GetObjectPtrLabelKHR = reinterpret_cast<void(APIENTRY*)(const void *, GLsizei, GLsizei *, GLchar *)>(loader.load("glGetObjectPtrLabelKHR"));
    flextGL.GetPointervKHR = reinterpret_cast<void(APIENTRY*)(GLenum, void **)>(loader.load("glGetPointervKHR"));
    flextGL.ObjectLabelKHR = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLsizei, const GLchar *)>(loader.load("glObjectLabelKHR"));
    flextGL.ObjectPtrLabelKHR = reinterpret_cast<void(APIENTRY*)(const void *, GLsizei, const GLchar *)>(loader.load("glObjectPtrLabelKHR"));
    flextGL.PopDebugGroupKHR = reinterpret_cast<void(APIENTRY*)(void)>(loader.load("glPopDebugGroupKHR"));
    flextGL.PushDebugGroupKHR = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLsizei, const GLchar *)>(loader.load("glPushDebugGroupKHR"));

    /* GL_KHR_robustness */
    flextGL.GetGraphicsResetStatusKHR = reinterpret_cast<GLenum(APIENTRY*)(void)>(loader.load("glGetGraphicsResetStatusKHR"));
    flextGL.GetnUniformfvKHR = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLfloat *)>(loader.load("glGetnUniformfvKHR"));
    flextGL.GetnUniformivKHR = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLint *)>(loader.load("glGetnUniformivKHR"));
    flextGL.GetnUniformuivKHR = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLuint *)>(loader.load("glGetnUniformuivKHR"));
    flextGL.ReadnPixelsKHR = reinterpret_cast<void(APIENTRY*)(GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, GLsizei, void *)>(loader.load("glReadnPixelsKHR"));

    /* GL_NV_polygon_mode */
    flextGL.PolygonModeNV = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum)>(loader.load("glPolygonModeNV"));

    /* GL_NV_sample_locations */
    flextGL.FramebufferSampleLocationsfvNV = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLsizei, const GLfloat *)>(loader.load("glFramebufferSampleLocationsfvNV"));
    flextGL.NamedFramebufferSampleLocationsfvNV = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLsizei, const GLfloat *)>(loader.load("glNamedFramebufferSampleLocationsfvNV"));
    flextGL.ResolveDepthValuesNV = reinterpret_cast<void(APIENTRY*)(void)>(loader.load("glResolveDepthValuesNV"));

    /* GL_OES_mapbuffer */
    flextGL.GetBufferPointervOES = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, void **)>(loader.load("glGetBufferPointervOES"));
    flextGL.MapBufferOES = reinterpret_cast<void *(APIENTRY*)(GLenum, GLenum)>(loader.load("glMapBufferOES"));
    flextGL.UnmapBufferOES = reinterpret_cast<GLboolean(APIENTRY*)(GLenum)>(loader.load("glUnmapBufferOES"));

    /* GL_OES_sample_shading */
    flextGL.MinSampleShadingOES = reinterpret_cast<void(APIENTRY*)(GLfloat)>(loader.load("glMinSampleShadingOES"));

    /* GL_OES_texture_storage_multisample_2d_array */
    flextGL.TexStorage3DMultisampleOES = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLsizei, GLboolean)>(loader.load("glTexStorage3DMultisampleOES"));

    /* GL_OVR_multiview */
    flextGL.FramebufferTextureMultiviewOVR = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLuint, GLint, GLint, GLsizei)>(loader.load("glFramebufferTextureMultiviewOVR"));
}
