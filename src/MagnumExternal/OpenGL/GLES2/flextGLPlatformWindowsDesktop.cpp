/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021 Vladimír Vondruš <mosra@centrum.cz>

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

#include "flextGLWindowsDesktop.h"

#include <cstdio>
#include <cstdlib>

#include "Magnum/Platform/Implementation/OpenGLFunctionLoader.h"

void flextGLInit(Magnum::GL::Context&) {
    Magnum::Platform::Implementation::OpenGLFunctionLoader loader;

    /* GL_ANGLE_framebuffer_blit */
    flextGL.BlitFramebufferANGLE = reinterpret_cast<void(APIENTRY*)(GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLbitfield, GLenum)>(loader.load("glBlitFramebufferANGLE"));

    /* GL_ANGLE_framebuffer_multisample */
    flextGL.RenderbufferStorageMultisampleANGLE = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, GLsizei, GLsizei)>(loader.load("glRenderbufferStorageMultisampleANGLE"));

    /* GL_ANGLE_instanced_arrays */
    flextGL.DrawArraysInstancedANGLE = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLsizei, GLsizei)>(loader.load("glDrawArraysInstancedANGLE"));
    flextGL.DrawElementsInstancedANGLE = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, const void *, GLsizei)>(loader.load("glDrawElementsInstancedANGLE"));
    flextGL.VertexAttribDivisorANGLE = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint)>(loader.load("glVertexAttribDivisorANGLE"));

    /* GL_ANGLE_multi_draw */
    flextGL.MultiDrawArraysANGLE = reinterpret_cast<void(APIENTRY*)(GLenum, const GLint *, const GLsizei *, GLsizei)>(loader.load("glMultiDrawArraysANGLE"));
    flextGL.MultiDrawArraysInstancedANGLE = reinterpret_cast<void(APIENTRY*)(GLenum, const GLint *, const GLsizei *, const GLsizei *, GLsizei)>(loader.load("glMultiDrawArraysInstancedANGLE"));
    flextGL.MultiDrawElementsANGLE = reinterpret_cast<void(APIENTRY*)(GLenum, const GLsizei *, GLenum, const GLvoid *const*, GLsizei)>(loader.load("glMultiDrawElementsANGLE"));
    flextGL.MultiDrawElementsInstancedANGLE = reinterpret_cast<void(APIENTRY*)(GLenum, const GLsizei *, GLenum, const GLvoid *const*, const GLsizei*, GLsizei)>(loader.load("glMultiDrawElementsInstancedANGLE"));

    /* GL_APPLE_framebuffer_multisample */
    flextGL.RenderbufferStorageMultisampleAPPLE = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, GLsizei, GLsizei)>(loader.load("glRenderbufferStorageMultisampleAPPLE"));
    flextGL.ResolveMultisampleFramebufferAPPLE = reinterpret_cast<void(APIENTRY*)(void)>(loader.load("glResolveMultisampleFramebufferAPPLE"));

    /* GL_ES_VERSION_2_0 */
    flextGL.ActiveTexture = reinterpret_cast<void(APIENTRY*)(GLenum)>(loader.load("glActiveTexture"));
    flextGL.AttachShader = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint)>(loader.load("glAttachShader"));
    flextGL.BindAttribLocation = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, const GLchar *)>(loader.load("glBindAttribLocation"));
    flextGL.BindBuffer = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint)>(loader.load("glBindBuffer"));
    flextGL.BindFramebuffer = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint)>(loader.load("glBindFramebuffer"));
    flextGL.BindRenderbuffer = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint)>(loader.load("glBindRenderbuffer"));
    flextGL.BlendColor = reinterpret_cast<void(APIENTRY*)(GLfloat, GLfloat, GLfloat, GLfloat)>(loader.load("glBlendColor"));
    flextGL.BlendEquation = reinterpret_cast<void(APIENTRY*)(GLenum)>(loader.load("glBlendEquation"));
    flextGL.BlendEquationSeparate = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum)>(loader.load("glBlendEquationSeparate"));
    flextGL.BlendFuncSeparate = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLenum, GLenum)>(loader.load("glBlendFuncSeparate"));
    flextGL.BufferData = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizeiptr, const void *, GLenum)>(loader.load("glBufferData"));
    flextGL.BufferSubData = reinterpret_cast<void(APIENTRY*)(GLenum, GLintptr, GLsizeiptr, const void *)>(loader.load("glBufferSubData"));
    flextGL.CheckFramebufferStatus = reinterpret_cast<GLenum(APIENTRY*)(GLenum)>(loader.load("glCheckFramebufferStatus"));
    flextGL.ClearDepthf = reinterpret_cast<void(APIENTRY*)(GLfloat)>(loader.load("glClearDepthf"));
    flextGL.CompileShader = reinterpret_cast<void(APIENTRY*)(GLuint)>(loader.load("glCompileShader"));
    flextGL.CompressedTexImage2D = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLenum, GLsizei, GLsizei, GLint, GLsizei, const void *)>(loader.load("glCompressedTexImage2D"));
    flextGL.CompressedTexSubImage2D = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLsizei, const void *)>(loader.load("glCompressedTexSubImage2D"));
    flextGL.CreateProgram = reinterpret_cast<GLuint(APIENTRY*)(void)>(loader.load("glCreateProgram"));
    flextGL.CreateShader = reinterpret_cast<GLuint(APIENTRY*)(GLenum)>(loader.load("glCreateShader"));
    flextGL.DeleteBuffers = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLuint *)>(loader.load("glDeleteBuffers"));
    flextGL.DeleteFramebuffers = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLuint *)>(loader.load("glDeleteFramebuffers"));
    flextGL.DeleteProgram = reinterpret_cast<void(APIENTRY*)(GLuint)>(loader.load("glDeleteProgram"));
    flextGL.DeleteRenderbuffers = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLuint *)>(loader.load("glDeleteRenderbuffers"));
    flextGL.DeleteShader = reinterpret_cast<void(APIENTRY*)(GLuint)>(loader.load("glDeleteShader"));
    flextGL.DepthRangef = reinterpret_cast<void(APIENTRY*)(GLfloat, GLfloat)>(loader.load("glDepthRangef"));
    flextGL.DetachShader = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint)>(loader.load("glDetachShader"));
    flextGL.DisableVertexAttribArray = reinterpret_cast<void(APIENTRY*)(GLuint)>(loader.load("glDisableVertexAttribArray"));
    flextGL.EnableVertexAttribArray = reinterpret_cast<void(APIENTRY*)(GLuint)>(loader.load("glEnableVertexAttribArray"));
    flextGL.FramebufferRenderbuffer = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLenum, GLuint)>(loader.load("glFramebufferRenderbuffer"));
    flextGL.FramebufferTexture2D = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLenum, GLuint, GLint)>(loader.load("glFramebufferTexture2D"));
    flextGL.GenBuffers = reinterpret_cast<void(APIENTRY*)(GLsizei, GLuint *)>(loader.load("glGenBuffers"));
    flextGL.GenFramebuffers = reinterpret_cast<void(APIENTRY*)(GLsizei, GLuint *)>(loader.load("glGenFramebuffers"));
    flextGL.GenRenderbuffers = reinterpret_cast<void(APIENTRY*)(GLsizei, GLuint *)>(loader.load("glGenRenderbuffers"));
    flextGL.GenerateMipmap = reinterpret_cast<void(APIENTRY*)(GLenum)>(loader.load("glGenerateMipmap"));
    flextGL.GetActiveAttrib = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLsizei, GLsizei *, GLint *, GLenum *, GLchar *)>(loader.load("glGetActiveAttrib"));
    flextGL.GetActiveUniform = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLsizei, GLsizei *, GLint *, GLenum *, GLchar *)>(loader.load("glGetActiveUniform"));
    flextGL.GetAttachedShaders = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, GLsizei *, GLuint *)>(loader.load("glGetAttachedShaders"));
    flextGL.GetAttribLocation = reinterpret_cast<GLint(APIENTRY*)(GLuint, const GLchar *)>(loader.load("glGetAttribLocation"));
    flextGL.GetBufferParameteriv = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLint *)>(loader.load("glGetBufferParameteriv"));
    flextGL.GetFramebufferAttachmentParameteriv = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLenum, GLint *)>(loader.load("glGetFramebufferAttachmentParameteriv"));
    flextGL.GetProgramInfoLog = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, GLsizei *, GLchar *)>(loader.load("glGetProgramInfoLog"));
    flextGL.GetProgramiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint *)>(loader.load("glGetProgramiv"));
    flextGL.GetRenderbufferParameteriv = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLint *)>(loader.load("glGetRenderbufferParameteriv"));
    flextGL.GetShaderInfoLog = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, GLsizei *, GLchar *)>(loader.load("glGetShaderInfoLog"));
    flextGL.GetShaderPrecisionFormat = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLint *, GLint *)>(loader.load("glGetShaderPrecisionFormat"));
    flextGL.GetShaderSource = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, GLsizei *, GLchar *)>(loader.load("glGetShaderSource"));
    flextGL.GetShaderiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint *)>(loader.load("glGetShaderiv"));
    flextGL.GetUniformLocation = reinterpret_cast<GLint(APIENTRY*)(GLuint, const GLchar *)>(loader.load("glGetUniformLocation"));
    flextGL.GetUniformfv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLfloat *)>(loader.load("glGetUniformfv"));
    flextGL.GetUniformiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint *)>(loader.load("glGetUniformiv"));
    flextGL.GetVertexAttribPointerv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, void **)>(loader.load("glGetVertexAttribPointerv"));
    flextGL.GetVertexAttribfv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLfloat *)>(loader.load("glGetVertexAttribfv"));
    flextGL.GetVertexAttribiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint *)>(loader.load("glGetVertexAttribiv"));
    flextGL.IsBuffer = reinterpret_cast<GLboolean(APIENTRY*)(GLuint)>(loader.load("glIsBuffer"));
    flextGL.IsFramebuffer = reinterpret_cast<GLboolean(APIENTRY*)(GLuint)>(loader.load("glIsFramebuffer"));
    flextGL.IsProgram = reinterpret_cast<GLboolean(APIENTRY*)(GLuint)>(loader.load("glIsProgram"));
    flextGL.IsRenderbuffer = reinterpret_cast<GLboolean(APIENTRY*)(GLuint)>(loader.load("glIsRenderbuffer"));
    flextGL.IsShader = reinterpret_cast<GLboolean(APIENTRY*)(GLuint)>(loader.load("glIsShader"));
    flextGL.LinkProgram = reinterpret_cast<void(APIENTRY*)(GLuint)>(loader.load("glLinkProgram"));
    flextGL.ReleaseShaderCompiler = reinterpret_cast<void(APIENTRY*)(void)>(loader.load("glReleaseShaderCompiler"));
    flextGL.RenderbufferStorage = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLsizei, GLsizei)>(loader.load("glRenderbufferStorage"));
    flextGL.SampleCoverage = reinterpret_cast<void(APIENTRY*)(GLfloat, GLboolean)>(loader.load("glSampleCoverage"));
    flextGL.ShaderBinary = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLuint *, GLenum, const void *, GLsizei)>(loader.load("glShaderBinary"));
    flextGL.ShaderSource = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, const GLchar *const*, const GLint *)>(loader.load("glShaderSource"));
    flextGL.StencilFuncSeparate = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLint, GLuint)>(loader.load("glStencilFuncSeparate"));
    flextGL.StencilMaskSeparate = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint)>(loader.load("glStencilMaskSeparate"));
    flextGL.StencilOpSeparate = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLenum, GLenum)>(loader.load("glStencilOpSeparate"));
    flextGL.Uniform1f = reinterpret_cast<void(APIENTRY*)(GLint, GLfloat)>(loader.load("glUniform1f"));
    flextGL.Uniform1fv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, const GLfloat *)>(loader.load("glUniform1fv"));
    flextGL.Uniform1i = reinterpret_cast<void(APIENTRY*)(GLint, GLint)>(loader.load("glUniform1i"));
    flextGL.Uniform1iv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, const GLint *)>(loader.load("glUniform1iv"));
    flextGL.Uniform2f = reinterpret_cast<void(APIENTRY*)(GLint, GLfloat, GLfloat)>(loader.load("glUniform2f"));
    flextGL.Uniform2fv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, const GLfloat *)>(loader.load("glUniform2fv"));
    flextGL.Uniform2i = reinterpret_cast<void(APIENTRY*)(GLint, GLint, GLint)>(loader.load("glUniform2i"));
    flextGL.Uniform2iv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, const GLint *)>(loader.load("glUniform2iv"));
    flextGL.Uniform3f = reinterpret_cast<void(APIENTRY*)(GLint, GLfloat, GLfloat, GLfloat)>(loader.load("glUniform3f"));
    flextGL.Uniform3fv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, const GLfloat *)>(loader.load("glUniform3fv"));
    flextGL.Uniform3i = reinterpret_cast<void(APIENTRY*)(GLint, GLint, GLint, GLint)>(loader.load("glUniform3i"));
    flextGL.Uniform3iv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, const GLint *)>(loader.load("glUniform3iv"));
    flextGL.Uniform4f = reinterpret_cast<void(APIENTRY*)(GLint, GLfloat, GLfloat, GLfloat, GLfloat)>(loader.load("glUniform4f"));
    flextGL.Uniform4fv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, const GLfloat *)>(loader.load("glUniform4fv"));
    flextGL.Uniform4i = reinterpret_cast<void(APIENTRY*)(GLint, GLint, GLint, GLint, GLint)>(loader.load("glUniform4i"));
    flextGL.Uniform4iv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, const GLint *)>(loader.load("glUniform4iv"));
    flextGL.UniformMatrix2fv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glUniformMatrix2fv"));
    flextGL.UniformMatrix3fv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glUniformMatrix3fv"));
    flextGL.UniformMatrix4fv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glUniformMatrix4fv"));
    flextGL.UseProgram = reinterpret_cast<void(APIENTRY*)(GLuint)>(loader.load("glUseProgram"));
    flextGL.ValidateProgram = reinterpret_cast<void(APIENTRY*)(GLuint)>(loader.load("glValidateProgram"));
    flextGL.VertexAttrib1f = reinterpret_cast<void(APIENTRY*)(GLuint, GLfloat)>(loader.load("glVertexAttrib1f"));
    flextGL.VertexAttrib1fv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLfloat *)>(loader.load("glVertexAttrib1fv"));
    flextGL.VertexAttrib2f = reinterpret_cast<void(APIENTRY*)(GLuint, GLfloat, GLfloat)>(loader.load("glVertexAttrib2f"));
    flextGL.VertexAttrib2fv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLfloat *)>(loader.load("glVertexAttrib2fv"));
    flextGL.VertexAttrib3f = reinterpret_cast<void(APIENTRY*)(GLuint, GLfloat, GLfloat, GLfloat)>(loader.load("glVertexAttrib3f"));
    flextGL.VertexAttrib3fv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLfloat *)>(loader.load("glVertexAttrib3fv"));
    flextGL.VertexAttrib4f = reinterpret_cast<void(APIENTRY*)(GLuint, GLfloat, GLfloat, GLfloat, GLfloat)>(loader.load("glVertexAttrib4f"));
    flextGL.VertexAttrib4fv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLfloat *)>(loader.load("glVertexAttrib4fv"));
    flextGL.VertexAttribPointer = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLenum, GLboolean, GLsizei, const void *)>(loader.load("glVertexAttribPointer"));

    /* GL_EXT_debug_label */
    flextGL.GetObjectLabelEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLsizei, GLsizei *, GLchar *)>(loader.load("glGetObjectLabelEXT"));
    flextGL.LabelObjectEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLsizei, const GLchar *)>(loader.load("glLabelObjectEXT"));

    /* GL_EXT_debug_marker */
    flextGL.InsertEventMarkerEXT = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLchar *)>(loader.load("glInsertEventMarkerEXT"));
    flextGL.PopGroupMarkerEXT = reinterpret_cast<void(APIENTRY*)(void)>(loader.load("glPopGroupMarkerEXT"));
    flextGL.PushGroupMarkerEXT = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLchar *)>(loader.load("glPushGroupMarkerEXT"));

    /* GL_EXT_discard_framebuffer */
    flextGL.DiscardFramebufferEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, const GLenum *)>(loader.load("glDiscardFramebufferEXT"));

    /* GL_EXT_disjoint_timer_query */
    flextGL.GetInteger64vEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLint64 *)>(loader.load("glGetInteger64vEXT"));
    flextGL.GetQueryObjecti64vEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint64 *)>(loader.load("glGetQueryObjecti64vEXT"));
    flextGL.GetQueryObjectivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint *)>(loader.load("glGetQueryObjectivEXT"));
    flextGL.GetQueryObjectui64vEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint64 *)>(loader.load("glGetQueryObjectui64vEXT"));
    flextGL.QueryCounterEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum)>(loader.load("glQueryCounterEXT"));

    /* GL_EXT_draw_buffers */
    flextGL.DrawBuffersEXT = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLenum *)>(loader.load("glDrawBuffersEXT"));

    /* GL_EXT_draw_buffers_indexed */
    flextGL.BlendEquationSeparateiEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLenum)>(loader.load("glBlendEquationSeparateiEXT"));
    flextGL.BlendEquationiEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum)>(loader.load("glBlendEquationiEXT"));
    flextGL.BlendFuncSeparateiEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLenum, GLenum, GLenum)>(loader.load("glBlendFuncSeparateiEXT"));
    flextGL.BlendFunciEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLenum)>(loader.load("glBlendFunciEXT"));
    flextGL.ColorMaskiEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLboolean, GLboolean, GLboolean, GLboolean)>(loader.load("glColorMaskiEXT"));
    flextGL.DisableiEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint)>(loader.load("glDisableiEXT"));
    flextGL.EnableiEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint)>(loader.load("glEnableiEXT"));
    flextGL.IsEnablediEXT = reinterpret_cast<GLboolean(APIENTRY*)(GLenum, GLuint)>(loader.load("glIsEnablediEXT"));

    /* GL_EXT_draw_elements_base_vertex */
    flextGL.DrawElementsBaseVertexEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, const void *, GLint)>(loader.load("glDrawElementsBaseVertexEXT"));
    flextGL.MultiDrawElementsBaseVertexEXT = reinterpret_cast<void(APIENTRY*)(GLenum, const GLsizei *, GLenum, const void *const*, GLsizei, const GLint *)>(loader.load("glMultiDrawElementsBaseVertexEXT"));

    /* GL_EXT_instanced_arrays */
    flextGL.DrawArraysInstancedEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLsizei, GLsizei)>(loader.load("glDrawArraysInstancedEXT"));
    flextGL.DrawElementsInstancedEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, const void *, GLsizei)>(loader.load("glDrawElementsInstancedEXT"));
    flextGL.VertexAttribDivisorEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint)>(loader.load("glVertexAttribDivisorEXT"));

    /* GL_EXT_map_buffer_range */
    flextGL.FlushMappedBufferRangeEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLintptr, GLsizeiptr)>(loader.load("glFlushMappedBufferRangeEXT"));
    flextGL.MapBufferRangeEXT = reinterpret_cast<void *(APIENTRY*)(GLenum, GLintptr, GLsizeiptr, GLbitfield)>(loader.load("glMapBufferRangeEXT"));

    /* GL_EXT_multi_draw_arrays */
    flextGL.MultiDrawArraysEXT = reinterpret_cast<void(APIENTRY*)(GLenum, const GLint *, const GLsizei *, GLsizei)>(loader.load("glMultiDrawArraysEXT"));
    flextGL.MultiDrawElementsEXT = reinterpret_cast<void(APIENTRY*)(GLenum, const GLsizei *, GLenum, const void *const*, GLsizei)>(loader.load("glMultiDrawElementsEXT"));

    /* GL_EXT_multisampled_render_to_texture */
    flextGL.FramebufferTexture2DMultisampleEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLenum, GLuint, GLint, GLsizei)>(loader.load("glFramebufferTexture2DMultisampleEXT"));
    flextGL.RenderbufferStorageMultisampleEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, GLsizei, GLsizei)>(loader.load("glRenderbufferStorageMultisampleEXT"));

    /* GL_EXT_occlusion_query_boolean */
    flextGL.BeginQueryEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint)>(loader.load("glBeginQueryEXT"));
    flextGL.DeleteQueriesEXT = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLuint *)>(loader.load("glDeleteQueriesEXT"));
    flextGL.EndQueryEXT = reinterpret_cast<void(APIENTRY*)(GLenum)>(loader.load("glEndQueryEXT"));
    flextGL.GenQueriesEXT = reinterpret_cast<void(APIENTRY*)(GLsizei, GLuint *)>(loader.load("glGenQueriesEXT"));
    flextGL.GetQueryObjectuivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint *)>(loader.load("glGetQueryObjectuivEXT"));
    flextGL.GetQueryivEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLint *)>(loader.load("glGetQueryivEXT"));
    flextGL.IsQueryEXT = reinterpret_cast<GLboolean(APIENTRY*)(GLuint)>(loader.load("glIsQueryEXT"));

    /* GL_EXT_polygon_offset_clamp */
    flextGL.PolygonOffsetClampEXT = reinterpret_cast<void(APIENTRY*)(GLfloat, GLfloat, GLfloat)>(loader.load("glPolygonOffsetClampEXT"));

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
    flextGL.ProgramUniform2fEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLfloat, GLfloat)>(loader.load("glProgramUniform2fEXT"));
    flextGL.ProgramUniform2fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLfloat *)>(loader.load("glProgramUniform2fvEXT"));
    flextGL.ProgramUniform2iEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLint)>(loader.load("glProgramUniform2iEXT"));
    flextGL.ProgramUniform2ivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLint *)>(loader.load("glProgramUniform2ivEXT"));
    flextGL.ProgramUniform3fEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLfloat, GLfloat, GLfloat)>(loader.load("glProgramUniform3fEXT"));
    flextGL.ProgramUniform3fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLfloat *)>(loader.load("glProgramUniform3fvEXT"));
    flextGL.ProgramUniform3iEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLint, GLint)>(loader.load("glProgramUniform3iEXT"));
    flextGL.ProgramUniform3ivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLint *)>(loader.load("glProgramUniform3ivEXT"));
    flextGL.ProgramUniform4fEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLfloat, GLfloat, GLfloat, GLfloat)>(loader.load("glProgramUniform4fEXT"));
    flextGL.ProgramUniform4fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLfloat *)>(loader.load("glProgramUniform4fvEXT"));
    flextGL.ProgramUniform4iEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLint, GLint, GLint)>(loader.load("glProgramUniform4iEXT"));
    flextGL.ProgramUniform4ivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLint *)>(loader.load("glProgramUniform4ivEXT"));
    flextGL.ProgramUniformMatrix2fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix2fvEXT"));
    flextGL.ProgramUniformMatrix3fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix3fvEXT"));
    flextGL.ProgramUniformMatrix4fvEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix4fvEXT"));
    flextGL.UseProgramStagesEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLbitfield, GLuint)>(loader.load("glUseProgramStagesEXT"));
    flextGL.ValidateProgramPipelineEXT = reinterpret_cast<void(APIENTRY*)(GLuint)>(loader.load("glValidateProgramPipelineEXT"));

    /* GL_EXT_texture_border_clamp */
    flextGL.GetSamplerParameterIivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint *)>(loader.load("glGetSamplerParameterIivEXT"));
    flextGL.GetSamplerParameterIuivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint *)>(loader.load("glGetSamplerParameterIuivEXT"));
    flextGL.GetTexParameterIivEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLint *)>(loader.load("glGetTexParameterIivEXT"));
    flextGL.GetTexParameterIuivEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLuint *)>(loader.load("glGetTexParameterIuivEXT"));
    flextGL.SamplerParameterIivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, const GLint *)>(loader.load("glSamplerParameterIivEXT"));
    flextGL.SamplerParameterIuivEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, const GLuint *)>(loader.load("glSamplerParameterIuivEXT"));
    flextGL.TexParameterIivEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, const GLint *)>(loader.load("glTexParameterIivEXT"));
    flextGL.TexParameterIuivEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, const GLuint *)>(loader.load("glTexParameterIuivEXT"));

    /* GL_EXT_texture_storage */
    flextGL.TexStorage2DEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, GLsizei, GLsizei)>(loader.load("glTexStorage2DEXT"));
    flextGL.TexStorage3DEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLsizei)>(loader.load("glTexStorage3DEXT"));

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

    /* GL_NV_draw_buffers */
    flextGL.DrawBuffersNV = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLenum *)>(loader.load("glDrawBuffersNV"));

    /* GL_NV_draw_instanced */
    flextGL.DrawArraysInstancedNV = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLsizei, GLsizei)>(loader.load("glDrawArraysInstancedNV"));
    flextGL.DrawElementsInstancedNV = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, const void *, GLsizei)>(loader.load("glDrawElementsInstancedNV"));

    /* GL_NV_framebuffer_blit */
    flextGL.BlitFramebufferNV = reinterpret_cast<void(APIENTRY*)(GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLbitfield, GLenum)>(loader.load("glBlitFramebufferNV"));

    /* GL_NV_framebuffer_multisample */
    flextGL.RenderbufferStorageMultisampleNV = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, GLsizei, GLsizei)>(loader.load("glRenderbufferStorageMultisampleNV"));

    /* GL_NV_instanced_arrays */
    flextGL.VertexAttribDivisorNV = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint)>(loader.load("glVertexAttribDivisorNV"));

    /* GL_NV_polygon_mode */
    flextGL.PolygonModeNV = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum)>(loader.load("glPolygonModeNV"));

    /* GL_NV_read_buffer */
    flextGL.ReadBufferNV = reinterpret_cast<void(APIENTRY*)(GLenum)>(loader.load("glReadBufferNV"));

    /* GL_NV_sample_locations */
    flextGL.FramebufferSampleLocationsfvNV = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLsizei, const GLfloat *)>(loader.load("glFramebufferSampleLocationsfvNV"));
    flextGL.NamedFramebufferSampleLocationsfvNV = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLsizei, const GLfloat *)>(loader.load("glNamedFramebufferSampleLocationsfvNV"));
    flextGL.ResolveDepthValuesNV = reinterpret_cast<void(APIENTRY*)(void)>(loader.load("glResolveDepthValuesNV"));

    /* GL_OES_draw_elements_base_vertex */
    flextGL.DrawElementsBaseVertexOES = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, const void *, GLint)>(loader.load("glDrawElementsBaseVertexOES"));

    /* GL_OES_mapbuffer */
    flextGL.GetBufferPointervOES = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, void **)>(loader.load("glGetBufferPointervOES"));
    flextGL.MapBufferOES = reinterpret_cast<void *(APIENTRY*)(GLenum, GLenum)>(loader.load("glMapBufferOES"));
    flextGL.UnmapBufferOES = reinterpret_cast<GLboolean(APIENTRY*)(GLenum)>(loader.load("glUnmapBufferOES"));

    /* GL_OES_texture_3D */
    flextGL.CompressedTexImage3DOES = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLenum, GLsizei, GLsizei, GLsizei, GLint, GLsizei, const void *)>(loader.load("glCompressedTexImage3DOES"));
    flextGL.CompressedTexSubImage3DOES = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLsizei, const void *)>(loader.load("glCompressedTexSubImage3DOES"));
    flextGL.CopyTexSubImage3DOES = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei)>(loader.load("glCopyTexSubImage3DOES"));
    flextGL.FramebufferTexture3DOES = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLenum, GLuint, GLint, GLint)>(loader.load("glFramebufferTexture3DOES"));
    flextGL.TexImage3DOES = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLenum, GLsizei, GLsizei, GLsizei, GLint, GLenum, GLenum, const void *)>(loader.load("glTexImage3DOES"));
    flextGL.TexSubImage3DOES = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, const void *)>(loader.load("glTexSubImage3DOES"));

    /* GL_OES_vertex_array_object */
    flextGL.BindVertexArrayOES = reinterpret_cast<void(APIENTRY*)(GLuint)>(loader.load("glBindVertexArrayOES"));
    flextGL.DeleteVertexArraysOES = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLuint *)>(loader.load("glDeleteVertexArraysOES"));
    flextGL.GenVertexArraysOES = reinterpret_cast<void(APIENTRY*)(GLsizei, GLuint *)>(loader.load("glGenVertexArraysOES"));
    flextGL.IsVertexArrayOES = reinterpret_cast<GLboolean(APIENTRY*)(GLuint)>(loader.load("glIsVertexArrayOES"));
}
