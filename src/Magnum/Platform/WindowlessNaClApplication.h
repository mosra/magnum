#ifndef Magnum_Platform_WindowlessNaClApplication_h
#define Magnum_Platform_WindowlessNaClApplication_h
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
 * @brief Class @ref Magnum::Platform::WindowlessNaClApplication
 */

#include <string>
#include <Corrade/compatibility.h>
#include <Corrade/Containers/EnumSet.h>
#include <ppapi/cpp/instance.h>
#include <ppapi/cpp/module.h>
#include <ppapi/cpp/graphics_3d_client.h>
#include <ppapi/gles2/gl2ext_ppapi.h>

#include "Magnum/Magnum.h"

namespace pp {
    class Graphics3D;
    class Fullscreen;
}

namespace Magnum { namespace Platform {

/** @nosubgrouping
@brief Windowless NaCl application

Application for offscreen rendering running in
[Google Chrome Native Client](https://developers.google.com/native-client/).

This application library is available only in @ref CORRADE_TARGET_NACL "Native Client".
It is built if `WITH_WINDOWLESSNACLAPPLICATION` is enabled in CMake. To use it,
you need to request `%WindowlessNaClApplication` component in CMake, add
`${MAGNUM_WINDOWLESSNACLAPPLICATION_INCLUDE_DIRS}` to include path and link to
`${MAGNUM_WINDOWLESSNACLAPPLICATION_LIBRARIES}`. If no other windowless
application is requested, you can also use generic
`${MAGNUM_WINDOWLESSAPPLICATION_INCLUDE_DIRS}` and
`${MAGNUM_WINDOWLESSAPPLICATION_LIBRARIES}` aliases to simplify porting. See
@ref building and @ref cmake for more information.

@section WindowlessNaClApplication-usage Usage

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

@section WindowlessNaClApplication-html HTML markup and NMF file

You need to provide HTML markup containing `&lt;embed&gt;` pointing to `*.nmf`
file describing the application. See @ref NaClApplication for more information.
You may want to hide the `&lt;embed&gt;` (for example using CSS
`visibility: hidden;`), as it probably won't display anything to default
framebuffer.

@section WindowlessNaClApplication-console Redirecting output to Chrome's JavaScript console

The application redirects @ref Corrade::Utility::Debug "Debug",
@ref Corrade::Utility::Warning "Warning" and @ref Corrade::Utility::Error "Error"
output to JavaScript console. See also @ref Corrade::Utility::NaClConsoleStreamBuffer
for more information.
*/
class WindowlessNaClApplication: public pp::Instance, public pp::Graphics3DClient {
    public:
        /** @brief Application arguments */
        typedef PP_Instance Arguments;

        class Configuration;

        /** @copydoc Sdl2Application::Sdl2Application(const Arguments&, const Configuration&) */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        explicit WindowlessNaClApplication(const Arguments& arguments, const Configuration& configuration = Configuration());
        #else
        /* To avoid "invalid use of incomplete type" */
        explicit WindowlessNaClApplication(const Arguments& arguments, const Configuration& configuration);
        explicit WindowlessNaClApplication(const Arguments& arguments);
        #endif

        /** @copydoc Sdl2Application::Sdl2Application(const Arguments&, std::nullptr_t) */
        #ifndef CORRADE_GCC45_COMPATIBILITY
        explicit WindowlessNaClApplication(const Arguments& arguments, std::nullptr_t);
        #else
        explicit WindowlessNaClApplication(const Arguments& arguments, void*);
        #endif

        /** @brief Copying is not allowed */
        WindowlessNaClApplication(const WindowlessNaClApplication&) = delete;

        /** @brief Moving is not allowed */
        WindowlessNaClApplication(WindowlessNaClApplication&&) = delete;

        /** @brief Copying is not allowed */
        WindowlessNaClApplication& operator=(const WindowlessNaClApplication&) = delete;

        /** @brief Moving is not allowed */
        WindowlessNaClApplication& operator=(WindowlessNaClApplication&&) = delete;

        /**
         * @brief Execute application
         * @return Value for returning from `main()`.
         */
        virtual int exec() = 0;

    protected:
        /* Nobody will need to have (and delete) WindowlessNaClApplication*,
           thus this is faster than public pure virtual destructor */
        ~WindowlessNaClApplication();

        /** @copydoc Sdl2Application::createContext() */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        void createContext(const Configuration& configuration = Configuration());
        #else
        /* To avoid "invalid use of incomplete type" */
        void createContext(const Configuration& configuration);
        void createContext();
        #endif

        /** @copydoc Sdl2Application::tryCreateContext() */
        bool tryCreateContext(const Configuration& configuration);

    private:
        struct ConsoleDebugOutput;

        void Graphics3DContextLost() override;

        bool Init(std::uint32_t, const char*, const char*) override;

        pp::Graphics3D* graphics;
        Context* c;
        ConsoleDebugOutput* debugOutput;
};

/**
@brief %Configuration

@see @ref WindowlessNaClApplication(), @ref createContext(),
    @ref tryCreateContext()
*/
class WindowlessNaClApplication::Configuration {
    public:
        constexpr /*implicit*/ Configuration() {}
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
@param application  Application class name

See @ref Magnum::Platform::WindowlessNaClApplication "Platform::WindowlessNaClApplication"
and @ref portability-applications for more information. When no other
windowless application header is included this macro is also aliased to
`MAGNUM_WINDOWLESSAPPLICATION_MAIN()`.
*/
/* look at that insane placement of __attribute__. WTF. */
#define MAGNUM_WINDOWLESSNACLAPPLICATION_MAIN(application)                  \
    namespace pp {                                                          \
        Module __attribute__ ((visibility ("default"))) * CreateModule();   \
        Module __attribute__ ((visibility ("default"))) * CreateModule() {  \
            return new Magnum::Platform::Implementation::WindowlessNaClModule<application>(); \
        }                                                                   \
    }

#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef MAGNUM_WINDOWLESSAPPLICATION_MAIN
typedef WindowlessNaClApplication WindowlessApplication;
#define MAGNUM_WINDOWLESSAPPLICATION_MAIN(className) MAGNUM_WINDOWLESSNACLAPPLICATION_MAIN(className)
#else
#undef MAGNUM_WINDOWLESSAPPLICATION_MAIN
#endif
#endif

}}

#endif
