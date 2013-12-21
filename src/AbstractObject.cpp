/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

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

#include <Utility/Assert.h>

#include "Extensions.h"
#include "Implementation/State.h"
#include "Implementation/DebugState.h"

namespace Magnum {

namespace {
    /* What the hell, Apple? Why you had to create this abomination _after_
       KHR_debug was released?! */
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

        CORRADE_ASSERT_UNREACHABLE();
    }
}

Int AbstractObject::maxLabelLength() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::KHR::debug>())
        return 0;

    GLint& value = Context::current()->state().debug->maxLabelLength;

    if(value == 0) {
        #ifndef MAGNUM_TARGET_GLES
        glGetIntegerv(GL_MAX_LABEL_LENGTH, &value);
        #else
        glGetIntegerv(GL_MAX_LABEL_LENGTH_KHR, &value);
        #endif
    }

    return value;
}

void AbstractObject::labelImplementationNoOp(GLenum, GLuint, const std::string&) {}

void AbstractObject::labelImplementationKhr(const GLenum identifier, const GLuint name, const std::string& label) {
    #ifndef MAGNUM_TARGET_GLES
    glObjectLabel(identifier, name, label.size(), label.data());
    #else
    static_cast<void>(identifier);
    static_cast<void>(name);
    static_cast<void>(label);
    CORRADE_INTERNAL_ASSERT(false);
    //glObjectLabelKHR(identifier, name, label.size(), label.data());
    #endif
}

void AbstractObject::labelImplementationExt(const GLenum identifier, const GLuint name, const std::string& label) {
    const GLenum type = extTypeFromKhrIdentifier(identifier);
    /** @todo Re-enable when extension wrangler is available for ES */
    #ifndef MAGNUM_TARGET_GLES
    glLabelObjectEXT(type, name, label.size(), label.data());
    #else
    static_cast<void>(type);
    static_cast<void>(name);
    static_cast<void>(label);
    CORRADE_INTERNAL_ASSERT(false);
    #endif
}

std::string AbstractObject::getLabelImplementationNoOp(GLenum, GLuint) { return  {}; }

std::string AbstractObject::getLabelImplementationKhr(const GLenum identifier, const GLuint name) {
    /**
     * @todo Get rid of this workaround when NVidia returns proper size for
     *      length=0 & label=nullptr (even crashes on length>0 & label=nullptr)
     */
    #if 0
    /* Get label size (w/o null terminator) */
    GLsizei size;
    #ifndef MAGNUM_TARGET_GLES
    glGetObjectLabel(type, name, 0, &size, nullptr);
    #else
    glGetObjectLabelKHR(type, name, 0, &size, nullptr);
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
    #else
    GLsizei size;
    std::string label;
    label.resize(maxLabelLength());
    /** @todo Re-enable when extension wrangler is available for ES */
    #ifndef MAGNUM_TARGET_GLES
    glGetObjectLabel(identifier, name, label.size(), &size, &label[0]);
    #else
    static_cast<void>(identifier);
    static_cast<void>(name);
    CORRADE_INTERNAL_ASSERT(false);
    //glGetObjectLabelKHR(identifier, name, label.size(), &size, &label[0]);
    #endif
    return label.substr(0, size);
    #endif
}

std::string AbstractObject::getLabelImplementationExt(const GLenum identifier, const GLuint name) {
    const GLenum type = extTypeFromKhrIdentifier(identifier);

    /* Get label size (w/o null terminator) */
    GLsizei size;
    /** @todo Re-enable when extension wrangler is available for ES */
    #ifndef MAGNUM_TARGET_GLES
    glGetObjectLabelEXT(type, name, 0, &size, nullptr);
    #else
    static_cast<void>(type);
    static_cast<void>(name);
    CORRADE_INTERNAL_ASSERT(false);
    #endif

    /* Make place also for the null terminator */
    std::string label;
    label.resize(size+1);
    /** @todo Re-enable when extension wrangler is available for ES */
    #ifndef MAGNUM_TARGET_GLES
    glGetObjectLabelEXT(identifier, name, size+1, nullptr, &label[0]);
    #endif

    /* Pop null terminator and return the string */
    label.resize(size);
    return label;
}

}
