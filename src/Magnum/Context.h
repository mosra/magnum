#ifndef Magnum_Context_h
#define Magnum_Context_h
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
 * @brief Class @ref Magnum::Context, @ref Magnum::Extension, macro @ref MAGNUM_ASSERT_VERSION_SUPPORTED(), @ref MAGNUM_ASSERT_EXTENSION_SUPPORTED()
 */

#include <cstdlib>
#include <array>
#include <bitset>
#include <vector>
#include <Corrade/Containers/EnumSet.h>

#include "Magnum/Magnum.h"
#include "Magnum/OpenGL.h"
#include "Magnum/Tags.h"
#include "Magnum/visibility.h"
#include "MagnumExternal/Optional/optional.hpp"

namespace Magnum {

namespace Implementation {
    struct ContextState;
    struct State;
}
namespace Platform { class Context; }

/**
@brief Run-time information about OpenGL extension

Encapsulates runtime information about OpenGL extension, such as name string,
minimal required OpenGL version and version in which the extension was adopted
to core.

See also @ref Extensions namespace, which contain compile-time information
about OpenGL extensions.
*/
class MAGNUM_EXPORT Extension {
    friend Context;

    public:
        /** @brief All extensions for given OpenGL version */
        static const std::vector<Extension>& extensions(Version version);

        /** @brief Minimal version required by this extension */
        constexpr Version requiredVersion() const { return _requiredVersion; }

        /** @brief Version in which this extension was adopted to core */
        constexpr Version coreVersion() const { return _coreVersion; }

        /** @brief Extension string */
        constexpr const char* string() const { return _string; }

    private:
        std::size_t _index;
        Version _requiredVersion;
        Version _coreVersion;
        const char* _string;

        constexpr Extension(std::size_t index, Version requiredVersion, Version coreVersion, const char* string): _index(index), _requiredVersion(requiredVersion), _coreVersion(coreVersion), _string(string) {}
};

/**
@brief Magnum context

Provides access to version and extension information. Instance available
through @ref Context::current() is automatically created during construction of
`*Application` classes in @ref Platform namespace. You can safely assume that
the instance is available during whole lifetime of `*Application` object. It's
also possible to create the context without using any `*Application` class
using @ref Platform::Context subclass, see @ref platform documentation for more
information.

## Command-line options

The context is configurable through command-line options, that are passed
either from the `Platform::*Application` classes or from the @ref Platform::Context
class. Usage:

    <application> [--magnum-help] [--magnum-disable-workarounds LIST] [--magnum-disable-extensions LIST] ...

Arguments:

-   `...` -- main application arguments (see `-h` or `--help` for details)
-   `--magnum-help` -- display this help message and exit
-   `--magnum-disable-workarounds LIST` -- driver workarounds to disable (see
    `src/Magnum/Implementation/driverSpecific.cpp` for detailed info)
    (environment: `MAGNUM_DISABLE_WORKAROUNDS`)
-   `--magnum-disable-extensions LIST` -- OpenGL extensions to disable
    (environment: `MAGNUM_DISABLE_EXTENSIONS`)

*/
class MAGNUM_EXPORT Context {
    friend Implementation::ContextState;
    friend Platform::Context;

    public:
        #ifndef MAGNUM_TARGET_WEBGL
        /**
         * @brief Context flag
         *
         * @see @ref Flags, @ref flags(),
         *      @ref Platform::Sdl2Application::Configuration::setFlags() "Platform::*Application::Configuration::setFlags()"
         * @requires_gles Context flags are not available in WebGL.
         */
        enum class Flag: GLint {
            /**
             * Debug context
             * @requires_gl43 Extension @extension{KHR,debug}
             * @requires_es_extension Extension @extension{ANDROID,extension_pack_es31a}/
             *      @extension2{KHR,debug,debug}
             */
            #ifndef MAGNUM_TARGET_GLES
            Debug = GL_CONTEXT_FLAG_DEBUG_BIT,
            #else
            Debug = GL_CONTEXT_FLAG_DEBUG_BIT_KHR,
            #endif

