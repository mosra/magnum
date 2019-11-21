/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
              Vladimír Vondruš <mosra@centrum.cz>

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
            flextglBlendFunc = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum)>(loader.load("glBlendFunc"));
            flextglClear = reinterpret_cast<void(APIENTRY*)(GLbitfield)>(loader.load("glClear"));
            flextglClearColor = reinterpret_cast<void(APIENTRY*)(GLfloat, GLfloat, GLfloat, GLfloat)>(loader.load("glClearColor"));
            flextglClearDepth = reinterpret_cast<void(APIENTRY*)(GLdouble)>(loader.load("glClearDepth"));
            flextglClearStencil = reinterpret_cast<void(APIENTRY*)(GLint)>(loader.load("glClearStencil"));
            flextglColorMask = reinterpret_cast<void(APIENTRY*)(GLboolean, GLboolean, GLboolean, GLboolean)>(loader.load("glColorMask"));
            flextglCullFace = reinterpret_cast<void(APIENTRY*)(GLenum)>(loader.load("glCullFace"));
            flextglDepthFunc = reinterpret_cast<void(APIENTRY*)(GLenum)>(loader.load("glDepthFunc"));
            flextglDepthMask = reinterpret_cast<void(APIENTRY*)(GLboolean)>(loader.load("glDepthMask"));
            flextglDepthRange = reinterpret_cast<void(APIENTRY*)(GLdouble, GLdouble)>(loader.load("glDepthRange"));
            flextglDisable = reinterpret_cast<void(APIENTRY*)(GLenum)>(loader.load("glDisable"));
            flextglDrawBuffer = reinterpret_cast<void(APIENTRY*)(GLenum)>(loader.load("glDrawBuffer"));
            flextglEnable = reinterpret_cast<void(APIENTRY*)(GLenum)>(loader.load("glEnable"));
            flextglFinish = reinterpret_cast<void(APIENTRY*)(void)>(loader.load("glFinish"));
            flextglFlush = reinterpret_cast<void(APIENTRY*)(void)>(loader.load("glFlush"));
            flextglFrontFace = reinterpret_cast<void(APIENTRY*)(GLenum)>(loader.load("glFrontFace"));
            flextglGetBooleanv = reinterpret_cast<void(APIENTRY*)(GLenum, GLboolean *)>(loader.load("glGetBooleanv"));
            flextglGetDoublev = reinterpret_cast<void(APIENTRY*)(GLenum, GLdouble *)>(loader.load("glGetDoublev"));
            flextglGetError = reinterpret_cast<GLenum(APIENTRY*)(void)>(loader.load("glGetError"));
            flextglGetFloatv = reinterpret_cast<void(APIENTRY*)(GLenum, GLfloat *)>(loader.load("glGetFloatv"));
            flextglGetIntegerv = reinterpret_cast<void(APIENTRY*)(GLenum, GLint *)>(loader.load("glGetIntegerv"));
            flextglGetString = reinterpret_cast<const GLubyte *(APIENTRY*)(GLenum)>(loader.load("glGetString"));
            flextglGetTexImage = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLenum, GLenum, void *)>(loader.load("glGetTexImage"));
            flextglGetTexLevelParameterfv = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLenum, GLfloat *)>(loader.load("glGetTexLevelParameterfv"));
            flextglGetTexLevelParameteriv = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLenum, GLint *)>(loader.load("glGetTexLevelParameteriv"));
            flextglGetTexParameterfv = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLfloat *)>(loader.load("glGetTexParameterfv"));
            flextglGetTexParameteriv = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLint *)>(loader.load("glGetTexParameteriv"));
            flextglHint = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum)>(loader.load("glHint"));
            flextglIsEnabled = reinterpret_cast<GLboolean(APIENTRY*)(GLenum)>(loader.load("glIsEnabled"));
            flextglLineWidth = reinterpret_cast<void(APIENTRY*)(GLfloat)>(loader.load("glLineWidth"));
            flextglLogicOp = reinterpret_cast<void(APIENTRY*)(GLenum)>(loader.load("glLogicOp"));
            flextglPixelStoref = reinterpret_cast<void(APIENTRY*)(GLenum, GLfloat)>(loader.load("glPixelStoref"));
            flextglPixelStorei = reinterpret_cast<void(APIENTRY*)(GLenum, GLint)>(loader.load("glPixelStorei"));
            flextglPointSize = reinterpret_cast<void(APIENTRY*)(GLfloat)>(loader.load("glPointSize"));
            flextglPolygonMode = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum)>(loader.load("glPolygonMode"));
            flextglReadBuffer = reinterpret_cast<void(APIENTRY*)(GLenum)>(loader.load("glReadBuffer"));
            flextglReadPixels = reinterpret_cast<void(APIENTRY*)(GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, void *)>(loader.load("glReadPixels"));
            flextglScissor = reinterpret_cast<void(APIENTRY*)(GLint, GLint, GLsizei, GLsizei)>(loader.load("glScissor"));
            flextglStencilFunc = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLuint)>(loader.load("glStencilFunc"));
            flextglStencilMask = reinterpret_cast<void(APIENTRY*)(GLuint)>(loader.load("glStencilMask"));
            flextglStencilOp = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLenum)>(loader.load("glStencilOp"));
            flextglTexImage1D = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLint, GLsizei, GLint, GLenum, GLenum, const void *)>(loader.load("glTexImage1D"));
            flextglTexImage2D = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const void *)>(loader.load("glTexImage2D"));
            flextglTexParameterf = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLfloat)>(loader.load("glTexParameterf"));
            flextglTexParameterfv = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, const GLfloat *)>(loader.load("glTexParameterfv"));
            flextglTexParameteri = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLint)>(loader.load("glTexParameteri"));
            flextglTexParameteriv = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, const GLint *)>(loader.load("glTexParameteriv"));
            flextglViewport = reinterpret_cast<void(APIENTRY*)(GLint, GLint, GLsizei, GLsizei)>(loader.load("glViewport"));

            /* GL_VERSION_1_1 */
            flextglBindTexture = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint)>(loader.load("glBindTexture"));
            flextglCopyTexImage1D = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLenum, GLint, GLint, GLsizei, GLint)>(loader.load("glCopyTexImage1D"));
            flextglCopyTexImage2D = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLenum, GLint, GLint, GLsizei, GLsizei, GLint)>(loader.load("glCopyTexImage2D"));
            flextglCopyTexSubImage1D = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLint, GLint, GLint, GLsizei)>(loader.load("glCopyTexSubImage1D"));
            flextglCopyTexSubImage2D = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei)>(loader.load("glCopyTexSubImage2D"));
            flextglDeleteTextures = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLuint *)>(loader.load("glDeleteTextures"));
            flextglDrawArrays = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLsizei)>(loader.load("glDrawArrays"));
            flextglDrawElements = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, const void *)>(loader.load("glDrawElements"));
            flextglGenTextures = reinterpret_cast<void(APIENTRY*)(GLsizei, GLuint *)>(loader.load("glGenTextures"));
            flextglIsTexture = reinterpret_cast<GLboolean(APIENTRY*)(GLuint)>(loader.load("glIsTexture"));
            flextglPolygonOffset = reinterpret_cast<void(APIENTRY*)(GLfloat, GLfloat)>(loader.load("glPolygonOffset"));
            flextglTexSubImage1D = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLint, GLsizei, GLenum, GLenum, const void *)>(loader.load("glTexSubImage1D"));
            flextglTexSubImage2D = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const void *)>(loader.load("glTexSubImage2D"));
        }
    }
    #else
    static_cast<void>(context);
    #endif

    /* GL_ARB_ES3_2_compatibility */
    flextglPrimitiveBoundingBoxARB = reinterpret_cast<void(APIENTRY*)(GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat)>(loader.load("glPrimitiveBoundingBoxARB"));

    /* GL_ARB_bindless_texture */
    flextglGetImageHandleARB = reinterpret_cast<GLuint64(APIENTRY*)(GLuint, GLint, GLboolean, GLint, GLenum)>(loader.load("glGetImageHandleARB"));
    flextglGetTextureHandleARB = reinterpret_cast<GLuint64(APIENTRY*)(GLuint)>(loader.load("glGetTextureHandleARB"));
    flextglGetTextureSamplerHandleARB = reinterpret_cast<GLuint64(APIENTRY*)(GLuint, GLuint)>(loader.load("glGetTextureSamplerHandleARB"));
    flextglGetVertexAttribLui64vARB = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint64EXT *)>(loader.load("glGetVertexAttribLui64vARB"));
    flextglIsImageHandleResidentARB = reinterpret_cast<GLboolean(APIENTRY*)(GLuint64)>(loader.load("glIsImageHandleResidentARB"));
    flextglIsTextureHandleResidentARB = reinterpret_cast<GLboolean(APIENTRY*)(GLuint64)>(loader.load("glIsTextureHandleResidentARB"));
    flextglMakeImageHandleNonResidentARB = reinterpret_cast<void(APIENTRY*)(GLuint64)>(loader.load("glMakeImageHandleNonResidentARB"));
    flextglMakeImageHandleResidentARB = reinterpret_cast<void(APIENTRY*)(GLuint64, GLenum)>(loader.load("glMakeImageHandleResidentARB"));
    flextglMakeTextureHandleNonResidentARB = reinterpret_cast<void(APIENTRY*)(GLuint64)>(loader.load("glMakeTextureHandleNonResidentARB"));
    flextglMakeTextureHandleResidentARB = reinterpret_cast<void(APIENTRY*)(GLuint64)>(loader.load("glMakeTextureHandleResidentARB"));
    flextglProgramUniformHandleui64ARB = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLuint64)>(loader.load("glProgramUniformHandleui64ARB"));
    flextglProgramUniformHandleui64vARB = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLuint64 *)>(loader.load("glProgramUniformHandleui64vARB"));
    flextglUniformHandleui64ARB = reinterpret_cast<void(APIENTRY*)(GLint, GLuint64)>(loader.load("glUniformHandleui64ARB"));
    flextglUniformHandleui64vARB = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, const GLuint64 *)>(loader.load("glUniformHandleui64vARB"));
    flextglVertexAttribL1ui64ARB = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint64EXT)>(loader.load("glVertexAttribL1ui64ARB"));
    flextglVertexAttribL1ui64vARB = reinterpret_cast<void(APIENTRY*)(GLuint, const GLuint64EXT *)>(loader.load("glVertexAttribL1ui64vARB"));

    /* GL_ARB_compute_variable_group_size */
    flextglDispatchComputeGroupSizeARB = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLuint, GLuint, GLuint, GLuint)>(loader.load("glDispatchComputeGroupSizeARB"));

    /* GL_ARB_robustness */
    flextglGetGraphicsResetStatusARB = reinterpret_cast<GLenum(APIENTRY*)(void)>(loader.load("glGetGraphicsResetStatusARB"));
    flextglGetnCompressedTexImageARB = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLsizei, void *)>(loader.load("glGetnCompressedTexImageARB"));
    flextglGetnTexImageARB = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLenum, GLenum, GLsizei, void *)>(loader.load("glGetnTexImageARB"));
    flextglGetnUniformdvARB = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLdouble *)>(loader.load("glGetnUniformdvARB"));
    flextglGetnUniformfvARB = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLfloat *)>(loader.load("glGetnUniformfvARB"));
    flextglGetnUniformivARB = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLint *)>(loader.load("glGetnUniformivARB"));
    flextglGetnUniformuivARB = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLuint *)>(loader.load("glGetnUniformuivARB"));
    flextglReadnPixelsARB = reinterpret_cast<void(APIENTRY*)(GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, GLsizei, void *)>(loader.load("glReadnPixelsARB"));

    /* GL_ARB_sample_locations */
    flextglEvaluateDepthValuesARB = reinterpret_cast<void(APIENTRY*)(void)>(loader.load("glEvaluateDepthValuesARB"));
    flextglFramebufferSampleLocationsfvARB = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLsizei, const GLfloat *)>(loader.load("glFramebufferSampleLocationsfvARB"));
    flextglNamedFramebufferSampleLocationsfvARB = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLsizei, const GLfloat *)>(loader.load("glNamedFramebufferSampleLocationsfvARB"));

    /* GL_ARB_sparse_buffer */
    flextglBufferPageCommitmentARB = reinterpret_cast<void(APIENTRY*)(GLenum, GLintptr, GLsizeiptr, GLboolean)>(loader.load("glBufferPageCommitmentARB"));
    flextglNamedBufferPageCommitmentARB = reinterpret_cast<void(APIENTRY*)(GLuint, GLintptr, GLsizeiptr, GLboolean)>(loader.load("glNamedBufferPageCommitmentARB"));
    flextglNamedBufferPageCommitmentEXT = reinterpret_cast<void(APIENTRY*)(GLuint, GLintptr, GLsizeiptr, GLboolean)>(loader.load("glNamedBufferPageCommitmentEXT"));

    /* GL_ARB_sparse_texture */
    flextglTexPageCommitmentARB = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLboolean)>(loader.load("glTexPageCommitmentARB"));

    /* GL_EXT_debug_label */
    flextglGetObjectLabelEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLsizei, GLsizei *, GLchar *)>(loader.load("glGetObjectLabelEXT"));
    flextglLabelObjectEXT = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLsizei, const GLchar *)>(loader.load("glLabelObjectEXT"));

    /* GL_EXT_debug_marker */
    flextglInsertEventMarkerEXT = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLchar *)>(loader.load("glInsertEventMarkerEXT"));
    flextglPopGroupMarkerEXT = reinterpret_cast<void(APIENTRY*)(void)>(loader.load("glPopGroupMarkerEXT"));
    flextglPushGroupMarkerEXT = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLchar *)>(loader.load("glPushGroupMarkerEXT"));

    /* GL_GREMEDY_string_marker */
    flextglStringMarkerGREMEDY = reinterpret_cast<void(APIENTRY*)(GLsizei, const void *)>(loader.load("glStringMarkerGREMEDY"));

    /* GL_KHR_blend_equation_advanced */
    flextglBlendBarrierKHR = reinterpret_cast<void(APIENTRY*)(void)>(loader.load("glBlendBarrierKHR"));

    /* GL_OVR_multiview */
    flextglFramebufferTextureMultiviewOVR = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLuint, GLint, GLint, GLsizei)>(loader.load("glFramebufferTextureMultiviewOVR"));

    /* GL_VERSION_1_2 */
    flextglCopyTexSubImage3D = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei)>(loader.load("glCopyTexSubImage3D"));
    flextglDrawRangeElements = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLuint, GLsizei, GLenum, const void *)>(loader.load("glDrawRangeElements"));
    flextglTexImage3D = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLint, GLsizei, GLsizei, GLsizei, GLint, GLenum, GLenum, const void *)>(loader.load("glTexImage3D"));
    flextglTexSubImage3D = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, const void *)>(loader.load("glTexSubImage3D"));

    /* GL_VERSION_1_3 */
    flextglActiveTexture = reinterpret_cast<void(APIENTRY*)(GLenum)>(loader.load("glActiveTexture"));
    flextglCompressedTexImage1D = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLenum, GLsizei, GLint, GLsizei, const void *)>(loader.load("glCompressedTexImage1D"));
    flextglCompressedTexImage2D = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLenum, GLsizei, GLsizei, GLint, GLsizei, const void *)>(loader.load("glCompressedTexImage2D"));
    flextglCompressedTexImage3D = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLenum, GLsizei, GLsizei, GLsizei, GLint, GLsizei, const void *)>(loader.load("glCompressedTexImage3D"));
    flextglCompressedTexSubImage1D = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLint, GLsizei, GLenum, GLsizei, const void *)>(loader.load("glCompressedTexSubImage1D"));
    flextglCompressedTexSubImage2D = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLsizei, const void *)>(loader.load("glCompressedTexSubImage2D"));
    flextglCompressedTexSubImage3D = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLsizei, const void *)>(loader.load("glCompressedTexSubImage3D"));
    flextglGetCompressedTexImage = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, void *)>(loader.load("glGetCompressedTexImage"));
    flextglSampleCoverage = reinterpret_cast<void(APIENTRY*)(GLfloat, GLboolean)>(loader.load("glSampleCoverage"));

    /* GL_VERSION_1_4 */
    flextglBlendColor = reinterpret_cast<void(APIENTRY*)(GLfloat, GLfloat, GLfloat, GLfloat)>(loader.load("glBlendColor"));
    flextglBlendEquation = reinterpret_cast<void(APIENTRY*)(GLenum)>(loader.load("glBlendEquation"));
    flextglBlendFuncSeparate = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLenum, GLenum)>(loader.load("glBlendFuncSeparate"));
    flextglMultiDrawArrays = reinterpret_cast<void(APIENTRY*)(GLenum, const GLint *, const GLsizei *, GLsizei)>(loader.load("glMultiDrawArrays"));
    flextglMultiDrawElements = reinterpret_cast<void(APIENTRY*)(GLenum, const GLsizei *, GLenum, const void *const*, GLsizei)>(loader.load("glMultiDrawElements"));
    flextglPointParameterf = reinterpret_cast<void(APIENTRY*)(GLenum, GLfloat)>(loader.load("glPointParameterf"));
    flextglPointParameterfv = reinterpret_cast<void(APIENTRY*)(GLenum, const GLfloat *)>(loader.load("glPointParameterfv"));
    flextglPointParameteri = reinterpret_cast<void(APIENTRY*)(GLenum, GLint)>(loader.load("glPointParameteri"));
    flextglPointParameteriv = reinterpret_cast<void(APIENTRY*)(GLenum, const GLint *)>(loader.load("glPointParameteriv"));

    /* GL_VERSION_1_5 */
    flextglBeginQuery = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint)>(loader.load("glBeginQuery"));
    flextglBindBuffer = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint)>(loader.load("glBindBuffer"));
    flextglBufferData = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizeiptr, const void *, GLenum)>(loader.load("glBufferData"));
    flextglBufferSubData = reinterpret_cast<void(APIENTRY*)(GLenum, GLintptr, GLsizeiptr, const void *)>(loader.load("glBufferSubData"));
    flextglDeleteBuffers = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLuint *)>(loader.load("glDeleteBuffers"));
    flextglDeleteQueries = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLuint *)>(loader.load("glDeleteQueries"));
    flextglEndQuery = reinterpret_cast<void(APIENTRY*)(GLenum)>(loader.load("glEndQuery"));
    flextglGenBuffers = reinterpret_cast<void(APIENTRY*)(GLsizei, GLuint *)>(loader.load("glGenBuffers"));
    flextglGenQueries = reinterpret_cast<void(APIENTRY*)(GLsizei, GLuint *)>(loader.load("glGenQueries"));
    flextglGetBufferParameteriv = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLint *)>(loader.load("glGetBufferParameteriv"));
    flextglGetBufferPointerv = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, void **)>(loader.load("glGetBufferPointerv"));
    flextglGetBufferSubData = reinterpret_cast<void(APIENTRY*)(GLenum, GLintptr, GLsizeiptr, void *)>(loader.load("glGetBufferSubData"));
    flextglGetQueryObjectiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint *)>(loader.load("glGetQueryObjectiv"));
    flextglGetQueryObjectuiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint *)>(loader.load("glGetQueryObjectuiv"));
    flextglGetQueryiv = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLint *)>(loader.load("glGetQueryiv"));
    flextglIsBuffer = reinterpret_cast<GLboolean(APIENTRY*)(GLuint)>(loader.load("glIsBuffer"));
    flextglIsQuery = reinterpret_cast<GLboolean(APIENTRY*)(GLuint)>(loader.load("glIsQuery"));
    flextglMapBuffer = reinterpret_cast<void *(APIENTRY*)(GLenum, GLenum)>(loader.load("glMapBuffer"));
    flextglUnmapBuffer = reinterpret_cast<GLboolean(APIENTRY*)(GLenum)>(loader.load("glUnmapBuffer"));

    /* GL_VERSION_2_0 */
    flextglAttachShader = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint)>(loader.load("glAttachShader"));
    flextglBindAttribLocation = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, const GLchar *)>(loader.load("glBindAttribLocation"));
    flextglBlendEquationSeparate = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum)>(loader.load("glBlendEquationSeparate"));
    flextglCompileShader = reinterpret_cast<void(APIENTRY*)(GLuint)>(loader.load("glCompileShader"));
    flextglCreateProgram = reinterpret_cast<GLuint(APIENTRY*)(void)>(loader.load("glCreateProgram"));
    flextglCreateShader = reinterpret_cast<GLuint(APIENTRY*)(GLenum)>(loader.load("glCreateShader"));
    flextglDeleteProgram = reinterpret_cast<void(APIENTRY*)(GLuint)>(loader.load("glDeleteProgram"));
    flextglDeleteShader = reinterpret_cast<void(APIENTRY*)(GLuint)>(loader.load("glDeleteShader"));
    flextglDetachShader = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint)>(loader.load("glDetachShader"));
    flextglDisableVertexAttribArray = reinterpret_cast<void(APIENTRY*)(GLuint)>(loader.load("glDisableVertexAttribArray"));
    flextglDrawBuffers = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLenum *)>(loader.load("glDrawBuffers"));
    flextglEnableVertexAttribArray = reinterpret_cast<void(APIENTRY*)(GLuint)>(loader.load("glEnableVertexAttribArray"));
    flextglGetActiveAttrib = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLsizei, GLsizei *, GLint *, GLenum *, GLchar *)>(loader.load("glGetActiveAttrib"));
    flextglGetActiveUniform = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLsizei, GLsizei *, GLint *, GLenum *, GLchar *)>(loader.load("glGetActiveUniform"));
    flextglGetAttachedShaders = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, GLsizei *, GLuint *)>(loader.load("glGetAttachedShaders"));
    flextglGetAttribLocation = reinterpret_cast<GLint(APIENTRY*)(GLuint, const GLchar *)>(loader.load("glGetAttribLocation"));
    flextglGetProgramInfoLog = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, GLsizei *, GLchar *)>(loader.load("glGetProgramInfoLog"));
    flextglGetProgramiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint *)>(loader.load("glGetProgramiv"));
    flextglGetShaderInfoLog = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, GLsizei *, GLchar *)>(loader.load("glGetShaderInfoLog"));
    flextglGetShaderSource = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, GLsizei *, GLchar *)>(loader.load("glGetShaderSource"));
    flextglGetShaderiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint *)>(loader.load("glGetShaderiv"));
    flextglGetUniformLocation = reinterpret_cast<GLint(APIENTRY*)(GLuint, const GLchar *)>(loader.load("glGetUniformLocation"));
    flextglGetUniformfv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLfloat *)>(loader.load("glGetUniformfv"));
    flextglGetUniformiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint *)>(loader.load("glGetUniformiv"));
    flextglGetVertexAttribPointerv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, void **)>(loader.load("glGetVertexAttribPointerv"));
    flextglGetVertexAttribdv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLdouble *)>(loader.load("glGetVertexAttribdv"));
    flextglGetVertexAttribfv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLfloat *)>(loader.load("glGetVertexAttribfv"));
    flextglGetVertexAttribiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint *)>(loader.load("glGetVertexAttribiv"));
    flextglIsProgram = reinterpret_cast<GLboolean(APIENTRY*)(GLuint)>(loader.load("glIsProgram"));
    flextglIsShader = reinterpret_cast<GLboolean(APIENTRY*)(GLuint)>(loader.load("glIsShader"));
    flextglLinkProgram = reinterpret_cast<void(APIENTRY*)(GLuint)>(loader.load("glLinkProgram"));
    flextglShaderSource = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, const GLchar *const*, const GLint *)>(loader.load("glShaderSource"));
    flextglStencilFuncSeparate = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLint, GLuint)>(loader.load("glStencilFuncSeparate"));
    flextglStencilMaskSeparate = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint)>(loader.load("glStencilMaskSeparate"));
    flextglStencilOpSeparate = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLenum, GLenum)>(loader.load("glStencilOpSeparate"));
    flextglUniform1f = reinterpret_cast<void(APIENTRY*)(GLint, GLfloat)>(loader.load("glUniform1f"));
    flextglUniform1fv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, const GLfloat *)>(loader.load("glUniform1fv"));
    flextglUniform1i = reinterpret_cast<void(APIENTRY*)(GLint, GLint)>(loader.load("glUniform1i"));
    flextglUniform1iv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, const GLint *)>(loader.load("glUniform1iv"));
    flextglUniform2f = reinterpret_cast<void(APIENTRY*)(GLint, GLfloat, GLfloat)>(loader.load("glUniform2f"));
    flextglUniform2fv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, const GLfloat *)>(loader.load("glUniform2fv"));
    flextglUniform2i = reinterpret_cast<void(APIENTRY*)(GLint, GLint, GLint)>(loader.load("glUniform2i"));
    flextglUniform2iv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, const GLint *)>(loader.load("glUniform2iv"));
    flextglUniform3f = reinterpret_cast<void(APIENTRY*)(GLint, GLfloat, GLfloat, GLfloat)>(loader.load("glUniform3f"));
    flextglUniform3fv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, const GLfloat *)>(loader.load("glUniform3fv"));
    flextglUniform3i = reinterpret_cast<void(APIENTRY*)(GLint, GLint, GLint, GLint)>(loader.load("glUniform3i"));
    flextglUniform3iv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, const GLint *)>(loader.load("glUniform3iv"));
    flextglUniform4f = reinterpret_cast<void(APIENTRY*)(GLint, GLfloat, GLfloat, GLfloat, GLfloat)>(loader.load("glUniform4f"));
    flextglUniform4fv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, const GLfloat *)>(loader.load("glUniform4fv"));
    flextglUniform4i = reinterpret_cast<void(APIENTRY*)(GLint, GLint, GLint, GLint, GLint)>(loader.load("glUniform4i"));
    flextglUniform4iv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, const GLint *)>(loader.load("glUniform4iv"));
    flextglUniformMatrix2fv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glUniformMatrix2fv"));
    flextglUniformMatrix3fv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glUniformMatrix3fv"));
    flextglUniformMatrix4fv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glUniformMatrix4fv"));
    flextglUseProgram = reinterpret_cast<void(APIENTRY*)(GLuint)>(loader.load("glUseProgram"));
    flextglValidateProgram = reinterpret_cast<void(APIENTRY*)(GLuint)>(loader.load("glValidateProgram"));
    flextglVertexAttrib1d = reinterpret_cast<void(APIENTRY*)(GLuint, GLdouble)>(loader.load("glVertexAttrib1d"));
    flextglVertexAttrib1dv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLdouble *)>(loader.load("glVertexAttrib1dv"));
    flextglVertexAttrib1f = reinterpret_cast<void(APIENTRY*)(GLuint, GLfloat)>(loader.load("glVertexAttrib1f"));
    flextglVertexAttrib1fv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLfloat *)>(loader.load("glVertexAttrib1fv"));
    flextglVertexAttrib1s = reinterpret_cast<void(APIENTRY*)(GLuint, GLshort)>(loader.load("glVertexAttrib1s"));
    flextglVertexAttrib1sv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLshort *)>(loader.load("glVertexAttrib1sv"));
    flextglVertexAttrib2d = reinterpret_cast<void(APIENTRY*)(GLuint, GLdouble, GLdouble)>(loader.load("glVertexAttrib2d"));
    flextglVertexAttrib2dv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLdouble *)>(loader.load("glVertexAttrib2dv"));
    flextglVertexAttrib2f = reinterpret_cast<void(APIENTRY*)(GLuint, GLfloat, GLfloat)>(loader.load("glVertexAttrib2f"));
    flextglVertexAttrib2fv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLfloat *)>(loader.load("glVertexAttrib2fv"));
    flextglVertexAttrib2s = reinterpret_cast<void(APIENTRY*)(GLuint, GLshort, GLshort)>(loader.load("glVertexAttrib2s"));
    flextglVertexAttrib2sv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLshort *)>(loader.load("glVertexAttrib2sv"));
    flextglVertexAttrib3d = reinterpret_cast<void(APIENTRY*)(GLuint, GLdouble, GLdouble, GLdouble)>(loader.load("glVertexAttrib3d"));
    flextglVertexAttrib3dv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLdouble *)>(loader.load("glVertexAttrib3dv"));
    flextglVertexAttrib3f = reinterpret_cast<void(APIENTRY*)(GLuint, GLfloat, GLfloat, GLfloat)>(loader.load("glVertexAttrib3f"));
    flextglVertexAttrib3fv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLfloat *)>(loader.load("glVertexAttrib3fv"));
    flextglVertexAttrib3s = reinterpret_cast<void(APIENTRY*)(GLuint, GLshort, GLshort, GLshort)>(loader.load("glVertexAttrib3s"));
    flextglVertexAttrib3sv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLshort *)>(loader.load("glVertexAttrib3sv"));
    flextglVertexAttrib4Nbv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLbyte *)>(loader.load("glVertexAttrib4Nbv"));
    flextglVertexAttrib4Niv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLint *)>(loader.load("glVertexAttrib4Niv"));
    flextglVertexAttrib4Nsv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLshort *)>(loader.load("glVertexAttrib4Nsv"));
    flextglVertexAttrib4Nub = reinterpret_cast<void(APIENTRY*)(GLuint, GLubyte, GLubyte, GLubyte, GLubyte)>(loader.load("glVertexAttrib4Nub"));
    flextglVertexAttrib4Nubv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLubyte *)>(loader.load("glVertexAttrib4Nubv"));
    flextglVertexAttrib4Nuiv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLuint *)>(loader.load("glVertexAttrib4Nuiv"));
    flextglVertexAttrib4Nusv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLushort *)>(loader.load("glVertexAttrib4Nusv"));
    flextglVertexAttrib4bv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLbyte *)>(loader.load("glVertexAttrib4bv"));
    flextglVertexAttrib4d = reinterpret_cast<void(APIENTRY*)(GLuint, GLdouble, GLdouble, GLdouble, GLdouble)>(loader.load("glVertexAttrib4d"));
    flextglVertexAttrib4dv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLdouble *)>(loader.load("glVertexAttrib4dv"));
    flextglVertexAttrib4f = reinterpret_cast<void(APIENTRY*)(GLuint, GLfloat, GLfloat, GLfloat, GLfloat)>(loader.load("glVertexAttrib4f"));
    flextglVertexAttrib4fv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLfloat *)>(loader.load("glVertexAttrib4fv"));
    flextglVertexAttrib4iv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLint *)>(loader.load("glVertexAttrib4iv"));
    flextglVertexAttrib4s = reinterpret_cast<void(APIENTRY*)(GLuint, GLshort, GLshort, GLshort, GLshort)>(loader.load("glVertexAttrib4s"));
    flextglVertexAttrib4sv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLshort *)>(loader.load("glVertexAttrib4sv"));
    flextglVertexAttrib4ubv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLubyte *)>(loader.load("glVertexAttrib4ubv"));
    flextglVertexAttrib4uiv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLuint *)>(loader.load("glVertexAttrib4uiv"));
    flextglVertexAttrib4usv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLushort *)>(loader.load("glVertexAttrib4usv"));
    flextglVertexAttribPointer = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLenum, GLboolean, GLsizei, const void *)>(loader.load("glVertexAttribPointer"));

    /* GL_VERSION_2_1 */
    flextglUniformMatrix2x3fv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glUniformMatrix2x3fv"));
    flextglUniformMatrix2x4fv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glUniformMatrix2x4fv"));
    flextglUniformMatrix3x2fv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glUniformMatrix3x2fv"));
    flextglUniformMatrix3x4fv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glUniformMatrix3x4fv"));
    flextglUniformMatrix4x2fv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glUniformMatrix4x2fv"));
    flextglUniformMatrix4x3fv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glUniformMatrix4x3fv"));

    /* GL_VERSION_3_0 */
    flextglBeginConditionalRender = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum)>(loader.load("glBeginConditionalRender"));
    flextglBeginTransformFeedback = reinterpret_cast<void(APIENTRY*)(GLenum)>(loader.load("glBeginTransformFeedback"));
    flextglBindBufferBase = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLuint)>(loader.load("glBindBufferBase"));
    flextglBindBufferRange = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLuint, GLintptr, GLsizeiptr)>(loader.load("glBindBufferRange"));
    flextglBindFragDataLocation = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, const GLchar *)>(loader.load("glBindFragDataLocation"));
    flextglBindFramebuffer = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint)>(loader.load("glBindFramebuffer"));
    flextglBindRenderbuffer = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint)>(loader.load("glBindRenderbuffer"));
    flextglBindVertexArray = reinterpret_cast<void(APIENTRY*)(GLuint)>(loader.load("glBindVertexArray"));
    flextglBlitFramebuffer = reinterpret_cast<void(APIENTRY*)(GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLbitfield, GLenum)>(loader.load("glBlitFramebuffer"));
    flextglCheckFramebufferStatus = reinterpret_cast<GLenum(APIENTRY*)(GLenum)>(loader.load("glCheckFramebufferStatus"));
    flextglClampColor = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum)>(loader.load("glClampColor"));
    flextglClearBufferfi = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLfloat, GLint)>(loader.load("glClearBufferfi"));
    flextglClearBufferfv = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, const GLfloat *)>(loader.load("glClearBufferfv"));
    flextglClearBufferiv = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, const GLint *)>(loader.load("glClearBufferiv"));
    flextglClearBufferuiv = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, const GLuint *)>(loader.load("glClearBufferuiv"));
    flextglColorMaski = reinterpret_cast<void(APIENTRY*)(GLuint, GLboolean, GLboolean, GLboolean, GLboolean)>(loader.load("glColorMaski"));
    flextglDeleteFramebuffers = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLuint *)>(loader.load("glDeleteFramebuffers"));
    flextglDeleteRenderbuffers = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLuint *)>(loader.load("glDeleteRenderbuffers"));
    flextglDeleteVertexArrays = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLuint *)>(loader.load("glDeleteVertexArrays"));
    flextglDisablei = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint)>(loader.load("glDisablei"));
    flextglEnablei = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint)>(loader.load("glEnablei"));
    flextglEndConditionalRender = reinterpret_cast<void(APIENTRY*)(void)>(loader.load("glEndConditionalRender"));
    flextglEndTransformFeedback = reinterpret_cast<void(APIENTRY*)(void)>(loader.load("glEndTransformFeedback"));
    flextglFlushMappedBufferRange = reinterpret_cast<void(APIENTRY*)(GLenum, GLintptr, GLsizeiptr)>(loader.load("glFlushMappedBufferRange"));
    flextglFramebufferRenderbuffer = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLenum, GLuint)>(loader.load("glFramebufferRenderbuffer"));
    flextglFramebufferTexture1D = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLenum, GLuint, GLint)>(loader.load("glFramebufferTexture1D"));
    flextglFramebufferTexture2D = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLenum, GLuint, GLint)>(loader.load("glFramebufferTexture2D"));
    flextglFramebufferTexture3D = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLenum, GLuint, GLint, GLint)>(loader.load("glFramebufferTexture3D"));
    flextglFramebufferTextureLayer = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLuint, GLint, GLint)>(loader.load("glFramebufferTextureLayer"));
    flextglGenFramebuffers = reinterpret_cast<void(APIENTRY*)(GLsizei, GLuint *)>(loader.load("glGenFramebuffers"));
    flextglGenRenderbuffers = reinterpret_cast<void(APIENTRY*)(GLsizei, GLuint *)>(loader.load("glGenRenderbuffers"));
    flextglGenVertexArrays = reinterpret_cast<void(APIENTRY*)(GLsizei, GLuint *)>(loader.load("glGenVertexArrays"));
    flextglGenerateMipmap = reinterpret_cast<void(APIENTRY*)(GLenum)>(loader.load("glGenerateMipmap"));
    flextglGetBooleani_v = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLboolean *)>(loader.load("glGetBooleani_v"));
    flextglGetFragDataLocation = reinterpret_cast<GLint(APIENTRY*)(GLuint, const GLchar *)>(loader.load("glGetFragDataLocation"));
    flextglGetFramebufferAttachmentParameteriv = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLenum, GLint *)>(loader.load("glGetFramebufferAttachmentParameteriv"));
    flextglGetIntegeri_v = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLint *)>(loader.load("glGetIntegeri_v"));
    flextglGetRenderbufferParameteriv = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLint *)>(loader.load("glGetRenderbufferParameteriv"));
    flextglGetStringi = reinterpret_cast<const GLubyte *(APIENTRY*)(GLenum, GLuint)>(loader.load("glGetStringi"));
    flextglGetTexParameterIiv = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLint *)>(loader.load("glGetTexParameterIiv"));
    flextglGetTexParameterIuiv = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLuint *)>(loader.load("glGetTexParameterIuiv"));
    flextglGetTransformFeedbackVarying = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLsizei, GLsizei *, GLsizei *, GLenum *, GLchar *)>(loader.load("glGetTransformFeedbackVarying"));
    flextglGetUniformuiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLuint *)>(loader.load("glGetUniformuiv"));
    flextglGetVertexAttribIiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint *)>(loader.load("glGetVertexAttribIiv"));
    flextglGetVertexAttribIuiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint *)>(loader.load("glGetVertexAttribIuiv"));
    flextglIsEnabledi = reinterpret_cast<GLboolean(APIENTRY*)(GLenum, GLuint)>(loader.load("glIsEnabledi"));
    flextglIsFramebuffer = reinterpret_cast<GLboolean(APIENTRY*)(GLuint)>(loader.load("glIsFramebuffer"));
    flextglIsRenderbuffer = reinterpret_cast<GLboolean(APIENTRY*)(GLuint)>(loader.load("glIsRenderbuffer"));
    flextglIsVertexArray = reinterpret_cast<GLboolean(APIENTRY*)(GLuint)>(loader.load("glIsVertexArray"));
    flextglMapBufferRange = reinterpret_cast<void *(APIENTRY*)(GLenum, GLintptr, GLsizeiptr, GLbitfield)>(loader.load("glMapBufferRange"));
    flextglRenderbufferStorage = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLsizei, GLsizei)>(loader.load("glRenderbufferStorage"));
    flextglRenderbufferStorageMultisample = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, GLsizei, GLsizei)>(loader.load("glRenderbufferStorageMultisample"));
    flextglTexParameterIiv = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, const GLint *)>(loader.load("glTexParameterIiv"));
    flextglTexParameterIuiv = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, const GLuint *)>(loader.load("glTexParameterIuiv"));
    flextglTransformFeedbackVaryings = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, const GLchar *const*, GLenum)>(loader.load("glTransformFeedbackVaryings"));
    flextglUniform1ui = reinterpret_cast<void(APIENTRY*)(GLint, GLuint)>(loader.load("glUniform1ui"));
    flextglUniform1uiv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, const GLuint *)>(loader.load("glUniform1uiv"));
    flextglUniform2ui = reinterpret_cast<void(APIENTRY*)(GLint, GLuint, GLuint)>(loader.load("glUniform2ui"));
    flextglUniform2uiv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, const GLuint *)>(loader.load("glUniform2uiv"));
    flextglUniform3ui = reinterpret_cast<void(APIENTRY*)(GLint, GLuint, GLuint, GLuint)>(loader.load("glUniform3ui"));
    flextglUniform3uiv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, const GLuint *)>(loader.load("glUniform3uiv"));
    flextglUniform4ui = reinterpret_cast<void(APIENTRY*)(GLint, GLuint, GLuint, GLuint, GLuint)>(loader.load("glUniform4ui"));
    flextglUniform4uiv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, const GLuint *)>(loader.load("glUniform4uiv"));
    flextglVertexAttribI1i = reinterpret_cast<void(APIENTRY*)(GLuint, GLint)>(loader.load("glVertexAttribI1i"));
    flextglVertexAttribI1iv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLint *)>(loader.load("glVertexAttribI1iv"));
    flextglVertexAttribI1ui = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint)>(loader.load("glVertexAttribI1ui"));
    flextglVertexAttribI1uiv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLuint *)>(loader.load("glVertexAttribI1uiv"));
    flextglVertexAttribI2i = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint)>(loader.load("glVertexAttribI2i"));
    flextglVertexAttribI2iv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLint *)>(loader.load("glVertexAttribI2iv"));
    flextglVertexAttribI2ui = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLuint)>(loader.load("glVertexAttribI2ui"));
    flextglVertexAttribI2uiv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLuint *)>(loader.load("glVertexAttribI2uiv"));
    flextglVertexAttribI3i = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLint)>(loader.load("glVertexAttribI3i"));
    flextglVertexAttribI3iv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLint *)>(loader.load("glVertexAttribI3iv"));
    flextglVertexAttribI3ui = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLuint, GLuint)>(loader.load("glVertexAttribI3ui"));
    flextglVertexAttribI3uiv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLuint *)>(loader.load("glVertexAttribI3uiv"));
    flextglVertexAttribI4bv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLbyte *)>(loader.load("glVertexAttribI4bv"));
    flextglVertexAttribI4i = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLint, GLint)>(loader.load("glVertexAttribI4i"));
    flextglVertexAttribI4iv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLint *)>(loader.load("glVertexAttribI4iv"));
    flextglVertexAttribI4sv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLshort *)>(loader.load("glVertexAttribI4sv"));
    flextglVertexAttribI4ubv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLubyte *)>(loader.load("glVertexAttribI4ubv"));
    flextglVertexAttribI4ui = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLuint, GLuint, GLuint)>(loader.load("glVertexAttribI4ui"));
    flextglVertexAttribI4uiv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLuint *)>(loader.load("glVertexAttribI4uiv"));
    flextglVertexAttribI4usv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLushort *)>(loader.load("glVertexAttribI4usv"));
    flextglVertexAttribIPointer = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLenum, GLsizei, const void *)>(loader.load("glVertexAttribIPointer"));

    /* GL_VERSION_3_1 */
    flextglCopyBufferSubData = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLintptr, GLintptr, GLsizeiptr)>(loader.load("glCopyBufferSubData"));
    flextglDrawArraysInstanced = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLsizei, GLsizei)>(loader.load("glDrawArraysInstanced"));
    flextglDrawElementsInstanced = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, const void *, GLsizei)>(loader.load("glDrawElementsInstanced"));
    flextglGetActiveUniformBlockName = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLsizei, GLsizei *, GLchar *)>(loader.load("glGetActiveUniformBlockName"));
    flextglGetActiveUniformBlockiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLenum, GLint *)>(loader.load("glGetActiveUniformBlockiv"));
    flextglGetActiveUniformName = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLsizei, GLsizei *, GLchar *)>(loader.load("glGetActiveUniformName"));
    flextglGetActiveUniformsiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, const GLuint *, GLenum, GLint *)>(loader.load("glGetActiveUniformsiv"));
    flextglGetUniformBlockIndex = reinterpret_cast<GLuint(APIENTRY*)(GLuint, const GLchar *)>(loader.load("glGetUniformBlockIndex"));
    flextglGetUniformIndices = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, const GLchar *const*, GLuint *)>(loader.load("glGetUniformIndices"));
    flextglPrimitiveRestartIndex = reinterpret_cast<void(APIENTRY*)(GLuint)>(loader.load("glPrimitiveRestartIndex"));
    flextglTexBuffer = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLuint)>(loader.load("glTexBuffer"));
    flextglUniformBlockBinding = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLuint)>(loader.load("glUniformBlockBinding"));

    /* GL_VERSION_3_2 */
    flextglClientWaitSync = reinterpret_cast<GLenum(APIENTRY*)(GLsync, GLbitfield, GLuint64)>(loader.load("glClientWaitSync"));
    flextglDeleteSync = reinterpret_cast<void(APIENTRY*)(GLsync)>(loader.load("glDeleteSync"));
    flextglDrawElementsBaseVertex = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, const void *, GLint)>(loader.load("glDrawElementsBaseVertex"));
    flextglDrawElementsInstancedBaseVertex = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, const void *, GLsizei, GLint)>(loader.load("glDrawElementsInstancedBaseVertex"));
    flextglDrawRangeElementsBaseVertex = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLuint, GLsizei, GLenum, const void *, GLint)>(loader.load("glDrawRangeElementsBaseVertex"));
    flextglFenceSync = reinterpret_cast<GLsync(APIENTRY*)(GLenum, GLbitfield)>(loader.load("glFenceSync"));
    flextglFramebufferTexture = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLuint, GLint)>(loader.load("glFramebufferTexture"));
    flextglGetBufferParameteri64v = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLint64 *)>(loader.load("glGetBufferParameteri64v"));
    flextglGetInteger64i_v = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLint64 *)>(loader.load("glGetInteger64i_v"));
    flextglGetInteger64v = reinterpret_cast<void(APIENTRY*)(GLenum, GLint64 *)>(loader.load("glGetInteger64v"));
    flextglGetMultisamplefv = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLfloat *)>(loader.load("glGetMultisamplefv"));
    flextglGetSynciv = reinterpret_cast<void(APIENTRY*)(GLsync, GLenum, GLsizei, GLsizei *, GLint *)>(loader.load("glGetSynciv"));
    flextglIsSync = reinterpret_cast<GLboolean(APIENTRY*)(GLsync)>(loader.load("glIsSync"));
    flextglMultiDrawElementsBaseVertex = reinterpret_cast<void(APIENTRY*)(GLenum, const GLsizei *, GLenum, const void *const*, GLsizei, const GLint *)>(loader.load("glMultiDrawElementsBaseVertex"));
    flextglProvokingVertex = reinterpret_cast<void(APIENTRY*)(GLenum)>(loader.load("glProvokingVertex"));
    flextglSampleMaski = reinterpret_cast<void(APIENTRY*)(GLuint, GLbitfield)>(loader.load("glSampleMaski"));
    flextglTexImage2DMultisample = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLboolean)>(loader.load("glTexImage2DMultisample"));
    flextglTexImage3DMultisample = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLsizei, GLboolean)>(loader.load("glTexImage3DMultisample"));
    flextglWaitSync = reinterpret_cast<void(APIENTRY*)(GLsync, GLbitfield, GLuint64)>(loader.load("glWaitSync"));

    /* GL_VERSION_3_3 */
    flextglBindFragDataLocationIndexed = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLuint, const GLchar *)>(loader.load("glBindFragDataLocationIndexed"));
    flextglBindSampler = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint)>(loader.load("glBindSampler"));
    flextglDeleteSamplers = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLuint *)>(loader.load("glDeleteSamplers"));
    flextglGenSamplers = reinterpret_cast<void(APIENTRY*)(GLsizei, GLuint *)>(loader.load("glGenSamplers"));
    flextglGetFragDataIndex = reinterpret_cast<GLint(APIENTRY*)(GLuint, const GLchar *)>(loader.load("glGetFragDataIndex"));
    flextglGetQueryObjecti64v = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint64 *)>(loader.load("glGetQueryObjecti64v"));
    flextglGetQueryObjectui64v = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint64 *)>(loader.load("glGetQueryObjectui64v"));
    flextglGetSamplerParameterIiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint *)>(loader.load("glGetSamplerParameterIiv"));
    flextglGetSamplerParameterIuiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint *)>(loader.load("glGetSamplerParameterIuiv"));
    flextglGetSamplerParameterfv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLfloat *)>(loader.load("glGetSamplerParameterfv"));
    flextglGetSamplerParameteriv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint *)>(loader.load("glGetSamplerParameteriv"));
    flextglIsSampler = reinterpret_cast<GLboolean(APIENTRY*)(GLuint)>(loader.load("glIsSampler"));
    flextglQueryCounter = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum)>(loader.load("glQueryCounter"));
    flextglSamplerParameterIiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, const GLint *)>(loader.load("glSamplerParameterIiv"));
    flextglSamplerParameterIuiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, const GLuint *)>(loader.load("glSamplerParameterIuiv"));
    flextglSamplerParameterf = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLfloat)>(loader.load("glSamplerParameterf"));
    flextglSamplerParameterfv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, const GLfloat *)>(loader.load("glSamplerParameterfv"));
    flextglSamplerParameteri = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint)>(loader.load("glSamplerParameteri"));
    flextglSamplerParameteriv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, const GLint *)>(loader.load("glSamplerParameteriv"));
    flextglVertexAttribDivisor = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint)>(loader.load("glVertexAttribDivisor"));
    flextglVertexAttribP1ui = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLboolean, GLuint)>(loader.load("glVertexAttribP1ui"));
    flextglVertexAttribP1uiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLboolean, const GLuint *)>(loader.load("glVertexAttribP1uiv"));
    flextglVertexAttribP2ui = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLboolean, GLuint)>(loader.load("glVertexAttribP2ui"));
    flextglVertexAttribP2uiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLboolean, const GLuint *)>(loader.load("glVertexAttribP2uiv"));
    flextglVertexAttribP3ui = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLboolean, GLuint)>(loader.load("glVertexAttribP3ui"));
    flextglVertexAttribP3uiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLboolean, const GLuint *)>(loader.load("glVertexAttribP3uiv"));
    flextglVertexAttribP4ui = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLboolean, GLuint)>(loader.load("glVertexAttribP4ui"));
    flextglVertexAttribP4uiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLboolean, const GLuint *)>(loader.load("glVertexAttribP4uiv"));

    /* GL_VERSION_4_0 */
    flextglBeginQueryIndexed = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLuint)>(loader.load("glBeginQueryIndexed"));
    flextglBindTransformFeedback = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint)>(loader.load("glBindTransformFeedback"));
    flextglBlendEquationSeparatei = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLenum)>(loader.load("glBlendEquationSeparatei"));
    flextglBlendEquationi = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum)>(loader.load("glBlendEquationi"));
    flextglBlendFuncSeparatei = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLenum, GLenum, GLenum)>(loader.load("glBlendFuncSeparatei"));
    flextglBlendFunci = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLenum)>(loader.load("glBlendFunci"));
    flextglDeleteTransformFeedbacks = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLuint *)>(loader.load("glDeleteTransformFeedbacks"));
    flextglDrawArraysIndirect = reinterpret_cast<void(APIENTRY*)(GLenum, const void *)>(loader.load("glDrawArraysIndirect"));
    flextglDrawElementsIndirect = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, const void *)>(loader.load("glDrawElementsIndirect"));
    flextglDrawTransformFeedback = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint)>(loader.load("glDrawTransformFeedback"));
    flextglDrawTransformFeedbackStream = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLuint)>(loader.load("glDrawTransformFeedbackStream"));
    flextglEndQueryIndexed = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint)>(loader.load("glEndQueryIndexed"));
    flextglGenTransformFeedbacks = reinterpret_cast<void(APIENTRY*)(GLsizei, GLuint *)>(loader.load("glGenTransformFeedbacks"));
    flextglGetActiveSubroutineName = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint, GLsizei, GLsizei *, GLchar *)>(loader.load("glGetActiveSubroutineName"));
    flextglGetActiveSubroutineUniformName = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint, GLsizei, GLsizei *, GLchar *)>(loader.load("glGetActiveSubroutineUniformName"));
    flextglGetActiveSubroutineUniformiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint, GLenum, GLint *)>(loader.load("glGetActiveSubroutineUniformiv"));
    flextglGetProgramStageiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLenum, GLint *)>(loader.load("glGetProgramStageiv"));
    flextglGetQueryIndexediv = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLenum, GLint *)>(loader.load("glGetQueryIndexediv"));
    flextglGetSubroutineIndex = reinterpret_cast<GLuint(APIENTRY*)(GLuint, GLenum, const GLchar *)>(loader.load("glGetSubroutineIndex"));
    flextglGetSubroutineUniformLocation = reinterpret_cast<GLint(APIENTRY*)(GLuint, GLenum, const GLchar *)>(loader.load("glGetSubroutineUniformLocation"));
    flextglGetUniformSubroutineuiv = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLuint *)>(loader.load("glGetUniformSubroutineuiv"));
    flextglGetUniformdv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLdouble *)>(loader.load("glGetUniformdv"));
    flextglIsTransformFeedback = reinterpret_cast<GLboolean(APIENTRY*)(GLuint)>(loader.load("glIsTransformFeedback"));
    flextglMinSampleShading = reinterpret_cast<void(APIENTRY*)(GLfloat)>(loader.load("glMinSampleShading"));
    flextglPatchParameterfv = reinterpret_cast<void(APIENTRY*)(GLenum, const GLfloat *)>(loader.load("glPatchParameterfv"));
    flextglPatchParameteri = reinterpret_cast<void(APIENTRY*)(GLenum, GLint)>(loader.load("glPatchParameteri"));
    flextglPauseTransformFeedback = reinterpret_cast<void(APIENTRY*)(void)>(loader.load("glPauseTransformFeedback"));
    flextglResumeTransformFeedback = reinterpret_cast<void(APIENTRY*)(void)>(loader.load("glResumeTransformFeedback"));
    flextglUniform1d = reinterpret_cast<void(APIENTRY*)(GLint, GLdouble)>(loader.load("glUniform1d"));
    flextglUniform1dv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, const GLdouble *)>(loader.load("glUniform1dv"));
    flextglUniform2d = reinterpret_cast<void(APIENTRY*)(GLint, GLdouble, GLdouble)>(loader.load("glUniform2d"));
    flextglUniform2dv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, const GLdouble *)>(loader.load("glUniform2dv"));
    flextglUniform3d = reinterpret_cast<void(APIENTRY*)(GLint, GLdouble, GLdouble, GLdouble)>(loader.load("glUniform3d"));
    flextglUniform3dv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, const GLdouble *)>(loader.load("glUniform3dv"));
    flextglUniform4d = reinterpret_cast<void(APIENTRY*)(GLint, GLdouble, GLdouble, GLdouble, GLdouble)>(loader.load("glUniform4d"));
    flextglUniform4dv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, const GLdouble *)>(loader.load("glUniform4dv"));
    flextglUniformMatrix2dv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, GLboolean, const GLdouble *)>(loader.load("glUniformMatrix2dv"));
    flextglUniformMatrix2x3dv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, GLboolean, const GLdouble *)>(loader.load("glUniformMatrix2x3dv"));
    flextglUniformMatrix2x4dv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, GLboolean, const GLdouble *)>(loader.load("glUniformMatrix2x4dv"));
    flextglUniformMatrix3dv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, GLboolean, const GLdouble *)>(loader.load("glUniformMatrix3dv"));
    flextglUniformMatrix3x2dv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, GLboolean, const GLdouble *)>(loader.load("glUniformMatrix3x2dv"));
    flextglUniformMatrix3x4dv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, GLboolean, const GLdouble *)>(loader.load("glUniformMatrix3x4dv"));
    flextglUniformMatrix4dv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, GLboolean, const GLdouble *)>(loader.load("glUniformMatrix4dv"));
    flextglUniformMatrix4x2dv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, GLboolean, const GLdouble *)>(loader.load("glUniformMatrix4x2dv"));
    flextglUniformMatrix4x3dv = reinterpret_cast<void(APIENTRY*)(GLint, GLsizei, GLboolean, const GLdouble *)>(loader.load("glUniformMatrix4x3dv"));
    flextglUniformSubroutinesuiv = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, const GLuint *)>(loader.load("glUniformSubroutinesuiv"));

    /* GL_VERSION_4_1 */
    flextglActiveShaderProgram = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint)>(loader.load("glActiveShaderProgram"));
    flextglBindProgramPipeline = reinterpret_cast<void(APIENTRY*)(GLuint)>(loader.load("glBindProgramPipeline"));
    flextglClearDepthf = reinterpret_cast<void(APIENTRY*)(GLfloat)>(loader.load("glClearDepthf"));
    flextglCreateShaderProgramv = reinterpret_cast<GLuint(APIENTRY*)(GLenum, GLsizei, const GLchar *const*)>(loader.load("glCreateShaderProgramv"));
    flextglDeleteProgramPipelines = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLuint *)>(loader.load("glDeleteProgramPipelines"));
    flextglDepthRangeArrayv = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, const GLdouble *)>(loader.load("glDepthRangeArrayv"));
    flextglDepthRangeIndexed = reinterpret_cast<void(APIENTRY*)(GLuint, GLdouble, GLdouble)>(loader.load("glDepthRangeIndexed"));
    flextglDepthRangef = reinterpret_cast<void(APIENTRY*)(GLfloat, GLfloat)>(loader.load("glDepthRangef"));
    flextglGenProgramPipelines = reinterpret_cast<void(APIENTRY*)(GLsizei, GLuint *)>(loader.load("glGenProgramPipelines"));
    flextglGetDoublei_v = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLdouble *)>(loader.load("glGetDoublei_v"));
    flextglGetFloati_v = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLfloat *)>(loader.load("glGetFloati_v"));
    flextglGetProgramBinary = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, GLsizei *, GLenum *, void *)>(loader.load("glGetProgramBinary"));
    flextglGetProgramPipelineInfoLog = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, GLsizei *, GLchar *)>(loader.load("glGetProgramPipelineInfoLog"));
    flextglGetProgramPipelineiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint *)>(loader.load("glGetProgramPipelineiv"));
    flextglGetShaderPrecisionFormat = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLint *, GLint *)>(loader.load("glGetShaderPrecisionFormat"));
    flextglGetVertexAttribLdv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLdouble *)>(loader.load("glGetVertexAttribLdv"));
    flextglIsProgramPipeline = reinterpret_cast<GLboolean(APIENTRY*)(GLuint)>(loader.load("glIsProgramPipeline"));
    flextglProgramBinary = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, const void *, GLsizei)>(loader.load("glProgramBinary"));
    flextglProgramParameteri = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint)>(loader.load("glProgramParameteri"));
    flextglProgramUniform1d = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLdouble)>(loader.load("glProgramUniform1d"));
    flextglProgramUniform1dv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLdouble *)>(loader.load("glProgramUniform1dv"));
    flextglProgramUniform1f = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLfloat)>(loader.load("glProgramUniform1f"));
    flextglProgramUniform1fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLfloat *)>(loader.load("glProgramUniform1fv"));
    flextglProgramUniform1i = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint)>(loader.load("glProgramUniform1i"));
    flextglProgramUniform1iv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLint *)>(loader.load("glProgramUniform1iv"));
    flextglProgramUniform1ui = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLuint)>(loader.load("glProgramUniform1ui"));
    flextglProgramUniform1uiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLuint *)>(loader.load("glProgramUniform1uiv"));
    flextglProgramUniform2d = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLdouble, GLdouble)>(loader.load("glProgramUniform2d"));
    flextglProgramUniform2dv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLdouble *)>(loader.load("glProgramUniform2dv"));
    flextglProgramUniform2f = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLfloat, GLfloat)>(loader.load("glProgramUniform2f"));
    flextglProgramUniform2fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLfloat *)>(loader.load("glProgramUniform2fv"));
    flextglProgramUniform2i = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLint)>(loader.load("glProgramUniform2i"));
    flextglProgramUniform2iv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLint *)>(loader.load("glProgramUniform2iv"));
    flextglProgramUniform2ui = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLuint, GLuint)>(loader.load("glProgramUniform2ui"));
    flextglProgramUniform2uiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLuint *)>(loader.load("glProgramUniform2uiv"));
    flextglProgramUniform3d = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLdouble, GLdouble, GLdouble)>(loader.load("glProgramUniform3d"));
    flextglProgramUniform3dv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLdouble *)>(loader.load("glProgramUniform3dv"));
    flextglProgramUniform3f = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLfloat, GLfloat, GLfloat)>(loader.load("glProgramUniform3f"));
    flextglProgramUniform3fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLfloat *)>(loader.load("glProgramUniform3fv"));
    flextglProgramUniform3i = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLint, GLint)>(loader.load("glProgramUniform3i"));
    flextglProgramUniform3iv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLint *)>(loader.load("glProgramUniform3iv"));
    flextglProgramUniform3ui = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLuint, GLuint, GLuint)>(loader.load("glProgramUniform3ui"));
    flextglProgramUniform3uiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLuint *)>(loader.load("glProgramUniform3uiv"));
    flextglProgramUniform4d = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLdouble, GLdouble, GLdouble, GLdouble)>(loader.load("glProgramUniform4d"));
    flextglProgramUniform4dv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLdouble *)>(loader.load("glProgramUniform4dv"));
    flextglProgramUniform4f = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLfloat, GLfloat, GLfloat, GLfloat)>(loader.load("glProgramUniform4f"));
    flextglProgramUniform4fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLfloat *)>(loader.load("glProgramUniform4fv"));
    flextglProgramUniform4i = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLint, GLint, GLint)>(loader.load("glProgramUniform4i"));
    flextglProgramUniform4iv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLint *)>(loader.load("glProgramUniform4iv"));
    flextglProgramUniform4ui = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLuint, GLuint, GLuint, GLuint)>(loader.load("glProgramUniform4ui"));
    flextglProgramUniform4uiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, const GLuint *)>(loader.load("glProgramUniform4uiv"));
    flextglProgramUniformMatrix2dv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLdouble *)>(loader.load("glProgramUniformMatrix2dv"));
    flextglProgramUniformMatrix2fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix2fv"));
    flextglProgramUniformMatrix2x3dv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLdouble *)>(loader.load("glProgramUniformMatrix2x3dv"));
    flextglProgramUniformMatrix2x3fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix2x3fv"));
    flextglProgramUniformMatrix2x4dv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLdouble *)>(loader.load("glProgramUniformMatrix2x4dv"));
    flextglProgramUniformMatrix2x4fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix2x4fv"));
    flextglProgramUniformMatrix3dv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLdouble *)>(loader.load("glProgramUniformMatrix3dv"));
    flextglProgramUniformMatrix3fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix3fv"));
    flextglProgramUniformMatrix3x2dv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLdouble *)>(loader.load("glProgramUniformMatrix3x2dv"));
    flextglProgramUniformMatrix3x2fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix3x2fv"));
    flextglProgramUniformMatrix3x4dv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLdouble *)>(loader.load("glProgramUniformMatrix3x4dv"));
    flextglProgramUniformMatrix3x4fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix3x4fv"));
    flextglProgramUniformMatrix4dv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLdouble *)>(loader.load("glProgramUniformMatrix4dv"));
    flextglProgramUniformMatrix4fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix4fv"));
    flextglProgramUniformMatrix4x2dv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLdouble *)>(loader.load("glProgramUniformMatrix4x2dv"));
    flextglProgramUniformMatrix4x2fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix4x2fv"));
    flextglProgramUniformMatrix4x3dv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLdouble *)>(loader.load("glProgramUniformMatrix4x3dv"));
    flextglProgramUniformMatrix4x3fv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLboolean, const GLfloat *)>(loader.load("glProgramUniformMatrix4x3fv"));
    flextglReleaseShaderCompiler = reinterpret_cast<void(APIENTRY*)(void)>(loader.load("glReleaseShaderCompiler"));
    flextglScissorArrayv = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, const GLint *)>(loader.load("glScissorArrayv"));
    flextglScissorIndexed = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLsizei, GLsizei)>(loader.load("glScissorIndexed"));
    flextglScissorIndexedv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLint *)>(loader.load("glScissorIndexedv"));
    flextglShaderBinary = reinterpret_cast<void(APIENTRY*)(GLsizei, const GLuint *, GLenum, const void *, GLsizei)>(loader.load("glShaderBinary"));
    flextglUseProgramStages = reinterpret_cast<void(APIENTRY*)(GLuint, GLbitfield, GLuint)>(loader.load("glUseProgramStages"));
    flextglValidateProgramPipeline = reinterpret_cast<void(APIENTRY*)(GLuint)>(loader.load("glValidateProgramPipeline"));
    flextglVertexAttribL1d = reinterpret_cast<void(APIENTRY*)(GLuint, GLdouble)>(loader.load("glVertexAttribL1d"));
    flextglVertexAttribL1dv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLdouble *)>(loader.load("glVertexAttribL1dv"));
    flextglVertexAttribL2d = reinterpret_cast<void(APIENTRY*)(GLuint, GLdouble, GLdouble)>(loader.load("glVertexAttribL2d"));
    flextglVertexAttribL2dv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLdouble *)>(loader.load("glVertexAttribL2dv"));
    flextglVertexAttribL3d = reinterpret_cast<void(APIENTRY*)(GLuint, GLdouble, GLdouble, GLdouble)>(loader.load("glVertexAttribL3d"));
    flextglVertexAttribL3dv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLdouble *)>(loader.load("glVertexAttribL3dv"));
    flextglVertexAttribL4d = reinterpret_cast<void(APIENTRY*)(GLuint, GLdouble, GLdouble, GLdouble, GLdouble)>(loader.load("glVertexAttribL4d"));
    flextglVertexAttribL4dv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLdouble *)>(loader.load("glVertexAttribL4dv"));
    flextglVertexAttribLPointer = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLenum, GLsizei, const void *)>(loader.load("glVertexAttribLPointer"));
    flextglViewportArrayv = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, const GLfloat *)>(loader.load("glViewportArrayv"));
    flextglViewportIndexedf = reinterpret_cast<void(APIENTRY*)(GLuint, GLfloat, GLfloat, GLfloat, GLfloat)>(loader.load("glViewportIndexedf"));
    flextglViewportIndexedfv = reinterpret_cast<void(APIENTRY*)(GLuint, const GLfloat *)>(loader.load("glViewportIndexedfv"));

    /* GL_VERSION_4_2 */
    flextglBindImageTexture = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLint, GLboolean, GLint, GLenum, GLenum)>(loader.load("glBindImageTexture"));
    flextglDrawArraysInstancedBaseInstance = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLsizei, GLsizei, GLuint)>(loader.load("glDrawArraysInstancedBaseInstance"));
    flextglDrawElementsInstancedBaseInstance = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, const void *, GLsizei, GLuint)>(loader.load("glDrawElementsInstancedBaseInstance"));
    flextglDrawElementsInstancedBaseVertexBaseInstance = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, const void *, GLsizei, GLint, GLuint)>(loader.load("glDrawElementsInstancedBaseVertexBaseInstance"));
    flextglDrawTransformFeedbackInstanced = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLsizei)>(loader.load("glDrawTransformFeedbackInstanced"));
    flextglDrawTransformFeedbackStreamInstanced = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLuint, GLsizei)>(loader.load("glDrawTransformFeedbackStreamInstanced"));
    flextglGetActiveAtomicCounterBufferiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLenum, GLint *)>(loader.load("glGetActiveAtomicCounterBufferiv"));
    flextglGetInternalformativ = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLenum, GLsizei, GLint *)>(loader.load("glGetInternalformativ"));
    flextglMemoryBarrier = reinterpret_cast<void(APIENTRY*)(GLbitfield)>(loader.load("glMemoryBarrier"));
    flextglTexStorage1D = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, GLsizei)>(loader.load("glTexStorage1D"));
    flextglTexStorage2D = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, GLsizei, GLsizei)>(loader.load("glTexStorage2D"));
    flextglTexStorage3D = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLsizei)>(loader.load("glTexStorage3D"));

    /* GL_VERSION_4_3 */
    flextglBindVertexBuffer = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLintptr, GLsizei)>(loader.load("glBindVertexBuffer"));
    flextglClearBufferData = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLenum, GLenum, const void *)>(loader.load("glClearBufferData"));
    flextglClearBufferSubData = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLintptr, GLsizeiptr, GLenum, GLenum, const void *)>(loader.load("glClearBufferSubData"));
    flextglCopyImageSubData = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint, GLint, GLint, GLint, GLuint, GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei)>(loader.load("glCopyImageSubData"));
    flextglDebugMessageCallback = reinterpret_cast<void(APIENTRY*)(GLDEBUGPROC, const void *)>(loader.load("glDebugMessageCallback"));
    flextglDebugMessageControl = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLenum, GLsizei, const GLuint *, GLboolean)>(loader.load("glDebugMessageControl"));
    flextglDebugMessageInsert = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar *)>(loader.load("glDebugMessageInsert"));
    flextglDispatchCompute = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLuint)>(loader.load("glDispatchCompute"));
    flextglDispatchComputeIndirect = reinterpret_cast<void(APIENTRY*)(GLintptr)>(loader.load("glDispatchComputeIndirect"));
    flextglFramebufferParameteri = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLint)>(loader.load("glFramebufferParameteri"));
    flextglGetDebugMessageLog = reinterpret_cast<GLuint(APIENTRY*)(GLuint, GLsizei, GLenum *, GLenum *, GLuint *, GLenum *, GLsizei *, GLchar *)>(loader.load("glGetDebugMessageLog"));
    flextglGetFramebufferParameteriv = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLint *)>(loader.load("glGetFramebufferParameteriv"));
    flextglGetInternalformati64v = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLenum, GLsizei, GLint64 *)>(loader.load("glGetInternalformati64v"));
    flextglGetObjectLabel = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLsizei, GLsizei *, GLchar *)>(loader.load("glGetObjectLabel"));
    flextglGetObjectPtrLabel = reinterpret_cast<void(APIENTRY*)(const void *, GLsizei, GLsizei *, GLchar *)>(loader.load("glGetObjectPtrLabel"));
    flextglGetPointerv = reinterpret_cast<void(APIENTRY*)(GLenum, void **)>(loader.load("glGetPointerv"));
    flextglGetProgramInterfaceiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLenum, GLint *)>(loader.load("glGetProgramInterfaceiv"));
    flextglGetProgramResourceIndex = reinterpret_cast<GLuint(APIENTRY*)(GLuint, GLenum, const GLchar *)>(loader.load("glGetProgramResourceIndex"));
    flextglGetProgramResourceLocation = reinterpret_cast<GLint(APIENTRY*)(GLuint, GLenum, const GLchar *)>(loader.load("glGetProgramResourceLocation"));
    flextglGetProgramResourceLocationIndex = reinterpret_cast<GLint(APIENTRY*)(GLuint, GLenum, const GLchar *)>(loader.load("glGetProgramResourceLocationIndex"));
    flextglGetProgramResourceName = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint, GLsizei, GLsizei *, GLchar *)>(loader.load("glGetProgramResourceName"));
    flextglGetProgramResourceiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint, GLsizei, const GLenum *, GLsizei, GLsizei *, GLint *)>(loader.load("glGetProgramResourceiv"));
    flextglInvalidateBufferData = reinterpret_cast<void(APIENTRY*)(GLuint)>(loader.load("glInvalidateBufferData"));
    flextglInvalidateBufferSubData = reinterpret_cast<void(APIENTRY*)(GLuint, GLintptr, GLsizeiptr)>(loader.load("glInvalidateBufferSubData"));
    flextglInvalidateFramebuffer = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, const GLenum *)>(loader.load("glInvalidateFramebuffer"));
    flextglInvalidateSubFramebuffer = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, const GLenum *, GLint, GLint, GLsizei, GLsizei)>(loader.load("glInvalidateSubFramebuffer"));
    flextglInvalidateTexImage = reinterpret_cast<void(APIENTRY*)(GLuint, GLint)>(loader.load("glInvalidateTexImage"));
    flextglInvalidateTexSubImage = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei)>(loader.load("glInvalidateTexSubImage"));
    flextglMultiDrawArraysIndirect = reinterpret_cast<void(APIENTRY*)(GLenum, const void *, GLsizei, GLsizei)>(loader.load("glMultiDrawArraysIndirect"));
    flextglMultiDrawElementsIndirect = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, const void *, GLsizei, GLsizei)>(loader.load("glMultiDrawElementsIndirect"));
    flextglObjectLabel = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLsizei, const GLchar *)>(loader.load("glObjectLabel"));
    flextglObjectPtrLabel = reinterpret_cast<void(APIENTRY*)(const void *, GLsizei, const GLchar *)>(loader.load("glObjectPtrLabel"));
    flextglPopDebugGroup = reinterpret_cast<void(APIENTRY*)(void)>(loader.load("glPopDebugGroup"));
    flextglPushDebugGroup = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLsizei, const GLchar *)>(loader.load("glPushDebugGroup"));
    flextglShaderStorageBlockBinding = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLuint)>(loader.load("glShaderStorageBlockBinding"));
    flextglTexBufferRange = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, GLuint, GLintptr, GLsizeiptr)>(loader.load("glTexBufferRange"));
    flextglTexStorage2DMultisample = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLboolean)>(loader.load("glTexStorage2DMultisample"));
    flextglTexStorage3DMultisample = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLsizei, GLboolean)>(loader.load("glTexStorage3DMultisample"));
    flextglTextureView = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint, GLenum, GLuint, GLuint, GLuint, GLuint)>(loader.load("glTextureView"));
    flextglVertexAttribBinding = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint)>(loader.load("glVertexAttribBinding"));
    flextglVertexAttribFormat = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLenum, GLboolean, GLuint)>(loader.load("glVertexAttribFormat"));
    flextglVertexAttribIFormat = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLenum, GLuint)>(loader.load("glVertexAttribIFormat"));
    flextglVertexAttribLFormat = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLenum, GLuint)>(loader.load("glVertexAttribLFormat"));
    flextglVertexBindingDivisor = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint)>(loader.load("glVertexBindingDivisor"));

    /* GL_VERSION_4_4 */
    flextglBindBuffersBase = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLsizei, const GLuint *)>(loader.load("glBindBuffersBase"));
    flextglBindBuffersRange = reinterpret_cast<void(APIENTRY*)(GLenum, GLuint, GLsizei, const GLuint *, const GLintptr *, const GLsizeiptr *)>(loader.load("glBindBuffersRange"));
    flextglBindImageTextures = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, const GLuint *)>(loader.load("glBindImageTextures"));
    flextglBindSamplers = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, const GLuint *)>(loader.load("glBindSamplers"));
    flextglBindTextures = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, const GLuint *)>(loader.load("glBindTextures"));
    flextglBindVertexBuffers = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, const GLuint *, const GLintptr *, const GLsizei *)>(loader.load("glBindVertexBuffers"));
    flextglBufferStorage = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizeiptr, const void *, GLbitfield)>(loader.load("glBufferStorage"));
    flextglClearTexImage = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLenum, GLenum, const void *)>(loader.load("glClearTexImage"));
    flextglClearTexSubImage = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, const void *)>(loader.load("glClearTexSubImage"));

    /* GL_VERSION_4_5 */
    flextglBindTextureUnit = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint)>(loader.load("glBindTextureUnit"));
    flextglBlitNamedFramebuffer = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLbitfield, GLenum)>(loader.load("glBlitNamedFramebuffer"));
    flextglCheckNamedFramebufferStatus = reinterpret_cast<GLenum(APIENTRY*)(GLuint, GLenum)>(loader.load("glCheckNamedFramebufferStatus"));
    flextglClearNamedBufferData = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLenum, GLenum, const void *)>(loader.load("glClearNamedBufferData"));
    flextglClearNamedBufferSubData = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLintptr, GLsizeiptr, GLenum, GLenum, const void *)>(loader.load("glClearNamedBufferSubData"));
    flextglClearNamedFramebufferfi = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint, GLfloat, GLint)>(loader.load("glClearNamedFramebufferfi"));
    flextglClearNamedFramebufferfv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint, const GLfloat *)>(loader.load("glClearNamedFramebufferfv"));
    flextglClearNamedFramebufferiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint, const GLint *)>(loader.load("glClearNamedFramebufferiv"));
    flextglClearNamedFramebufferuiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint, const GLuint *)>(loader.load("glClearNamedFramebufferuiv"));
    flextglClipControl = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum)>(loader.load("glClipControl"));
    flextglCompressedTextureSubImage1D = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLsizei, GLenum, GLsizei, const void *)>(loader.load("glCompressedTextureSubImage1D"));
    flextglCompressedTextureSubImage2D = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLsizei, const void *)>(loader.load("glCompressedTextureSubImage2D"));
    flextglCompressedTextureSubImage3D = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLsizei, const void *)>(loader.load("glCompressedTextureSubImage3D"));
    flextglCopyNamedBufferSubData = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLintptr, GLintptr, GLsizeiptr)>(loader.load("glCopyNamedBufferSubData"));
    flextglCopyTextureSubImage1D = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLint, GLint, GLsizei)>(loader.load("glCopyTextureSubImage1D"));
    flextglCopyTextureSubImage2D = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei)>(loader.load("glCopyTextureSubImage2D"));
    flextglCopyTextureSubImage3D = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei)>(loader.load("glCopyTextureSubImage3D"));
    flextglCreateBuffers = reinterpret_cast<void(APIENTRY*)(GLsizei, GLuint *)>(loader.load("glCreateBuffers"));
    flextglCreateFramebuffers = reinterpret_cast<void(APIENTRY*)(GLsizei, GLuint *)>(loader.load("glCreateFramebuffers"));
    flextglCreateProgramPipelines = reinterpret_cast<void(APIENTRY*)(GLsizei, GLuint *)>(loader.load("glCreateProgramPipelines"));
    flextglCreateQueries = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLuint *)>(loader.load("glCreateQueries"));
    flextglCreateRenderbuffers = reinterpret_cast<void(APIENTRY*)(GLsizei, GLuint *)>(loader.load("glCreateRenderbuffers"));
    flextglCreateSamplers = reinterpret_cast<void(APIENTRY*)(GLsizei, GLuint *)>(loader.load("glCreateSamplers"));
    flextglCreateTextures = reinterpret_cast<void(APIENTRY*)(GLenum, GLsizei, GLuint *)>(loader.load("glCreateTextures"));
    flextglCreateTransformFeedbacks = reinterpret_cast<void(APIENTRY*)(GLsizei, GLuint *)>(loader.load("glCreateTransformFeedbacks"));
    flextglCreateVertexArrays = reinterpret_cast<void(APIENTRY*)(GLsizei, GLuint *)>(loader.load("glCreateVertexArrays"));
    flextglDisableVertexArrayAttrib = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint)>(loader.load("glDisableVertexArrayAttrib"));
    flextglEnableVertexArrayAttrib = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint)>(loader.load("glEnableVertexArrayAttrib"));
    flextglFlushMappedNamedBufferRange = reinterpret_cast<void(APIENTRY*)(GLuint, GLintptr, GLsizeiptr)>(loader.load("glFlushMappedNamedBufferRange"));
    flextglGenerateTextureMipmap = reinterpret_cast<void(APIENTRY*)(GLuint)>(loader.load("glGenerateTextureMipmap"));
    flextglGetCompressedTextureImage = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, void *)>(loader.load("glGetCompressedTextureImage"));
    flextglGetCompressedTextureSubImage = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLsizei, void *)>(loader.load("glGetCompressedTextureSubImage"));
    flextglGetGraphicsResetStatus = reinterpret_cast<GLenum(APIENTRY*)(void)>(loader.load("glGetGraphicsResetStatus"));
    flextglGetNamedBufferParameteri64v = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint64 *)>(loader.load("glGetNamedBufferParameteri64v"));
    flextglGetNamedBufferParameteriv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint *)>(loader.load("glGetNamedBufferParameteriv"));
    flextglGetNamedBufferPointerv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, void **)>(loader.load("glGetNamedBufferPointerv"));
    flextglGetNamedBufferSubData = reinterpret_cast<void(APIENTRY*)(GLuint, GLintptr, GLsizeiptr, void *)>(loader.load("glGetNamedBufferSubData"));
    flextglGetNamedFramebufferAttachmentParameteriv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLenum, GLint *)>(loader.load("glGetNamedFramebufferAttachmentParameteriv"));
    flextglGetNamedFramebufferParameteriv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint *)>(loader.load("glGetNamedFramebufferParameteriv"));
    flextglGetNamedRenderbufferParameteriv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint *)>(loader.load("glGetNamedRenderbufferParameteriv"));
    flextglGetQueryBufferObjecti64v = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLenum, GLintptr)>(loader.load("glGetQueryBufferObjecti64v"));
    flextglGetQueryBufferObjectiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLenum, GLintptr)>(loader.load("glGetQueryBufferObjectiv"));
    flextglGetQueryBufferObjectui64v = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLenum, GLintptr)>(loader.load("glGetQueryBufferObjectui64v"));
    flextglGetQueryBufferObjectuiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLenum, GLintptr)>(loader.load("glGetQueryBufferObjectuiv"));
    flextglGetTextureImage = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLenum, GLenum, GLsizei, void *)>(loader.load("glGetTextureImage"));
    flextglGetTextureLevelParameterfv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLenum, GLfloat *)>(loader.load("glGetTextureLevelParameterfv"));
    flextglGetTextureLevelParameteriv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLenum, GLint *)>(loader.load("glGetTextureLevelParameteriv"));
    flextglGetTextureParameterIiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint *)>(loader.load("glGetTextureParameterIiv"));
    flextglGetTextureParameterIuiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint *)>(loader.load("glGetTextureParameterIuiv"));
    flextglGetTextureParameterfv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLfloat *)>(loader.load("glGetTextureParameterfv"));
    flextglGetTextureParameteriv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint *)>(loader.load("glGetTextureParameteriv"));
    flextglGetTextureSubImage = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, GLsizei, void *)>(loader.load("glGetTextureSubImage"));
    flextglGetTransformFeedbacki64_v = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint, GLint64 *)>(loader.load("glGetTransformFeedbacki64_v"));
    flextglGetTransformFeedbacki_v = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint, GLint *)>(loader.load("glGetTransformFeedbacki_v"));
    flextglGetTransformFeedbackiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint *)>(loader.load("glGetTransformFeedbackiv"));
    flextglGetVertexArrayIndexed64iv = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLenum, GLint64 *)>(loader.load("glGetVertexArrayIndexed64iv"));
    flextglGetVertexArrayIndexediv = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLenum, GLint *)>(loader.load("glGetVertexArrayIndexediv"));
    flextglGetVertexArrayiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint *)>(loader.load("glGetVertexArrayiv"));
    flextglGetnCompressedTexImage = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLsizei, void *)>(loader.load("glGetnCompressedTexImage"));
    flextglGetnTexImage = reinterpret_cast<void(APIENTRY*)(GLenum, GLint, GLenum, GLenum, GLsizei, void *)>(loader.load("glGetnTexImage"));
    flextglGetnUniformdv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLdouble *)>(loader.load("glGetnUniformdv"));
    flextglGetnUniformfv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLfloat *)>(loader.load("glGetnUniformfv"));
    flextglGetnUniformiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLint *)>(loader.load("glGetnUniformiv"));
    flextglGetnUniformuiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLsizei, GLuint *)>(loader.load("glGetnUniformuiv"));
    flextglInvalidateNamedFramebufferData = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, const GLenum *)>(loader.load("glInvalidateNamedFramebufferData"));
    flextglInvalidateNamedFramebufferSubData = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, const GLenum *, GLint, GLint, GLsizei, GLsizei)>(loader.load("glInvalidateNamedFramebufferSubData"));
    flextglMapNamedBuffer = reinterpret_cast<void *(APIENTRY*)(GLuint, GLenum)>(loader.load("glMapNamedBuffer"));
    flextglMapNamedBufferRange = reinterpret_cast<void *(APIENTRY*)(GLuint, GLintptr, GLsizeiptr, GLbitfield)>(loader.load("glMapNamedBufferRange"));
    flextglMemoryBarrierByRegion = reinterpret_cast<void(APIENTRY*)(GLbitfield)>(loader.load("glMemoryBarrierByRegion"));
    flextglNamedBufferData = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizeiptr, const void *, GLenum)>(loader.load("glNamedBufferData"));
    flextglNamedBufferStorage = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizeiptr, const void *, GLbitfield)>(loader.load("glNamedBufferStorage"));
    flextglNamedBufferSubData = reinterpret_cast<void(APIENTRY*)(GLuint, GLintptr, GLsizeiptr, const void *)>(loader.load("glNamedBufferSubData"));
    flextglNamedFramebufferDrawBuffer = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum)>(loader.load("glNamedFramebufferDrawBuffer"));
    flextglNamedFramebufferDrawBuffers = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, const GLenum *)>(loader.load("glNamedFramebufferDrawBuffers"));
    flextglNamedFramebufferParameteri = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint)>(loader.load("glNamedFramebufferParameteri"));
    flextglNamedFramebufferReadBuffer = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum)>(loader.load("glNamedFramebufferReadBuffer"));
    flextglNamedFramebufferRenderbuffer = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLenum, GLuint)>(loader.load("glNamedFramebufferRenderbuffer"));
    flextglNamedFramebufferTexture = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint, GLint)>(loader.load("glNamedFramebufferTexture"));
    flextglNamedFramebufferTextureLayer = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint, GLint, GLint)>(loader.load("glNamedFramebufferTextureLayer"));
    flextglNamedRenderbufferStorage = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLsizei, GLsizei)>(loader.load("glNamedRenderbufferStorage"));
    flextglNamedRenderbufferStorageMultisample = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, GLenum, GLsizei, GLsizei)>(loader.load("glNamedRenderbufferStorageMultisample"));
    flextglReadnPixels = reinterpret_cast<void(APIENTRY*)(GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, GLsizei, void *)>(loader.load("glReadnPixels"));
    flextglTextureBarrier = reinterpret_cast<void(APIENTRY*)(void)>(loader.load("glTextureBarrier"));
    flextglTextureBuffer = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint)>(loader.load("glTextureBuffer"));
    flextglTextureBufferRange = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLuint, GLintptr, GLsizeiptr)>(loader.load("glTextureBufferRange"));
    flextglTextureParameterIiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, const GLint *)>(loader.load("glTextureParameterIiv"));
    flextglTextureParameterIuiv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, const GLuint *)>(loader.load("glTextureParameterIuiv"));
    flextglTextureParameterf = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLfloat)>(loader.load("glTextureParameterf"));
    flextglTextureParameterfv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, const GLfloat *)>(loader.load("glTextureParameterfv"));
    flextglTextureParameteri = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, GLint)>(loader.load("glTextureParameteri"));
    flextglTextureParameteriv = reinterpret_cast<void(APIENTRY*)(GLuint, GLenum, const GLint *)>(loader.load("glTextureParameteriv"));
    flextglTextureStorage1D = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, GLenum, GLsizei)>(loader.load("glTextureStorage1D"));
    flextglTextureStorage2D = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, GLenum, GLsizei, GLsizei)>(loader.load("glTextureStorage2D"));
    flextglTextureStorage2DMultisample = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, GLenum, GLsizei, GLsizei, GLboolean)>(loader.load("glTextureStorage2DMultisample"));
    flextglTextureStorage3D = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, GLenum, GLsizei, GLsizei, GLsizei)>(loader.load("glTextureStorage3D"));
    flextglTextureStorage3DMultisample = reinterpret_cast<void(APIENTRY*)(GLuint, GLsizei, GLenum, GLsizei, GLsizei, GLsizei, GLboolean)>(loader.load("glTextureStorage3DMultisample"));
    flextglTextureSubImage1D = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLsizei, GLenum, GLenum, const void *)>(loader.load("glTextureSubImage1D"));
    flextglTextureSubImage2D = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const void *)>(loader.load("glTextureSubImage2D"));
    flextglTextureSubImage3D = reinterpret_cast<void(APIENTRY*)(GLuint, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, const void *)>(loader.load("glTextureSubImage3D"));
    flextglTransformFeedbackBufferBase = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLuint)>(loader.load("glTransformFeedbackBufferBase"));
    flextglTransformFeedbackBufferRange = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLuint, GLintptr, GLsizeiptr)>(loader.load("glTransformFeedbackBufferRange"));
    flextglUnmapNamedBuffer = reinterpret_cast<GLboolean(APIENTRY*)(GLuint)>(loader.load("glUnmapNamedBuffer"));
    flextglVertexArrayAttribBinding = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLuint)>(loader.load("glVertexArrayAttribBinding"));
    flextglVertexArrayAttribFormat = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLint, GLenum, GLboolean, GLuint)>(loader.load("glVertexArrayAttribFormat"));
    flextglVertexArrayAttribIFormat = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLint, GLenum, GLuint)>(loader.load("glVertexArrayAttribIFormat"));
    flextglVertexArrayAttribLFormat = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLint, GLenum, GLuint)>(loader.load("glVertexArrayAttribLFormat"));
    flextglVertexArrayBindingDivisor = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLuint)>(loader.load("glVertexArrayBindingDivisor"));
    flextglVertexArrayElementBuffer = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint)>(loader.load("glVertexArrayElementBuffer"));
    flextglVertexArrayVertexBuffer = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLuint, GLintptr, GLsizei)>(loader.load("glVertexArrayVertexBuffer"));
    flextglVertexArrayVertexBuffers = reinterpret_cast<void(APIENTRY*)(GLuint, GLuint, GLsizei, const GLuint *, const GLintptr *, const GLsizei *)>(loader.load("glVertexArrayVertexBuffers"));

    /* GL_VERSION_4_6 */
    flextglMultiDrawArraysIndirectCount = reinterpret_cast<void(APIENTRY*)(GLenum, const void *, GLintptr, GLsizei, GLsizei)>(loader.load("glMultiDrawArraysIndirectCount"));
    flextglMultiDrawElementsIndirectCount = reinterpret_cast<void(APIENTRY*)(GLenum, GLenum, const void *, GLintptr, GLsizei, GLsizei)>(loader.load("glMultiDrawElementsIndirectCount"));
    flextglPolygonOffsetClamp = reinterpret_cast<void(APIENTRY*)(GLfloat, GLfloat, GLfloat)>(loader.load("glPolygonOffsetClamp"));
    flextglSpecializeShader = reinterpret_cast<void(APIENTRY*)(GLuint, const GLchar *, GLuint, const GLuint *, const GLuint *)>(loader.load("glSpecializeShader"));
}
