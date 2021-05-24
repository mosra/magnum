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

#include <Corrade/Utility/Utility.h>

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
         * followed by @ref create(const Configuration&).
         */
        explicit GLContext(Int argc, const char** argv, const Configuration& configuration = {}): GLContext{NoCreate, argc, argv} {
            create(configuration);
        }

        /** @overload */
        explicit GLContext(Int argc, char** argv, const Configuration& configuration = {}): GLContext{argc, const_cast<const char**>(argv), configuration} {}

        /** @overload */
        explicit GLContext(Int argc, std::nullptr_t argv, const Configuration& configuration = {}): GLContext{argc, static_cast<const char**>(argv), configuration} {}

        /**
         * @brief Default constructor
         *
         * Equivalent to passing @cpp {0, nullptr, configuration} @ce to
         * @ref GLContext(Int, const char**, const Configuration&). Even if the
         * command-line options are not propagated, it's still possible to
         * affect the setup behavior from the environment or by passing a
         * @relativeref{GL::Context,Configuration} instance. See
         * @ref GL-Context-usage for more information.
         */
        explicit GLContext(const Configuration& configuration = {}): GLContext{0, nullptr, configuration} {}

        /**
         * @brief Construct without creating the context
         *
         * Parses command-line arguments and sets @ref version() to
         * @ref GL::Version::None, everything else is left in an empty state.
         * Use @ref create() or @ref tryCreate() to create the context.
         * @see @ref GLContext(Int, const char**, const Configuration&)
         */
        explicit GLContext(NoCreateT, Int argc, const char** argv):
            #ifndef CORRADE_TARGET_EMSCRIPTEN
            GL::Context{NoCreate, argc, argv, flextGLInit} {}
            #else
            GL::Context{NoCreate, argc, argv, nullptr} {}
            #endif

        /** @overload */
        explicit GLContext(NoCreateT, Int argc, char** argv): GLContext{NoCreate, argc, const_cast<const char**>(argv)} {}

        /** @overload */
        explicit GLContext(NoCreateT, Int argc, std::nullptr_t argv): GLContext{NoCreate, argc, static_cast<const char**>(argv)} {}

        #ifndef DOXYGEN_GENERATING_OUTPUT
        /* Used privately to inject additional command-line arguments */
        explicit GLContext(NoCreateT, Utility::Arguments& args, Int argc, char** argv): GLContext{NoCreate, args, argc, const_cast<const char**>(argv)} {}
        explicit GLContext(NoCreateT, Utility::Arguments& args, Int argc, std::nullptr_t argv): GLContext{NoCreate, args, argc, static_cast<const char**>(argv)} {}
        explicit GLContext(NoCreateT, Utility::Arguments& args, Int argc, const char** argv):
            #ifndef CORRADE_TARGET_EMSCRIPTEN
            GL::Context{NoCreate, args, argc, argv, flextGLInit} {}
            #else
            GL::Context{NoCreate, args, argc, argv, nullptr} {}
            #endif
        #endif

        /**
         * @brief Construct with creation delayed to later
         *
         * Equivalent to passing @cpp {NoCreate, 0, nullptr} @ce to
         * @ref GLContext(NoCreateT, Int, const char**). Even if the
         * command-line options are not propagated, it's still possible to
         * affect the renderer behavior from the environment or by passing
         * a @relativeref{GL::Context,Configuration} instance to @ref create()
         * or @ref tryCreate(). See @ref GL-Context-usage for more information.
         */
        explicit GLContext(NoCreateT): GLContext{NoCreate, 0, nullptr} {}

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
         * @see @ref GLContext(Int, char**, const Configuration&),
         *      @ref GL-Context-usage, @fn_gl{Get} with @def_gl{MAJOR_VERSION},
         *      @def_gl{MINOR_VERSION}, @def_gl{CONTEXT_FLAGS},
         *      @def_gl{NUM_EXTENSIONS}, @fn_gl{GetString} with
         *      @def_gl{EXTENSIONS}
         */
        void create(const Configuration& configuration = Configuration{}) {
            return GL::Context::create(configuration);
        }

        /**
         * @brief Try to create the context
         *
         * Unlike @ref create(), instead of exiting prints a message to error
         * output and returns `false` on error.
         */
        bool tryCreate(const Configuration& configuration = Configuration{}) {
            return GL::Context::tryCreate(configuration);
        }
};

}}
#else
#error this header is available only in the OpenGL build
#endif

#endif