            /**
             * Context without error reporting
             * @requires_extension Extension @extension{KHR,no_error}
             * @requires_es_extension Extension @extension2{KHR,no_error,no_error}
             */
            NoError = GL_CONTEXT_FLAG_NO_ERROR_BIT_KHR,

            #ifndef MAGNUM_TARGET_GLES
            /**
             * Context with robust access
             * @requires_extension Extension @extension{ARB,robustness}
             * @requires_es_extension Extension @extension{EXT,robustness}
             * @todo In ES available under glGetIntegerv(CONTEXT_ROBUST_ACCESS_EXT),
             *      how to make it compatible?
             */
            RobustAccess = GL_CONTEXT_FLAG_ROBUST_ACCESS_BIT_ARB
            #endif
        };

        /**
         * @brief Context flags
         *
         * @see @ref flags()
         * @requires_gles Context flags are not available in WebGL.
         */
        typedef Containers::EnumSet<Flag> Flags;
        #endif

        /**
         * @brief State to reset
         *
         * @see @ref States, @ref resetState()
         */
        enum class State: UnsignedInt {
            /** Reset tracked buffer-related bindings and state */
            Buffers = 1 << 0,

            /** Reset tracked framebuffer-related bindings and state */
            Framebuffers = 1 << 1,

            /** Reset tracked mesh-related bindings */
            Meshes = 1 << 2,

            /** Reset tracked pixel storage-related state */
            PixelStorage = 1 << 3,

            /** Reset tracked renderer-related state */
            Renderer = 1 << 4,

            /** Reset tracked shader-related bindings */
            Shaders = 1 << 5,

            /** Reset tracked texture-related bindings and state */
            Textures = 1 << 6,

            #ifndef MAGNUM_TARGET_GLES2
            /** Reset tracked transform feedback-related bindings */
            TransformFeedback = 1 << 7
            #endif
        };

        /**
         * @brief States to reset
         *
         * @see @ref resetState()
         */
        typedef Containers::EnumSet<State> States;

        /**
         * @brief Detected driver
         *
         * @see @ref DetectedDriver, @ref detectedDriver()
         */
        enum class DetectedDriver: UnsignedShort {
            #ifndef MAGNUM_TARGET_WEBGL
            /**
             * Binary AMD desktop drivers on Windows and Linux
             * @requires_gles Not detectable on WebGL, as browsers
             *      intentionally hide most of the driver information.
             */
            Amd = 1 << 0,

            #ifdef MAGNUM_BUILD_DEPRECATED
            /** @copydoc DetectedDriver::Amd
             * @deprecated Use @ref DetectedDriver::Amd instead.
             */
            AMD CORRADE_DEPRECATED_ENUM("use DetectedDriver::Amd instead") = UnsignedShort(DetectedDriver::Amd),
            #endif
            #endif

            #ifdef MAGNUM_TARGET_GLES
            /**
             * OpenGL ES implementation by ANGLE (translated to D3D), used by
             * browsers on Windows for WebGL. As the WebGL specification
             * explicitly disallows exposing driver information to the
             * application, this check cannot be done reliably.
             */
            Angle = 1 << 1,

            #ifdef MAGNUM_BUILD_DEPRECATED
            /** @copydoc DetectedDriver::Angle
             * @deprecated Use @ref DetectedDriver::Angle instead.
             */
            ProbablyAngle CORRADE_DEPRECATED_ENUM("use DetectedDriver::Angle instead") = UnsignedShort(DetectedDriver::Angle),
            #endif
            #endif

            #ifndef MAGNUM_TARGET_WEBGL
            /**
             * Intel desktop drivers on Windows
             * @requires_gles Not detectable on WebGL, as browsers
             *      intentionally hide most of the driver information.
             */
            IntelWindows = 1 << 2,

