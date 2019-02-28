#ifndef Magnum_GL_Context_h
#define Magnum_GL_Context_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
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
 * @brief Class @ref Magnum::GL::Context, @ref Magnum::GL::Extension, macro @ref MAGNUM_ASSERT_GL_VERSION_SUPPORTED(), @ref MAGNUM_ASSERT_GL_EXTENSION_SUPPORTED()
 */

#include <cstdlib>
#include <array>
#include <bitset>
#include <vector>
#include <Corrade/Containers/EnumSet.h>
#include <Corrade/Containers/Optional.h>

#include "Magnum/Magnum.h"
#include "Magnum/Tags.h"
#include "Magnum/GL/GL.h"
#include "Magnum/GL/OpenGL.h"

#include "Magnum/GL/visibility.h"

namespace Magnum {

namespace Platform { class GLContext; }

namespace GL {

namespace Implementation {
    struct ContextState;
    struct State;

    enum: std::size_t {
        ExtensionCount =
            #ifndef MAGNUM_TARGET_GLES
            192
            #elif !defined(MAGNUM_TARGET_WEBGL)
            128
            #else
            24
            #endif
    };
}

/**
@brief Run-time information about OpenGL extension

Encapsulates runtime information about OpenGL extension, such as name string,
minimal required OpenGL version and version in which the extension was adopted
to core.

See also @ref Extensions namespace, which contain compile-time information
about OpenGL extensions.
*/
class MAGNUM_GL_EXPORT Extension {
    public:
        /** @brief All extensions for given OpenGL version */
        static const std::vector<Extension>& extensions(Version version);

        /** @brief Internal unique extension index */
        constexpr std::size_t index() const { return _index; }

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
using @ref Platform::GLContext subclass, see @ref platform documentation for
more information.

@section GL-Context-command-line Command-line options

The context is configurable through command-line options, that are passed
either from the `Platform::*Application` classes or from the
@ref Platform::GLContext class. Usage:

@code{.sh}
<application> [--magnum-help] [--magnum-disable-workarounds LIST]
              [--magnum-disable-extensions LIST]
              [--magnum-log default|quiet|verbose] ...
@endcode

Arguments:

-   `...` --- main application arguments (see `-h` or `--help` for details)
-   `--magnum-help` --- display this help message and exit
-   `--magnum-disable-workarounds LIST` --- driver workarounds to disable (see
    @ref opengl-workarounds for detailed info) (environment: `MAGNUM_DISABLE_WORKAROUNDS`)
-   `--magnum-disable-extensions LIST` --- OpenGL extensions to disable
    (environment: `MAGNUM_DISABLE_EXTENSIONS`)
-   `--magnum-log default|quiet|verbose` --- console logging
    (environment: `MAGNUM_LOG`) (default: `default`)
-   `--magnum-gpu-preference none|integrated|dedicated`  GPU preference
    (environment: `MAGNUM_GPU_PREFERENCE`) (default: `none`)

Note that all options are prefixed with `--magnum-` to avoid conflicts with
options passed to the application itself. Options that don't have this prefix
are completely ignored, see documentation of the
@ref Utility-Arguments-delegating "Utility::Arguments" class for details.

The GPU preference allows you to request either the integrated or dedicated GPU
on systems having both onboard and dedicated GPUs. Currently this works only
on Windows and Linux / BSD systems, on other systems this option prints a
warning and otherwise does nothing.

-   On Windows this is done by querying the `NvOptimusEnablement` and
    `AmdPowerXpressRequestHighPerformance` symbols in the application
    executable and, if they exist, setting them to either `0` for the
    `integrated` option or `1` for the `dedicated` option. The symbols have to
    be exported by the application executable (not a dynamic library). These
    symbols are exported by default when you link to one of the (static)
    `*Context` or `*Application` libraries, and can be controlled using the
    `BUILD_GPU_PREFERENCE_SYMBOLS` CMake option when building Magnum (see
    @ref building-features for more information).
-   On Linux and BSD this affects the `DRI_PRIME` environment variable that's
    used by Mesa drivers that support PRIME.
*/
class MAGNUM_GL_EXPORT Context {
    public:
        #ifndef MAGNUM_TARGET_WEBGL
        /**
         * @brief Context flag
         *
         * @see @ref Flags, @ref flags(),
         *      @ref Platform::Sdl2Application::Configuration::setFlags() "Platform::*Application::Configuration::setFlags()"
         * @m_enum_values_as_keywords
         * @requires_gles Context flags are not available in WebGL.
         */
        enum class Flag: GLint {
            /**
             * Debug context
             * @requires_gl43 Extension @gl_extension{KHR,debug}
             * @requires_gles32 Extension @gl_extension{ANDROID,extension_pack_es31a} /
             *      @gl_extension2{KHR,debug,debug}
             */
            #ifndef MAGNUM_TARGET_GLES2
            Debug = GL_CONTEXT_FLAG_DEBUG_BIT,
            #else
            Debug = GL_CONTEXT_FLAG_DEBUG_BIT_KHR,
            #endif

