#ifndef Magnum_GL_Implementation_ShaderProgramState_h
#define Magnum_GL_Implementation_ShaderProgramState_h
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

#include <Corrade/Utility/StlForwardString.h>

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
    void(AbstractShaderProgram::*transformFeedbackVaryingsImplementation)(Containers::ArrayView<const std::string>, AbstractShaderProgram::TransformFeedbackBufferMode);
    #endif
    void(*cleanLogImplementation)(std::string&);

    void(*uniform1fvImplementation)(GLuint, GLint, GLsizei, const GLfloat*);
    void(*uniform2fvImplementation)(GLuint, GLint, GLsizei, const GLfloat*);
    void(*uniform3fvImplementation)(GLuint, GLint, GLsizei, const GLfloat*);
    void(*uniform4fvImplementation)(GLuint, GLint, GLsizei, const GLfloat*);
    void(*uniform1ivImplementation)(GLuint, GLint, GLsizei, const GLint*);
    void(*uniform2ivImplementation)(GLuint, GLint, GLsizei, const GLint*);
    void(*uniform3ivImplementation)(GLuint, GLint, GLsizei, const GLint*);
    void(*uniform4ivImplementation)(GLuint, GLint, GLsizei, const GLint*);
    #ifndef MAGNUM_TARGET_GLES2
    void(*uniform1uivImplementation)(GLuint, GLint, GLsizei, const GLuint*);
    void(*uniform2uivImplementation)(GLuint, GLint, GLsizei, const GLuint*);
    void(*uniform3uivImplementation)(GLuint, GLint, GLsizei, const GLuint*);
    void(*uniform4uivImplementation)(GLuint, GLint, GLsizei, const GLuint*);
    #endif
    #ifndef MAGNUM_TARGET_GLES
    void(*uniform1dvImplementation)(GLuint, GLint, GLsizei, const GLdouble*);
    void(*uniform2dvImplementation)(GLuint, GLint, GLsizei, const GLdouble*);
    void(*uniform3dvImplementation)(GLuint, GLint, GLsizei, const GLdouble*);
    void(*uniform4dvImplementation)(GLuint, GLint, GLsizei, const GLdouble*);
    #endif

    void(*uniformMatrix2fvImplementation)(GLuint, GLint, GLsizei, GLboolean, const GLfloat*);
    void(*uniformMatrix3fvImplementation)(GLuint, GLint, GLsizei, GLboolean, const GLfloat*);
    void(*uniformMatrix4fvImplementation)(GLuint, GLint, GLsizei, GLboolean, const GLfloat*);
    #ifndef MAGNUM_TARGET_GLES2
    void(*uniformMatrix2x3fvImplementation)(GLuint, GLint, GLsizei, GLboolean, const GLfloat*);
    void(*uniformMatrix3x2fvImplementation)(GLuint, GLint, GLsizei, GLboolean, const GLfloat*);
    void(*uniformMatrix2x4fvImplementation)(GLuint, GLint, GLsizei, GLboolean, const GLfloat*);
    void(*uniformMatrix4x2fvImplementation)(GLuint, GLint, GLsizei, GLboolean, const GLfloat*);
    void(*uniformMatrix3x4fvImplementation)(GLuint, GLint, GLsizei, GLboolean, const GLfloat*);
    void(*uniformMatrix4x3fvImplementation)(GLuint, GLint, GLsizei, GLboolean, const GLfloat*);
    #endif
    #ifndef MAGNUM_TARGET_GLES
    void(*uniformMatrix2dvImplementation)(GLuint, GLint, GLsizei, GLboolean, const GLdouble*);
    void(*uniformMatrix3dvImplementation)(GLuint, GLint, GLsizei, GLboolean, const GLdouble*);
    void(*uniformMatrix4dvImplementation)(GLuint, GLint, GLsizei, GLboolean, const GLdouble*);
    void(*uniformMatrix2x3dvImplementation)(GLuint, GLint, GLsizei, GLboolean, const GLdouble*);
    void(*uniformMatrix3x2dvImplementation)(GLuint, GLint, GLsizei, GLboolean, const GLdouble*);
    void(*uniformMatrix2x4dvImplementation)(GLuint, GLint, GLsizei, GLboolean, const GLdouble*);
    void(*uniformMatrix4x2dvImplementation)(GLuint, GLint, GLsizei, GLboolean, const GLdouble*);
    void(*uniformMatrix3x4dvImplementation)(GLuint, GLint, GLsizei, GLboolean, const GLdouble*);
    void(*uniformMatrix4x3dvImplementation)(GLuint, GLint, GLsizei, GLboolean, const GLdouble*);
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