            /**
             * Mesa drivers on Windows and Linux. See also
             * @ref DetectedDriver::Svga3D.
             * @requires_gles Not detectable on WebGL, as browsers
             *      intentionally hide most of the driver information.
             */
            Mesa = 1 << 3,

            /**
             * Binary NVidia drivers on Windows and Linux
             * @requires_gles Not detectable on WebGL, as browsers
             *      intentionally hide most of the driver information.
             */
            NVidia = 1 << 4,
            #endif

            #ifndef MAGNUM_TARGET_WEBGL
            /**
             * VMware guest GL driver SVGA3D, implemented using Mesa, both
             * Windows and Linux guests. See https://www.mesa3d.org/vmware-guest.html
             * for more information. Detected in combination with
             * @ref DetectedDriver::Mesa.
             * @requires_gles Not detectable on WebGL, as browsers
             *      intentionally hide most of the driver information.
             */
            Svga3D = 1 << 5
            #endif
        };

        /**
         * @brief Detected drivers
         *
         * @see @ref detectedDriver()
         */
        typedef Containers::EnumSet<DetectedDriver> DetectedDrivers;

        /**
         * @brief Whether there is any current context
         *
         * If Magnum is built with @ref MAGNUM_BUILD_MULTITHREADED, current
         * context is thread-local instead of global (the default).
         * @see @ref current()
         */
        static bool hasCurrent();

        /**
         * @brief Current context
         *
         * Expect that there is current context. If Magnum is built with
         * @ref MAGNUM_BUILD_MULTITHREADED, current context is thread-local
         * instead of global (the default).
         * @see @ref hasCurrent()
         */
        static Context& current();

        /** @brief Copying is not allowed */
        Context(const Context&) = delete;

        /** @brief Move constructor */
        Context(Context&& other);

        ~Context();

        /** @brief Copying is not allowed */
        Context& operator=(const Context&) = delete;

        /** @brief Move assignment is not allowed */
        Context& operator=(Context&&) = delete;

        #if defined(MAGNUM_BUILD_DEPRECATED) && !defined(DOXYGEN_GENERATING_OUTPUT)
        CORRADE_DEPRECATED("Context::current() returns reference now") Context* operator->() { return this; }
        CORRADE_DEPRECATED("Context::current() returns reference now") operator Context*() { return this; }
        #endif

        /**
         * @brief OpenGL version
         *
         * @see @ref versionString(), @ref shadingLanguageVersionString()
         */
        Version version() const { return _version; }

        /**
         * @brief Vendor string
         *
         * The result is *not* cached, repeated queries will result in repeated
         * OpenGL calls.
         * @see @ref rendererString(), @fn_gl{GetString} with @def_gl{VENDOR}
         */
        std::string vendorString() const;

        /**
         * @brief Renderer string
         *
         * The result is *not* cached, repeated queries will result in repeated
         * OpenGL calls.
         * @see @ref vendorString(), @fn_gl{GetString} with @def_gl{RENDERER}
         */
        std::string rendererString() const;

        /**
         * @brief Version string
         *
         * The result is *not* cached, repeated queries will result in repeated
         * OpenGL calls.
         * @see @ref shadingLanguageVersionString(), @ref version(),
         *      @fn_gl{GetString} with @def_gl{VERSION}
         */
        std::string versionString() const;

        /**
         * @brief Shading language version string
         *
         * The result is *not* cached, repeated queries will result in repeated
         * OpenGL calls.
         * @see @ref versionString(), @ref version(), @fn_gl{GetString} with
         *      @def_gl{SHADING_LANGUAGE_VERSION}
         */
        std::string shadingLanguageVersionString() const;

