#ifndef Magnum_GL_Implementation_ShaderProgramState_h
#define Magnum_GL_Implementation_ShaderProgramState_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2022 Vladislav Oleshko <vladislav.oleshko@gmail.com>
    Copyright @ 2022 Hugo Amiard <hugo.amiard@wonderlandengine.com>

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

#include "Magnum/Magnum.h"
#include "Magnum/GL/GL.h"
#include "Magnum/GL/OpenGL.h"
#include "Magnum/Math/Vector3.h"

#ifndef MAGNUM_TARGET_GLES2
#include "Magnum/GL/AbstractShaderProgram.h"
#endif

namespace Magnum { namespace GL { namespace Implementation {

struct ShaderProgramState {
    explicit ShaderProgramState(Context& context, Containers::StaticArrayView<Implementation::ExtensionCount, const char*> extensions);

    void reset();

    #ifndef MAGNUM_TARGET_GLES2
    void(*transformFeedbackVaryingsImplementation)(AbstractShaderProgram&, const Containers::StringIterable&, AbstractShaderProgram::TransformFeedbackBufferMode);
    #endif
    void(*cleanLogImplementation)(Containers::String&);
    /* This is a direct pointer to a GL function, so needs a __stdcall on
       Windows to compile properly on 32 bits */
    void(APIENTRY *completionStatusImplementation)(GLuint, GLenum, GLint* value);

    #ifndef MAGNUM_TARGET_WEBGL
    void(APIENTRY *uniform1fvImplementation)(GLuint, GLint, GLsizei, const GLfloat*);
    void(APIENTRY *uniform2fvImplementation)(GLuint, GLint, GLsizei, const GLfloat*);
    void(APIENTRY *uniform3fvImplementation)(GLuint, GLint, GLsizei, const GLfloat*);
    void(APIENTRY *uniform4fvImplementation)(GLuint, GLint, GLsizei, const GLfloat*);
    void(APIENTRY *uniform1fImplementation)(GLuint, GLint, GLfloat);
    void(APIENTRY *uniform2fImplementation)(GLuint, GLint, GLfloat, GLfloat);
    void(APIENTRY *uniform3fImplementation)(GLuint, GLint, GLfloat, GLfloat, GLfloat);
    void(APIENTRY *uniform4fImplementation)(GLuint, GLint, GLfloat, GLfloat, GLfloat, GLfloat);
    void(APIENTRY *uniform1ivImplementation)(GLuint, GLint, GLsizei, const GLint*);
    void(APIENTRY *uniform2ivImplementation)(GLuint, GLint, GLsizei, const GLint*);
    void(APIENTRY *uniform3ivImplementation)(GLuint, GLint, GLsizei, const GLint*);
    void(APIENTRY *uniform4ivImplementation)(GLuint, GLint, GLsizei, const GLint*);
    void(APIENTRY *uniform1iImplementation)(GLuint, GLint, GLint);
    void(APIENTRY *uniform2iImplementation)(GLuint, GLint, GLint, GLint);
    void(APIENTRY *uniform3iImplementation)(GLuint, GLint, GLint, GLint, GLint);
    void(APIENTRY *uniform4iImplementation)(GLuint, GLint, GLint, GLint, GLint, GLint);
    #ifndef MAGNUM_TARGET_GLES2
    void(APIENTRY *uniform1uivImplementation)(GLuint, GLint, GLsizei, const GLuint*);
    void(APIENTRY *uniform2uivImplementation)(GLuint, GLint, GLsizei, const GLuint*);
    void(APIENTRY *uniform3uivImplementation)(GLuint, GLint, GLsizei, const GLuint*);
    void(APIENTRY *uniform4uivImplementation)(GLuint, GLint, GLsizei, const GLuint*);
    void(APIENTRY *uniform1uiImplementation)(GLuint, GLint, GLuint);
    void(APIENTRY *uniform2uiImplementation)(GLuint, GLint, GLuint, GLuint);
    void(APIENTRY *uniform3uiImplementation)(GLuint, GLint, GLuint, GLuint, GLuint);
    void(APIENTRY *uniform4uiImplementation)(GLuint, GLint, GLuint, GLuint, GLuint, GLuint);
    #endif
    #ifndef MAGNUM_TARGET_GLES
    void(APIENTRY *uniform1dvImplementation)(GLuint, GLint, GLsizei, const GLdouble*);
    void(APIENTRY *uniform2dvImplementation)(GLuint, GLint, GLsizei, const GLdouble*);
    void(APIENTRY *uniform3dvImplementation)(GLuint, GLint, GLsizei, const GLdouble*);
    void(APIENTRY *uniform4dvImplementation)(GLuint, GLint, GLsizei, const GLdouble*);
    void(APIENTRY *uniform1dImplementation)(GLuint, GLint, GLdouble);
    void(APIENTRY *uniform2dImplementation)(GLuint, GLint, GLdouble, GLdouble);
    void(APIENTRY *uniform3dImplementation)(GLuint, GLint, GLdouble, GLdouble, GLdouble);
    void(APIENTRY *uniform4dImplementation)(GLuint, GLint, GLdouble, GLdouble, GLdouble, GLdouble);
    #endif

