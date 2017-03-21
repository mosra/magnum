#ifndef Magnum_Platform_WindowlessIosApplication_h
#define Magnum_Platform_WindowlessIosApplication_h
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

/** @file
 * @brief Class @ref Magnum::Platform::WindowlessIosApplication, @ref Magnum::Platform::WindowlessIosContext, macro @ref MAGNUM_WINDOWLESSIOSAPPLICATION_MAIN()
 */

#include <memory>
#include <Corrade/Containers/EnumSet.h>

#include "Magnum/Magnum.h"
#include "Magnum/OpenGL.h"
#include "Magnum/Tags.h"
#include "Magnum/Platform/Platform.h"

#ifdef __OBJC__
@class EAGLContext;
#else
struct EAGLContext;
#endif

namespace Magnum { namespace Platform {

/**
@brief Windowless iOS context

GL context using EAGL on iOS, used in @ref WindowlessIosApplication. Does not
have any default framebuffer. It is built if `WITH_WINDOWLESSIOSAPPLICATION` is
enabled in CMake.

Meant to be used when there is a need to manage (multiple) GL contexts
manually. See @ref platform-windowless-contexts for more information. If no
other application header is included, this class is also aliased to
`Platform::WindowlessGLContext`.
*/
class WindowlessIosContext {
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
        explicit WindowlessIosContext(const Configuration& configuration, Context* context = nullptr);

        /**
         * @brief Construct without creating the context
         *
         * Move a instance with created context over to make it usable.
         */
        explicit WindowlessIosContext(NoCreateT) {}

        /** @brief Copying is not allowed */
        WindowlessIosContext(const WindowlessIosContext&) = delete;

        /** @brief Move constructor */
        WindowlessIosContext(WindowlessIosContext&& other);

        /** @brief Copying is not allowed */
        WindowlessIosContext& operator=(const WindowlessIosContext&) = delete;

        /** @brief Move assignment */
        WindowlessIosContext& operator=(WindowlessIosContext&& other);

        /**
         * @brief Destructor
         *
         * Destroys the context, if any.
         */
        ~WindowlessIosContext();

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
        EAGLContext* _context{};
};

/**
@brief Configuration

@see @ref WindowlessIosContext(),
    @ref WindowlessIosApplication::WindowlessIosApplication(),
    @ref WindowlessIosApplication::createContext(),
    @ref WindowlessIosApplication::tryCreateContext()
*/
class WindowlessIosContext::Configuration {
    public:
        constexpr /*implicit*/ Configuration() {}
};

/**
@brief Windowless iOS application

Application for offscreen rendering using @ref WindowlessIosContext. Does not
have any default framebuffer. It is built if `WITH_WINDOWLESSIOSAPPLICATION` is
enabled in CMake.

## Bootstrap application

Fully contained windowless application using @ref WindowlessIosApplication
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

In CMake you need to request `WindowlessIosApplication` component and link to
`Magnum::WindowlessIosApplication` target. If no other windowless application
is requested, you can also use generic `Magnum::WindowlessApplication` alias to
simplify porting. Again, see @ref building and @ref cmake for more information.

Place your code into @ref exec(). The subclass can be then used in main
function using @ref MAGNUM_WINDOWLESSIOSAPPLICATION_MAIN() macro. See
@ref platform for more information.
@code
class MyApplication: public Platform::WindowlessIosApplication {
    // implement required methods...
};
MAGNUM_WINDOWLESSIOSAPPLICATION_MAIN(MyApplication)
@endcode

If no other application header is included, this class is also aliased to
`Platform::WindowlessApplication` and the macro is aliased to
`MAGNUM_WINDOWLESSAPPLICATION_MAIN()` to simplify porting.
*/
class WindowlessIosApplication {
    public:
        /** @brief Application arguments */
        struct Arguments {
            /** @brief Constructor */
            /*implicit*/ constexpr Arguments(int& argc, char** argv) noexcept: argc{argc}, argv{argv} {}