        /**
         * @brief Shading language version strings
         *
         * The result is *not* cached, repeated queries will result in repeated
         * OpenGL calls.
         * @see @ref versionString(), @ref version(), @fn_gl{Get} with
         *      @def_gl{NUM_SHADING_LANGUAGE_VERSIONS}, @fn_gl{GetString} with
         *      @def_gl{SHADING_LANGUAGE_VERSION}
         */
        std::vector<std::string> shadingLanguageVersionStrings() const;

        /**
         * @brief Extension strings
         *
         * The result is *not* cached, repeated queries will result in repeated
         * OpenGL calls. Note that this function returns list of all extensions
         * reported by the driver (even those not supported by Magnum), see
         * @ref supportedExtensions(), @ref Extension::extensions() or
         * @ref isExtensionSupported() for alternatives.
         * @see @fn_gl{Get} with @def_gl{NUM_EXTENSIONS}, @fn_gl{GetString}
         *      with @def_gl{EXTENSIONS}
         */
        std::vector<std::string> extensionStrings() const;

        #ifndef MAGNUM_TARGET_WEBGL
        /**
         * @brief Context flags
         *
         * @requires_gles Context flags are not available in WebGL.
         */
        Flags flags() const { return _flags; }
        #endif

        /**
         * @brief Supported extensions
         *
         * The list contains only extensions from OpenGL versions newer than
         * the current.
         * @see @ref isExtensionSupported(), @ref Extension::extensions()
         */
        const std::vector<Extension>& supportedExtensions() const {
            return _supportedExtensions;
        }

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Detect if current OpenGL context is a core profile
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls.
         * @see @fn_gl{Get} with @def_gl{CORE_PROFILE_MASK}
         * @requires_gl Not available on OpenGL ES or WebGL.
         */
        bool isCoreProfile();
        #endif

        /**
         * @brief Whether given OpenGL version is supported
         *
         * @see @ref supportedVersion(), @ref MAGNUM_ASSERT_VERSION_SUPPORTED()
         */
        bool isVersionSupported(Version version) const;

        /**
         * @brief Get supported OpenGL version
         *
         * Returns first supported OpenGL version from passed list. Convenient
         * equivalent to subsequent @ref isVersionSupported() calls, e.g.:
         * @code
         * Version v = isVersionSupported(Version::GL330) ? Version::GL330 : Version::GL210;
         * Version v = supportedVersion({Version::GL330, Version::GL210});
         * @endcode
         *
         * If no version from the list is supported, returns lowest available
         * OpenGL version (@ref Version::GL210 for desktop OpenGL,
         * @ref Version::GLES200 for OpenGL ES).
         * @see @ref isExtensionSupported(Version) const
         */
        Version supportedVersion(std::initializer_list<Version> versions) const;

        /**
         * @brief Whether given extension is supported
         *
         * Extensions usable with this function are listed in @ref Extensions
         * namespace in header @ref Extensions.h. Example usage:
         * @code
         * if(Context::current().isExtensionSupported<Extensions::GL::ARB::tessellation_shader>()) {
         *     // draw fancy detailed model
         * } else {
         *     // texture fallback
         * }
         * @endcode
         *
         * @see @ref isExtensionSupported(const Extension&) const,
         *      @ref MAGNUM_ASSERT_EXTENSION_SUPPORTED(),
         *      @ref isExtensionDisabled()
         */
        template<class T> bool isExtensionSupported() const {
            return isExtensionSupported<T>(version());
        }

        /**
         * @brief Whether given extension is supported in given version
         *
         * Similar to @ref isExtensionSupported(), but checks also that the
         * minimal required version of the extension is larger or equal to
         * @p version. Useful mainly in shader compilation when the decisions
         * depend on selected GLSL version, for example:
         * @code
         * const Version version = Context::current()supportedVersion({Version::GL320, Version::GL300, Version::GL210});
         * if(Context::current().isExtensionSupported<Extensions::GL::ARB::explicit_attrib_location>(version)) {
         *     // Called only if ARB_explicit_attrib_location is supported
         *     // *and* version is higher than GL 3.1
         * }
         * @endcode
         */
        template<class T> bool isExtensionSupported(Version version) const {
            return _extensionRequiredVersion[T::Index] <= version && _extensionStatus[T::Index];
        }