            #ifndef MAGNUM_TARGET_GLES
            /**
             * Forward compatible context
             * @see @ref isCoreProfile()
             * @requires_gl Core/compatibility profile distinction and forward
             *      compatibility applies only to desktop GL.
             */
            ForwardCompatible = GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT,
            #endif

            /**
             * Context without error reporting
             * @requires_gl46 Extension @gl_extension{KHR,no_error}
             * @requires_es_extension Extension @gl_extension2{KHR,no_error,no_error}
             */
            #ifndef MAGNUM_TARGET_GLES
            NoError = GL_CONTEXT_FLAG_NO_ERROR_BIT,
            #else
            NoError = GL_CONTEXT_FLAG_NO_ERROR_BIT_KHR,
            #endif

            #ifndef MAGNUM_TARGET_GLES2
            /**
             * Context with robust access
             * @requires_gl45 Extension @gl_extension{KHR,robustness} or
             *      @gl_extension{ARB,robustness}
             * @requires_gles32 Extension @gl_extension{KHR,robustness} or
             *      @gl_extension{EXT,robustness}
             * @todo In ES2 available under glGetIntegerv(CONTEXT_ROBUST_ACCESS_EXT),
             *      how to make it compatible?
             */
            RobustAccess = GL_CONTEXT_FLAG_ROBUST_ACCESS_BIT
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
         * @see @ref States, @ref resetState(), @ref opengl-state-tracking
         */
        enum class State: UnsignedInt {
            /** Reset tracked buffer-related bindings and state */
            Buffers = 1 << 0,

            /** Reset tracked framebuffer-related bindings and state */
            Framebuffers = 1 << 1,

            /** Reset tracked mesh-related bindings */
            Meshes = 1 << 2,

            /**
             * Unbind currently bound VAO.
             *
             * Magnum by default uses VAOs --- each time a @ref Mesh is drawn
             * or configured, its VAO is bound, but it is *not* unbound
             * afterwards to avoid needless state changes. This may introduce
             * problems when using third-party OpenGL code --- it may break
             * internal state of a mesh that was used the most recently.
             * Similar issue can happen the other way. Calling @ref resetState()
             * with @ref State::MeshVao included unbounds any currently bound
             * VAO to fix such case.
             */
            MeshVao = 1 << 3,

            /**
             * Bind a "scratch" VAO on core profile.
             *
             * Use if external code is not VAO-aware and would otherwise try to
             * enable vertex attributes on the default (zero) VAO, causing GL
             * errors. Meant to be used together with @ref State::MeshVao (or
             * @ref State::EnterExternal).
             *
             * Does nothing on compatibility profile and ES / WebGL platforms,
             * as using the default VAO is allowed there.
             */
            BindScratchVao = 1 << 4,

            /** Reset tracked pixel storage-related state */
            PixelStorage = 1 << 5,

            /** Reset tracked renderer-related state */
            Renderer = 1 << 6,

            /** Reset tracked shader-related bindings */
            Shaders = 1 << 7,

            /** Reset tracked texture-related bindings and state */
            Textures = 1 << 8,

