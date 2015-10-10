#ifndef Magnum_Platform_Context_h
#define Magnum_Platform_Context_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015
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

#include <Corrade/Utility/Debug.h>

#include "Magnum/Context.h"
#include "Magnum/OpenGL.h"

namespace Magnum { namespace Platform {

/**
@brief Platform-specific context

In most cases not needed to be used directly as the initialization is done
automatically in `*Application` classes. See @ref platform for more
information.
*/
class Context: public Magnum::Context {
    public:
        /**
         * @brief Try to create the context
         *
         * Unline @ref Context(), this function returns `nullptr` on error
         * instead of application exit and thus is usable for context creation
         * fallbacks.
         */
        static std::unique_ptr<Context> tryCreate() {
            std::unique_ptr<Context> c{new Context{NoCreate}};
            return c->create() ? std::move(c) : nullptr;
        }

        /**
         * @brief Constructor
         *
         * Does initial setup, loads OpenGL function pointers using
         * platform-specific API, detects available features and enables them
         * throughout the engine. If detected version is unsupported or any
         * other error occurs, a message is printed to output and the
         * application exits. See @ref tryCreate() for an alternative.
         * @see @fn_gl{Get} with @def_gl{MAJOR_VERSION}, @def_gl{MINOR_VERSION},
         *      @def_gl{CONTEXT_FLAGS}, @def_gl{NUM_EXTENSIONS},
         *      @fn_gl{GetString} with @def_gl{EXTENSIONS}
         */
        explicit Context():
            #if !defined(CORRADE_TARGET_EMSCRIPTEN) && !defined(CORRADE_TARGET_NACL)
            Magnum::Context{flextGLInit} {}
            #else
            Magnum::Context{nullptr} {}
            #endif

    private:
        explicit Context(NoCreateT):
            #if !defined(CORRADE_TARGET_EMSCRIPTEN) && !defined(CORRADE_TARGET_NACL)
            Magnum::Context{NoCreate, flextGLInit} {}
            #else
            Magnum::Context{NoCreate, nullptr} {}
            #endif
};

}}

#endif