    void(APIENTRY *uniformMatrix2fvImplementation)(GLuint, GLint, GLsizei, GLboolean, const GLfloat*);
    void(APIENTRY *uniformMatrix3fvImplementation)(GLuint, GLint, GLsizei, GLboolean, const GLfloat*);
    void(APIENTRY *uniformMatrix4fvImplementation)(GLuint, GLint, GLsizei, GLboolean, const GLfloat*);
    #ifndef MAGNUM_TARGET_GLES2
    void(APIENTRY *uniformMatrix2x3fvImplementation)(GLuint, GLint, GLsizei, GLboolean, const GLfloat*);
    void(APIENTRY *uniformMatrix3x2fvImplementation)(GLuint, GLint, GLsizei, GLboolean, const GLfloat*);
    void(APIENTRY *uniformMatrix2x4fvImplementation)(GLuint, GLint, GLsizei, GLboolean, const GLfloat*);
    void(APIENTRY *uniformMatrix4x2fvImplementation)(GLuint, GLint, GLsizei, GLboolean, const GLfloat*);
    void(APIENTRY *uniformMatrix3x4fvImplementation)(GLuint, GLint, GLsizei, GLboolean, const GLfloat*);
    void(APIENTRY *uniformMatrix4x3fvImplementation)(GLuint, GLint, GLsizei, GLboolean, const GLfloat*);
    #endif
    #ifndef MAGNUM_TARGET_GLES
    void(APIENTRY *uniformMatrix2dvImplementation)(GLuint, GLint, GLsizei, GLboolean, const GLdouble*);
    void(APIENTRY *uniformMatrix3dvImplementation)(GLuint, GLint, GLsizei, GLboolean, const GLdouble*);
    void(APIENTRY *uniformMatrix4dvImplementation)(GLuint, GLint, GLsizei, GLboolean, const GLdouble*);
    void(APIENTRY *uniformMatrix2x3dvImplementation)(GLuint, GLint, GLsizei, GLboolean, const GLdouble*);
    void(APIENTRY *uniformMatrix3x2dvImplementation)(GLuint, GLint, GLsizei, GLboolean, const GLdouble*);
    void(APIENTRY *uniformMatrix2x4dvImplementation)(GLuint, GLint, GLsizei, GLboolean, const GLdouble*);
    void(APIENTRY *uniformMatrix4x2dvImplementation)(GLuint, GLint, GLsizei, GLboolean, const GLdouble*);
    void(APIENTRY *uniformMatrix3x4dvImplementation)(GLuint, GLint, GLsizei, GLboolean, const GLdouble*);
    void(APIENTRY *uniformMatrix4x3dvImplementation)(GLuint, GLint, GLsizei, GLboolean, const GLdouble*);
    #endif
    #endif

    /* Currently used program */
    GLuint current;

    GLint maxVertexAttributes;
    #ifndef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_WEBGL
    GLint maxGeometryOutputVertices,
        maxAtomicCounterBufferSize,
        maxComputeSharedMemorySize,
        maxComputeWorkGroupInvocations,
        maxImageUnits,
        maxCombinedShaderOutputResources,
        maxUniformLocations;
    Vector3i maxComputeWorkGroupCount,
        maxComputeWorkGroupSize;
    #endif
    GLint minTexelOffset,
        maxTexelOffset,
        maxUniformBlockSize;
    #ifndef MAGNUM_TARGET_WEBGL
    GLint64 maxShaderStorageBlockSize;
    #endif
    #endif

    #ifndef MAGNUM_TARGET_GLES
    GLint maxImageSamples;
    #endif
};

}}}

#endif
