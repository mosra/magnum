#ifndef Magnum_Platform_WindowlessNaClApplication_h
#define Magnum_Platform_WindowlessNaClApplication_h
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
 * @brief Class Magnum::Platform::WindowlessNaClApplication
 */

#include <string>
#include <Containers/EnumSet.h>
#include <ppapi/cpp/instance.h>
#include <ppapi/cpp/module.h>
#include <ppapi/cpp/graphics_3d_client.h>
#include <ppapi/gles2/gl2ext_ppapi.h>

#include "Math/Vector2.h"
#include "Magnum.h"

#include "corradeCompatibility.h"

namespace pp {
    class Graphics3D;
    class Fullscreen;
}

namespace Magnum { namespace Platform {

/** @nosubgrouping
@brief Windowless NaCl application

Application running in [Google Chrome Native Client](https://developers.google.com/native-client/).
See @ref platform for brief introduction.

@section WindowlessNaClApplication-usage Usage

Place your code into exec(). The subclass must be then registered to NaCl API
using MAGNUM_WINDOWLESSNACLAPPLICATION_MAIN() macro.
@code
class MyApplication: public Magnum::Platform::WindowlessNaClApplication {
    // implement required methods...
};
MAGNUM_WINDOWLESSNACLAPPLICATION_MAIN(MyApplication)
@endcode

If no other application header is included this class is also aliased to
`Platform::WindowlessApplication` and the macro is aliased to
`MAGNUM_WINDOWLESSAPPLICATION_MAIN()` to simplify porting.

@section WindowlessNaClApplication-html HTML markup and NMF file

You need to provide HTML markup containing `&lt;embed&gt;` pointing to `*.nmf`
file describing the application.

@todoc Document this better, add "bootstrap" examples

@subsection WindowlessNaClApplication-html-console Redirecting output to Chrome's JavaScript console

The application redirects @ref Debug, @ref Warning and @ref Error output to
JavaScript console. See also @ref Corrade::Utility::NaClConsoleStreamBuffer for
more information.
*/
class WindowlessNaClApplication: public pp::Instance, public pp::Graphics3DClient {
    public:
        /** @brief Application arguments */
        typedef PP_Instance Arguments;

        class Configuration;

        /**
         * @brief Default constructor
         * @param arguments     Application arguments
         *
         * Creates application with default configuration. See Configuration
         * for more information.
         */
        explicit WindowlessNaClApplication(const Arguments& arguments);

        /**
         * @brief Constructor
         * @param arguments     Application arguments
         * @param configuration Configuration
         *
         * The @p configuration is deleted afterwards. If `nullptr` is passed
         * as @p configuration, the context is not created and must be created
         * with createContext().
         */
        explicit WindowlessNaClApplication(const Arguments& arguments, Configuration* configuration);

        /**
         * @brief Execute application
         * @return Value for returning from `main()`.
         */
        virtual int exec() = 0;

    protected:
        /* Nobody will need to have (and delete) WindowlessNaClApplication*,
           thus this is faster than public pure virtual destructor */
        ~WindowlessNaClApplication();

        /** @copydoc GlutApplication::createContext() */
        void createContext(Configuration* configuration);

        /** @copydoc GlutApplication::tryCreateContext() */
        bool tryCreateContext(Configuration* configuration);

    private:
        struct ConsoleDebugOutput;

        void Graphics3DContextLost() override {
            CORRADE_ASSERT(false, "NaClApplication: context unexpectedly lost", );
        }

        bool Init(std::uint32_t, const char*, const char*) override;

        pp::Graphics3D* graphics;
        Context* c;
        ConsoleDebugOutput* debugOutput;
};

/**
@brief %Configuration

@see WindowlessNaClApplication(), createContext(), tryCreateContext()
*/
class WindowlessNaClApplication::Configuration {
    Configuration(const Configuration&) = delete;
    Configuration(Configuration&&) = delete;
    Configuration& operator=(const Configuration&) = delete;
    Configuration& operator=(Configuration&&) = delete;

    public:
        constexpr explicit Configuration() {}
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

See WindowlessNaClApplication and @ref portability-applications for more
information. When no other windowless application header is included this macro
is also aliased to `MAGNUM_WINDOWLESSAPPLICATION_MAIN()`.
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
