#ifndef Magnum_GL_Implementation_ShaderProgramState_h
#define Magnum_GL_Implementation_ShaderProgramState_h
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

#include <string>
#include <vector>

#include "Magnum/GL/GL.h"
#include "Magnum/GL/OpenGL.h"
#include "Magnum/Math/Vector3.h"

#ifndef MAGNUM_TARGET_GLES2
#include "Magnum/GL/AbstractShaderProgram.h"
#endif

namespace Magnum { namespace GL { namespace Implementation {

struct ShaderProgramState {
    explicit ShaderProgramState(Context& context, std::vector<std::string>& extensions);

    void reset();

    #ifndef MAGNUM_TARGET_GLES2
    void(AbstractShaderProgram::*transformFeedbackVaryingsImplementation)(Containers::ArrayView<const std::string>, AbstractShaderProgram::TransformFeedbackBufferMode);
    #endif

    void(AbstractShaderProgram::*uniform1fvImplementation)(GLint, GLsizei, const GLfloat*);
    void(AbstractShaderProgram::*uniform2fvImplementation)(GLint, GLsizei, const Math::Vector<2, GLfloat>*);
    void(AbstractShaderProgram::*uniform3fvImplementation)(GLint, GLsizei, const Math::Vector<3, GLfloat>*);
    void(AbstractShaderProgram::*uniform4fvImplementation)(GLint, GLsizei, const Math::Vector<4, GLfloat>*);
    void(AbstractShaderProgram::*uniform1ivImplementation)(GLint, GLsizei, const GLint*);
    void(AbstractShaderProgram::*uniform2ivImplementation)(GLint, GLsizei, const Math::Vector<2, GLint>*);
    void(AbstractShaderProgram::*uniform3ivImplementation)(GLint, GLsizei, const Math::Vector<3, GLint>*);
    void(AbstractShaderProgram::*uniform4ivImplementation)(GLint, GLsizei, const Math::Vector<4, GLint>*);
    #ifndef MAGNUM_TARGET_GLES2
    void(AbstractShaderProgram::*uniform1uivImplementation)(GLint, GLsizei, const GLuint*);
    void(AbstractShaderProgram::*uniform2uivImplementation)(GLint, GLsizei, const Math::Vector<2, GLuint>*);
    void(AbstractShaderProgram::*uniform3uivImplementation)(GLint, GLsizei, const Math::Vector<3, GLuint>*);
    void(AbstractShaderProgram::*uniform4uivImplementation)(GLint, GLsizei, const Math::Vector<4, GLuint>*);
    #endif
    #ifndef MAGNUM_TARGET_GLES
    void(AbstractShaderProgram::*uniform1dvImplementation)(GLint, GLsizei, const GLdouble*);
    void(AbstractShaderProgram::*uniform2dvImplementation)(GLint, GLsizei, const Math::Vector<2, GLdouble>*);
    void(AbstractShaderProgram::*uniform3dvImplementation)(GLint, GLsizei, const Math::Vector<3, GLdouble>*);
    void(AbstractShaderProgram::*uniform4dvImplementation)(GLint, GLsizei, const Math::Vector<4, GLdouble>*);
    #endif

    void(AbstractShaderProgram::*uniformMatrix2fvImplementation)(GLint, GLsizei, const Math::RectangularMatrix<2, 2, GLfloat>*);
    void(AbstractShaderProgram::*uniformMatrix3fvImplementation)(GLint, GLsizei, const Math::RectangularMatrix<3, 3, GLfloat>*);
    void(AbstractShaderProgram::*uniformMatrix4fvImplementation)(GLint, GLsizei, const Math::RectangularMatrix<4, 4, GLfloat>*);
    #ifndef MAGNUM_TARGET_GLES2
    void(AbstractShaderProgram::*uniformMatrix2x3fvImplementation)(GLint, GLsizei, const Math::RectangularMatrix<2, 3, GLfloat>*);
    void(AbstractShaderProgram::*uniformMatrix3x2fvImplementation)(GLint, GLsizei, const Math::RectangularMatrix<3, 2, GLfloat>*);
    void(AbstractShaderProgram::*uniformMatrix2x4fvImplementation)(GLint, GLsizei, const Math::RectangularMatrix<2, 4, GLfloat>*);
    void(AbstractShaderProgram::*uniformMatrix4x2fvImplementation)(GLint, GLsizei, const Math::RectangularMatrix<4, 2, GLfloat>*);
    void(AbstractShaderProgram::*uniformMatrix3x4fvImplementation)(GLint, GLsizei, const Math::RectangularMatrix<3, 4, GLfloat>*);
    void(AbstractShaderProgram::*uniformMatrix4x3fvImplementation)(GLint, GLsizei, const Math::RectangularMatrix<4, 3, GLfloat>*);
    #endif
    #ifndef MAGNUM_TARGET_GLES
    void(AbstractShaderProgram::*uniformMatrix2dvImplementation)(GLint, GLsizei, const Math::RectangularMatrix<2, 2, GLdouble>*);
    void(AbstractShaderProgram::*uniformMatrix3dvImplementation)(GLint, GLsizei, const Math::RectangularMatrix<3, 3, GLdouble>*);
    void(AbstractShaderProgram::*uniformMatrix4dvImplementation)(GLint, GLsizei, const Math::RectangularMatrix<4, 4, GLdouble>*);
    void(AbstractShaderProgram::*uniformMatrix2x3dvImplementation)(GLint, GLsizei, const Math::RectangularMatrix<2, 3, GLdouble>*);
    void(AbstractShaderProgram::*uniformMatrix3x2dvImplementation)(GLint, GLsizei, const Math::RectangularMatrix<3, 2, GLdouble>*);
    void(AbstractShaderProgram::*uniformMatrix2x4dvImplementation)(GLint, GLsizei, const Math::RectangularMatrix<2, 4, GLdouble>*);
    void(AbstractShaderProgram::*uniformMatrix4x2dvImplementation)(GLint, GLsizei, const Math::RectangularMatrix<4, 2, GLdouble>*);
    void(AbstractShaderProgram::*uniformMatrix3x4dvImplementation)(GLint, GLsizei, const Math::RectangularMatrix<3, 4, GLdouble>*);
    void(AbstractShaderProgram::*uniformMatrix4x3dvImplementation)(GLint, GLsizei, const Math::RectangularMatrix<4, 3, GLdouble>*);
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