            #ifndef MAGNUM_TARGET_GLES2
            /** Reset tracked transform feedback-related bindings */
            TransformFeedback = 1 << 9,
            #endif

            /**
             * Reset state on entering section with external OpenGL code.
             *
             * Resets all state that could cause external code to accidentally
             * modify Magnum objects. This includes only @ref State::MeshVao.
             * In some pathological cases you may want to enable
             * @ref State::BindScratchVao as well.
             */
            EnterExternal = MeshVao,

            /**
             * Reset state on exiting section with external OpenGL code.
             *
             * Resets Magnum state tracker to avoid being confused by external
             * state changes. This resets all states.
             */
            ExitExternal = Buffers|Framebuffers|Meshes|MeshVao|PixelStorage|Renderer|Shaders|Textures
                #ifndef MAGNUM_TARGET_GLES2
                |TransformFeedback
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
         * @see @ref rendererString(), @fn_gl{GetString} with
         *      @def_gl_keyword{VENDOR}
         */
        std::string vendorString() const;

        /**
         * @brief Renderer string
         *
         * The result is *not* cached, repeated queries will result in repeated
         * OpenGL calls.
         * @see @ref vendorString(), @fn_gl{GetString} with
         *      @def_gl_keyword{RENDERER}
         */
        std::string rendererString() const;

        /**
         * @brief Version string
         *
         * The result is *not* cached, repeated queries will result in repeated
         * OpenGL calls.
         * @see @ref shadingLanguageVersionString(), @ref version(),
         *      @fn_gl{GetString} with @def_gl_keyword{VERSION}
         */
        std::string versionString() const;

        /**
         * @brief Shading language version string
         *
         * The result is *not* cached, repeated queries will result in repeated
         * OpenGL calls.
         * @see @ref versionString(), @ref version(), @fn_gl{GetString} with
         *      @def_gl_keyword{SHADING_LANGUAGE_VERSION}
         */
        std::string shadingLanguageVersionString() const;

        /**
         * @brief Shading language version strings
         *
         * The result is *not* cached, repeated queries will result in repeated
         * OpenGL calls.
         * @see @ref versionString(), @ref version(), @fn_gl{Get} with
         *      @def_gl_keyword{NUM_SHADING_LANGUAGE_VERSIONS}, @fn_gl{GetString}
         *      with @def_gl_keyword{SHADING_LANGUAGE_VERSION}
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
         * @see @fn_gl{Get} with @def_gl_keyword{NUM_EXTENSIONS},
         *      @fn_gl{GetString} with @def_gl_keyword{EXTENSIONS}
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
         * @see @fn_gl{Get} with @def_gl_keyword{CORE_PROFILE_MASK},
         *      @ref Flag::ForwardCompatible
         * @requires_gl Core/compatibility profile distinction and forward
         *      compatibility applies only to desktop GL.
         */
        bool isCoreProfile();
        #endif

        /**
         * @brief Whether given OpenGL version is supported
         *
         * @see @ref supportedVersion(), @ref MAGNUM_ASSERT_GL_VERSION_SUPPORTED()
         */
        bool isVersionSupported(Version version) const;

        /**
         * @brief Get supported OpenGL version
         *
         * Returns first supported OpenGL version from passed list. Convenient
         * equivalent to subsequent @ref isVersionSupported() calls --- the two
         * following examples produce the same result:
         *
         * @snippet MagnumGL.cpp Context-supportedVersion
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
         *
         * @snippet MagnumGL.cpp Context-isExtensionSupported
         *
         * @see @ref isExtensionSupported(const Extension&) const,
         *      @ref MAGNUM_ASSERT_GL_EXTENSION_SUPPORTED(),
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
         *
         * @snippet MagnumGL.cpp Context-isExtensionSupported-version
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
         *      @ref MAGNUM_ASSERT_GL_EXTENSION_SUPPORTED()
         */
        bool isExtensionSupported(const Extension& extension) const {
            return isVersionSupported(_extensionRequiredVersion[extension.index()]) && _extensionStatus[extension.index()];
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
            return isVersionSupported(extension.requiredVersion()) && !isVersionSupported(_extensionRequiredVersion[extension.index()]);
        }

