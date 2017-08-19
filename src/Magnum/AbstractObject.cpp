/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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

#include "AbstractObject.h"

#include <Corrade/Utility/Assert.h>
#include <Corrade/Containers/ArrayView.h>

#include "Magnum/Context.h"
#include "Magnum/Extensions.h"

#include "Implementation/State.h"
#ifndef MAGNUM_TARGET_WEBGL
#include "Implementation/DebugState.h"
#endif

namespace Magnum {

#ifndef MAGNUM_TARGET_WEBGL
namespace {
    inline GLenum extTypeFromKhrIdentifier(GLenum khrIdentifier) {
        switch(khrIdentifier) {
            #ifndef MAGNUM_TARGET_GLES
            case GL_BUFFER:
            #else
            case GL_BUFFER_KHR:
            #endif
                return GL_BUFFER_OBJECT_EXT;

            #ifndef MAGNUM_TARGET_GLES
            case GL_SHADER:
            #else
            case GL_SHADER_KHR:
            #endif
                return GL_SHADER_OBJECT_EXT;

            #ifndef MAGNUM_TARGET_GLES
            case GL_PROGRAM:
            #else
            case GL_PROGRAM_KHR:
            #endif
                return GL_PROGRAM_OBJECT_EXT;

            #ifndef MAGNUM_TARGET_GLES
            case GL_VERTEX_ARRAY:
            #else
            case GL_VERTEX_ARRAY_KHR:
            #endif
                return GL_VERTEX_ARRAY_OBJECT_EXT;

            #ifndef MAGNUM_TARGET_GLES
            case GL_QUERY:
            #else
            case GL_QUERY_KHR:
            #endif
                return GL_QUERY_OBJECT_EXT;

            /** @todo Why isn't `GL_PROGRAM_PIPELINE_KHR` in ES's KHR_debug? */
            #ifndef MAGNUM_TARGET_GLES
            case GL_PROGRAM_PIPELINE:
            #else
            case 0x82E4: //GL_PROGRAM_PIPELINE_KHR:
            #endif
                return GL_PROGRAM_PIPELINE_OBJECT_EXT;

            /**
             * @todo Shouldn't ES2's KHR_debug have `GL_TRANSFORM_FEEDBACK_KHR`
             *      instead of `GL_TRANSFORM_FEEDBACK`? (it's a new enum in 2.0)
             *      Also the original @extension{EXT,debug_label} mentions it
             *      only for ES3 (i.e. no mention of @extension{EXT,transform_feedback})
             */
            case GL_TRANSFORM_FEEDBACK:
            #ifndef MAGNUM_TARGET_GLES
            case GL_SAMPLER:
            #else
            case GL_SAMPLER_KHR:
            #endif
            case GL_TEXTURE:
            case GL_RENDERBUFFER:
            case GL_FRAMEBUFFER:
                return khrIdentifier;
        }

        CORRADE_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
    }
}

Int AbstractObject::maxLabelLength() {
    if(!Context::current().isExtensionSupported<Extensions::GL::KHR::debug>())
        return 0;

    GLint& value = Context::current().state().debug->maxLabelLength;

    if(value == 0) {
        #ifndef MAGNUM_TARGET_GLES
        glGetIntegerv(GL_MAX_LABEL_LENGTH, &value);
        #else
        glGetIntegerv(GL_MAX_LABEL_LENGTH_KHR, &value);
        #endif
    }

    return value;
}

void AbstractObject::labelImplementationNoOp(GLenum, GLuint, Containers::ArrayView<const char>) {}

void AbstractObject::labelImplementationKhr(const GLenum identifier, const GLuint name, const Containers::ArrayView<const char> label) {
    #ifndef MAGNUM_TARGET_GLES
    glObjectLabel(identifier, name, label.size(), label);
    #else
    glObjectLabelKHR(identifier, name, label.size(), label);
    #endif
}

void AbstractObject::labelImplementationExt(const GLenum identifier, const GLuint name, const Containers::ArrayView<const char> label) {
    const GLenum type = extTypeFromKhrIdentifier(identifier);
    glLabelObjectEXT(type, name, label.size(), label);
}

std::string AbstractObject::getLabelImplementationNoOp(GLenum, GLuint) { return  {}; }

std::string AbstractObject::getLabelImplementationKhr(const GLenum identifier, const GLuint name) {
    /* Get label size (w/o null terminator). Specifying 0 as size is not
       allowed, thus we pass the maximum instead. */
    GLsizei size = 0;
    #ifndef MAGNUM_TARGET_GLES
    glGetObjectLabel(identifier, name, maxLabelLength(), &size, nullptr);
    #else
    glGetObjectLabelKHR(identifier, name, maxLabelLength(), &size, nullptr);
    #endif

    /* Make place also for the null terminator */
    std::string label;
    label.resize(size+1);
    #ifndef MAGNUM_TARGET_GLES
    glGetObjectLabel(identifier, name, size+1, nullptr, &label[0]);
    #else
    glGetObjectLabelKHR(identifier, name, size+1, nullptr, &label[0]);
    #endif

    /* Pop null terminator and return the string */
    label.resize(size);
    return label;
}

std::string AbstractObject::getLabelImplementationExt(const GLenum identifier, const GLuint name) {
    GLsizei size = 0;

    /* Get label size (w/o null terminator) */
    const GLenum type = extTypeFromKhrIdentifier(identifier);
    glGetObjectLabelEXT(type, name, 0, &size, nullptr);

    /* Make place also for the null terminator */
    std::string label;
    label.resize(size+1);
    glGetObjectLabelEXT(type, name, size+1, nullptr, &label[0]);

    /* Pop null terminator and return the string */
    label.resize(size);
    return label;
}
#endif

}