        /**
         * @brief Whether given extension is supported
         *
         * Can be used e.g. for listing extensions available on current
         * hardware, but for general usage prefer @ref isExtensionSupported() const,
         * as it does most operations in compile time.
         * @see @ref supportedExtensions(), @ref Extension::extensions(),
         *      @ref MAGNUM_ASSERT_EXTENSION_SUPPORTED()
         */
        bool isExtensionSupported(const Extension& extension) const {
            return isVersionSupported(_extensionRequiredVersion[extension._index]) && _extensionStatus[extension._index];
        }

        /**
         * @brief Whether given extension is disabled
         *
         * Can be used for detecting driver bug workarounds. Disabled
         * extensions return `false` in @ref isExtensionSupported() even if
         * they are advertised as being supported by the driver.
         */
        template<class T> bool isExtensionDisabled() const {
            return isExtensionDisabled<T>(version());
        }

        /**
         * @brief Whether given extension is disabled for given version
         *
         * Similar to above, but can also check for extensions which are
         * disabled only for particular versions.
         */
        template<class T> bool isExtensionDisabled(Version version) const {
            /* The extension is advertised, but the minimal version has been increased */
            return T::requiredVersion() <= version && _extensionRequiredVersion[T::Index] > version;
        }

        /**
         * @brief Whether given extension is disabled
         *
         * Can be used e.g. for listing extensions available on current
         * hardware, but for general usage prefer @ref isExtensionDisabled() const,
         * as it does most operations in compile time.
         */
        bool isExtensionDisabled(const Extension& extension) const {
            return isVersionSupported(extension._requiredVersion) && !isVersionSupported(_extensionRequiredVersion[extension._index]);
        }

        /**
         * @brief Reset internal state tracker
         * @param states    Tracked states to reset. Default is all state.
         *
         * The engine internally tracks object bindings and other state to
         * avoid redundant OpenGL calls. In some cases (e.g. when non-Magnum
         * code makes GL calls) the internal tracker no longer reflects actual
         * state and needs to be reset to avoid strange issues.
         */
        void resetState(States states = ~States{});

        /**
         * @brief Detect driver
         *
         * Tries to detect driver using various OpenGL state queries. Once the
         * detection is done, the result is cached, repeated queries don't
         * result in repeated GL calls. Used primarily for enabling
         * driver-specific workarounds.
         */
        DetectedDrivers detectedDriver();

    #ifdef DOXYGEN_GENERATING_OUTPUT
    private:
    #endif
        bool isDriverWorkaroundDisabled(const std::string& workaround);
        Implementation::State& state() { return *_state; }

        /* This function is called from MeshState constructor, which means the
           state() pointer is not ready yet so we have to pass it directly */
        MAGNUM_LOCAL bool isCoreProfileInternal(Implementation::ContextState& state);

    private:
        explicit Context(NoCreateT, Int argc, const char** argv, void functionLoader());

        bool tryCreate();
        void create();
        void disableDriverWorkaround(const std::string& workaround);

        /* Defined in Implementation/driverSpecific.cpp */
        MAGNUM_LOCAL void setupDriverWorkarounds();

        #ifndef MAGNUM_TARGET_GLES
        MAGNUM_LOCAL bool isCoreProfileImplementationDefault();
        MAGNUM_LOCAL bool isCoreProfileImplementationNV();
        #endif

        void(*_functionLoader)();
        Version _version;
        #ifndef MAGNUM_TARGET_WEBGL
        Flags _flags;
        #endif

        std::array<Version, 256> _extensionRequiredVersion;
        std::bitset<256> _extensionStatus;
        std::vector<Extension> _supportedExtensions;