        /**
         * @brief Reset internal state tracker
         *
         * The engine internally tracks object bindings and other state to
         * avoid redundant OpenGL calls. In some cases (e.g. when non-Magnum
         * code makes GL calls) the internal tracker no longer reflects actual
         * state. Equivalently the third party code can cause accidental
         * modifications of Magnum objects. It's thus advised to call this
         * function as a barrier between Magnum code and third-party GL code.
         *
         * The default, when calling this function with no parameters, will
         * reset all state. That's the safest option, but may have considerable
         * performance impact when third-party and Magnum code is combined very
         * often. For greater control it's possible to reset only particular
         * states from the @ref State enum.
         *
         * See also @ref opengl-state-tracking for more information.
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
        MAGNUM_GL_LOCAL bool isCoreProfileInternal(Implementation::ContextState& state);

    private:
        #ifndef DOXYGEN_GENERATING_OUTPUT /* https://bugzilla.gnome.org/show_bug.cgi?id=776986 */
        friend Implementation::ContextState;
        friend Platform::GLContext;
        #endif

        explicit Context(NoCreateT, Int argc, const char** argv, void functionLoader(Context&));
        explicit Context(NoCreateT, Utility::Arguments&& args, Int argc, const char** argv, void functionLoader(Context&)): Context{NoCreate, args, argc, argv, functionLoader} {}
        explicit Context(NoCreateT, Utility::Arguments& args, Int argc, const char** argv, void functionLoader(Context&));

        bool tryCreate();
        void create();
        void disableDriverWorkaround(const std::string& workaround);

        /* Defined in Implementation/driverSpecific.cpp */
        MAGNUM_GL_LOCAL void setupDriverWorkarounds();

        #ifndef MAGNUM_TARGET_GLES
        MAGNUM_GL_LOCAL bool isCoreProfileImplementationDefault();
        MAGNUM_GL_LOCAL bool isCoreProfileImplementationNV();
        #endif

        void(*_functionLoader)(Context&){};
        Version _version;
        #ifndef MAGNUM_TARGET_WEBGL
        Flags _flags;
        #endif

        std::array<Version, Implementation::ExtensionCount> _extensionRequiredVersion;
        std::bitset<Implementation::ExtensionCount> _extensionStatus;
        std::vector<Extension> _supportedExtensions;

        Implementation::State* _state;

        Containers::Optional<DetectedDrivers> _detectedDrivers;

