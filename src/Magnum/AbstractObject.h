#ifndef Magnum_AbstractObject_h
#define Magnum_AbstractObject_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014
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

/** @file
 * @brief Class @ref Magnum::AbstractObject
 */

#include <string>
#include <Corrade/Containers/Containers.h>

#include "Magnum/Magnum.h"
#include "Magnum/OpenGL.h"
#include "Magnum/visibility.h"

namespace Magnum {

namespace Implementation { struct DebugState; }

/**
@brief Base for all OpenGL objects
*/
class MAGNUM_EXPORT AbstractObject {
    friend struct Implementation::DebugState;

    public:
        /**
         * @brief Max object label length
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If OpenGL 4.3 is not supported and @extension{KHR,debug}
         * desktop or ES extension is not available, returns `0`. Note that
         * @extension2{EXT,debug_label} has no such limit.
         * @see @ref AbstractQuery::setLabel(), @ref AbstractShaderProgram::setLabel(),
         *      @ref AbstractTexture::setLabel(), @ref Buffer::setLabel(),
         *      @ref BufferTexture::setLabel(), @ref Framebuffer::setLabel(),
         *      @ref Mesh::setLabel(), @ref Renderbuffer::setLabel(),
         *      @ref Shader::setLabel(), @fn_gl{Get} with @def_gl{MAX_LABEL_LENGTH}
         */
        static Int maxLabelLength();

    protected:
        MAGNUM_LOCAL AbstractObject();
        MAGNUM_LOCAL ~AbstractObject();

    private:
        static MAGNUM_LOCAL void labelImplementationNoOp(GLenum, GLuint, Containers::ArrayReference<const char> label);
        static MAGNUM_LOCAL void labelImplementationExt(GLenum identifier, GLuint name, Containers::ArrayReference<const char> label);
        static MAGNUM_LOCAL void labelImplementationKhr(GLenum identifier, GLuint name, Containers::ArrayReference<const char> label);
        static MAGNUM_LOCAL std::string getLabelImplementationNoOp(GLenum, GLuint);
        static MAGNUM_LOCAL std::string getLabelImplementationExt(GLenum identifier, GLuint name);
        static MAGNUM_LOCAL std::string getLabelImplementationKhr(GLenum identifier, GLuint name);
};

inline AbstractObject::AbstractObject() = default;
inline AbstractObject::~AbstractObject() = default;

}

#endif
