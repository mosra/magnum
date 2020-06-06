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

#ifdef MAGNUM_PLATFORM_USE_EGL
#include <cstring>
#include <EGL/egl.h>
#include "Magnum/GL/Context.h"
#endif

void flextGLInit(Magnum::GL::Context& context) {
    Magnum::Platform::Implementation::OpenGLFunctionLoader loader;

    #ifdef MAGNUM_PLATFORM_USE_EGL
    {
        /* EGL contexts on NVidia 390 drivers don't have correct statically
           linked GL 1.0 and 1.1 functions (such as glGetString()) and one has
           to retrieve them explicitly using eglGetProcAddress(). */
        EGLDisplay display = eglGetCurrentDisplay();
        const char* vendor = eglQueryString(display, EGL_VENDOR);
        if(std::strcmp(vendor, "NVIDIA") == 0 && !context.isDriverWorkaroundDisabled("nv-egl-incorrect-gl11-function-pointers")) {

            /* GL_VERSION_1_0 */
            flextGL.BlendFunc = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum)>(loader.load("glBlendFunc"));
            flextGL.Clear = reinterpret_cast<void(APIENTRY*)(GLbitfield)>(loader.load("glClear"));
            flextGL.ClearColor = reinterpret_cast<void(APIENTRY*)(GLfloat, GLfloat, GLfloat, GLfloat)>(loader.load("glClearColor"));
            flextGL.ClearDepth = reinterpret_cast<void(APIENTRY*)(GLdouble)>(loader.load("glClearDepth"));
            flextGL.ClearStencil = reinterpret_cast<void(APIENTRY*)(GLint)>(loader.load("glClearStencil"));
            flextGL.ColorMask = reinterpret_cast<void(APIENTRY*)(GLboolean, GLboolean, GLboolean, GLboolean)>(loader.load("glColorMask"));
            flextGL.CullFace = reinterpret_cast<void(APIENTRY*)(GLenum)>(loader.load("glCullFace"));
            flextGL.DepthFunc = reinterpret_cast<void(APIENTRY*)(GLenum)>(loader.load("glDepthFunc"));
            flextGL.DepthMask = reinterpret_cast<void(APIENTRY*)(GLboolean)>(loader.load("glDepthMask"));
            flextGL.DepthRange = reinterpret_cast<void(APIENTRY*)(GLdouble, GLdouble)>(loader.load("glDepthRange"));
            flextGL.Disable = reinterpret_cast<void(APIENTRY*)(GLenum)>(loader.load("glDisable"));
            flextGL.DrawBuffer = reinterpret_cast<void(APIENTRY*)(GLenum)>(loader.load("glDrawBuffer"));
            flextGL.Enable = reinterpret_cast<void(APIENTRY*)(GLenum)>(loader.load("glEnable"));
            flextGL.Finish = reinterpret_cast<void(APIENTRY*)(void)>(loader.load("glFinish"));
            flextGL.Flush = reinterpret_cast<void(APIENTRY*)(void)>(loader.load("glFlush"));
            flextGL.FrontFace = reinterpret_cast<void(APIENTRY*)(GLenum)>(loader.load("glFrontFace"));
            flextGL.GetBooleanv = reinterpret_cast<void(APIENTRY*)(GLenum, GLboolean *)>(loader.load("glGetBooleanv"));
            flextGL.GetDoublev = reinterpret_cast<void(APIENTRY*)(GLenum, GLdouble *)>(loader.load("glGetDoublev"));
            flextGL.GetError = reinterpret_cast<GLenum(APIENTRY*)(void)>(loader.load("glGetError"));
            flextGL.GetFloatv = reinterpret_cast<void(APIENTRY*)(GLenum, GLfloat *)>(loader.load("glGetFloatv"));
            flextGL.GetIntegerv = reinterpret_cast<void(APIENTRY*)(GLenum, GLint *)>(loader.load("glGetIntegerv"));
            flextGL.GetString = reinterpret_cast<const GLubyte *(APIENTRY*)(GLenum)>(loader.load("glGetString"));
            flextGL.GetTexImage = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLenum, GLenum, void *)>(loader.load("glGetTexImage"));
            flextGL.GetTexLevelParameterfv = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLenum, GLfloat *)>(loader.load("glGetTexLevelParameterfv"));
            flextGL.GetTexLevelParameteriv = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLenum, GLint *)>(loader.load("glGetTexLevelParameteriv"));
            flextGL.GetTexParameterfv = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLfloat *)>(loader.load("glGetTexParameterfv"));
            flextGL.GetTexParameteriv = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLint *)>(loader.load("glGetTexParameteriv"));
            flextGL.Hint = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum)>(loader.load("glHint"));
            flextGL.IsEnabled = reinterpret_cast<GLboolean(APIENTRY*)(GLenum)>(loader.load("glIsEnabled"));
            flextGL.LineWidth = reinterpret_cast<void(APIENTRY*)(GLfloat)>(loader.load("glLineWidth"));
            flextGL.LogicOp = reinterpret_cast<void(APIENTRY*)(GLenum)>(loader.load("glLogicOp"));
            flextGL.PixelStoref = reinterpret_cast<void(APIENTRY*)(GLenum, GLfloat)>(loader.load("glPixelStoref"));
            flextGL.PixelStorei = reinterpret_cast<void(APIENTRY*)(GLenum, GLint)>(loader.load("glPixelStorei"));
            flextGL.PointSize = reinterpret_cast<void(APIENTRY*)(GLfloat)>(loader.load("glPointSize"));
            flextGL.PolygonMode = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum)>(loader.load("glPolygonMode"));
            flextGL.ReadBuffer = reinterpret_cast<void(APIENTRY*)(GLenum)>(loader.load("glReadBuffer"));
            flextGL.ReadPixels = reinterpret_cast<void(APIENTRY*)(GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, void *)>(loader.load("glReadPixels"));
            flextGL.Scissor = reinterpret_cast<void(APIENTRY*)(GLint, GLint, GLsizei, GLsizei)>(loader.load("glScissor"));
            flextGL.StencilFunc = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLuint)>(loader.load("glStencilFunc"));
            flextGL.StencilMask = reinterpret_cast<void(APIENTRY*)(GLuint)>(loader.load("glStencilMask"));
            flextGL.StencilOp = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLenum)>(loader.load("glStencilOp"));
            flextGL.TexImage1D = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLint, GLsizei, GLint, GLenum, GLenum, const void *)>(loader.load("glTexImage1D"));
            flextGL.TexImage2D = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const void *)>(loader.load("glTexImage2D"));
            flextGL.TexParameterf = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLfloat)>(loader.load("glTexParameterf"));
            flextGL.TexParameterfv = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, const GLfloat *)>(loader.load("glTexParameterfv"));
            flextGL.TexParameteri = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLint)>(loader.load("glTexParameteri"));
            flextGL.TexParameteriv = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, const GLint *)>(loader.load("glTexParameteriv"));
            flextGL.Viewport = reinterpret_cast<void(APIENTRY*)(GLint, GLint, GLsizei, GLsizei)>(loader.load("glViewport"));

            /* GL_VERSION_1_1 */
            flextGL.BindTexture = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint)>(loader.load("glBindTexture"));
            flextGL.CopyTexImage1D = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLenum, GLint, GLint, GLsizei, GLint)>(loader.load("glCopyTexImage1D"));
            flextGL.CopyTexImage2D = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLenum, GLint, GLint, GLsizei, GLsizei, GLint)>(loader.load("glCopyTexImage2D"));
            flextGL.CopyTexSubImage1D = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLint, GLint, GLint, GLsizei)>(loader.load("glCopyTexSubImage1D"));
            flextGL.CopyTexSubImage2D = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei)>(loader.load("glCopyTexSubImage2D"));
            flextGL.DeleteTextures = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLuint *)>(loader.load("glDeleteTextures"));
            flextGL.DrawArrays = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLsizei)>(loader.load("glDrawArrays"));
            flextGL.DrawElements = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, const void *)>(loader.load("glDrawElements"));
            flextGL.GenTextures = reinterpret_cast<void(APIENTRY*)(GLsizei, GLuint *)>(loader.load("glGenTextures"));
            flextGL.IsTexture = reinterpret_cast<GLboolean(APIENTRY*)(GLuint)>(loader.load("glIsTexture"));
            flextGL.PolygonOffset = reinterpret_cast<void(APIENTRY*)(GLfloat, GLfloat)>(loader.load("glPolygonOffset"));
            flextGL.TexSubImage1D = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLint, GLsizei, GLenum, GLenum, const void *)>(loader.load("glTexSubImage1D"));
            flextGL.TexSubImage2D = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const void *)>(loader.load("glTexSubImage2D"));
        }
    }
    #else
    static_cast<void>(context);
    #endif

    /* GL_AMD_sample_positions */
    flextGL.SetMultisamplefvAMD = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, const GLfloat *)>(loader.load("glSetMultisamplefvAMD"));

    /* GL_ARB_ES3_2_compatibility */
    flextGL.PrimitiveBoundingBoxARB = reinterpret_cast<void(APIENTRY*)(GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat)>(loader.load("glPrimitiveBoundingBoxARB"));

    /* GL_ARB_bindless_texture */
    flextGL.GetImageHandleARB = reinterpret_cast<GLuint64(APIENTRY*)(GLuint, GLint, GLboolean, GLint, GLenum)>(loader.load("glGetImageHandleARB"));
    flextGL.GetTextureHandleARB = reinterpret_cast<GLuint64(APIENTRY*)(GLuint)>(loader.load("glGetTextureHandleARB"));
    flextGL.GetTextureSamplerHandleARB = reinterpret_cast<GLuint64(APIENTRY*)(GLuint, GLuint)>(loader.load("glGetTextureSamplerHandleARB"));
    flextGL.GetVertexAttribLui64vARB = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint64EXT *)>(loader.load("glGetVertexAttribLui64vARB"));
    flextGL.IsImageHandleResidentARB = reinterpret_cast<GLboolean(APIENTRY*)(GLuint64)>(loader.load("glIsImageHandleResidentARB"));
    flextGL.IsTextureHandleResidentARB = reinterpret_cast<GLboolean(APIENTRY*)(GLuint64)>(loader.load("glIsTextureHandleResidentARB"));
    flextGL.MakeImageHandleNonResidentARB = reinterpret_cast<void(APIENTRY*)(GLuint64)>(loader.load("glMakeImageHandleNonResidentARB"));
    flextGL.MakeImageHandleResidentARB = reinterpret_cast<void(APIENTRY*)(GLuint64, GLenum)>(loader.load("glMakeImageHandleResidentARB"));
    flextGL.MakeTextureHandleNonResidentARB = reinterpret_cast<void(APIENTRY*)(GLuint64)>(loader.load("glMakeTextureHandleNonResidentARB"));
    flextGL.MakeTextureHandleResidentARB = reinterpret_cast<void(APIENTRY*)(GLuint64)>(loader.load("glMakeTextureHandleResidentARB"));
    flextGL.ProgramUniformHandleui64ARB = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLuint64)>(loader.load("glProgramUniformHandleui64ARB"));
    flextGL.ProgramUniformHandleui64vARB = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLuint64 *)>(loader.load("glProgramUniformHandleui64vARB"));
    flextGL.UniformHandleui64ARB = reinterpret_cast<void(APIENTRY*)(GLint, GLuint64)>(loader.load("glUniformHandleui64ARB"));
    flextGL.UniformHandleui64vARB = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, const GLuint64 *)>(loader.load("glUniformHandleui64vARB"));
    flextGL.VertexAttribL1ui64ARB = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint64EXT)>(loader.load("glVertexAttribL1ui64ARB"));
    flextGL.VertexAttribL1ui64vARB = reinterpret_cast<void(APIENTRY*)(GLuint, const GLuint64EXT *)>(loader.load("glVertexAttribL1ui64vARB"));

    /* GL_ARB_compute_variable_group_size */
    flextGL.DispatchComputeGroupSizeARB = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLuint, GLuint, GLuint, GLuint)>(loader.load("glDispatchComputeGroupSizeARB"));

    /* GL_ARB_robustness */
    flextGL.GetGraphicsResetStatusARB = reinterpret_cast<GLenum(APIENTRY*)(void)>(loader.load("glGetGraphicsResetStatusARB"));
    flextGL.GetnCompressedTexImageARB = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLsizei, void *)>(loader.load("glGetnCompressedTexImageARB"));
    flextGL.GetnTexImageARB = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLenum, GLenum, GLsizei, void *)>(loader.load("glGetnTexImageARB"));
    flextGL.GetnUniformdvARB = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLdouble *)>(loader.load("glGetnUniformdvARB"));
    flextGL.GetnUniformfvARB = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLfloat *)>(loader.load("glGetnUniformfvARB"));
    flextGL.GetnUniformivARB = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLint *)>(loader.load("glGetnUniformivARB"));
    flextGL.GetnUniformuivARB = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLuint *)>(loader.load("glGetnUniformuivARB"));
    flextGL.ReadnPixelsARB = reinterpret_cast<void(APIENTRY*)(GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, GLsizei, void *)>(loader.load("glReadnPixelsARB"));

    /* GL_ARB_sample_locations */
    flextGL.EvaluateDepthValuesARB = reinterpret_cast<void(APIENTRY*)(void)>(loader.load("glEvaluateDepthValuesARB"));
    flextGL.FramebufferSampleLocationsfvARB = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLsizei, const GLfloat *)>(loader.load("glFramebufferSampleLocationsfvARB"));
    flextGL.NamedFramebufferSampleLocationsfvARB = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLsizei, const GLfloat *)>(loader.load("glNamedFramebufferSampleLocationsfvARB"));

    /* GL_ARB_sparse_buffer */
    flextGL.BufferPageCommitmentARB = reinterpret_cast<void(APIENTRY*)(GLenum, GLintptr, GLsizeiptr, GLboolean)>(loader.load("glBufferPageCommitmentARB"));
    flextGL.NamedBufferPageCommitmentARB = reinterpret_cast<void(APIENTRY*)(GLuint, GLintptr, GLsizeiptr, GLboolean)>(loader.load("glNamedBufferPageCommitmentARB"));
    flextGL.NamedBufferPageCommitmentEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLintptr, GLsizeiptr, GLboolean)>(loader.load("glNamedBufferPageCommitmentEXT"));

    /* GL_ARB_sparse_texture */
    flextGL.TexPageCommitmentARB = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLboolean)>(loader.load("glTexPageCommitmentARB"));

    /* GL_EXT_debug_label */
    flextGL.GetObjectLabelEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLsizei, GLsizei *, GLchar *)>(loader.load("glGetObjectLabelEXT"));
    flextGL.LabelObjectEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLsizei, const GLchar *)>(loader.load("glLabelObjectEXT"));

    /* GL_EXT_debug_marker */
    flextGL.InsertEventMarkerEXT = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLchar *)>(loader.load("glInsertEventMarkerEXT"));
    flextGL.PopGroupMarkerEXT = reinterpret_cast<void(APIENTRY*)(void)>(loader.load("glPopGroupMarkerEXT"));
    flextGL.PushGroupMarkerEXT = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLchar *)>(loader.load("glPushGroupMarkerEXT"));

    /* GL_GREMEDY_string_marker */
    flextGL.StringMarkerGREMEDY = reinterpret_cast<void(APIENTRY*)(GLsizei, const void *)>(loader.load("glStringMarkerGREMEDY"));

    /* GL_KHR_blend_equation_advanced */
    flextGL.BlendBarrierKHR = reinterpret_cast<void(APIENTRY*)(void)>(loader.load("glBlendBarrierKHR"));

    /* GL_NV_sample_locations */
    flextGL.FramebufferSampleLocationsfvNV = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLsizei, const GLfloat *)>(loader.load("glFramebufferSampleLocationsfvNV"));
    flextGL.NamedFramebufferSampleLocationsfvNV = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLsizei, const GLfloat *)>(loader.load("glNamedFramebufferSampleLocationsfvNV"));
    flextGL.ResolveDepthValuesNV = reinterpret_cast<void(APIENTRY*)(void)>(loader.load("glResolveDepthValuesNV"));

    /* GL_OVR_multiview */
    flextGL.FramebufferTextureMultiviewOVR = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLuint, GLint, GLint, GLsizei)>(loader.load("glFramebufferTextureMultiviewOVR"));

    /* GL_VERSION_1_2 */
    flextGL.CopyTexSubImage3D = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei)>(loader.load("glCopyTexSubImage3D"));
    flextGL.DrawRangeElements = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLuint, GLsizei, GLenum, const void *)>(loader.load("glDrawRangeElements"));
    flextGL.TexImage3D = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLint, GLsizei, GLsizei, GLsizei, GLint, GLenum, GLenum, const void *)>(loader.load("glTexImage3D"));
    flextGL.TexSubImage3D = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, const void *)>(loader.load("glTexSubImage3D"));

    /* GL_VERSION_1_3 */
    flextGL.ActiveTexture = reinterpret_cast<void(APIENTRY*)(GLenum)>(loader.load("glActiveTexture"));
    flextGL.CompressedTexImage1D = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLenum, GLsizei, GLint, GLsizei, const void *)>(loader.load("glCompressedTexImage1D"));
    flextGL.CompressedTexImage2D = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLenum, GLsizei, GLsizei, GLint, GLsizei, const void *)>(loader.load("glCompressedTexImage2D"));
    flextGL.CompressedTexImage3D = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLenum, GLsizei, GLsizei, GLsizei, GLint, GLsizei, const void *)>(loader.load("glCompressedTexImage3D"));
    flextGL.CompressedTexSubImage1D = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLint, GLsizei, GLenum, GLsizei, const void *)>(loader.load("glCompressedTexSubImage1D"));
    flextGL.CompressedTexSubImage2D = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLsizei, const void *)>(loader.load("glCompressedTexSubImage2D"));
    flextGL.CompressedTexSubImage3D = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLsizei, const void *)>(loader.load("glCompressedTexSubImage3D"));
    flextGL.GetCompressedTexImage = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, void *)>(loader.load("glGetCompressedTexImage"));
    flextGL.SampleCoverage = reinterpret_cast<void(APIENTRY*)(GLfloat, GLboolean)>(loader.load("glSampleCoverage"));

    /* GL_VERSION_1_4 */
    flextGL.BlendColor = reinterpret_cast<void(APIENTRY*)(GLfloat, GLfloat, GLfloat, GLfloat)>(loader.load("glBlendColor"));
    flextGL.BlendEquation = reinterpret_cast<void(APIENTRY*)(GLenum)>(loader.load("glBlendEquation"));
    flextGL.BlendFuncSeparate = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLenum, GLenum)>(loader.load("glBlendFuncSeparate"));
    flextGL.MultiDrawArrays = reinterpret_cast<void(APIENTRY*)(GLenum, const GLint *, const GLsizei *, GLsizei)>(loader.load("glMultiDrawArrays"));
    flextGL.MultiDrawElements = reinterpret_cast<void(APIENTRY*)(GLenum, const GLsizei *, GLenum, const void *const*, GLsizei)>(loader.load("glMultiDrawElements"));
    flextGL.PointParameterf = reinterpret_cast<void(APIENTRY*)(GLenum, GLfloat)>(loader.load("glPointParameterf"));
    flextGL.PointParameterfv = reinterpret_cast<void(APIENTRY*)(GLenum, const GLfloat *)>(loader.load("glPointParameterfv"));
    flextGL.PointParameteri = reinterpret_cast<void(APIENTRY*)(GLenum, GLint)>(loader.load("glPointParameteri"));
    flextGL.PointParameteriv = reinterpret_cast<void(APIENTRY*)(GLenum, const GLint *)>(loader.load("glPointParameteriv"));

    /* GL_VERSION_1_5 */
    flextGL.BeginQuery = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint)>(loader.load("glBeginQuery"));
    flextGL.BindBuffer = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint)>(loader.load("glBindBuffer"));
    flextGL.BufferData = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizeiptr, const void *, GLenum)>(loader.load("glBufferData"));
    flextGL.BufferSubData = reinterpret_cast<void(APIENTRY*)(GLenum, GLintptr, GLsizeiptr, const void *)>(loader.load("glBufferSubData"));
    flextGL.DeleteBuffers = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLuint *)>(loader.load("glDeleteBuffers"));
    flextGL.DeleteQueries = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLuint *)>(loader.load("glDeleteQueries"));
    flextGL.EndQuery = reinterpret_cast<void(APIENTRY*)(GLenum)>(loader.load("glEndQuery"));
    flextGL.GenBuffers = reinterpret_cast<void(APIENTRY*)(GLsizei, GLuint *)>(loader.load("glGenBuffers"));
    flextGL.GenQueries = reinterpret_cast<void(APIENTRY*)(GLsizei, GLuint *)>(loader.load("glGenQueries"));
    flextGL.GetBufferParameteriv = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLint *)>(loader.load("glGetBufferParameteriv"));
    flextGL.GetBufferPointerv = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, void **)>(loader.load("glGetBufferPointerv"));
    flextGL.GetBufferSubData = reinterpret_cast<void(APIENTRY*)(GLenum, GLintptr, GLsizeiptr, void *)>(loader.load("glGetBufferSubData"));
    flextGL.GetQueryObjectiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint *)>(loader.load("glGetQueryObjectiv"));
    flextGL.GetQueryObjectuiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint *)>(loader.load("glGetQueryObjectuiv"));
    flextGL.GetQueryiv = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLint *)>(loader.load("glGetQueryiv"));
    flextGL.IsBuffer = reinterpret_cast<GLboolean(APIENTRY*)(GLuint)>(loader.load("glIsBuffer"));
    flextGL.IsQuery = reinterpret_cast<GLboolean(APIENTRY*)(GLuint)>(loader.load("glIsQuery"));
    flextGL.MapBuffer = reinterpret_cast<void *(APIENTRY*)(GLenum, GLenum)>(loader.load("glMapBuffer"));
    flextGL.UnmapBuffer = reinterpret_cast<GLboolean(APIENTRY*)(GLenum)>(loader.load("glUnmapBuffer"));

    /* GL_VERSION_2_0 */
    flextGL.AttachShader = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint)>(loader.load("glAttachShader"));
    flextGL.BindAttribLocation = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, const GLchar *)>(loader.load("glBindAttribLocation"));
    flextGL.BlendEquationSeparate = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum)>(loader.load("glBlendEquationSeparate"));
    flextGL.CompileShader = reinterpret_cast<void(APIENTRY*)(GLuint)>(loader.load("glCompileShader"));
    flextGL.CreateProgram = reinterpret_cast<GLuint(APIENTRY*)(void)>(loader.load("glCreateProgram"));
    flextGL.CreateShader = reinterpret_cast<GLuint(APIENTRY*)(GLenum)>(loader.load("glCreateShader"));
    flextGL.DeleteProgram = reinterpret_cast<void(APIENTRY*)(GLuint)>(loader.load("glDeleteProgram"));
    flextGL.DeleteShader = reinterpret_cast<void(APIENTRY*)(GLuint)>(loader.load("glDeleteShader"));
    flextGL.DetachShader = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint)>(loader.load("glDetachShader"));
    flextGL.DisableVertexAttribArray = reinterpret_cast<void(APIENTRY*)(GLuint)>(loader.load("glDisableVertexAttribArray"));
    flextGL.DrawBuffers = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLenum *)>(loader.load("glDrawBuffers"));
    flextGL.EnableVertexAttribArray = reinterpret_cast<void(APIENTRY*)(GLuint)>(loader.load("glEnableVertexAttribArray"));
    flextGL.GetActiveAttrib = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLsizei, GLsizei *, GLint *, GLenum *, GLchar *)>(loader.load("glGetActiveAttrib"));
    flextGL.GetActiveUniform = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLsizei, GLsizei *, GLint *, GLenum *, GLchar *)>(loader.load("glGetActiveUniform"));
    flextGL.GetAttachedShaders = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, GLsizei *, GLuint *)>(loader.load("glGetAttachedShaders"));
    flextGL.GetAttribLocation = reinterpret_cast<GLint(APIENTRY*)(GLuint, const GLchar *)>(loader.load("glGetAttribLocation"));
    flextGL.GetProgramInfoLog = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, GLsizei *, GLchar *)>(loader.load("glGetProgramInfoLog"));
    flextGL.GetProgramiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint *)>(loader.load("glGetProgramiv"));
    flextGL.GetShaderInfoLog = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, GLsizei *, GLchar *)>(loader.load("glGetShaderInfoLog"));
    flextGL.GetShaderSource = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, GLsizei *, GLchar *)>(loader.load("glGetShaderSource"));
    flextGL.GetShaderiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint *)>(loader.load("glGetShaderiv"));
    flextGL.GetUniformLocation = reinterpret_cast<GLint(APIENTRY*)(GLuint, const GLchar *)>(loader.load("glGetUniformLocation"));
    flextGL.GetUniformfv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLfloat *)>(loader.load("glGetUniformfv"));
    flextGL.GetUniformiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint *)>(loader.load("glGetUniformiv"));
    flextGL.GetVertexAttribPointerv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, void **)>(loader.load("glGetVertexAttribPointerv"));
    flextGL.GetVertexAttribdv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLdouble *)>(loader.load("glGetVertexAttribdv"));
    flextGL.GetVertexAttribfv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLfloat *)>(loader.load("glGetVertexAttribfv"));
    flextGL.GetVertexAttribiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint *)>(loader.load("glGetVertexAttribiv"));
    flextGL.IsProgram = reinterpret_cast<GLboolean(APIENTRY*)(GLuint)>(loader.load("glIsProgram"));
    flextGL.IsShader = reinterpret_cast<GLboolean(APIENTRY*)(GLuint)>(loader.load("glIsShader"));
    flextGL.LinkProgram = reinterpret_cast<void(APIENTRY*)(GLuint)>(loader.load("glLinkProgram"));
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
    flextGL.VertexAttrib1d = reinterpret_cast<void(APIENTRY*)(GLuint, GLdouble)>(loader.load("glVertexAttrib1d"));
    flextGL.VertexAttrib1dv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLdouble *)>(loader.load("glVertexAttrib1dv"));
    flextGL.VertexAttrib1f = reinterpret_cast<void(APIENTRY*)(GLuint, GLfloat)>(loader.load("glVertexAttrib1f"));
    flextGL.VertexAttrib1fv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLfloat *)>(loader.load("glVertexAttrib1fv"));
    flextGL.VertexAttrib1s = reinterpret_cast<void(APIENTRY*)(GLuint, GLshort)>(loader.load("glVertexAttrib1s"));
    flextGL.VertexAttrib1sv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLshort *)>(loader.load("glVertexAttrib1sv"));
    flextGL.VertexAttrib2d = reinterpret_cast<void(APIENTRY*)(GLuint, GLdouble, GLdouble)>(loader.load("glVertexAttrib2d"));
    flextGL.VertexAttrib2dv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLdouble *)>(loader.load("glVertexAttrib2dv"));
    flextGL.VertexAttrib2f = reinterpret_cast<void(APIENTRY*)(GLuint, GLfloat, GLfloat)>(loader.load("glVertexAttrib2f"));
    flextGL.VertexAttrib2fv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLfloat *)>(loader.load("glVertexAttrib2fv"));
    flextGL.VertexAttrib2s = reinterpret_cast<void(APIENTRY*)(GLuint, GLshort, GLshort)>(loader.load("glVertexAttrib2s"));
    flextGL.VertexAttrib2sv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLshort *)>(loader.load("glVertexAttrib2sv"));
    flextGL.VertexAttrib3d = reinterpret_cast<void(APIENTRY*)(GLuint, GLdouble, GLdouble, GLdouble)>(loader.load("glVertexAttrib3d"));
    flextGL.VertexAttrib3dv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLdouble *)>(loader.load("glVertexAttrib3dv"));
    flextGL.VertexAttrib3f = reinterpret_cast<void(APIENTRY*)(GLuint, GLfloat, GLfloat, GLfloat)>(loader.load("glVertexAttrib3f"));
    flextGL.VertexAttrib3fv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLfloat *)>(loader.load("glVertexAttrib3fv"));
    flextGL.VertexAttrib3s = reinterpret_cast<void(APIENTRY*)(GLuint, GLshort, GLshort, GLshort)>(loader.load("glVertexAttrib3s"));
    flextGL.VertexAttrib3sv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLshort *)>(loader.load("glVertexAttrib3sv"));
    flextGL.VertexAttrib4Nbv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLbyte *)>(loader.load("glVertexAttrib4Nbv"));
    flextGL.VertexAttrib4Niv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLint *)>(loader.load("glVertexAttrib4Niv"));
    flextGL.VertexAttrib4Nsv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLshort *)>(loader.load("glVertexAttrib4Nsv"));
    flextGL.VertexAttrib4Nub = reinterpret_cast<void(APIENTRY*)(GLuint, GLubyte, GLubyte, GLubyte, GLubyte)>(loader.load("glVertexAttrib4Nub"));
    flextGL.VertexAttrib4Nubv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLubyte *)>(loader.load("glVertexAttrib4Nubv"));
    flextGL.VertexAttrib4Nuiv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLuint *)>(loader.load("glVertexAttrib4Nuiv"));
    flextGL.VertexAttrib4Nusv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLushort *)>(loader.load("glVertexAttrib4Nusv"));
    flextGL.VertexAttrib4bv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLbyte *)>(loader.load("glVertexAttrib4bv"));
    flextGL.VertexAttrib4d = reinterpret_cast<void(APIENTRY*)(GLuint, GLdouble, GLdouble, GLdouble, GLdouble)>(loader.load("glVertexAttrib4d"));
    flextGL.VertexAttrib4dv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLdouble *)>(loader.load("glVertexAttrib4dv"));
    flextGL.VertexAttrib4f = reinterpret_cast<void(APIENTRY*)(GLuint, GLfloat, GLfloat, GLfloat, GLfloat)>(loader.load("glVertexAttrib4f"));
    flextGL.VertexAttrib4fv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLfloat *)>(loader.load("glVertexAttrib4fv"));
    flextGL.VertexAttrib4iv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLint *)>(loader.load("glVertexAttrib4iv"));
    flextGL.VertexAttrib4s = reinterpret_cast<void(APIENTRY*)(GLuint, GLshort, GLshort, GLshort, GLshort)>(loader.load("glVertexAttrib4s"));
    flextGL.VertexAttrib4sv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLshort *)>(loader.load("glVertexAttrib4sv"));
    flextGL.VertexAttrib4ubv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLubyte *)>(loader.load("glVertexAttrib4ubv"));
    flextGL.VertexAttrib4uiv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLuint *)>(loader.load("glVertexAttrib4uiv"));
    flextGL.VertexAttrib4usv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLushort *)>(loader.load("glVertexAttrib4usv"));
    flextGL.VertexAttribPointer = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLenum, GLboolean, GLsizei, const void *)>(loader.load("glVertexAttribPointer"));

    /* GL_VERSION_2_1 */
    flextGL.UniformMatrix2x3fv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glUniformMatrix2x3fv"));
    flextGL.UniformMatrix2x4fv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glUniformMatrix2x4fv"));
    flextGL.UniformMatrix3x2fv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glUniformMatrix3x2fv"));
    flextGL.UniformMatrix3x4fv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glUniformMatrix3x4fv"));
    flextGL.UniformMatrix4x2fv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glUniformMatrix4x2fv"));
    flextGL.UniformMatrix4x3fv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glUniformMatrix4x3fv"));

    /* GL_VERSION_3_0 */
    flextGL.BeginConditionalRender = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum)>(loader.load("glBeginConditionalRender"));
    flextGL.BeginTransformFeedback = reinterpret_cast<void(APIENTRY*)(GLenum)>(loader.load("glBeginTransformFeedback"));
    flextGL.BindBufferBase = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLuint)>(loader.load("glBindBufferBase"));
    flextGL.BindBufferRange = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLuint, GLintptr, GLsizeiptr)>(loader.load("glBindBufferRange"));
    flextGL.BindFragDataLocation = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, const GLchar *)>(loader.load("glBindFragDataLocation"));
    flextGL.BindFramebuffer = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint)>(loader.load("glBindFramebuffer"));
    flextGL.BindRenderbuffer = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint)>(loader.load("glBindRenderbuffer"));
    flextGL.BindVertexArray = reinterpret_cast<void(APIENTRY*)(GLuint)>(loader.load("glBindVertexArray"));
    flextGL.BlitFramebuffer = reinterpret_cast<void(APIENTRY*)(GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLbitfield, GLenum)>(loader.load("glBlitFramebuffer"));
    flextGL.CheckFramebufferStatus = reinterpret_cast<GLenum(APIENTRY*)(GLenum)>(loader.load("glCheckFramebufferStatus"));
    flextGL.ClampColor = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum)>(loader.load("glClampColor"));
    flextGL.ClearBufferfi = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLfloat, GLint)>(loader.load("glClearBufferfi"));
    flextGL.ClearBufferfv = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, const GLfloat *)>(loader.load("glClearBufferfv"));
    flextGL.ClearBufferiv = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, const GLint *)>(loader.load("glClearBufferiv"));
    flextGL.ClearBufferuiv = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, const GLuint *)>(loader.load("glClearBufferuiv"));
    flextGL.ColorMaski = reinterpret_cast<void(APIENTRY*)(GLuint, GLboolean, GLboolean, GLboolean, GLboolean)>(loader.load("glColorMaski"));
    flextGL.DeleteFramebuffers = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLuint *)>(loader.load("glDeleteFramebuffers"));
    flextGL.DeleteRenderbuffers = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLuint *)>(loader.load("glDeleteRenderbuffers"));
    flextGL.DeleteVertexArrays = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLuint *)>(loader.load("glDeleteVertexArrays"));
    flextGL.Disablei = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint)>(loader.load("glDisablei"));
    flextGL.Enablei = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint)>(loader.load("glEnablei"));
    flextGL.EndConditionalRender = reinterpret_cast<void(APIENTRY*)(void)>(loader.load("glEndConditionalRender"));
    flextGL.EndTransformFeedback = reinterpret_cast<void(APIENTRY*)(void)>(loader.load("glEndTransformFeedback"));
    flextGL.FlushMappedBufferRange = reinterpret_cast<void(APIENTRY*)(GLenum, GLintptr, GLsizeiptr)>(loader.load("glFlushMappedBufferRange"));
    flextGL.FramebufferRenderbuffer = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLenum, GLuint)>(loader.load("glFramebufferRenderbuffer"));
    flextGL.FramebufferTexture1D = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLenum, GLuint, GLint)>(loader.load("glFramebufferTexture1D"));
    flextGL.FramebufferTexture2D = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLenum, GLuint, GLint)>(loader.load("glFramebufferTexture2D"));
    flextGL.FramebufferTexture3D = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLenum, GLuint, GLint, GLint)>(loader.load("glFramebufferTexture3D"));
    flextGL.FramebufferTextureLayer = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLuint, GLint, GLint)>(loader.load("glFramebufferTextureLayer"));
    flextGL.GenFramebuffers = reinterpret_cast<void(APIENTRY*)(GLsizei, GLuint *)>(loader.load("glGenFramebuffers"));
    flextGL.GenRenderbuffers = reinterpret_cast<void(APIENTRY*)(GLsizei, GLuint *)>(loader.load("glGenRenderbuffers"));
    flextGL.GenVertexArrays = reinterpret_cast<void(APIENTRY*)(GLsizei, GLuint *)>(loader.load("glGenVertexArrays"));
    flextGL.GenerateMipmap = reinterpret_cast<void(APIENTRY*)(GLenum)>(loader.load("glGenerateMipmap"));
    flextGL.GetBooleani_v = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLboolean *)>(loader.load("glGetBooleani_v"));
    flextGL.GetFragDataLocation = reinterpret_cast<GLint(APIENTRY*)(GLuint, const GLchar *)>(loader.load("glGetFragDataLocation"));
    flextGL.GetFramebufferAttachmentParameteriv = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLenum, GLint *)>(loader.load("glGetFramebufferAttachmentParameteriv"));
    flextGL.GetIntegeri_v = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLint *)>(loader.load("glGetIntegeri_v"));
    flextGL.GetRenderbufferParameteriv = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLint *)>(loader.load("glGetRenderbufferParameteriv"));
    flextGL.GetStringi = reinterpret_cast<const GLubyte *(APIENTRY*)(GLenum, GLuint)>(loader.load("glGetStringi"));
    flextGL.GetTexParameterIiv = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLint *)>(loader.load("glGetTexParameterIiv"));
    flextGL.GetTexParameterIuiv = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLuint *)>(loader.load("glGetTexParameterIuiv"));
    flextGL.GetTransformFeedbackVarying = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLsizei, GLsizei *, GLsizei *, GLenum *, GLchar *)>(loader.load("glGetTransformFeedbackVarying"));
    flextGL.GetUniformuiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLuint *)>(loader.load("glGetUniformuiv"));
    flextGL.GetVertexAttribIiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint *)>(loader.load("glGetVertexAttribIiv"));
    flextGL.GetVertexAttribIuiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint *)>(loader.load("glGetVertexAttribIuiv"));
    flextGL.IsEnabledi = reinterpret_cast<GLboolean(APIENTRY*)(GLenum, GLuint)>(loader.load("glIsEnabledi"));
    flextGL.IsFramebuffer = reinterpret_cast<GLboolean(APIENTRY*)(GLuint)>(loader.load("glIsFramebuffer"));
    flextGL.IsRenderbuffer = reinterpret_cast<GLboolean(APIENTRY*)(GLuint)>(loader.load("glIsRenderbuffer"));
    flextGL.IsVertexArray = reinterpret_cast<GLboolean(APIENTRY*)(GLuint)>(loader.load("glIsVertexArray"));
    flextGL.MapBufferRange = reinterpret_cast<void *(APIENTRY*)(GLenum, GLintptr, GLsizeiptr, GLbitfield)>(loader.load("glMapBufferRange"));
    flextGL.RenderbufferStorage = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLsizei, GLsizei)>(loader.load("glRenderbufferStorage"));
    flextGL.RenderbufferStorageMultisample = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, GLsizei, GLsizei)>(loader.load("glRenderbufferStorageMultisample"));
    flextGL.TexParameterIiv = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, const GLint *)>(loader.load("glTexParameterIiv"));
    flextGL.TexParameterIuiv = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, const GLuint *)>(loader.load("glTexParameterIuiv"));
    flextGL.TransformFeedbackVaryings = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, const GLchar *const*, GLenum)>(loader.load("glTransformFeedbackVaryings"));
    flextGL.Uniform1ui = reinterpret_cast<void(APIENTRY*)(GLint, GLuint)>(loader.load("glUniform1ui"));
    flextGL.Uniform1uiv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, const GLuint *)>(loader.load("glUniform1uiv"));
    flextGL.Uniform2ui = reinterpret_cast<void(APIENTRY*)(GLint, GLuint, GLuint)>(loader.load("glUniform2ui"));
    flextGL.Uniform2uiv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, const GLuint *)>(loader.load("glUniform2uiv"));
    flextGL.Uniform3ui = reinterpret_cast<void(APIENTRY*)(GLint, GLuint, GLuint, GLuint)>(loader.load("glUniform3ui"));
    flextGL.Uniform3uiv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, const GLuint *)>(loader.load("glUniform3uiv"));
    flextGL.Uniform4ui = reinterpret_cast<void(APIENTRY*)(GLint, GLuint, GLuint, GLuint, GLuint)>(loader.load("glUniform4ui"));
    flextGL.Uniform4uiv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, const GLuint *)>(loader.load("glUniform4uiv"));
    flextGL.VertexAttribI1i = reinterpret_cast<void(APIENTRY*)(GLuint, GLint)>(loader.load("glVertexAttribI1i"));
    flextGL.VertexAttribI1iv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLint *)>(loader.load("glVertexAttribI1iv"));
    flextGL.VertexAttribI1ui = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint)>(loader.load("glVertexAttribI1ui"));
    flextGL.VertexAttribI1uiv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLuint *)>(loader.load("glVertexAttribI1uiv"));
    flextGL.VertexAttribI2i = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint)>(loader.load("glVertexAttribI2i"));
    flextGL.VertexAttribI2iv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLint *)>(loader.load("glVertexAttribI2iv"));
    flextGL.VertexAttribI2ui = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLuint)>(loader.load("glVertexAttribI2ui"));
    flextGL.VertexAttribI2uiv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLuint *)>(loader.load("glVertexAttribI2uiv"));
    flextGL.VertexAttribI3i = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLint)>(loader.load("glVertexAttribI3i"));
    flextGL.VertexAttribI3iv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLint *)>(loader.load("glVertexAttribI3iv"));
    flextGL.VertexAttribI3ui = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLuint, GLuint)>(loader.load("glVertexAttribI3ui"));
    flextGL.VertexAttribI3uiv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLuint *)>(loader.load("glVertexAttribI3uiv"));
    flextGL.VertexAttribI4bv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLbyte *)>(loader.load("glVertexAttribI4bv"));
    flextGL.VertexAttribI4i = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLint, GLint)>(loader.load("glVertexAttribI4i"));
    flextGL.VertexAttribI4iv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLint *)>(loader.load("glVertexAttribI4iv"));
    flextGL.VertexAttribI4sv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLshort *)>(loader.load("glVertexAttribI4sv"));
    flextGL.VertexAttribI4ubv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLubyte *)>(loader.load("glVertexAttribI4ubv"));
    flextGL.VertexAttribI4ui = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLuint, GLuint, GLuint)>(loader.load("glVertexAttribI4ui"));
    flextGL.VertexAttribI4uiv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLuint *)>(loader.load("glVertexAttribI4uiv"));
    flextGL.VertexAttribI4usv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLushort *)>(loader.load("glVertexAttribI4usv"));
    flextGL.VertexAttribIPointer = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLenum, GLsizei, const void *)>(loader.load("glVertexAttribIPointer"));

    /* GL_VERSION_3_1 */
    flextGL.CopyBufferSubData = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLintptr, GLintptr, GLsizeiptr)>(loader.load("glCopyBufferSubData"));
    flextGL.DrawArraysInstanced = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLsizei, GLsizei)>(loader.load("glDrawArraysInstanced"));
    flextGL.DrawElementsInstanced = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, const void *, GLsizei)>(loader.load("glDrawElementsInstanced"));
    flextGL.GetActiveUniformBlockName = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLsizei, GLsizei *, GLchar *)>(loader.load("glGetActiveUniformBlockName"));
    flextGL.GetActiveUniformBlockiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLenum, GLint *)>(loader.load("glGetActiveUniformBlockiv"));
    flextGL.GetActiveUniformName = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLsizei, GLsizei *, GLchar *)>(loader.load("glGetActiveUniformName"));
    flextGL.GetActiveUniformsiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, const GLuint *, GLenum, GLint *)>(loader.load("glGetActiveUniformsiv"));
    flextGL.GetUniformBlockIndex = reinterpret_cast<GLuint(APIENTRY*)(GLuint, const GLchar *)>(loader.load("glGetUniformBlockIndex"));
    flextGL.GetUniformIndices = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, const GLchar *const*, GLuint *)>(loader.load("glGetUniformIndices"));
    flextGL.PrimitiveRestartIndex = reinterpret_cast<void(APIENTRY*)(GLuint)>(loader.load("glPrimitiveRestartIndex"));
    flextGL.TexBuffer = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLuint)>(loader.load("glTexBuffer"));
    flextGL.UniformBlockBinding = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLuint)>(loader.load("glUniformBlockBinding"));

    /* GL_VERSION_3_2 */
    flextGL.ClientWaitSync = reinterpret_cast<GLenum(APIENTRY*)(GLsync, GLbitfield, GLuint64)>(loader.load("glClientWaitSync"));
    flextGL.DeleteSync = reinterpret_cast<void(APIENTRY*)(GLsync)>(loader.load("glDeleteSync"));
    flextGL.DrawElementsBaseVertex = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, const void *, GLint)>(loader.load("glDrawElementsBaseVertex"));
    flextGL.DrawElementsInstancedBaseVertex = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, const void *, GLsizei, GLint)>(loader.load("glDrawElementsInstancedBaseVertex"));
    flextGL.DrawRangeElementsBaseVertex = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLuint, GLsizei, GLenum, const void *, GLint)>(loader.load("glDrawRangeElementsBaseVertex"));
    flextGL.FenceSync = reinterpret_cast<GLsync(APIENTRY*)(GLenum, GLbitfield)>(loader.load("glFenceSync"));
    flextGL.FramebufferTexture = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLuint, GLint)>(loader.load("glFramebufferTexture"));
    flextGL.GetBufferParameteri64v = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLint64 *)>(loader.load("glGetBufferParameteri64v"));
    flextGL.GetInteger64i_v = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLint64 *)>(loader.load("glGetInteger64i_v"));
    flextGL.GetInteger64v = reinterpret_cast<void(APIENTRY*)(GLenum, GLint64 *)>(loader.load("glGetInteger64v"));
    flextGL.GetMultisamplefv = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLfloat *)>(loader.load("glGetMultisamplefv"));
    flextGL.GetSynciv = reinterpret_cast<void(APIENTRY*)(GLsync, GLenum, GLsizei, GLsizei *, GLint *)>(loader.load("glGetSynciv"));
    flextGL.IsSync = reinterpret_cast<GLboolean(APIENTRY*)(GLsync)>(loader.load("glIsSync"));
    flextGL.MultiDrawElementsBaseVertex = reinterpret_cast<void(APIENTRY*)(GLenum, const GLsizei *, GLenum, const void *const*, GLsizei, const GLint *)>(loader.load("glMultiDrawElementsBaseVertex"));
    flextGL.ProvokingVertex = reinterpret_cast<void(APIENTRY*)(GLenum)>(loader.load("glProvokingVertex"));
    flextGL.SampleMaski = reinterpret_cast<void(APIENTRY*)(GLuint, GLbitfield)>(loader.load("glSampleMaski"));
    flextGL.TexImage2DMultisample = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLboolean)>(loader.load("glTexImage2DMultisample"));
    flextGL.TexImage3DMultisample = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLsizei, GLboolean)>(loader.load("glTexImage3DMultisample"));
    flextGL.WaitSync = reinterpret_cast<void(APIENTRY*)(GLsync, GLbitfield, GLuint64)>(loader.load("glWaitSync"));

    /* GL_VERSION_3_3 */
    flextGL.BindFragDataLocationIndexed = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLuint, const GLchar *)>(loader.load("glBindFragDataLocationIndexed"));
    flextGL.BindSampler = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint)>(loader.load("glBindSampler"));
    flextGL.DeleteSamplers = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLuint *)>(loader.load("glDeleteSamplers"));
    flextGL.GenSamplers = reinterpret_cast<void(APIENTRY*)(GLsizei, GLuint *)>(loader.load("glGenSamplers"));
    flextGL.GetFragDataIndex = reinterpret_cast<GLint(APIENTRY*)(GLuint, const GLchar *)>(loader.load("glGetFragDataIndex"));
    flextGL.GetQueryObjecti64v = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint64 *)>(loader.load("glGetQueryObjecti64v"));
    flextGL.GetQueryObjectui64v = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint64 *)>(loader.load("glGetQueryObjectui64v"));
    flextGL.GetSamplerParameterIiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint *)>(loader.load("glGetSamplerParameterIiv"));
    flextGL.GetSamplerParameterIuiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint *)>(loader.load("glGetSamplerParameterIuiv"));
    flextGL.GetSamplerParameterfv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLfloat *)>(loader.load("glGetSamplerParameterfv"));
    flextGL.GetSamplerParameteriv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint *)>(loader.load("glGetSamplerParameteriv"));
    flextGL.IsSampler = reinterpret_cast<GLboolean(APIENTRY*)(GLuint)>(loader.load("glIsSampler"));
    flextGL.QueryCounter = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum)>(loader.load("glQueryCounter"));
    flextGL.SamplerParameterIiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, const GLint *)>(loader.load("glSamplerParameterIiv"));
    flextGL.SamplerParameterIuiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, const GLuint *)>(loader.load("glSamplerParameterIuiv"));
    flextGL.SamplerParameterf = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLfloat)>(loader.load("glSamplerParameterf"));
    flextGL.SamplerParameterfv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, const GLfloat *)>(loader.load("glSamplerParameterfv"));
    flextGL.SamplerParameteri = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint)>(loader.load("glSamplerParameteri"));
    flextGL.SamplerParameteriv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, const GLint *)>(loader.load("glSamplerParameteriv"));
    flextGL.VertexAttribDivisor = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint)>(loader.load("glVertexAttribDivisor"));
    flextGL.VertexAttribP1ui = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLboolean, GLuint)>(loader.load("glVertexAttribP1ui"));
    flextGL.VertexAttribP1uiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLboolean, const GLuint *)>(loader.load("glVertexAttribP1uiv"));
    flextGL.VertexAttribP2ui = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLboolean, GLuint)>(loader.load("glVertexAttribP2ui"));
    flextGL.VertexAttribP2uiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLboolean, const GLuint *)>(loader.load("glVertexAttribP2uiv"));
    flextGL.VertexAttribP3ui = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLboolean, GLuint)>(loader.load("glVertexAttribP3ui"));
    flextGL.VertexAttribP3uiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLboolean, const GLuint *)>(loader.load("glVertexAttribP3uiv"));
    flextGL.VertexAttribP4ui = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLboolean, GLuint)>(loader.load("glVertexAttribP4ui"));
    flextGL.VertexAttribP4uiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLboolean, const GLuint *)>(loader.load("glVertexAttribP4uiv"));

    /* GL_VERSION_4_0 */
    flextGL.BeginQueryIndexed = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLuint)>(loader.load("glBeginQueryIndexed"));
    flextGL.BindTransformFeedback = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint)>(loader.load("glBindTransformFeedback"));
    flextGL.BlendEquationSeparatei = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLenum)>(loader.load("glBlendEquationSeparatei"));
    flextGL.BlendEquationi = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum)>(loader.load("glBlendEquationi"));
    flextGL.BlendFuncSeparatei = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLenum, GLenum, GLenum)>(loader.load("glBlendFuncSeparatei"));
    flextGL.BlendFunci = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLenum)>(loader.load("glBlendFunci"));
    flextGL.DeleteTransformFeedbacks = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLuint *)>(loader.load("glDeleteTransformFeedbacks"));
    flextGL.DrawArraysIndirect = reinterpret_cast<void(APIENTRY*)(GLenum, const void *)>(loader.load("glDrawArraysIndirect"));
    flextGL.DrawElementsIndirect = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, const void *)>(loader.load("glDrawElementsIndirect"));
    flextGL.DrawTransformFeedback = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint)>(loader.load("glDrawTransformFeedback"));
    flextGL.DrawTransformFeedbackStream = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLuint)>(loader.load("glDrawTransformFeedbackStream"));
    flextGL.EndQueryIndexed = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint)>(loader.load("glEndQueryIndexed"));
    flextGL.GenTransformFeedbacks = reinterpret_cast<void(APIENTRY*)(GLsizei, GLuint *)>(loader.load("glGenTransformFeedbacks"));
    flextGL.GetActiveSubroutineName = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint, GLsizei, GLsizei *, GLchar *)>(loader.load("glGetActiveSubroutineName"));
    flextGL.GetActiveSubroutineUniformName = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint, GLsizei, GLsizei *, GLchar *)>(loader.load("glGetActiveSubroutineUniformName"));
    flextGL.GetActiveSubroutineUniformiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint, GLenum, GLint *)>(loader.load("glGetActiveSubroutineUniformiv"));
    flextGL.GetProgramStageiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLenum, GLint *)>(loader.load("glGetProgramStageiv"));
    flextGL.GetQueryIndexediv = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLenum, GLint *)>(loader.load("glGetQueryIndexediv"));
    flextGL.GetSubroutineIndex = reinterpret_cast<GLuint(APIENTRY*)(GLuint, GLenum, const GLchar *)>(loader.load("glGetSubroutineIndex"));
    flextGL.GetSubroutineUniformLocation = reinterpret_cast<GLint(APIENTRY*)(GLuint, GLenum, const GLchar *)>(loader.load("glGetSubroutineUniformLocation"));
    flextGL.GetUniformSubroutineuiv = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLuint *)>(loader.load("glGetUniformSubroutineuiv"));
    flextGL.GetUniformdv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLdouble *)>(loader.load("glGetUniformdv"));
    flextGL.IsTransformFeedback = reinterpret_cast<GLboolean(APIENTRY*)(GLuint)>(loader.load("glIsTransformFeedback"));
    flextGL.MinSampleShading = reinterpret_cast<void(APIENTRY*)(GLfloat)>(loader.load("glMinSampleShading"));
    flextGL.PatchParameterfv = reinterpret_cast<void(APIENTRY*)(GLenum, const GLfloat *)>(loader.load("glPatchParameterfv"));
    flextGL.PatchParameteri = reinterpret_cast<void(APIENTRY*)(GLenum, GLint)>(loader.load("glPatchParameteri"));
    flextGL.PauseTransformFeedback = reinterpret_cast<void(APIENTRY*)(void)>(loader.load("glPauseTransformFeedback"));
    flextGL.ResumeTransformFeedback = reinterpret_cast<void(APIENTRY*)(void)>(loader.load("glResumeTransformFeedback"));
    flextGL.Uniform1d = reinterpret_cast<void(APIENTRY*)(GLint, GLdouble)>(loader.load("glUniform1d"));
    flextGL.Uniform1dv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, const GLdouble *)>(loader.load("glUniform1dv"));
    flextGL.Uniform2d = reinterpret_cast<void(APIENTRY*)(GLint, GLdouble, GLdouble)>(loader.load("glUniform2d"));
    flextGL.Uniform2dv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, const GLdouble *)>(loader.load("glUniform2dv"));
    flextGL.Uniform3d = reinterpret_cast<void(APIENTRY*)(GLint, GLdouble, GLdouble, GLdouble)>(loader.load("glUniform3d"));
    flextGL.Uniform3dv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, const GLdouble *)>(loader.load("glUniform3dv"));
    flextGL.Uniform4d = reinterpret_cast<void(APIENTRY*)(GLint, GLdouble, GLdouble, GLdouble, GLdouble)>(loader.load("glUniform4d"));
    flextGL.Uniform4dv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, const GLdouble *)>(loader.load("glUniform4dv"));
    flextGL.UniformMatrix2dv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, GLboolean, const GLdouble *)>(loader.load("glUniformMatrix2dv"));
    flextGL.UniformMatrix2x3dv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, GLboolean, const GLdouble *)>(loader.load("glUniformMatrix2x3dv"));
    flextGL.UniformMatrix2x4dv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, GLboolean, const GLdouble *)>(loader.load("glUniformMatrix2x4dv"));
    flextGL.UniformMatrix3dv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, GLboolean, const GLdouble *)>(loader.load("glUniformMatrix3dv"));
    flextGL.UniformMatrix3x2dv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, GLboolean, const GLdouble *)>(loader.load("glUniformMatrix3x2dv"));
    flextGL.UniformMatrix3x4dv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, GLboolean, const GLdouble *)>(loader.load("glUniformMatrix3x4dv"));
    flextGL.UniformMatrix4dv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, GLboolean, const GLdouble *)>(loader.load("glUniformMatrix4dv"));
    flextGL.UniformMatrix4x2dv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, GLboolean, const GLdouble *)>(loader.load("glUniformMatrix4x2dv"));
    flextGL.UniformMatrix4x3dv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, GLboolean, const GLdouble *)>(loader.load("glUniformMatrix4x3dv"));
    flextGL.UniformSubroutinesuiv = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, const GLuint *)>(loader.load("glUniformSubroutinesuiv"));

    /* GL_VERSION_4_1 */
    flextGL.ActiveShaderProgram = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint)>(loader.load("glActiveShaderProgram"));
    flextGL.BindProgramPipeline = reinterpret_cast<void(APIENTRY*)(GLuint)>(loader.load("glBindProgramPipeline"));
    flextGL.ClearDepthf = reinterpret_cast<void(APIENTRY*)(GLfloat)>(loader.load("glClearDepthf"));
    flextGL.CreateShaderProgramv = reinterpret_cast<GLuint(APIENTRY*)(GLenum, GLsizei, const GLchar *const*)>(loader.load("glCreateShaderProgramv"));
    flextGL.DeleteProgramPipelines = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLuint *)>(loader.load("glDeleteProgramPipelines"));
    flextGL.DepthRangeArrayv = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, const GLdouble *)>(loader.load("glDepthRangeArrayv"));
    flextGL.DepthRangeIndexed = reinterpret_cast<void(APIENTRY*)(GLuint, GLdouble, GLdouble)>(loader.load("glDepthRangeIndexed"));
    flextGL.DepthRangef = reinterpret_cast<void(APIENTRY*)(GLfloat, GLfloat)>(loader.load("glDepthRangef"));
    flextGL.GenProgramPipelines = reinterpret_cast<void(APIENTRY*)(GLsizei, GLuint *)>(loader.load("glGenProgramPipelines"));
    flextGL.GetDoublei_v = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLdouble *)>(loader.load("glGetDoublei_v"));
    flextGL.GetFloati_v = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLfloat *)>(loader.load("glGetFloati_v"));
    flextGL.GetProgramBinary = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, GLsizei *, GLenum *, void *)>(loader.load("glGetProgramBinary"));
    flextGL.GetProgramPipelineInfoLog = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, GLsizei *, GLchar *)>(loader.load("glGetProgramPipelineInfoLog"));
    flextGL.GetProgramPipelineiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint *)>(loader.load("glGetProgramPipelineiv"));
    flextGL.GetShaderPrecisionFormat = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLint *, GLint *)>(loader.load("glGetShaderPrecisionFormat"));
    flextGL.GetVertexAttribLdv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLdouble *)>(loader.load("glGetVertexAttribLdv"));
    flextGL.IsProgramPipeline = reinterpret_cast<GLboolean(APIENTRY*)(GLuint)>(loader.load("glIsProgramPipeline"));
    flextGL.ProgramBinary = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, const void *, GLsizei)>(loader.load("glProgramBinary"));
    flextGL.ProgramParameteri = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint)>(loader.load("glProgramParameteri"));
    flextGL.ProgramUniform1d = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLdouble)>(loader.load("glProgramUniform1d"));
    flextGL.ProgramUniform1dv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLdouble *)>(loader.load("glProgramUniform1dv"));
    flextGL.ProgramUniform1f = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLfloat)>(loader.load("glProgramUniform1f"));
    flextGL.ProgramUniform1fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLfloat *)>(loader.load("glProgramUniform1fv"));
    flextGL.ProgramUniform1i = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint)>(loader.load("glProgramUniform1i"));
    flextGL.ProgramUniform1iv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLint *)>(loader.load("glProgramUniform1iv"));
    flextGL.ProgramUniform1ui = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLuint)>(loader.load("glProgramUniform1ui"));
    flextGL.ProgramUniform1uiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLuint *)>(loader.load("glProgramUniform1uiv"));
    flextGL.ProgramUniform2d = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLdouble, GLdouble)>(loader.load("glProgramUniform2d"));
    flextGL.ProgramUniform2dv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLdouble *)>(loader.load("glProgramUniform2dv"));
    flextGL.ProgramUniform2f = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLfloat, GLfloat)>(loader.load("glProgramUniform2f"));
    flextGL.ProgramUniform2fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLfloat *)>(loader.load("glProgramUniform2fv"));
    flextGL.ProgramUniform2i = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLint)>(loader.load("glProgramUniform2i"));
    flextGL.ProgramUniform2iv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLint *)>(loader.load("glProgramUniform2iv"));
    flextGL.ProgramUniform2ui = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLuint, GLuint)>(loader.load("glProgramUniform2ui"));
    flextGL.ProgramUniform2uiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLuint *)>(loader.load("glProgramUniform2uiv"));
    flextGL.ProgramUniform3d = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLdouble, GLdouble, GLdouble)>(loader.load("glProgramUniform3d"));
    flextGL.ProgramUniform3dv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLdouble *)>(loader.load("glProgramUniform3dv"));
    flextGL.ProgramUniform3f = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLfloat, GLfloat, GLfloat)>(loader.load("glProgramUniform3f"));
    flextGL.ProgramUniform3fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLfloat *)>(loader.load("glProgramUniform3fv"));
    flextGL.ProgramUniform3i = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLint, GLint)>(loader.load("glProgramUniform3i"));
    flextGL.ProgramUniform3iv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLint *)>(loader.load("glProgramUniform3iv"));
    flextGL.ProgramUniform3ui = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLuint, GLuint, GLuint)>(loader.load("glProgramUniform3ui"));
    flextGL.ProgramUniform3uiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLuint *)>(loader.load("glProgramUniform3uiv"));
    flextGL.ProgramUniform4d = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLdouble, GLdouble, GLdouble, GLdouble)>(loader.load("glProgramUniform4d"));
    flextGL.ProgramUniform4dv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLdouble *)>(loader.load("glProgramUniform4dv"));
    flextGL.ProgramUniform4f = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLfloat, GLfloat, GLfloat, GLfloat)>(loader.load("glProgramUniform4f"));
    flextGL.ProgramUniform4fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLfloat *)>(loader.load("glProgramUniform4fv"));
    flextGL.ProgramUniform4i = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLint, GLint, GLint)>(loader.load("glProgramUniform4i"));
    flextGL.ProgramUniform4iv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLint *)>(loader.load("glProgramUniform4iv"));
    flextGL.ProgramUniform4ui = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLuint, GLuint, GLuint, GLuint)>(loader.load("glProgramUniform4ui"));
    flextGL.ProgramUniform4uiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLuint *)>(loader.load("glProgramUniform4uiv"));
    flextGL.ProgramUniformMatrix2dv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLdouble *)>(loader.load("glProgramUniformMatrix2dv"));
    flextGL.ProgramUniformMatrix2fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix2fv"));
    flextGL.ProgramUniformMatrix2x3dv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLdouble *)>(loader.load("glProgramUniformMatrix2x3dv"));
    flextGL.ProgramUniformMatrix2x3fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix2x3fv"));
    flextGL.ProgramUniformMatrix2x4dv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLdouble *)>(loader.load("glProgramUniformMatrix2x4dv"));
    flextGL.ProgramUniformMatrix2x4fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix2x4fv"));
    flextGL.ProgramUniformMatrix3dv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLdouble *)>(loader.load("glProgramUniformMatrix3dv"));
    flextGL.ProgramUniformMatrix3fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix3fv"));
    flextGL.ProgramUniformMatrix3x2dv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLdouble *)>(loader.load("glProgramUniformMatrix3x2dv"));
    flextGL.ProgramUniformMatrix3x2fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix3x2fv"));
    flextGL.ProgramUniformMatrix3x4dv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLdouble *)>(loader.load("glProgramUniformMatrix3x4dv"));
    flextGL.ProgramUniformMatrix3x4fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix3x4fv"));
    flextGL.ProgramUniformMatrix4dv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLdouble *)>(loader.load("glProgramUniformMatrix4dv"));
    flextGL.ProgramUniformMatrix4fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix4fv"));
    flextGL.ProgramUniformMatrix4x2dv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLdouble *)>(loader.load("glProgramUniformMatrix4x2dv"));
    flextGL.ProgramUniformMatrix4x2fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix4x2fv"));
    flextGL.ProgramUniformMatrix4x3dv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLdouble *)>(loader.load("glProgramUniformMatrix4x3dv"));
    flextGL.ProgramUniformMatrix4x3fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix4x3fv"));
    flextGL.ReleaseShaderCompiler = reinterpret_cast<void(APIENTRY*)(void)>(loader.load("glReleaseShaderCompiler"));
    flextGL.ScissorArrayv = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, const GLint *)>(loader.load("glScissorArrayv"));
    flextGL.ScissorIndexed = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLsizei, GLsizei)>(loader.load("glScissorIndexed"));
    flextGL.ScissorIndexedv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLint *)>(loader.load("glScissorIndexedv"));
    flextGL.ShaderBinary = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLuint *, GLenum, const void *, GLsizei)>(loader.load("glShaderBinary"));
    flextGL.UseProgramStages = reinterpret_cast<void(APIENTRY*)(GLuint, GLbitfield, GLuint)>(loader.load("glUseProgramStages"));
    flextGL.ValidateProgramPipeline = reinterpret_cast<void(APIENTRY*)(GLuint)>(loader.load("glValidateProgramPipeline"));
    flextGL.VertexAttribL1d = reinterpret_cast<void(APIENTRY*)(GLuint, GLdouble)>(loader.load("glVertexAttribL1d"));
    flextGL.VertexAttribL1dv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLdouble *)>(loader.load("glVertexAttribL1dv"));
    flextGL.VertexAttribL2d = reinterpret_cast<void(APIENTRY*)(GLuint, GLdouble, GLdouble)>(loader.load("glVertexAttribL2d"));
    flextGL.VertexAttribL2dv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLdouble *)>(loader.load("glVertexAttribL2dv"));
    flextGL.VertexAttribL3d = reinterpret_cast<void(APIENTRY*)(GLuint, GLdouble, GLdouble, GLdouble)>(loader.load("glVertexAttribL3d"));
    flextGL.VertexAttribL3dv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLdouble *)>(loader.load("glVertexAttribL3dv"));
    flextGL.VertexAttribL4d = reinterpret_cast<void(APIENTRY*)(GLuint, GLdouble, GLdouble, GLdouble, GLdouble)>(loader.load("glVertexAttribL4d"));
    flextGL.VertexAttribL4dv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLdouble *)>(loader.load("glVertexAttribL4dv"));
    flextGL.VertexAttribLPointer = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLenum, GLsizei, const void *)>(loader.load("glVertexAttribLPointer"));
    flextGL.ViewportArrayv = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, const GLfloat *)>(loader.load("glViewportArrayv"));
    flextGL.ViewportIndexedf = reinterpret_cast<void(APIENTRY*)(GLuint, GLfloat, GLfloat, GLfloat, GLfloat)>(loader.load("glViewportIndexedf"));
    flextGL.ViewportIndexedfv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLfloat *)>(loader.load("glViewportIndexedfv"));

    /* GL_VERSION_4_2 */
    flextGL.BindImageTexture = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLint, GLboolean, GLint, GLenum, GLenum)>(loader.load("glBindImageTexture"));
    flextGL.DrawArraysInstancedBaseInstance = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLsizei, GLsizei, GLuint)>(loader.load("glDrawArraysInstancedBaseInstance"));
    flextGL.DrawElementsInstancedBaseInstance = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, const void *, GLsizei, GLuint)>(loader.load("glDrawElementsInstancedBaseInstance"));
    flextGL.DrawElementsInstancedBaseVertexBaseInstance = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, const void *, GLsizei, GLint, GLuint)>(loader.load("glDrawElementsInstancedBaseVertexBaseInstance"));
    flextGL.DrawTransformFeedbackInstanced = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLsizei)>(loader.load("glDrawTransformFeedbackInstanced"));
    flextGL.DrawTransformFeedbackStreamInstanced = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLuint, GLsizei)>(loader.load("glDrawTransformFeedbackStreamInstanced"));
    flextGL.GetActiveAtomicCounterBufferiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLenum, GLint *)>(loader.load("glGetActiveAtomicCounterBufferiv"));
    flextGL.GetInternalformativ = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLenum, GLsizei, GLint *)>(loader.load("glGetInternalformativ"));
    flextGL.MemoryBarrier = reinterpret_cast<void(APIENTRY*)(GLbitfield)>(loader.load("glMemoryBarrier"));
    flextGL.TexStorage1D = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, GLsizei)>(loader.load("glTexStorage1D"));
    flextGL.TexStorage2D = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, GLsizei, GLsizei)>(loader.load("glTexStorage2D"));
    flextGL.TexStorage3D = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLsizei)>(loader.load("glTexStorage3D"));

    /* GL_VERSION_4_3 */
    flextGL.BindVertexBuffer = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLintptr, GLsizei)>(loader.load("glBindVertexBuffer"));
    flextGL.ClearBufferData = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLenum, GLenum, const void *)>(loader.load("glClearBufferData"));
    flextGL.ClearBufferSubData = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLintptr, GLsizeiptr, GLenum, GLenum, const void *)>(loader.load("glClearBufferSubData"));
    flextGL.CopyImageSubData = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint, GLint, GLint, GLint, GLuint, GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei)>(loader.load("glCopyImageSubData"));
    flextGL.DebugMessageCallback = reinterpret_cast<void(APIENTRY*)(GLDEBUGPROC, const void *)>(loader.load("glDebugMessageCallback"));
    flextGL.DebugMessageControl = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLenum, GLsizei, const GLuint *, GLboolean)>(loader.load("glDebugMessageControl"));
    flextGL.DebugMessageInsert = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar *)>(loader.load("glDebugMessageInsert"));
    flextGL.DispatchCompute = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLuint)>(loader.load("glDispatchCompute"));
    flextGL.DispatchComputeIndirect = reinterpret_cast<void(APIENTRY*)(GLintptr)>(loader.load("glDispatchComputeIndirect"));
    flextGL.FramebufferParameteri = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLint)>(loader.load("glFramebufferParameteri"));
    flextGL.GetDebugMessageLog = reinterpret_cast<GLuint(APIENTRY*)(GLuint, GLsizei, GLenum *, GLenum *, GLuint *, GLenum *, GLsizei *, GLchar *)>(loader.load("glGetDebugMessageLog"));
    flextGL.GetFramebufferParameteriv = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLint *)>(loader.load("glGetFramebufferParameteriv"));
    flextGL.GetInternalformati64v = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLenum, GLsizei, GLint64 *)>(loader.load("glGetInternalformati64v"));
    flextGL.GetObjectLabel = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLsizei, GLsizei *, GLchar *)>(loader.load("glGetObjectLabel"));
    flextGL.GetObjectPtrLabel = reinterpret_cast<void(APIENTRY*)(const void *, GLsizei, GLsizei *, GLchar *)>(loader.load("glGetObjectPtrLabel"));
    flextGL.GetPointerv = reinterpret_cast<void(APIENTRY*)(GLenum, void **)>(loader.load("glGetPointerv"));
    flextGL.GetProgramInterfaceiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLenum, GLint *)>(loader.load("glGetProgramInterfaceiv"));
    flextGL.GetProgramResourceIndex = reinterpret_cast<GLuint(APIENTRY*)(GLuint, GLenum, const GLchar *)>(loader.load("glGetProgramResourceIndex"));
    flextGL.GetProgramResourceLocation = reinterpret_cast<GLint(APIENTRY*)(GLuint, GLenum, const GLchar *)>(loader.load("glGetProgramResourceLocation"));
    flextGL.GetProgramResourceLocationIndex = reinterpret_cast<GLint(APIENTRY*)(GLuint, GLenum, const GLchar *)>(loader.load("glGetProgramResourceLocationIndex"));
    flextGL.GetProgramResourceName = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint, GLsizei, GLsizei *, GLchar *)>(loader.load("glGetProgramResourceName"));
    flextGL.GetProgramResourceiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint, GLsizei, const GLenum *, GLsizei, GLsizei *, GLint *)>(loader.load("glGetProgramResourceiv"));
    flextGL.InvalidateBufferData = reinterpret_cast<void(APIENTRY*)(GLuint)>(loader.load("glInvalidateBufferData"));
    flextGL.InvalidateBufferSubData = reinterpret_cast<void(APIENTRY*)(GLuint, GLintptr, GLsizeiptr)>(loader.load("glInvalidateBufferSubData"));
    flextGL.InvalidateFramebuffer = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, const GLenum *)>(loader.load("glInvalidateFramebuffer"));
    flextGL.InvalidateSubFramebuffer = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, const GLenum *, GLint, GLint, GLsizei, GLsizei)>(loader.load("glInvalidateSubFramebuffer"));
    flextGL.InvalidateTexImage = reinterpret_cast<void(APIENTRY*)(GLuint, GLint)>(loader.load("glInvalidateTexImage"));
    flextGL.InvalidateTexSubImage = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei)>(loader.load("glInvalidateTexSubImage"));
    flextGL.MultiDrawArraysIndirect = reinterpret_cast<void(APIENTRY*)(GLenum, const void *, GLsizei, GLsizei)>(loader.load("glMultiDrawArraysIndirect"));
    flextGL.MultiDrawElementsIndirect = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, const void *, GLsizei, GLsizei)>(loader.load("glMultiDrawElementsIndirect"));
    flextGL.ObjectLabel = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLsizei, const GLchar *)>(loader.load("glObjectLabel"));
    flextGL.ObjectPtrLabel = reinterpret_cast<void(APIENTRY*)(const void *, GLsizei, const GLchar *)>(loader.load("glObjectPtrLabel"));
    flextGL.PopDebugGroup = reinterpret_cast<void(APIENTRY*)(void)>(loader.load("glPopDebugGroup"));
    flextGL.PushDebugGroup = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLsizei, const GLchar *)>(loader.load("glPushDebugGroup"));
    flextGL.ShaderStorageBlockBinding = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLuint)>(loader.load("glShaderStorageBlockBinding"));
    flextGL.TexBufferRange = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLuint, GLintptr, GLsizeiptr)>(loader.load("glTexBufferRange"));
    flextGL.TexStorage2DMultisample = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLboolean)>(loader.load("glTexStorage2DMultisample"));
    flextGL.TexStorage3DMultisample = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLsizei, GLboolean)>(loader.load("glTexStorage3DMultisample"));
    flextGL.TextureView = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint, GLenum, GLuint, GLuint, GLuint, GLuint)>(loader.load("glTextureView"));
    flextGL.VertexAttribBinding = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint)>(loader.load("glVertexAttribBinding"));
    flextGL.VertexAttribFormat = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLenum, GLboolean, GLuint)>(loader.load("glVertexAttribFormat"));
    flextGL.VertexAttribIFormat = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLenum, GLuint)>(loader.load("glVertexAttribIFormat"));
    flextGL.VertexAttribLFormat = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLenum, GLuint)>(loader.load("glVertexAttribLFormat"));
    flextGL.VertexBindingDivisor = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint)>(loader.load("glVertexBindingDivisor"));

    /* GL_VERSION_4_4 */
    flextGL.BindBuffersBase = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLsizei, const GLuint *)>(loader.load("glBindBuffersBase"));
    flextGL.BindBuffersRange = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLsizei, const GLuint *, const GLintptr *, const GLsizeiptr *)>(loader.load("glBindBuffersRange"));
    flextGL.BindImageTextures = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, const GLuint *)>(loader.load("glBindImageTextures"));
    flextGL.BindSamplers = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, const GLuint *)>(loader.load("glBindSamplers"));
    flextGL.BindTextures = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, const GLuint *)>(loader.load("glBindTextures"));
    flextGL.BindVertexBuffers = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, const GLuint *, const GLintptr *, const GLsizei *)>(loader.load("glBindVertexBuffers"));
    flextGL.BufferStorage = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizeiptr, const void *, GLbitfield)>(loader.load("glBufferStorage"));
    flextGL.ClearTexImage = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLenum, GLenum, const void *)>(loader.load("glClearTexImage"));
    flextGL.ClearTexSubImage = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, const void *)>(loader.load("glClearTexSubImage"));

    /* GL_VERSION_4_5 */
    flextGL.BindTextureUnit = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint)>(loader.load("glBindTextureUnit"));
    flextGL.BlitNamedFramebuffer = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLbitfield, GLenum)>(loader.load("glBlitNamedFramebuffer"));
    flextGL.CheckNamedFramebufferStatus = reinterpret_cast<GLenum(APIENTRY*)(GLuint, GLenum)>(loader.load("glCheckNamedFramebufferStatus"));
    flextGL.ClearNamedBufferData = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLenum, GLenum, const void *)>(loader.load("glClearNamedBufferData"));
    flextGL.ClearNamedBufferSubData = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLintptr, GLsizeiptr, GLenum, GLenum, const void *)>(loader.load("glClearNamedBufferSubData"));
    flextGL.ClearNamedFramebufferfi = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint, GLfloat, GLint)>(loader.load("glClearNamedFramebufferfi"));
    flextGL.ClearNamedFramebufferfv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint, const GLfloat *)>(loader.load("glClearNamedFramebufferfv"));
    flextGL.ClearNamedFramebufferiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint, const GLint *)>(loader.load("glClearNamedFramebufferiv"));
    flextGL.ClearNamedFramebufferuiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint, const GLuint *)>(loader.load("glClearNamedFramebufferuiv"));
    flextGL.ClipControl = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum)>(loader.load("glClipControl"));
    flextGL.CompressedTextureSubImage1D = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLsizei, GLenum, GLsizei, const void *)>(loader.load("glCompressedTextureSubImage1D"));
    flextGL.CompressedTextureSubImage2D = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLsizei, const void *)>(loader.load("glCompressedTextureSubImage2D"));
    flextGL.CompressedTextureSubImage3D = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLsizei, const void *)>(loader.load("glCompressedTextureSubImage3D"));
    flextGL.CopyNamedBufferSubData = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLintptr, GLintptr, GLsizeiptr)>(loader.load("glCopyNamedBufferSubData"));
    flextGL.CopyTextureSubImage1D = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLint, GLint, GLsizei)>(loader.load("glCopyTextureSubImage1D"));
    flextGL.CopyTextureSubImage2D = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei)>(loader.load("glCopyTextureSubImage2D"));
    flextGL.CopyTextureSubImage3D = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei)>(loader.load("glCopyTextureSubImage3D"));
    flextGL.CreateBuffers = reinterpret_cast<void(APIENTRY*)(GLsizei, GLuint *)>(loader.load("glCreateBuffers"));
    flextGL.CreateFramebuffers = reinterpret_cast<void(APIENTRY*)(GLsizei, GLuint *)>(loader.load("glCreateFramebuffers"));
    flextGL.CreateProgramPipelines = reinterpret_cast<void(APIENTRY*)(GLsizei, GLuint *)>(loader.load("glCreateProgramPipelines"));
    flextGL.CreateQueries = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLuint *)>(loader.load("glCreateQueries"));
    flextGL.CreateRenderbuffers = reinterpret_cast<void(APIENTRY*)(GLsizei, GLuint *)>(loader.load("glCreateRenderbuffers"));
    flextGL.CreateSamplers = reinterpret_cast<void(APIENTRY*)(GLsizei, GLuint *)>(loader.load("glCreateSamplers"));
    flextGL.CreateTextures = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLuint *)>(loader.load("glCreateTextures"));
    flextGL.CreateTransformFeedbacks = reinterpret_cast<void(APIENTRY*)(GLsizei, GLuint *)>(loader.load("glCreateTransformFeedbacks"));
    flextGL.CreateVertexArrays = reinterpret_cast<void(APIENTRY*)(GLsizei, GLuint *)>(loader.load("glCreateVertexArrays"));
    flextGL.DisableVertexArrayAttrib = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint)>(loader.load("glDisableVertexArrayAttrib"));
    flextGL.EnableVertexArrayAttrib = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint)>(loader.load("glEnableVertexArrayAttrib"));
    flextGL.FlushMappedNamedBufferRange = reinterpret_cast<void(APIENTRY*)(GLuint, GLintptr, GLsizeiptr)>(loader.load("glFlushMappedNamedBufferRange"));
    flextGL.GenerateTextureMipmap = reinterpret_cast<void(APIENTRY*)(GLuint)>(loader.load("glGenerateTextureMipmap"));
    flextGL.GetCompressedTextureImage = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, void *)>(loader.load("glGetCompressedTextureImage"));
    flextGL.GetCompressedTextureSubImage = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLsizei, void *)>(loader.load("glGetCompressedTextureSubImage"));
    flextGL.GetGraphicsResetStatus = reinterpret_cast<GLenum(APIENTRY*)(void)>(loader.load("glGetGraphicsResetStatus"));
    flextGL.GetNamedBufferParameteri64v = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint64 *)>(loader.load("glGetNamedBufferParameteri64v"));
    flextGL.GetNamedBufferParameteriv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint *)>(loader.load("glGetNamedBufferParameteriv"));
    flextGL.GetNamedBufferPointerv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, void **)>(loader.load("glGetNamedBufferPointerv"));
    flextGL.GetNamedBufferSubData = reinterpret_cast<void(APIENTRY*)(GLuint, GLintptr, GLsizeiptr, void *)>(loader.load("glGetNamedBufferSubData"));
    flextGL.GetNamedFramebufferAttachmentParameteriv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLenum, GLint *)>(loader.load("glGetNamedFramebufferAttachmentParameteriv"));
    flextGL.GetNamedFramebufferParameteriv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint *)>(loader.load("glGetNamedFramebufferParameteriv"));
    flextGL.GetNamedRenderbufferParameteriv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint *)>(loader.load("glGetNamedRenderbufferParameteriv"));
    flextGL.GetQueryBufferObjecti64v = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLenum, GLintptr)>(loader.load("glGetQueryBufferObjecti64v"));
    flextGL.GetQueryBufferObjectiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLenum, GLintptr)>(loader.load("glGetQueryBufferObjectiv"));
    flextGL.GetQueryBufferObjectui64v = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLenum, GLintptr)>(loader.load("glGetQueryBufferObjectui64v"));
    flextGL.GetQueryBufferObjectuiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLenum, GLintptr)>(loader.load("glGetQueryBufferObjectuiv"));
    flextGL.GetTextureImage = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLenum, GLenum, GLsizei, void *)>(loader.load("glGetTextureImage"));
    flextGL.GetTextureLevelParameterfv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLenum, GLfloat *)>(loader.load("glGetTextureLevelParameterfv"));
    flextGL.GetTextureLevelParameteriv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLenum, GLint *)>(loader.load("glGetTextureLevelParameteriv"));
    flextGL.GetTextureParameterIiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint *)>(loader.load("glGetTextureParameterIiv"));
    flextGL.GetTextureParameterIuiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint *)>(loader.load("glGetTextureParameterIuiv"));
    flextGL.GetTextureParameterfv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLfloat *)>(loader.load("glGetTextureParameterfv"));
    flextGL.GetTextureParameteriv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint *)>(loader.load("glGetTextureParameteriv"));
    flextGL.GetTextureSubImage = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, GLsizei, void *)>(loader.load("glGetTextureSubImage"));
    flextGL.GetTransformFeedbacki64_v = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint, GLint64 *)>(loader.load("glGetTransformFeedbacki64_v"));
    flextGL.GetTransformFeedbacki_v = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint, GLint *)>(loader.load("glGetTransformFeedbacki_v"));
    flextGL.GetTransformFeedbackiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint *)>(loader.load("glGetTransformFeedbackiv"));
    flextGL.GetVertexArrayIndexed64iv = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLenum, GLint64 *)>(loader.load("glGetVertexArrayIndexed64iv"));
    flextGL.GetVertexArrayIndexediv = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLenum, GLint *)>(loader.load("glGetVertexArrayIndexediv"));
    flextGL.GetVertexArrayiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint *)>(loader.load("glGetVertexArrayiv"));
    flextGL.GetnCompressedTexImage = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLsizei, void *)>(loader.load("glGetnCompressedTexImage"));
    flextGL.GetnTexImage = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLenum, GLenum, GLsizei, void *)>(loader.load("glGetnTexImage"));
    flextGL.GetnUniformdv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLdouble *)>(loader.load("glGetnUniformdv"));
    flextGL.GetnUniformfv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLfloat *)>(loader.load("glGetnUniformfv"));
    flextGL.GetnUniformiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLint *)>(loader.load("glGetnUniformiv"));
    flextGL.GetnUniformuiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLuint *)>(loader.load("glGetnUniformuiv"));
    flextGL.InvalidateNamedFramebufferData = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, const GLenum *)>(loader.load("glInvalidateNamedFramebufferData"));
    flextGL.InvalidateNamedFramebufferSubData = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, const GLenum *, GLint, GLint, GLsizei, GLsizei)>(loader.load("glInvalidateNamedFramebufferSubData"));
    flextGL.MapNamedBuffer = reinterpret_cast<void *(APIENTRY*)(GLuint, GLenum)>(loader.load("glMapNamedBuffer"));
    flextGL.MapNamedBufferRange = reinterpret_cast<void *(APIENTRY*)(GLuint, GLintptr, GLsizeiptr, GLbitfield)>(loader.load("glMapNamedBufferRange"));
    flextGL.MemoryBarrierByRegion = reinterpret_cast<void(APIENTRY*)(GLbitfield)>(loader.load("glMemoryBarrierByRegion"));
    flextGL.NamedBufferData = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizeiptr, const void *, GLenum)>(loader.load("glNamedBufferData"));
    flextGL.NamedBufferStorage = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizeiptr, const void *, GLbitfield)>(loader.load("glNamedBufferStorage"));
    flextGL.NamedBufferSubData = reinterpret_cast<void(APIENTRY*)(GLuint, GLintptr, GLsizeiptr, const void *)>(loader.load("glNamedBufferSubData"));
    flextGL.NamedFramebufferDrawBuffer = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum)>(loader.load("glNamedFramebufferDrawBuffer"));
    flextGL.NamedFramebufferDrawBuffers = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, const GLenum *)>(loader.load("glNamedFramebufferDrawBuffers"));
    flextGL.NamedFramebufferParameteri = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint)>(loader.load("glNamedFramebufferParameteri"));
    flextGL.NamedFramebufferReadBuffer = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum)>(loader.load("glNamedFramebufferReadBuffer"));
    flextGL.NamedFramebufferRenderbuffer = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLenum, GLuint)>(loader.load("glNamedFramebufferRenderbuffer"));
    flextGL.NamedFramebufferTexture = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint, GLint)>(loader.load("glNamedFramebufferTexture"));
    flextGL.NamedFramebufferTextureLayer = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint, GLint, GLint)>(loader.load("glNamedFramebufferTextureLayer"));
    flextGL.NamedRenderbufferStorage = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLsizei, GLsizei)>(loader.load("glNamedRenderbufferStorage"));
    flextGL.NamedRenderbufferStorageMultisample = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, GLenum, GLsizei, GLsizei)>(loader.load("glNamedRenderbufferStorageMultisample"));
    flextGL.ReadnPixels = reinterpret_cast<void(APIENTRY*)(GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, GLsizei, void *)>(loader.load("glReadnPixels"));
    flextGL.TextureBarrier = reinterpret_cast<void(APIENTRY*)(void)>(loader.load("glTextureBarrier"));
    flextGL.TextureBuffer = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint)>(loader.load("glTextureBuffer"));
    flextGL.TextureBufferRange = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint, GLintptr, GLsizeiptr)>(loader.load("glTextureBufferRange"));
    flextGL.TextureParameterIiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, const GLint *)>(loader.load("glTextureParameterIiv"));
    flextGL.TextureParameterIuiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, const GLuint *)>(loader.load("glTextureParameterIuiv"));
    flextGL.TextureParameterf = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLfloat)>(loader.load("glTextureParameterf"));
    flextGL.TextureParameterfv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, const GLfloat *)>(loader.load("glTextureParameterfv"));
    flextGL.TextureParameteri = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint)>(loader.load("glTextureParameteri"));
    flextGL.TextureParameteriv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, const GLint *)>(loader.load("glTextureParameteriv"));
    flextGL.TextureStorage1D = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, GLenum, GLsizei)>(loader.load("glTextureStorage1D"));
    flextGL.TextureStorage2D = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, GLenum, GLsizei, GLsizei)>(loader.load("glTextureStorage2D"));
    flextGL.TextureStorage2DMultisample = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, GLenum, GLsizei, GLsizei, GLboolean)>(loader.load("glTextureStorage2DMultisample"));
    flextGL.TextureStorage3D = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, GLenum, GLsizei, GLsizei, GLsizei)>(loader.load("glTextureStorage3D"));
    flextGL.TextureStorage3DMultisample = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, GLenum, GLsizei, GLsizei, GLsizei, GLboolean)>(loader.load("glTextureStorage3DMultisample"));
    flextGL.TextureSubImage1D = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLsizei, GLenum, GLenum, const void *)>(loader.load("glTextureSubImage1D"));
    flextGL.TextureSubImage2D = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const void *)>(loader.load("glTextureSubImage2D"));
    flextGL.TextureSubImage3D = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, const void *)>(loader.load("glTextureSubImage3D"));
    flextGL.TransformFeedbackBufferBase = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLuint)>(loader.load("glTransformFeedbackBufferBase"));
    flextGL.TransformFeedbackBufferRange = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLuint, GLintptr, GLsizeiptr)>(loader.load("glTransformFeedbackBufferRange"));
    flextGL.UnmapNamedBuffer = reinterpret_cast<GLboolean(APIENTRY*)(GLuint)>(loader.load("glUnmapNamedBuffer"));
    flextGL.VertexArrayAttribBinding = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLuint)>(loader.load("glVertexArrayAttribBinding"));
    flextGL.VertexArrayAttribFormat = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLint, GLenum, GLboolean, GLuint)>(loader.load("glVertexArrayAttribFormat"));
    flextGL.VertexArrayAttribIFormat = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLint, GLenum, GLuint)>(loader.load("glVertexArrayAttribIFormat"));
    flextGL.VertexArrayAttribLFormat = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLint, GLenum, GLuint)>(loader.load("glVertexArrayAttribLFormat"));
    flextGL.VertexArrayBindingDivisor = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLuint)>(loader.load("glVertexArrayBindingDivisor"));
    flextGL.VertexArrayElementBuffer = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint)>(loader.load("glVertexArrayElementBuffer"));
    flextGL.VertexArrayVertexBuffer = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLuint, GLintptr, GLsizei)>(loader.load("glVertexArrayVertexBuffer"));
    flextGL.VertexArrayVertexBuffers = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLsizei, const GLuint *, const GLintptr *, const GLsizei *)>(loader.load("glVertexArrayVertexBuffers"));

    /* GL_VERSION_4_6 */
    flextGL.MultiDrawArraysIndirectCount = reinterpret_cast<void(APIENTRY*)(GLenum, const void *, GLintptr, GLsizei, GLsizei)>(loader.load("glMultiDrawArraysIndirectCount"));
    flextGL.MultiDrawElementsIndirectCount = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, const void *, GLintptr, GLsizei, GLsizei)>(loader.load("glMultiDrawElementsIndirectCount"));
    flextGL.PolygonOffsetClamp = reinterpret_cast<void(APIENTRY*)(GLfloat, GLfloat, GLfloat)>(loader.load("glPolygonOffsetClamp"));
    flextGL.SpecializeShader = reinterpret_cast<void(APIENTRY*)(GLuint, const GLchar *, GLuint, const GLuint *, const GLuint *)>(loader.load("glSpecializeShader"));
}
