#ifndef Magnum_Platform_GLContext_h
#define Magnum_Platform_GLContext_h
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

#ifdef MAGNUM_TARGET_GL
/** @file
 * @brief Class @ref Magnum::Platform::GLContext
 */
#endif

#include <Corrade/Utility/Debug.h>

#include "Magnum/configure.h"

#ifdef MAGNUM_TARGET_GL
#include "Magnum/GL/Context.h"
#include "Magnum/GL/OpenGL.h"

namespace Magnum { namespace Platform {

/**
@brief Platform-specific OpenGL context

In most cases not needed to be used directly as the initialization is done
automatically in `*Application` classes. See @ref platform for more
information.

@note This class is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL enabled (done by default). See @ref building-features
    for more information.
*/
class GLContext: public GL::Context {
    public:
        /**
         * @brief Constructor
         *
         * Equivalent to calling @ref GLContext(NoCreateT, Int, const char**)
         * followed by @ref create().
         */
        explicit GLContext(Int argc, const char** argv, InternalFlags flags = InternalFlag::Default): GLContext{NoCreate, argc, argv, flags} { create(); }

        /** @overload */
        explicit GLContext(Int argc, char** argv, InternalFlags flags = InternalFlag::Default): GLContext{argc, const_cast<const char**>(argv), flags} {}

        /** @overload */
        explicit GLContext(Int argc, std::nullptr_t argv, InternalFlags flags = InternalFlag::Default): GLContext{argc, static_cast<const char**>(argv), flags} {}

        /**
         * @brief Default constructor
         *
         * Equivalent to passing @cpp {0, nullptr} @ce to
         * @ref GLContext(Int, const char**). Even if the command-line options
         * are not propagated, it's still possible to affect the renderer
         * behavior from the environment. See @ref GL-Context-command-line for
         * more information.
         */
        explicit GLContext(InternalFlags flags = InternalFlag::Default): GLContext{0, nullptr, flags} {}

        /**
         * @brief Construct without creating the context
         *
         * Parses command-line arguments and sets @ref version() to
         * @ref GL::Version::None, everything else is left in an empty state.
         * Use @ref create() or @ref tryCreate() to create the context.
         * @see @ref GLContext(Int, const char**)
         */
        explicit GLContext(NoCreateT, Int argc, const char** argv, InternalFlags flags = InternalFlag::Default):
            #ifndef CORRADE_TARGET_EMSCRIPTEN
            GL::Context{NoCreate, argc, argv, flextGLInit, flags} {}
            #else
            GL::Context{NoCreate, argc, argv, nullptr, flags} {}
            #endif

        /** @overload */
        explicit GLContext(NoCreateT, Int argc, char** argv, InternalFlags flags = InternalFlag::Default): GLContext{NoCreate, argc, const_cast<const char**>(argv), flags} {}

        /** @overload */
        explicit GLContext(NoCreateT, Int argc, std::nullptr_t argv, InternalFlags flags = InternalFlag::Default): GLContext{NoCreate, argc, static_cast<const char**>(argv), flags} {}

        #ifndef DOXYGEN_GENERATING_OUTPUT
        /* Used privately to inject additional command-line arguments */
        explicit GLContext(NoCreateT, Utility::Arguments& args, Int argc, char** argv, InternalFlags flags = InternalFlag::Default): GLContext{NoCreate, args, argc, const_cast<const char**>(argv), flags} {}
        explicit GLContext(NoCreateT, Utility::Arguments& args, Int argc, std::nullptr_t argv, InternalFlags flags = InternalFlag::Default): GLContext{NoCreate, args, argc, static_cast<const char**>(argv), flags} {}
        explicit GLContext(NoCreateT, Utility::Arguments& args, Int argc, const char** argv, InternalFlags flags = InternalFlag::Default):
            #ifndef CORRADE_TARGET_EMSCRIPTEN
            GL::Context{NoCreate, args, argc, argv, flextGLInit, flags} {}
            #else
            GL::Context{NoCreate, args, argc, argv, nullptr, flags} {}
            #endif
        #endif

        /**
         * @brief Construct with creation delayed to later
         *
         * Equivalent to passing @cpp {NoCreate, 0, nullptr} @ce to
         * @ref GLContext(NoCreateT, Int, const char**). Even if the
         * command-line options are not propagated, it's still possible to
         * affect the renderer behavior from the environment. See
         * @ref GL-Context-command-line for more information.
         */
        explicit GLContext(NoCreateT, InternalFlags flags = InternalFlag::Default): GLContext{NoCreate, 0, nullptr, flags} {}

        /**
         * @brief Create the context
         *
         * Meant to be called on a @ref GLContext(NoCreateT, Int, char**) "NoCreate"'d
         * instance. Parses command-line arguments, loads OpenGL function
         * pointers using a platform-specific API, does initial setup, detects
         * available features and enables them throughout the engine. If
         * detected version is unsupported or any other error occurs, a message
         * is printed to output and the application exits. See @ref tryCreate()
         * for an alternative.
         * @see @ref GLContext(Int, char**), @ref GL-Context-command-line,
         *      @fn_gl{Get} with @def_gl{MAJOR_VERSION},
         *      @def_gl{MINOR_VERSION}, @def_gl{CONTEXT_FLAGS},
         *      @def_gl{NUM_EXTENSIONS}, @fn_gl{GetString} with
         *      @def_gl{EXTENSIONS}
         */
        void create() { return GL::Context::create(); }

        /**
         * @brief Try to create the context
         *
         * Unlike @ref create(), instead of exiting prints a message to error
         * output and returns `false` on error.
         */
        bool tryCreate() { return GL::Context::tryCreate(); }
};

}}
#else
#error this header is available only in the OpenGL build
#endif

#endif