        /* True means known and disabled, false means known */
        std::vector<std::pair<std::string, bool>> _driverWorkarounds;
        std::vector<std::string> _disabledExtensions;
        bool _displayInitializationLog;
};

#ifndef MAGNUM_TARGET_WEBGL
CORRADE_ENUMSET_OPERATORS(Context::Flags)
#endif
CORRADE_ENUMSET_OPERATORS(Context::DetectedDrivers)
CORRADE_ENUMSET_OPERATORS(Context::States)

#ifndef MAGNUM_TARGET_WEBGL
/** @debugoperatorclassenum{Context,Context::Flag} */
MAGNUM_GL_EXPORT Debug& operator<<(Debug& debug, Context::Flag value);

/** @debugoperatorclassenum{Context,Context::Flags} */
MAGNUM_GL_EXPORT Debug& operator<<(Debug& debug, Context::Flags value);
#endif

/** @debugoperatorclassenum{Context,Context::DetectedDriver} */
MAGNUM_GL_EXPORT Debug& operator<<(Debug& debug, Context::DetectedDriver value);

/** @debugoperatorclassenum{Context,Context::DetectedDrivers} */
MAGNUM_GL_EXPORT Debug& operator<<(Debug& debug, Context::DetectedDrivers value);

/** @hideinitializer
@brief Assert that given OpenGL version is supported
@param version      Version

Useful for initial checks on availability of required features.

By default, if assertion fails, an message is printed to error output and the
application aborts. If `CORRADE_NO_ASSERT` is defined, this macro does nothing.
Example usage:

@snippet MagnumGL.cpp Context-MAGNUM_ASSERT_GL_VERSION_SUPPORTED

@see @ref Magnum::GL::Context::isVersionSupported() "GL::Context::isVersionSupported()",
    @ref MAGNUM_ASSERT_GL_EXTENSION_SUPPORTED(), @ref CORRADE_ASSERT(),
    @ref CORRADE_INTERNAL_ASSERT()
*/
#ifdef CORRADE_NO_ASSERT
#define MAGNUM_ASSERT_GL_VERSION_SUPPORTED(version) do {} while(0)
#else
#define MAGNUM_ASSERT_GL_VERSION_SUPPORTED(version)                         \
    do {                                                                    \
        if(!Magnum::GL::Context::current().isVersionSupported(version)) {   \
            Corrade::Utility::Error() << "Magnum: required version" << version << "is not supported"; \
            std::abort();                                                   \
        }                                                                   \
    } while(0)
#endif

/** @hideinitializer
@brief Assert that given OpenGL extension is supported
@param extension    Extension name (from the @ref Magnum::GL::Extensions "Extensions"
    namespace)

Useful for initial checks on availability of required features.

By default, if assertion fails, an message is printed to error output and the
application aborts. If `CORRADE_NO_ASSERT` is defined, this macro does nothing.
Example usage:

@snippet MagnumGL.cpp Context-MAGNUM_ASSERT_GL_EXTENSION_SUPPORTED

@see @ref Magnum::GL::Context::isExtensionSupported() "Context::isExtensionSupported()",
    @ref MAGNUM_ASSERT_GL_VERSION_SUPPORTED(), @ref CORRADE_ASSERT(),
    @ref CORRADE_INTERNAL_ASSERT()
*/
#ifdef CORRADE_NO_ASSERT
#define MAGNUM_ASSERT_GL_EXTENSION_SUPPORTED(extension) do {} while(0)
#else
#define MAGNUM_ASSERT_GL_EXTENSION_SUPPORTED(extension)                     \
    do {                                                                    \
        if(!Magnum::GL::Context::current().isExtensionSupported<extension>()) { \
            Corrade::Utility::Error() << "Magnum: required extension" << extension::string() << "is not supported"; \
            std::abort();                                                   \
        }                                                                   \
    } while(0)
#endif

}

#ifdef MAGNUM_BUILD_DEPRECATED
/* Note: needs to be prefixed with Magnum:: otherwise Doxygen can't find it */

/** @brief @copybrief GL::Extension
 * @deprecated Use @ref GL::Extension instead.
 */
typedef CORRADE_DEPRECATED("use GL::Extension instead") Magnum::GL::Extension Extension;

/** @brief @copybrief GL::Context
 * @deprecated Use @ref GL::Context instead.
 */
typedef CORRADE_DEPRECATED("use GL::Context instead") Magnum::GL::Context Context;

/** @brief @copybrief MAGNUM_ASSERT_GL_VERSION_SUPPORTED()
 * @deprecated Use @ref MAGNUM_ASSERT_GL_VERSION_SUPPORTED()
 */
#define MAGNUM_ASSERT_VERSION_SUPPORTED(version) \
    CORRADE_DEPRECATED_MACRO(MAGNUM_ASSERT_VERSION_SUPPORTED(), "use MAGNUM_ASSERT_GL_VERSION_SUPPORTED() instead") \
    MAGNUM_ASSERT_GL_VERSION_SUPPORTED(version)

/** @brief @copybrief MAGNUM_ASSERT_GL_EXTENSION_SUPPORTED()
 * @deprecated Use @ref MAGNUM_ASSERT_GL_EXTENSION_SUPPORTED()
 */
#define MAGNUM_ASSERT_EXTENSION_SUPPORTED(extension) \
    CORRADE_DEPRECATED_MACRO(MAGNUM_ASSERT_EXTENSION_SUPPORTED(), "use MAGNUM_ASSERT_GL_EXTENSION_SUPPORTED() instead") \
    MAGNUM_ASSERT_GL_EXTENSION_SUPPORTED(extension)
#endif

}

#endif
