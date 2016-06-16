#ifndef Magnum_Platform_WindowlessNaClApplication_h
#define Magnum_Platform_WindowlessNaClApplication_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016
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
 * @brief Class @ref Magnum::Platform::WindowlessNaClApplication, @ref Magnum::Platform::WindowlessNaClContext, macro @ref MAGNUM_WINDOWLESSNACLAPPLICATION_MAIN()
 */

#include <memory>
#include <string>
#include <Corrade/Containers/EnumSet.h>

/* Needs to be first */
#include "Magnum/OpenGL.h"

#include <ppapi/cpp/instance.h>
#include <ppapi/cpp/module.h>
#include <ppapi/cpp/graphics_3d_client.h>
#include <ppapi/gles2/gl2ext_ppapi.h>

#include "Magnum/Magnum.h"
#include "Magnum/Tags.h"
#include "Magnum/Platform/Platform.h"

namespace pp {
    class Graphics3D;
    class Fullscreen;
}

namespace Magnum { namespace Platform {

/**
@brief Windowless NaCl context

GL context running in [Google Chrome Native Client](https://developers.google.com/native-client/),
used in @ref WindowlessNaClApplication. Does not have any default framebuffer.
It is built if `WITH_WINDOWLESSNACLAPPLICATION` is enabled in CMake.

Meant to be used when there is a need to manage (multiple) GL contexts
manually. See @ref platform-windowless-contexts for more information. If no
other application header is included, this class is also aliased to
`Platform::WindowlessGLContext`.
*/
class WindowlessNaClContext {
    public:
        class Configuration;

        /**
         * @brief Constructor
         * @param instance      Pepper instance handle
         * @param configuration Context configuration
         * @param context       Optional Magnum context instance constructed
         *      using @ref NoCreate to manage driver workarounds
         *
         * Once the context is created, make it current using @ref makeCurrent()
         * and create @ref Platform::Context instance to be able to use Magnum.
         * @see @ref isCreated()
         */
        explicit WindowlessNaClContext(pp::Instance& instance, const Configuration& configuration, const Context* context = nullptr);

        /**
         * @brief Construct without creating the context
         *
         * Move a instance with created context over to make it usable.
         */
        explicit WindowlessNaClContext(NoCreateT) {}

        /** @brief Copying is not allowed */
        WindowlessNaClContext(const WindowlessNaClContext&) = delete;

        /** @brief Move constructor */
        WindowlessNaClContext(WindowlessNaClContext&&);

        /** @brief Copying is not allowed */
        WindowlessNaClContext& operator=(const WindowlessNaClContext&) = delete;

        /** @brief Move assignment */
        WindowlessNaClContext& operator=(WindowlessNaClContext&&);

        /**
         * @brief Destructor
         *
         * Destroys the context, if any.
         */
        ~WindowlessNaClContext();

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
        std::unique_ptr<pp::Graphics3D> _context;
};

/**
@brief Configuration

@see @ref WindowlessNaClContext(),
    @ref WindowlessNaClApplication::WindowlessCglApplication(),
    @ref WindowlessNaClApplication::createContext(),
    @ref WindowlessNaClApplication::tryCreateContext()
*/
class WindowlessNaClContext::Configuration {
    public:
        constexpr /*implicit*/ Configuration() {}
};

/** @nosubgrouping
@brief Windowless NaCl application

Application for offscreen rendering using @ref WindowlessNaClContext. This
application library is available only in @ref CORRADE_TARGET_NACL "Native Client",
see respective sections in @ref building-corrade-cross-nacl "Corrade's" and
@ref building-cross-nacl "Magnum's" building documentation. It is built if
`WITH_WINDOWLESSNACLAPPLICATION` is enabled in CMake.

## Bootstrap application

The usage is very similar to @ref NaClApplication, for which fully contained
base application along with CMake setup is available, see its documentation for
more information.

## General Usage

In CMake you need to request `WindowlessNaClApplication` component, add
`${MAGNUM_WINDOWLESSNACLAPPLICATION_INCLUDE_DIRS}` to include path and link to
`${MAGNUM_WINDOWLESSNACLAPPLICATION_LIBRARIES}`. If no other windowless
application is requested, you can also use generic
`${MAGNUM_WINDOWLESSAPPLICATION_INCLUDE_DIRS}` and
`${MAGNUM_WINDOWLESSAPPLICATION_LIBRARIES}` aliases to simplify porting. See
@ref building and @ref cmake for more information.

Place your code into @ref exec(). The subclass must be then registered to NaCl
API using @ref MAGNUM_WINDOWLESSNACLAPPLICATION_MAIN() macro. See @ref platform
for more information.
@code
class MyApplication: public Platform::WindowlessNaClApplication {
    // implement required methods...
};
MAGNUM_WINDOWLESSNACLAPPLICATION_MAIN(MyApplication)
@endcode

If no other application header is included, this class is also aliased to
`Platform::WindowlessApplication` and the macro is aliased to
`MAGNUM_WINDOWLESSAPPLICATION_MAIN()` to simplify porting.

### HTML markup and NMF file

You need to provide HTML markup containing `&lt;embed&gt;` pointing to `*.nmf`
file describing the application. See @ref NaClApplication for more information.
You may want to hide the `&lt;embed&gt;` (for example using CSS
`visibility: hidden;`), as it probably won't display anything to default
framebuffer.

## Redirecting output to Chrome's JavaScript console

The application by default redirects @ref Corrade::Utility::Debug "Debug",
@ref Corrade::Utility::Warning "Warning" and @ref Corrade::Utility::Error "Error"
output to JavaScript console. See also @ref Corrade::Utility::NaClConsoleStreamBuffer
for more information.
*/
class WindowlessNaClApplication: public pp::Instance, public pp::Graphics3DClient {
    public:
        /** @brief Application arguments */
        typedef PP_Instance Arguments;

