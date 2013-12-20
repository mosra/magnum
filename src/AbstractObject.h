#ifndef Magnum_AbstractObject_h
#define Magnum_AbstractObject_h
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

/** @file
 * @brief Class @ref Magnum::AbstractObject
 */

#include <string>

#include "OpenGL.h"
#include "Types.h"
#include "magnumVisibility.h"

namespace Magnum {

namespace Implementation {
    struct DebugState;
}

/**
@brief Base for all OpenGL objects
*/
class MAGNUM_EXPORT AbstractObject {
    friend class Context;
    friend class Implementation::DebugState;

    public:
        /**
         * @brief Max object label length
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If @extension{KHR,debug} desktop or ES extension is
         * not available, returns `0`. Note that @extension{EXT,debug_label}
         * has no such limit.
         * @see @ref AbstractQuery::setLabel(), @ref AbstractShaderProgram::setLabel(),
         *      @ref AbstractTexture::setLabel(), @ref Buffer::setLabel(),
         *      @ref BufferTexture::setLabel(), @ref Framebuffer::setLabel(),
         *      @ref Mesh::setLabel(), @ref Renderbuffer::setLabel(),
         *      @ref Shader::setLabel(), @fn_gl{Get} with @def_gl{MAX_LABEL_LENGTH}
         */
        static Int maxLabelLength();

    protected:
        MAGNUM_LOCAL AbstractObject() = default;
        MAGNUM_LOCAL ~AbstractObject() = default;

    private:
        static MAGNUM_LOCAL void labelImplementationNoOp(GLenum, GLuint, const std::string&);
        static MAGNUM_LOCAL void labelImplementationExt(GLenum identifier, GLuint name, const std::string& label);
        static MAGNUM_LOCAL void labelImplementationKhr(GLenum identifier, GLuint name, const std::string& label);
        static MAGNUM_LOCAL std::string getLabelImplementationNoOp(GLenum, GLuint);
        static MAGNUM_LOCAL std::string getLabelImplementationExt(GLenum identifier, GLuint name);
        static MAGNUM_LOCAL std::string getLabelImplementationKhr(GLenum identifier, GLuint name);
};

}

#endif
