#ifndef Magnum_Platform_WindowlessEglApplication_h
#define Magnum_Platform_WindowlessEglApplication_h
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

/** @file
 * @brief Class @ref Magnum::Platform::WindowlessEglApplication, @ref Magnum::Platform::WindowlessEglContext, macro @ref MAGNUM_WINDOWLESSEGLAPPLICATION_MAIN()
 */

#include <memory>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <Corrade/Containers/EnumSet.h>
/* undef Xlib nonsense to avoid conflicts */
#undef Always
#undef Complex
#undef None
#undef Status

#include "Magnum/Magnum.h"
#include "Magnum/OpenGL.h"
#include "Magnum/Tags.h"
#include "Magnum/Platform/Platform.h"

namespace Magnum { namespace Platform {

/**
@brief Windowless EGL context

GL context using EGL without any windowing system, used in
@ref WindowlessEglApplication. Does not have any default framebuffer. It is
built if `WITH_WINDOWLESSEGLAPPLICATION` is enabled in CMake.

Meant to be used when there is a need to manage (multiple) GL contexts
manually. See @ref platform-windowless-contexts for more information. If no
other application header is included, this class is also aliased to
`Platform::WindowlessGLContext`.
*/
class WindowlessEglContext {
    public:
        class Configuration;

        /**
         * @brief Constructor
         * @param configuration Context configuration
         * @param context       Optional Magnum context instance constructed
         *      using @ref NoCreate to manage driver workarounds
         *
         * Once the context is created, make it current using @ref makeCurrent()
         * and create @ref Platform::Context instance to be able to use Magnum.
         * @see @ref isCreated()
         */
        explicit WindowlessEglContext(const Configuration& configuration, Context* context = nullptr);

        /**
         * @brief Construct without creating the context
         *
         * Move a instance with created context over to make it usable.
         */
        explicit WindowlessEglContext(NoCreateT) {}

        /** @brief Copying is not allowed */
        WindowlessEglContext(const WindowlessEglContext&) = delete;

        /** @brief Move constructor */
        WindowlessEglContext(WindowlessEglContext&& other);

        /** @brief Copying is not allowed */
        WindowlessEglContext& operator=(const WindowlessEglContext&) = delete;

        /** @brief Move assignment */
        WindowlessEglContext& operator=(WindowlessEglContext&& other);

        /**
         * @brief Destructor
         *
         * Destroys the context, if any.
         */
        ~WindowlessEglContext();

        /** @brief Whether the context is created */
        bool isCreated() const { return _context; }

        /**
         * @brief Make the context current
         *
         * Prints error message and returns `false` on failure, otherwise
         * returns `true`.
         */
        bool makeCurrent();

    private:
        EGLDisplay _display{};
        EGLContext _context{};
};

/**
@brief Configuration

@see @ref WindowlessEglContext(),
    @ref WindowlessEglApplication::WindowlessEglApplication(),
    @ref WindowlessEglApplication::createContext(),
    @ref WindowlessEglApplication::tryCreateContext()
*/
class WindowlessEglContext::Configuration {
    public:
        /**
         * @brief Context flag
         *
         * @see @ref Flags, @ref setFlags(), @ref Context::Flag
         */
        enum class Flag: int {
            Debug = EGL_CONTEXT_OPENGL_DEBUG_BIT_KHR    /**< Create debug context */
        };

        /**
         * @brief Context flags
         *
         * @see @ref setFlags(), @ref Context::Flags
         */
        #ifndef DOXYGEN_GENERATING_OUTPUT
        typedef Containers::EnumSet<Flag, EGL_CONTEXT_OPENGL_DEBUG_BIT_KHR> Flags;
        #else
        typedef Containers::EnumSet<Flag> Flags;
        #endif

        constexpr /*implicit*/ Configuration() {}

        /** @brief Context flags */
        Flags flags() const { return _flags; }

        /**
         * @brief Set context flags
         * @return Reference to self (for method chaining)
         *
         * Default is no flag. See also @ref Context::flags().
         */
        Configuration& setFlags(Flags flags) {
            _flags = flags;
            return *this;
        }

    private:
        Flags _flags;
};

/**
@brief Windowless EGL application

Application for offscreen rendering using @ref WindowlessEglContext. Supported
mainly on OpenGL ES drivers, for desktop OpenGL the only driver that supports
this configuration is NVidia >= 355. See other `Windowless*Application` classes
for an alternative.

It is built if `WITH_WINDOWLESSEGLAPPLICATION` is enabled in CMake.

## Bootstrap application

Fully contained windowless application using @ref WindowlessEglApplication
along with CMake setup is available in `windowless` branch of
[Magnum Bootstrap](https://github.com/mosra/magnum-bootstrap) repository,
download it as [tar.gz](https://github.com/mosra/magnum-bootstrap/archive/windowless.tar.gz)
or [zip](https://github.com/mosra/magnum-bootstrap/archive/windowless.zip)
file. After extracting the downloaded archive you can build and run the
application with these four commands:

    mkdir build && cd build
    cmake ..
    cmake --build .
    ./src/MyApplication # or ./src/Debug/MyApplication

See @ref cmake for more information.

## General usage

In CMake you need to request `WindowlessEglApplication` component and link to
`Magnum::WindowlessEglApplication` target. If no other windowless application
is requested, you can also use generic `Magnum::WindowlessApplication` alias to
simplify porting. Again, see @ref building and @ref cmake for more information.

Place your code into @ref exec(). The subclass can be then used in main
function using @ref MAGNUM_WINDOWLESSEGLAPPLICATION_MAIN() macro. See
@ref platform for more information.
@code
class MyApplication: public Platform::WindowlessEglApplication {
    // implement required methods...
};
MAGNUM_WINDOWLESSEGLAPPLICATION_MAIN(MyApplication)
@endcode

If no other application header is included, this class is also aliased to
`Platform::WindowlessApplication` and the macro is aliased to
`MAGNUM_WINDOWLESSAPPLICATION_MAIN()` to simplify porting.
*/
class WindowlessEglApplication {
    public:
        /** @brief Application arguments */
        struct Arguments {
            /** @brief Constructor */
            /*implicit*/ constexpr Arguments(int& argc, char** argv) noexcept: argc{argc}, argv{argv}{}