        /**
         * @brief Configuration
         *
         * @see @ref WindowlessNaClApplication(), @ref createContext(),
         *      @ref tryCreateContext()
         */
        typedef WindowlessNaClContext::Configuration Configuration;

        /**
         * @brief Default constructor
         * @param arguments     Application arguments
         * @param configuration Configuration
         *
         * Creates application with default or user-specified configuration.
         * See @ref Configuration for more information. The program exits if
         * the context cannot be created, see @ref tryCreateContext() for an
         * alternative.
         * @see @ref WindowlessNaClContext
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        explicit WindowlessNaClApplication(const Arguments& arguments, const Configuration& configuration = Configuration());
        #else
        /* To avoid "invalid use of incomplete type" */
        explicit WindowlessNaClApplication(const Arguments& arguments, const Configuration& configuration);
        explicit WindowlessNaClApplication(const Arguments& arguments);
        #endif

        /**
         * @brief Constructor
         * @param arguments     Application arguments
         *
         * Unlike above, the context is not created and must be created later
         * with @ref createContext() or @ref tryCreateContext().
         */
        explicit WindowlessNaClApplication(const Arguments& arguments, NoCreateT);

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @copybrief WindowlessNaClApplication(const Arguments&, NoCreateT)
         * @deprecated Use @ref WindowlessNaClApplication(const Arguments&, NoCreateT) instead.
         */
        CORRADE_DEPRECATED("use WindowlessNaClApplication(const Arguments&, NoCreateT) instead") explicit WindowlessNaClApplication(const Arguments& arguments, std::nullptr_t): WindowlessNaClApplication{arguments, NoCreate} {}
        #endif

        /** @brief Copying is not allowed */
        WindowlessNaClApplication(const WindowlessNaClApplication&) = delete;

        /** @brief Moving is not allowed */
        WindowlessNaClApplication(WindowlessNaClApplication&&) = delete;

        /** @brief Copying is not allowed */
        WindowlessNaClApplication& operator=(const WindowlessNaClApplication&) = delete;

        /** @brief Moving is not allowed */
        WindowlessNaClApplication& operator=(WindowlessNaClApplication&&) = delete;

    #ifdef DOXYGEN_GENERATING_OUTPUT
    protected:
    #else
    private:
    #endif
        /**
         * @brief Execute application
         * @return Value for returning from `main()`
         *
         * This function is not meant to be called from user code, see
         * @ref MAGNUM_WINDOWLESSNACLAPPLICATION_MAIN() for usage information.
         */
        virtual int exec() = 0;

    protected:
        /* Nobody will need to have (and delete) WindowlessNaClApplication*,
           thus this is faster than public pure virtual destructor */
        ~WindowlessNaClApplication();

        /**
         * @brief Create context with given configuration
         *
         * Must be called if and only if the context wasn't created by the
         * constructor itself. Error message is printed and the program exits
         * if the context cannot be created, see @ref tryCreateContext() for an
         * alternative.
         * @see @ref WindowlessNaClContext
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
        struct ConsoleDebugOutput;

        void Graphics3DContextLost() override;

        bool Init(std::uint32_t, const char*, const char*) override;

        WindowlessNaClContext _glContext;
        std::unique_ptr<Platform::Context> _context;
        std::unique_ptr<ConsoleDebugOutput> _debugOutput;
};

namespace Implementation {
    template<class Application> class WindowlessNaClModule: public pp::Module {
        public:
            ~WindowlessNaClModule() { glTerminatePPAPI(); }

            bool Init() override {
                return glInitializePPAPI(get_browser_interface());
            }

            pp::Instance* CreateInstance(PP_Instance instance) {
                return new Application(instance);
            }
    };
}

/** @hideinitializer
@brief Entry point for windowless NaCl application
@param className Class name

See @ref Magnum::Platform::WindowlessNaClApplication "Platform::WindowlessNaClApplication"
for usage information. This macro abstracts out platform-specific entry point
code (the classic `main()` function cannot be used in NaCl). See
@ref portability-applications for more information. When no other windowless
application header is included this macro is also aliased to
`MAGNUM_WINDOWLESSAPPLICATION_MAIN()`.
*/
/* look at that insane placement of __attribute__. WTF. */
#define MAGNUM_WINDOWLESSNACLAPPLICATION_MAIN(className)                    \
    namespace pp {                                                          \
        Module __attribute__ ((visibility ("default"))) * CreateModule();   \
        Module __attribute__ ((visibility ("default"))) * CreateModule() {  \
            return new Magnum::Platform::Implementation::WindowlessNaClModule<className>(); \
        }                                                                   \
    }

#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef MAGNUM_WINDOWLESSAPPLICATION_MAIN
typedef WindowlessNaClApplication WindowlessApplication;
typedef WindowlessNaClContext WindowlessGLContext;
#define MAGNUM_WINDOWLESSAPPLICATION_MAIN(className) MAGNUM_WINDOWLESSNACLAPPLICATION_MAIN(className)
#else
#undef MAGNUM_WINDOWLESSAPPLICATION_MAIN
#endif
#endif

}}

#endif