        Implementation::State* _state;

        std::optional<DetectedDrivers> _detectedDrivers;

        /* True means known and disabled, false means known */
        std::vector<std::pair<std::string, bool>> _driverWorkarounds;
        std::vector<std::string> _disabledExtensions;
        bool _displayInitializationLog;
};

#ifndef MAGNUM_TARGET_WEBGL
CORRADE_ENUMSET_OPERATORS(Context::Flags)
#endif
CORRADE_ENUMSET_OPERATORS(Context::DetectedDrivers)

#ifndef MAGNUM_TARGET_WEBGL
/** @debugoperatorclassenum{Magnum::Context,Magnum::Context::Flag} */
MAGNUM_EXPORT Debug& operator<<(Debug& debug, Context::Flag value);

/** @debugoperatorclassenum{Magnum::Context,Magnum::Context::Flags} */
MAGNUM_EXPORT Debug& operator<<(Debug& debug, Context::Flags value);
#endif

/** @debugoperatorclassenum{Magnum::Context,Magnum::Context::DetectedDriver} */
MAGNUM_EXPORT Debug& operator<<(Debug& debug, Context::DetectedDriver value);

/** @debugoperatorclassenum{Magnum::Context,Magnum::Context::DetectedDrivers} */
MAGNUM_EXPORT Debug& operator<<(Debug& debug, Context::DetectedDrivers value);

/** @hideinitializer
@brief Assert that given OpenGL version is supported
@param version      Version

Useful for initial checks on availability of required features.

By default, if assertion fails, an message is printed to error output and the
application aborts. If `CORRADE_NO_ASSERT` is defined, this macro does nothing.
Example usage:
@code
MAGNUM_ASSERT_VERSION_SUPPORTED(Version::GL330);
@endcode

@see @ref Magnum::Context::isVersionSupported() "Context::isVersionSupported()",
    @ref MAGNUM_ASSERT_EXTENSION_SUPPORTED(), @ref CORRADE_ASSERT(),
    @ref CORRADE_INTERNAL_ASSERT()
*/
#ifdef CORRADE_NO_ASSERT
#define MAGNUM_ASSERT_VERSION_SUPPORTED(version) do {} while(0)
#else
#define MAGNUM_ASSERT_VERSION_SUPPORTED(version)                            \
    do {                                                                    \
        if(!Magnum::Context::current().isVersionSupported(version)) {       \
            Corrade::Utility::Error() << "Magnum: required version" << version << "is not supported"; \
            std::abort();                                                   \
        }                                                                   \
    } while(0)
#endif

/** @hideinitializer
@brief Assert that given OpenGL extension is supported
@param extension    Extension name (from @ref Magnum::Extensions "Extensions"
    namespace)

Useful for initial checks on availability of required features.

By default, if assertion fails, an message is printed to error output and the
application aborts. If `CORRADE_NO_ASSERT` is defined, this macro does nothing.
Example usage:
@code
MAGNUM_ASSERT_EXTENSION_SUPPORTED(Extensions::GL::ARB::geometry_shader4);
@endcode

@see @ref Magnum::Context::isExtensionSupported() "Context::isExtensionSupported()",
    @ref MAGNUM_ASSERT_VERSION_SUPPORTED(), @ref CORRADE_ASSERT(),
    @ref CORRADE_INTERNAL_ASSERT()
*/
#ifdef CORRADE_NO_ASSERT
#define MAGNUM_ASSERT_EXTENSION_SUPPORTED(extension) do {} while(0)
#else
#define MAGNUM_ASSERT_EXTENSION_SUPPORTED(extension)                        \
    do {                                                                    \
        if(!Magnum::Context::current().isExtensionSupported<extension>()) { \
            Corrade::Utility::Error() << "Magnum: required extension" << extension::string() << "is not supported"; \
            std::abort();                                                   \
        }                                                                   \
    } while(0)
#endif

}

#endif
