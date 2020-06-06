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

#include "flextGLWindowsDesktop.h"

#include <cstdio>
#include <cstdlib>

#include "Magnum/Platform/Implementation/OpenGLFunctionLoader.h"

void flextGLInit(Magnum::GL::Context&) {
    Magnum::Platform::Implementation::OpenGLFunctionLoader loader;

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

    /* GL_ES_VERSION_3_0 */
    flextGL.BeginQuery = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint)>(loader.load("glBeginQuery"));
    flextGL.BeginTransformFeedback = reinterpret_cast<void(APIENTRY*)(GLenum)>(loader.load("glBeginTransformFeedback"));
    flextGL.BindBufferBase = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLuint)>(loader.load("glBindBufferBase"));
    flextGL.BindBufferRange = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLuint, GLintptr, GLsizeiptr)>(loader.load("glBindBufferRange"));
    flextGL.BindSampler = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint)>(loader.load("glBindSampler"));
    flextGL.BindTransformFeedback = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint)>(loader.load("glBindTransformFeedback"));
    flextGL.BindVertexArray = reinterpret_cast<void(APIENTRY*)(GLuint)>(loader.load("glBindVertexArray"));
    flextGL.BlitFramebuffer = reinterpret_cast<void(APIENTRY*)(GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLbitfield, GLenum)>(loader.load("glBlitFramebuffer"));
    flextGL.ClearBufferfi = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLfloat, GLint)>(loader.load("glClearBufferfi"));
    flextGL.ClearBufferfv = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, const GLfloat *)>(loader.load("glClearBufferfv"));
    flextGL.ClearBufferiv = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, const GLint *)>(loader.load("glClearBufferiv"));
    flextGL.ClearBufferuiv = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, const GLuint *)>(loader.load("glClearBufferuiv"));
    flextGL.ClientWaitSync = reinterpret_cast<GLenum(APIENTRY*)(GLsync, GLbitfield, GLuint64)>(loader.load("glClientWaitSync"));
    flextGL.CompressedTexImage3D = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLenum, GLsizei, GLsizei, GLsizei, GLint, GLsizei, const void *)>(loader.load("glCompressedTexImage3D"));
    flextGL.CompressedTexSubImage3D = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLsizei, const void *)>(loader.load("glCompressedTexSubImage3D"));
    flextGL.CopyBufferSubData = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLintptr, GLintptr, GLsizeiptr)>(loader.load("glCopyBufferSubData"));
    flextGL.CopyTexSubImage3D = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei)>(loader.load("glCopyTexSubImage3D"));
    flextGL.DeleteQueries = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLuint *)>(loader.load("glDeleteQueries"));
    flextGL.DeleteSamplers = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLuint *)>(loader.load("glDeleteSamplers"));
    flextGL.DeleteSync = reinterpret_cast<void(APIENTRY*)(GLsync)>(loader.load("glDeleteSync"));
    flextGL.DeleteTransformFeedbacks = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLuint *)>(loader.load("glDeleteTransformFeedbacks"));
    flextGL.DeleteVertexArrays = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLuint *)>(loader.load("glDeleteVertexArrays"));
    flextGL.DrawArraysInstanced = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLsizei, GLsizei)>(loader.load("glDrawArraysInstanced"));
    flextGL.DrawBuffers = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLenum *)>(loader.load("glDrawBuffers"));
    flextGL.DrawElementsInstanced = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, const void *, GLsizei)>(loader.load("glDrawElementsInstanced"));
    flextGL.DrawRangeElements = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLuint, GLsizei, GLenum, const void *)>(loader.load("glDrawRangeElements"));
    flextGL.EndQuery = reinterpret_cast<void(APIENTRY*)(GLenum)>(loader.load("glEndQuery"));
    flextGL.EndTransformFeedback = reinterpret_cast<void(APIENTRY*)(void)>(loader.load("glEndTransformFeedback"));
    flextGL.FenceSync = reinterpret_cast<GLsync(APIENTRY*)(GLenum, GLbitfield)>(loader.load("glFenceSync"));
    flextGL.FlushMappedBufferRange = reinterpret_cast<void(APIENTRY*)(GLenum, GLintptr, GLsizeiptr)>(loader.load("glFlushMappedBufferRange"));
    flextGL.FramebufferTextureLayer = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLuint, GLint, GLint)>(loader.load("glFramebufferTextureLayer"));
    flextGL.GenQueries = reinterpret_cast<void(APIENTRY*)(GLsizei, GLuint *)>(loader.load("glGenQueries"));
    flextGL.GenSamplers = reinterpret_cast<void(APIENTRY*)(GLsizei, GLuint *)>(loader.load("glGenSamplers"));
    flextGL.GenTransformFeedbacks = reinterpret_cast<void(APIENTRY*)(GLsizei, GLuint *)>(loader.load("glGenTransformFeedbacks"));
    flextGL.GenVertexArrays = reinterpret_cast<void(APIENTRY*)(GLsizei, GLuint *)>(loader.load("glGenVertexArrays"));
    flextGL.GetActiveUniformBlockName = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLsizei, GLsizei *, GLchar *)>(loader.load("glGetActiveUniformBlockName"));
    flextGL.GetActiveUniformBlockiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLenum, GLint *)>(loader.load("glGetActiveUniformBlockiv"));
    flextGL.GetActiveUniformsiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, const GLuint *, GLenum, GLint *)>(loader.load("glGetActiveUniformsiv"));
    flextGL.GetBufferParameteri64v = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLint64 *)>(loader.load("glGetBufferParameteri64v"));
    flextGL.GetBufferPointerv = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, void **)>(loader.load("glGetBufferPointerv"));
    flextGL.GetFragDataLocation = reinterpret_cast<GLint(APIENTRY*)(GLuint, const GLchar *)>(loader.load("glGetFragDataLocation"));
    flextGL.GetInteger64i_v = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLint64 *)>(loader.load("glGetInteger64i_v"));
    flextGL.GetInteger64v = reinterpret_cast<void(APIENTRY*)(GLenum, GLint64 *)>(loader.load("glGetInteger64v"));
    flextGL.GetIntegeri_v = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLint *)>(loader.load("glGetIntegeri_v"));
    flextGL.GetInternalformativ = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLenum, GLsizei, GLint *)>(loader.load("glGetInternalformativ"));
    flextGL.GetProgramBinary = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, GLsizei *, GLenum *, void *)>(loader.load("glGetProgramBinary"));
    flextGL.GetQueryObjectuiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint *)>(loader.load("glGetQueryObjectuiv"));
    flextGL.GetQueryiv = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLint *)>(loader.load("glGetQueryiv"));
    flextGL.GetSamplerParameterfv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLfloat *)>(loader.load("glGetSamplerParameterfv"));
    flextGL.GetSamplerParameteriv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint *)>(loader.load("glGetSamplerParameteriv"));
    flextGL.GetStringi = reinterpret_cast<const GLubyte *(APIENTRY*)(GLenum, GLuint)>(loader.load("glGetStringi"));
    flextGL.GetSynciv = reinterpret_cast<void(APIENTRY*)(GLsync, GLenum, GLsizei, GLsizei *, GLint *)>(loader.load("glGetSynciv"));
    flextGL.GetTransformFeedbackVarying = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLsizei, GLsizei *, GLsizei *, GLenum *, GLchar *)>(loader.load("glGetTransformFeedbackVarying"));
    flextGL.GetUniformBlockIndex = reinterpret_cast<GLuint(APIENTRY*)(GLuint, const GLchar *)>(loader.load("glGetUniformBlockIndex"));
    flextGL.GetUniformIndices = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, const GLchar *const*, GLuint *)>(loader.load("glGetUniformIndices"));
    flextGL.GetUniformuiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLuint *)>(loader.load("glGetUniformuiv"));
    flextGL.GetVertexAttribIiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint *)>(loader.load("glGetVertexAttribIiv"));
    flextGL.GetVertexAttribIuiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint *)>(loader.load("glGetVertexAttribIuiv"));
    flextGL.InvalidateFramebuffer = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, const GLenum *)>(loader.load("glInvalidateFramebuffer"));
    flextGL.InvalidateSubFramebuffer = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, const GLenum *, GLint, GLint, GLsizei, GLsizei)>(loader.load("glInvalidateSubFramebuffer"));
    flextGL.IsQuery = reinterpret_cast<GLboolean(APIENTRY*)(GLuint)>(loader.load("glIsQuery"));
    flextGL.IsSampler = reinterpret_cast<GLboolean(APIENTRY*)(GLuint)>(loader.load("glIsSampler"));
    flextGL.IsSync = reinterpret_cast<GLboolean(APIENTRY*)(GLsync)>(loader.load("glIsSync"));
    flextGL.IsTransformFeedback = reinterpret_cast<GLboolean(APIENTRY*)(GLuint)>(loader.load("glIsTransformFeedback"));
    flextGL.IsVertexArray = reinterpret_cast<GLboolean(APIENTRY*)(GLuint)>(loader.load("glIsVertexArray"));
    flextGL.MapBufferRange = reinterpret_cast<void *(APIENTRY*)(GLenum, GLintptr, GLsizeiptr, GLbitfield)>(loader.load("glMapBufferRange"));
    flextGL.PauseTransformFeedback = reinterpret_cast<void(APIENTRY*)(void)>(loader.load("glPauseTransformFeedback"));
    flextGL.ProgramBinary = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, const void *, GLsizei)>(loader.load("glProgramBinary"));
    flextGL.ProgramParameteri = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint)>(loader.load("glProgramParameteri"));
    flextGL.RenderbufferStorageMultisample = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, GLsizei, GLsizei)>(loader.load("glRenderbufferStorageMultisample"));
    flextGL.ResumeTransformFeedback = reinterpret_cast<void(APIENTRY*)(void)>(loader.load("glResumeTransformFeedback"));
    flextGL.SamplerParameterf = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLfloat)>(loader.load("glSamplerParameterf"));
    flextGL.SamplerParameterfv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, const GLfloat *)>(loader.load("glSamplerParameterfv"));
    flextGL.SamplerParameteri = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint)>(loader.load("glSamplerParameteri"));
    flextGL.SamplerParameteriv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, const GLint *)>(loader.load("glSamplerParameteriv"));
    flextGL.TexImage3D = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLint, GLsizei, GLsizei, GLsizei, GLint, GLenum, GLenum, const void *)>(loader.load("glTexImage3D"));
    flextGL.TexStorage2D = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, GLsizei, GLsizei)>(loader.load("glTexStorage2D"));
    flextGL.TexStorage3D = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLsizei)>(loader.load("glTexStorage3D"));
    flextGL.TexSubImage3D = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, const void *)>(loader.load("glTexSubImage3D"));
    flextGL.TransformFeedbackVaryings = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, const GLchar *const*, GLenum)>(loader.load("glTransformFeedbackVaryings"));
    flextGL.Uniform1ui = reinterpret_cast<void(APIENTRY*)(GLint, GLuint)>(loader.load("glUniform1ui"));
    flextGL.Uniform1uiv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, const GLuint *)>(loader.load("glUniform1uiv"));
    flextGL.Uniform2ui = reinterpret_cast<void(APIENTRY*)(GLint, GLuint, GLuint)>(loader.load("glUniform2ui"));
    flextGL.Uniform2uiv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, const GLuint *)>(loader.load("glUniform2uiv"));
    flextGL.Uniform3ui = reinterpret_cast<void(APIENTRY*)(GLint, GLuint, GLuint, GLuint)>(loader.load("glUniform3ui"));
    flextGL.Uniform3uiv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, const GLuint *)>(loader.load("glUniform3uiv"));
    flextGL.Uniform4ui = reinterpret_cast<void(APIENTRY*)(GLint, GLuint, GLuint, GLuint, GLuint)>(loader.load("glUniform4ui"));
    flextGL.Uniform4uiv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, const GLuint *)>(loader.load("glUniform4uiv"));
    flextGL.UniformBlockBinding = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLuint)>(loader.load("glUniformBlockBinding"));
    flextGL.UniformMatrix2x3fv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glUniformMatrix2x3fv"));
    flextGL.UniformMatrix2x4fv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glUniformMatrix2x4fv"));
    flextGL.UniformMatrix3x2fv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glUniformMatrix3x2fv"));
    flextGL.UniformMatrix3x4fv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glUniformMatrix3x4fv"));
    flextGL.UniformMatrix4x2fv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glUniformMatrix4x2fv"));
    flextGL.UniformMatrix4x3fv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glUniformMatrix4x3fv"));
    flextGL.UnmapBuffer = reinterpret_cast<GLboolean(APIENTRY*)(GLenum)>(loader.load("glUnmapBuffer"));
    flextGL.VertexAttribDivisor = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint)>(loader.load("glVertexAttribDivisor"));
    flextGL.VertexAttribI4i = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLint, GLint)>(loader.load("glVertexAttribI4i"));
    flextGL.VertexAttribI4iv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLint *)>(loader.load("glVertexAttribI4iv"));
    flextGL.VertexAttribI4ui = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLuint, GLuint, GLuint)>(loader.load("glVertexAttribI4ui"));
    flextGL.VertexAttribI4uiv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLuint *)>(loader.load("glVertexAttribI4uiv"));
    flextGL.VertexAttribIPointer = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLenum, GLsizei, const void *)>(loader.load("glVertexAttribIPointer"));
    flextGL.WaitSync = reinterpret_cast<void(APIENTRY*)(GLsync, GLbitfield, GLuint64)>(loader.load("glWaitSync"));

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