            int& argc;      /**< @brief Argument count */
            char** argv;    /**< @brief Argument values */
        };

        /**
         * @brief Configuration
         *
         * @see @ref WindowlessEglApplication(), @ref createContext(),
         *      @ref tryCreateContext()
         */
        typedef WindowlessEglContext::Configuration Configuration;

        /**
         * @brief Default constructor
         * @param arguments     Application arguments
         * @param configuration Configuration
         *
         * Creates application with default or user-specified configuration.
         * See @ref Configuration for more information. The program exits if
         * the context cannot be created, see @ref tryCreateContext() for an
         * alternative.
         * @see @ref WindowlessEglContext
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        explicit WindowlessEglApplication(const Arguments& arguments, const Configuration& configuration = Configuration());
        #else
        /* To avoid "invalid use of incomplete type" */
        explicit WindowlessEglApplication(const Arguments& arguments, const Configuration& configuration);
        explicit WindowlessEglApplication(const Arguments& arguments);
        #endif

        /**
         * @brief Constructor
         * @param arguments     Application arguments
         *
         * Unlike above, the context is not created and must be created later
         * with @ref createContext() or @ref tryCreateContext().
         */
        explicit WindowlessEglApplication(const Arguments& arguments, NoCreateT);

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @copybrief WindowlessEglApplication(const Arguments&, NoCreateT)
         * @deprecated Use @ref WindowlessEglApplication(const Arguments&, NoCreateT) instead.
         */
        CORRADE_DEPRECATED("use WindowlessEglApplication(const Arguments&, NoCreateT) instead") explicit WindowlessEglApplication(const Arguments& arguments, std::nullptr_t): WindowlessEglApplication{arguments, NoCreate} {}
        #endif

        /** @brief Copying is not allowed */
        WindowlessEglApplication(const WindowlessEglApplication&) = delete;

        /** @brief Moving is not allowed */
        WindowlessEglApplication(WindowlessEglApplication&&) = delete;

        /** @brief Copying is not allowed */
        WindowlessEglApplication& operator=(const WindowlessEglApplication&) = delete;

        /** @brief Moving is not allowed */
        WindowlessEglApplication& operator=(WindowlessEglApplication&&) = delete;

        /**
         * @brief Execute application
         * @return Value for returning from `main()`
         *
         * See @ref MAGNUM_WINDOWLESSEGLAPPLICATION_MAIN() for usage
         * information.
         */
        virtual int exec() = 0;

    protected:
        /* Nobody will need to have (and delete) WindowlessEglApplication*,
           thus this is faster than public pure virtual destructor */
        ~WindowlessEglApplication();

        /**
         * @brief Create context with given configuration
         *
         * Must be called if and only if the context wasn't created by the
         * constructor itself. Error message is printed and the program exits
         * if the context cannot be created, see @ref tryCreateContext() for an
         * alternative.
         * @see @ref WindowlessEglContext
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        void createContext(const Configuration& configuration = Configuration());
        #else
        /* To avoid "invalid use of incomplete type" */
        void createContext(const Configuration& configuration);
        void createContext();
        #endif

        /**
         * @brief Try to create context with given configuration
         *
         * Unlike @ref createContext() returns `false` if the context cannot be
         * created, `true` otherwise.
         */
        bool tryCreateContext(const Configuration& configuration);

    private:
        WindowlessEglContext _glContext;
        std::unique_ptr<Platform::Context> _context;
};

/** @hideinitializer
@brief Entry point for windowless EGL application
@param className Class name

See @ref Magnum::Platform::WindowlessEglApplication "Platform::WindowlessEglApplication"
for usage information. This macro abstracts out platform-specific entry point
code and is equivalent to the following, see @ref portability-applications for
more information.
@code
int main(int argc, char** argv) {
    className app({argc, argv});
    return app.exec();
}
@endcode
When no other windowless application header is included this macro is also
aliased to `MAGNUM_WINDOWLESSAPPLICATION_MAIN()`.
*/
#define MAGNUM_WINDOWLESSEGLAPPLICATION_MAIN(className)                     \
    int main(int argc, char** argv) {                                       \
        className app({argc, argv});                                        \
        return app.exec();                                                  \
    }

#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef MAGNUM_WINDOWLESSAPPLICATION_MAIN
typedef WindowlessEglApplication WindowlessApplication;
typedef WindowlessEglContext WindowlessGLContext;
#define MAGNUM_WINDOWLESSAPPLICATION_MAIN(className) MAGNUM_WINDOWLESSEGLAPPLICATION_MAIN(className)
#else
#undef MAGNUM_WINDOWLESSAPPLICATION_MAIN
#endif
#endif

}}

#endif