            int& argc;      /**< @brief Argument count */
            char** argv;    /**< @brief Argument values */
        };

        /**
         * @brief Configuration
         *
         * @see @ref WindowlessIosApplication(), @ref createContext(),
         *      @ref tryCreateContext()
         */
        typedef WindowlessIosContext::Configuration Configuration;

        /**
         * @brief Default constructor
         * @param arguments     Application arguments
         * @param configuration Configuration
         *
         * Creates application with default or user-specified configuration.
         * See @ref Configuration for more information. The program exits if
         * the context cannot be created, see @ref tryCreateContext() for an
         * alternative.
         * @see @ref WindowlessIosContext
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        explicit WindowlessIosApplication(const Arguments& arguments, const Configuration& configuration = Configuration());
        #else
        /* To avoid "invalid use of incomplete type" */
        explicit WindowlessIosApplication(const Arguments& arguments, const Configuration& configuration);
        explicit WindowlessIosApplication(const Arguments& arguments);
        #endif

        /**
         * @brief Constructor
         * @param arguments     Application arguments
         *
         * Unlike above, the context is not created and must be created later
         * with @ref createContext() or @ref tryCreateContext().
         */
        explicit WindowlessIosApplication(const Arguments& arguments, NoCreateT);

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @copybrief WindowlessIosApplication(const Arguments&, NoCreateT)
         * @deprecated Use @ref WindowlessIosApplication(const Arguments&, NoCreateT) instead.
         */
        CORRADE_DEPRECATED("use WindowlessIosApplication(const Arguments&, NoCreateT) instead") explicit WindowlessIosApplication(const Arguments& arguments, std::nullptr_t): WindowlessIosApplication{arguments, NoCreate} {}
        #endif

        /** @brief Copying is not allowed */
        WindowlessIosApplication(const WindowlessIosApplication&) = delete;

        /** @brief Moving is not allowed */
        WindowlessIosApplication(WindowlessIosApplication&&) = delete;

        /** @brief Copying is not allowed */
        WindowlessIosApplication& operator=(const WindowlessIosApplication&) = delete;

        /** @brief Moving is not allowed */
        WindowlessIosApplication& operator=(WindowlessIosApplication&&) = delete;

        /**
         * @brief Execute application
         * @return Value for returning from `main()`
         *
         * See @ref MAGNUM_WINDOWLESSIOSAPPLICATION_MAIN() for usage
         * information.
         */
        virtual int exec() = 0;

    protected:
        /* Nobody will need to have (and delete) WindowlessIosApplication*,
           thus this is faster than public pure virtual destructor */
        ~WindowlessIosApplication();

        /**
         * @brief Create context with given configuration
         *
         * Must be called if and only if the context wasn't created by the
         * constructor itself. Error message is printed and the program exits
         * if the context cannot be created, see @ref tryCreateContext() for an
         * alternative.
         * @see @ref WindowlessIosContext
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
        WindowlessIosContext _glContext;
        std::unique_ptr<Platform::Context> _context;
};

/** @hideinitializer
@brief Entry point for windowless EGL application
@param className Class name

See @ref Magnum::Platform::WindowlessIosApplication "Platform::WindowlessIosApplication"
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
#define MAGNUM_WINDOWLESSIOSAPPLICATION_MAIN(className)                     \
    int main(int argc, char** argv) {                                       \
        className app({argc, argv});                                        \
        return app.exec();                                                  \
    }

#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef MAGNUM_WINDOWLESSAPPLICATION_MAIN
typedef WindowlessIosApplication WindowlessApplication;
typedef WindowlessIosContext WindowlessGLContext;
#define MAGNUM_WINDOWLESSAPPLICATION_MAIN(className) MAGNUM_WINDOWLESSIOSAPPLICATION_MAIN(className)
#else
#undef MAGNUM_WINDOWLESSAPPLICATION_MAIN
#endif
#endif

}}

#endif
