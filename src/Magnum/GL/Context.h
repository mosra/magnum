#ifndef Magnum_GL_Context_h
#define Magnum_GL_Context_h
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

/** @file
 * @brief Class @ref Magnum::GL::Context, @ref Magnum::GL::Extension, macro @ref MAGNUM_ASSERT_GL_VERSION_SUPPORTED(), @ref MAGNUM_ASSERT_GL_EXTENSION_SUPPORTED()
 */

#include <cstdlib>
#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/ArrayTuple.h>
#include <Corrade/Containers/EnumSet.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/StaticArray.h>

#include "Magnum/Magnum.h"
#include "Magnum/Math/BoolVector.h"
#include "Magnum/Tags.h"
#include "Magnum/GL/GL.h"
#include "Magnum/GL/OpenGL.h"

#include "Magnum/GL/visibility.h"

#ifdef MAGNUM_BUILD_DEPRECATED
/* For return types of Context::versionString() etc., which used to be a
   std::string. Not ideal, but at least something. */
#include <Corrade/Containers/StringStl.h>
#endif

namespace Magnum {

namespace GL {

namespace Implementation {
    struct ContextState;
    struct State;

    template<class...> class IsExtension;
    template<> class IsExtension<> { public: enum: bool { value = true }; };
    CORRADE_HAS_TYPE(IsExtension<U>, decltype(T::Index));
    template<class T, class U, class ...Args> class IsExtension<T, U, Args...> {
        /** @todo C++17: use &&... instead of all this */
        public: enum: bool { value = IsExtension<T>::value && IsExtension<U, Args...>::value };
    };

    /* Context::Configuration::Flag, but because we need to use it inside
       Context before the Configuration class is defined, it has to be here */
    enum class ContextConfigurationFlag: UnsignedLong {
        /* Keeping the 32-bit range reserved for actual GL context flags */
        Windowless = 1ull << 59,
        QuietLog = 1ull << 60,
        VerboseLog = 1ull << 61,
        GpuValidation = 1ull << 62,
        GpuValidationNoError = 1ull << 63
    };
    typedef Containers::EnumSet<ContextConfigurationFlag> ContextConfigurationFlags;
    CORRADE_ENUMSET_OPERATORS(ContextConfigurationFlags)
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
        static Containers::ArrayView<const Extension> extensions(Version version);

        /** @brief Internal unique extension index */
        constexpr std::size_t index() const { return _index; }

        /** @brief Minimal version required by this extension */
        constexpr Version requiredVersion() const { return _requiredVersion; }

        /** @brief Version in which this extension was adopted to core */
        constexpr Version coreVersion() const { return _coreVersion; }

        /** @brief Extension string */
        constexpr const char* string() const { return _string; }

        /** @brief Construct from a compile-time extension */
        template<class E, class = typename std::enable_if<Implementation::IsExtension<E>::value>::type> constexpr /*implicit*/ Extension(const E&): _index{E::Index}, _requiredVersion{E::requiredVersion()}, _coreVersion{E::coreVersion()}, _string{E::string()} {}

    private:
        std::size_t _index;
        Version _requiredVersion;
        Version _coreVersion;
        const char* _string;
};

/**
@brief Magnum OpenGL context

Provides access to OpenGL version and extension information and manages
Magnum's internal OpenGL state tracker.

@section GL-Context-usage Creating a context

In order to use any Magnum OpenGL functionality, an instance of this class has
to exist and be made current. If you use any `Platform::*Application` classes,
an instance available through @ref Context::current() is automatically created
during construction (or after @ref Platform::Sdl2Application::create() "Platform::*Application::create()"
/ @relativeref{Platform::Sdl2Application,tryCreate()} and you can safely assume
the instance is available during the whole `*Application` lifetime. It's also
possible to create the context without using any `*Application` class using the
@ref Platform::GLContext subclass, see @ref platform for more information.

Various options can be passed using the @ref Configuration class, which is then
extended by various
@ref Platform::Sdl2Application::GLConfiguration "Platform::*Application::GLConfiguration"
and @ref Platform::WindowlessEglContext::Configuration "Platform::Windowless*Application::Configuration"
subclasses.

@subsection GL-Context-usage-command-line Command-line options

In addition to the @ref Configuration, the context is configurable through
command-line options, that are passed either from the `Platform::*Application`
classes or from the @ref Platform::GLContext class. In case an option is set in
both the @ref Configuration and on command-line / environment, the two are
combined together --- flags ORed together, lists joined. Usage:

@code{.sh}
<application> [--magnum-help] [--magnum-disable-workarounds LIST]
              [--magnum-disable-extensions LIST]
              [--magnum-gpu-validation off|on]
              [--magnum-log default|quiet|verbose] ...
@endcode

Arguments:

-   `...` --- main application arguments (see `-h` or `--help` for details)
-   `--magnum-help` --- display this help message and exit
-   `--magnum-disable-workarounds LIST` --- driver workarounds to disable (see
    @ref opengl-workarounds for detailed info) (environment:
    `MAGNUM_DISABLE_WORKAROUNDS`). Corresponds to
    @ref Configuration::addDisabledWorkarounds().
-   `--magnum-disable-extensions LIST` --- API extensions to disable
    (environment: `MAGNUM_DISABLE_EXTENSIONS`). Corresponds to
    @ref Configuration::addDisabledExtensions().
-   `--magnum-gpu-validation off|on|no-error` --- GPU validation using
    @gl_extension{KHR,debug}, if present (environment:
    `MAGNUM_GPU_VALIDATION`) (default: `off`). This sets up @ref DebugOutput
    callbacks and also causes
    @ref Platform::Sdl2Application::GLConfiguration::Flag::Debug "GLConfiguration::Flag::Debug"
    to be enabled for context creation for both windowed and windowless
    applications on supported platforms. Corresponds to
    @ref Configuration::Flag::GpuValidation /
    @ref Configuration::Flag::GpuValidationNoError.
-   `--magnum-log default|quiet|verbose` --- console logging
    (environment: `MAGNUM_LOG`) (default: `default`). Corresponds to
    @ref Configuration::Flag::QuietLog and
    @relativeref{Configuration::Flag,VerboseLog}.

Note that all options are prefixed with `--magnum-` to avoid conflicts with
options passed to the application itself. Options that don't have this prefix
are completely ignored, see documentation of the
@ref Utility-Arguments-delegating "Utility::Arguments" class for details.

Particular application implementations add more options for DPI scaling or
GPU selection, see @ref Platform::Sdl2Application, @ref Platform::GlfwApplication
and @ref Platform::WindowlessEglApplication for details.

@subsection GL-Context-usage-multiple Using multiple OpenGL contexts

By default, Magnum assumes you have one OpenGL context active at all times, and
all state tracking is done by the @ref Context instance that's associated with
it. When you are using multiple OpenGL contexts, each of them needs to have a
corresponding @ref Context instance active at the same time, and you need to
ensure you only access OpenGL objects that were created by the same context as
is currently active.

To prevent accidents in common cases, the @ref Context class expects that no
other instance is active during its creation. In order to create additional
instances for other OpenGL contexts, *first* you need to "unset" the current one
with @ref makeCurrent() and *then* create another instance, which will then
become implicitly active:

@snippet MagnumGL-framebuffer.cpp Context-makeCurrent-nullptr

Once all needed instances are created, switch between them right after making
the underlying GL context current:

@snippet MagnumGL-framebuffer.cpp Context-makeCurrent

@section GL-Context-multithreading Thread safety

If Corrade is compiled with @ref CORRADE_BUILD_MULTITHREADED (the default), the
@ref hasCurrent() and @ref current() accessors are thread-local, matching the
OpenGL context thread locality. This might cause some performance penalties ---
if you are sure that you never need to have multiple independent thread-local
Magnum context, build Corrade with the option disabled.
*/
class MAGNUM_GL_EXPORT Context {
    public:
        class Configuration;

        #ifndef MAGNUM_TARGET_WEBGL
        /**
         * @brief Context flag
         *
         * @see @ref Flags, @ref flags(),
         *      @ref Platform::Sdl2Application::GLConfiguration::setFlags() "Platform::*Application::GLConfiguration::setFlags()"
         * @m_enum_values_as_keywords
         * @requires_gles Context flags are not available in WebGL.
         */
        enum class Flag: GLint {
            /**
             * Debug context. Enabled automatically by @ref Platform windowed
             * and windowless application implementations if the
             * @ref Configuration::Flag::GpuValidation flag is set or if the
             * `--magnum-gpu-validation`
             * @ref GL-Context-usage-command-line "command-line option" is
             * set to `on`.
             * @requires_gl43 Extension @gl_extension{KHR,debug}
             * @requires_gles32 Extension @gl_extension{ANDROID,extension_pack_es31a} /
             *      @gl_extension{KHR,debug}
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
             * Context without error reporting. Enabled automatically by
             * @ref Platform windowed and windowless application
             * implementations if the @ref Configuration::Flag::GpuValidationNoError
             * flag is set or if the `--magnum-gpu-validation`
             * @ref GL-Context-usage-command-line "command-line option" is
             * set to `no-error`.
             * @requires_gl46 Extension @gl_extension{KHR,no_error}
             * @requires_es_extension Extension @gl_extension{KHR,no_error}
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

            #ifndef MAGNUM_TARGET_GLES2
            /**
             * Unbind currently bound PBO.
             *
             * Not all third-party code is aware of PBOs, and if a PBO is bound
             * when Magnum transfers control to an unaware code, it can cause
             * various issues with textures. This is a similar, but rarer,
             * case to @ref State::MeshVao / @ref State::BindScratchVao.
             */
            UnbindPixelBuffer = 1 << 1,
            #endif

            /** Reset tracked framebuffer-related bindings and state */
            Framebuffers = 1 << 2,

            /** Reset tracked mesh-related bindings */
            Meshes = 1 << 3,

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
            MeshVao = 1 << 4,

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
            BindScratchVao = 1 << 5,

            /** Reset tracked pixel storage-related state */
            PixelStorage = 1 << 6,

            /** Reset tracked renderer-related state */
            Renderer = 1 << 7,

            /** Reset tracked shader-related bindings */
            Shaders = 1 << 8,

            /** Reset tracked texture-related bindings and state */
            Textures = 1 << 9,

            #ifndef MAGNUM_TARGET_GLES2
            /** Reset tracked transform feedback-related bindings */
            TransformFeedback = 1 << 10,
            #endif

            /**
             * Reset state on entering section with external OpenGL code.
             *
             * Resets all state that could cause external code to accidentally
             * modify Magnum objects. This includes @ref State::MeshVao and
             * @ref State::UnbindPixelBuffer. In some pathological cases you
             * may want to enable @ref State::BindScratchVao as well.
             */
            EnterExternal = MeshVao
                #ifndef MAGNUM_TARGET_GLES2
                |UnbindPixelBuffer
                #endif
                ,

            /**
             * Reset state on exiting section with external OpenGL code.
             *
             * Resets Magnum state tracker to avoid being confused by external
             * state changes. This resets all states, however
             * @ref State::UnbindPixelBuffer is excluded as Magnum's state
             * tracker will ensure no PBO is bound when calling related OpenGL
             * APIs.
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
             * Proprietary AMD desktop drivers on Windows and Linux. In
             * contrast, AMDGPU Mesa drivers report as
             * @ref DetectedDriver::Mesa instead.
             * @requires_gles Not detectable on WebGL, as browsers
             *      intentionally hide most of the driver information.
             */
            Amd = 1 << 0,
            #endif

            #if defined(MAGNUM_TARGET_GLES) || defined(DOXYGEN_GENERATING_OUTPUT)
            /**
             * OpenGL ES implementation by ANGLE (translated to D3D), used by
             * browsers on Windows for WebGL. As the WebGL specification
             * explicitly disallows exposing driver information to the
             * application, this check cannot be done reliably. See also
             * @ref DetectedDriver::SwiftShader.
             * @requires_gles ANGLE doesn't support desktop OpenGL contexts.
             */
            Angle = 1 << 1,
            #endif

            #ifndef MAGNUM_TARGET_WEBGL
            /**
             * Intel desktop drivers on Windows
             * @requires_gles Not detectable on WebGL, as browsers
             *      intentionally hide most of the driver information.
             */
            IntelWindows = 1 << 2,

            /**
             * Mesa drivers on Windows and Linux. In particular, Intel, AMD
             * and NVidia Mesa drivers match as this. See also
             * @ref DetectedDriver::Svga3D.
             * @requires_gles Not detectable on WebGL, as browsers
             *      intentionally hide most of the driver information.
             */
            Mesa = 1 << 3,

            /**
             * Proprietary NVidia drivers on Windows and Linux
             * @requires_gles Not detectable on WebGL, as browsers
             *      intentionally hide most of the driver information.
             */
            NVidia = 1 << 4,

            /**
             * VMware guest GL driver SVGA3D, implemented using Mesa, both
             * Windows and Linux guests. See https://www.mesa3d.org/vmware-guest.html
             * for more information. Detected in combination with
             * @ref DetectedDriver::Mesa.
             * @requires_gles Not detectable on WebGL, as browsers
             *      intentionally hide most of the driver information.
             */
            Svga3D = 1 << 5,

            #if defined(MAGNUM_TARGET_GLES) || defined(DOXYGEN_GENERATING_OUTPUT)
            /**
             * [SwiftShader](https://github.com/google/swiftshader) software
             * renderer for OpenGL ES. Usually used by browsers in cases where
             * a GPU isn't available. See also @ref DetectedDriver::Angle.
             * @requires_gles SwiftShader doesn't support desktop OpenGL
             *      contexts. Not detectable on WebGL, as browsers
             *      intentionally hide most of the driver information.
             * @m_since{2019,10}
             */
            SwiftShader = 1 << 6,
            #endif
            #endif

            #if defined(CORRADE_TARGET_ANDROID) || defined(DOXYGEN_GENERATING_OUTPUT)
            /**
             * ARM Mali drivers on OpenGL ES.
             * @partialsupport Available only on
             *      @ref CORRADE_TARGET_ANDROID "Android".
             * @m_since{2019,10}
             */
            ArmMali = 1 << 7
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
         * If Corrade is built with @ref CORRADE_BUILD_MULTITHREADED, current
         * context is thread-local instead of global (the default).
         * @see @ref current()
         */
        static bool hasCurrent();

        /**
         * @brief Current context
         *
         * Expect that there is current context. If Corrade is built with
         * @ref CORRADE_BUILD_MULTITHREADED, current context is thread-local
         * instead of global (the default).
         * @see @ref hasCurrent()
         */
        static Context& current();

        /**
         * @brief Make a context current
         * @m_since{2019,10}
         *
         * To be used when you need to manage multiple OpenGL contexts. See
         * @ref GL-Context-usage-multiple for more information.
         */
        static void makeCurrent(Context* context);

        /** @brief Copying is not allowed */
        Context(const Context&) = delete;

        /** @brief Move constructor */
        Context(Context&& other) noexcept;

        ~Context();

        /** @brief Copying is not allowed */
        Context& operator=(const Context&) = delete;

        /** @brief Move assignment is not allowed */
        Context& operator=(Context&&) = delete;

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
         * OpenGL calls. The returned view is always
         * @relativeref{Corrade,Containers::StringViewFlag::NullTerminated} and
         * @relativeref{Corrade::Containers::StringViewFlag,Global}.
         * @see @ref rendererString(), @fn_gl{GetString} with
         *      @def_gl_keyword{VENDOR}
         */
        Containers::StringView vendorString() const;

        /**
         * @brief Renderer string
         *
         * The result is *not* cached, repeated queries will result in repeated
         * OpenGL calls. The returned view is always
         * @relativeref{Corrade,Containers::StringViewFlag::NullTerminated} and
         * @relativeref{Corrade::Containers::StringViewFlag,Global}.
         * @see @ref vendorString(), @fn_gl{GetString} with
         *      @def_gl_keyword{RENDERER}
         */
        Containers::StringView rendererString() const;

        /**
         * @brief Version string
         *
         * The result is *not* cached, repeated queries will result in repeated
         * OpenGL calls. The returned view is always
         * @relativeref{Corrade,Containers::StringViewFlag::NullTerminated} and
         * @relativeref{Corrade::Containers::StringViewFlag,Global}.
         * @see @ref shadingLanguageVersionString(), @ref version(),
         *      @fn_gl{GetString} with @def_gl_keyword{VERSION}
         */
        Containers::StringView versionString() const;

        /**
         * @brief Shading language version string
         *
         * The result is *not* cached, repeated queries will result in repeated
         * OpenGL calls. The returned view is always
         * @relativeref{Corrade,Containers::StringViewFlag::NullTerminated} and
         * @relativeref{Corrade::Containers::StringViewFlag,Global}.
         * @see @ref versionString(), @ref version(), @fn_gl{GetString} with
         *      @def_gl_keyword{SHADING_LANGUAGE_VERSION}
         */
        Containers::StringView shadingLanguageVersionString() const;

        /**
         * @brief Shading language version strings
         *
         * The result is *not* cached, repeated queries will result in repeated
         * OpenGL calls. The returned view is always
         * @relativeref{Corrade,Containers::StringViewFlag::NullTerminated} and
         * @relativeref{Corrade::Containers::StringViewFlag,Global}.
         * @see @ref versionString(), @ref version(), @fn_gl{Get} with
         *      @def_gl_keyword{NUM_SHADING_LANGUAGE_VERSIONS}, @fn_gl{GetString}
         *      with @def_gl_keyword{SHADING_LANGUAGE_VERSION}
         */
        Containers::Array<Containers::StringView> shadingLanguageVersionStrings() const;

        /**
         * @brief Extension strings
         *
         * The result is *not* cached, repeated queries will result in repeated
         * OpenGL calls. Note that this function returns list of all extensions
         * reported by the driver (even those not supported by Magnum), see
         * @ref supportedExtensions(), @ref Extension::extensions() or
         * @ref isExtensionSupported() for alternatives. The returned views are
         * always @relativeref{Corrade,Containers::StringViewFlag::NullTerminated}
         * and @relativeref{Corrade::Containers::StringViewFlag,Global}.
         * @see @fn_gl{Get} with @def_gl_keyword{NUM_EXTENSIONS},
         *      @fn_gl{GetString} with @def_gl_keyword{EXTENSIONS}
         */
        Containers::Array<Containers::StringView> extensionStrings() const;

        #ifndef MAGNUM_TARGET_WEBGL
        /**
         * @brief Context flags
         *
         * @requires_gles Context flags are not available in WebGL.
         */
        Flags flags() const { return _flags; }
        #endif

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief Supported extensions
         *
         * The list contains only extensions from OpenGL versions newer than
         * the current.
         * @m_deprecated_since_latest Deprecated as it doesn't provide anything
         *      that @ref extensionStrings(), @ref Extension::extensions() and
         *      @ref isExtensionSupported() wouldn't provide already and only
        *       causes extra overhead during context creation.
         */
        CORRADE_DEPRECATED("use extensionStrings(), Extension::extensions() and isExtensionSupported() instead") Containers::ArrayView<const Extension> supportedExtensions() const {
            return _supportedExtensions;
        }
        #endif

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
         * Extensions usable with this function are listed in the
         * @ref Extensions namespace in the @ref Magnum/GL/Extensions.h header
         * and in the @ref opengl-support "OpenGL support tables". Example
         * usage:
         *
         * @snippet MagnumGL.cpp Context-isExtensionSupported
         *
         * @see @ref isExtensionSupported(const Extension&) const,
         *      @ref MAGNUM_ASSERT_GL_EXTENSION_SUPPORTED(),
         *      @ref isExtensionDisabled()
         */
        template<class E> bool isExtensionSupported() const {
            return isExtensionSupported<E>(version());
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
        template<class E> bool isExtensionSupported(Version version) const {
            static_assert(Implementation::IsExtension<E>::value, "expected an OpenGL extension");
            return _extensionRequiredVersion[E::Index] <= version && _extensionStatus[E::Index];
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
         * extensions return @cpp false @ce in @ref isExtensionSupported() even
         * if they are advertised as being supported by the driver.
         * @see @ref Configuration::addDisabledExtensions(),
         *      @ref GL-Context-usage-command-line
         */
        template<class E> bool isExtensionDisabled() const {
            return isExtensionDisabled<E>(version());
        }

        /**
         * @brief Whether given extension is disabled for given version
         *
         * Similar to above, but can also check for extensions which are
         * disabled only for particular versions.
         * @see @ref Configuration::addDisabledExtensions(),
         *      @ref GL-Context-usage-command-line
         */
        template<class E> bool isExtensionDisabled(Version version) const {
            static_assert(Implementation::IsExtension<E>::value, "expected an OpenGL extension");
            /* The extension is advertised, but the minimal version has been
               increased */
            return E::requiredVersion() <= version && _extensionRequiredVersion[E::Index] > version;
        }

        /**
         * @brief Whether given extension is disabled
         *
         * Can be used e.g. for listing extensions available on current
         * hardware, but for general usage prefer
         * @ref isExtensionDisabled() const, as it does most operations at
         * compile time.
         * @see @ref Configuration::addDisabledExtensions(),
         *      @ref GL-Context-usage-command-line
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
        bool isDriverWorkaroundDisabled(Containers::StringView workaround);
        Implementation::State& state() { return *_state; }

        /* This function is called from MeshState constructor, which means the
           state() pointer is not ready yet so we have to pass it directly */
        MAGNUM_GL_LOCAL bool isCoreProfileInternal(Implementation::ContextState& state);

        Implementation::ContextConfigurationFlags configurationFlags() const { return _configurationFlags; }

    #ifdef DOXYGEN_GENERATING_OUTPUT
    private:
    #else
    protected:
    #endif
        /* Made protected so it's possible to test the NoCreate constructor and
           also not needed to friend Platform::GLContext. */
        explicit Context(NoCreateT, Int argc, const char** argv, void functionLoader(Context&));
        explicit Context(NoCreateT, Utility::Arguments&& args, Int argc, const char** argv, void functionLoader(Context&)): Context{NoCreate, args, argc, argv, functionLoader} {}
        explicit Context(NoCreateT, Utility::Arguments& args, Int argc, const char** argv, void functionLoader(Context&));

        bool tryCreate(const Configuration& configuration);
        void create(const Configuration& configuration);

    private:
        #ifndef DOXYGEN_GENERATING_OUTPUT /* https://bugzilla.gnome.org/show_bug.cgi?id=776986 */
        friend Implementation::ContextState;
        #endif

        MAGNUM_GL_LOCAL void disableDriverWorkaround(Containers::StringView workaround);

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

        Math::BoolVector<Implementation::ExtensionCount> _extensionStatus;
        /* For all extensions that are marked as supported in _extensionStatus,
           this field contains the minimal required GL version the extension
           needs. Extensions that are disabled have None here. */
        Containers::StaticArray<Implementation::ExtensionCount, Version> _extensionRequiredVersion;
        #ifdef MAGNUM_BUILD_DEPRECATED
        Containers::Array<Extension> _supportedExtensions;
        #endif

        Containers::ArrayTuple _stateData;
        Implementation::State* _state;

        Containers::Optional<DetectedDrivers> _detectedDrivers;

        /** @todo these are all needed only until the state gets created and
            then can be discarded -- what to do? we could avoid including
            Array altogether */
        /* True means known and disabled, false means known */
        Containers::Array<std::pair<Containers::StringView, bool>> _driverWorkarounds;
        Containers::Array<Extension> _disabledExtensions;
        Implementation::ContextConfigurationFlags _configurationFlags;
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

/**
@brief Configuration
@m_since_latest

Affects how Magnum's context creation behaves. See @ref GL-Context-usage for
more information.
@see @ref Platform::GLContext::GLContext(),
    @ref Platform::GLContext::create(),
    @ref Platform::GLContext::tryCreate()
*/
class MAGNUM_GL_EXPORT Context::Configuration {
    public:
        /**
         * @brief Context setup flag
         *
         * Flags affecting the actual GL context are merged with this enum in
         * @ref Platform::Sdl2Application::GLConfiguration::Flag "Platform::*Application::GLConfiguration::Flag"
         * and @ref Platform::WindowlessEglContext::Configuration::Flag "Platform::Windowless*Application::Configuration::Flag"
         * and after context creation available through @ref Context::Flag /
         * @ref Context::flags().
         * @see @ref Flags, @ref setFlags()
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        enum class Flag: UnsignedLong {
            /* Docs only, keep in sync with
               Implementation::ContextConfigurationFlag please */

            /**
             * Treat the context as windowless, assume there's no default
             * framebuffer and thus don't touch @ref defaultFramebuffer in any
             * way. Useful for preventing race conditions when creating OpenGL
             * contexts in background threads.
             *
             * This flag is implicitly enabled in all
             * @ref Platform::WindowlessEglContext::Configuration "Platform::Windowless*Application::Configuration",
             * and, conversely, not possible to enable in any
             * @ref Platform::Sdl2Application::GLConfiguration::Flag "Platform::*Application::GLConfiguration".
             */
            Windowless = 1ull << 59,

            /**
             * Print only warnings and errors instead of the usual startup log
             * listing used extensions and workarounds. Ignored if
             * @ref Flag::VerboseLog is set.
             *
             * Corresponds to the `--magnum-log quiet`
             * @ref GL-Context-usage-command-line "command-line option".
             */
            QuietLog = 1ull << 60,

            /**
             * Print additional information on startup in addition to the usual
             * startup log that lists used extensions and workarounds. Has a
             * precedence over @ref Flag::QuietLog.
             *
             * Corresponds to the `--magnum-log verbose`
             * @ref GL-Context-usage-command-line "command-line option".
             */
            VerboseLog = 1ull << 61,

            /**
             * Enable GPU validation, if available. Has a precedence over
             * @ref Flag::GpuValidationNoError.
             *
             * Corresponds to the `--magnum-gou-validation on`
             * @ref GL-Context-usage-command-line "command-line option".
             */
            GpuValidation = 1ull << 62,

            /**
             * Enable a context without error reporting, if available. Ignored
             * if @ref Flag::GpuValidation is set.
             *
             * Corresponds to the `--magnum-gou-validation no-error`
             * @ref GL-Context-usage-command-line "command-line option".
             */
            GpuValidationNoError = 1ull << 63
        };
        #else
        typedef Implementation::ContextConfigurationFlag Flag;
        #endif

        /**
         * @brief Context setup flags
         *
         * @see @ref setFlags(), @ref GL::Context::Flags
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        typedef Containers::EnumSet<Flag> Flags;
        #else
        typedef Implementation::ContextConfigurationFlags Flags;
        #endif

        /*implicit*/ Configuration();

        /** @brief Copy constructor */
        /* The class has to be copyable in order to make command-line and
           configuration options easy to merge in application implementations
           -- these make a mutable copy and add what arrived via command line */
        Configuration(const Configuration& other);

        /** @brief Move constructor */
        Configuration(Configuration&&) noexcept;

        ~Configuration();

        /** @brief Copy constructor */
        Configuration& operator=(const Configuration& other);

        /** @brief Move assignment */
        Configuration& operator=(Configuration&&) noexcept;

        /** @brief Context setup flags */
        Flags flags() const { return _flags; }

        /**
         * @brief Set context setup flags
         * @return Reference to self (for method chaining)
         *
         * By default no flags are set. Note that subclasses might have
         * different flag defaults, see their documentation for more
         * information. To avoid clearing default flags by accident, prefer to
         * use @ref addFlags() and @ref clearFlags() instead.
         *
         * Particular @ref Flag values correspond to the `--magnum-log` and
         * `--magnum-gpu-validation`
         * @ref GL-Context-usage-command-line "command-line options".
         */
        Configuration& setFlags(Flags flags) {
            _flags = flags;
            return *this;
        }

        /**
         * @brief Add context setup flags
         * @return Reference to self (for method chaining)
         *
         * Unlike @ref setFlags(), ORs the flags with existing instead of
         * replacing them. Useful for preserving the defaults.
         * @see @ref clearFlags()
         */
        Configuration& addFlags(Flags flags) {
            _flags |= flags;
            return *this;
        }

        /**
         * @brief Clear context setup flags
         * @return Reference to self (for method chaining)
         *
         * Unlike @ref setFlags(), ANDs the inverse of @p flags with existing
         * instead of replacing them. Useful for removing default flags.
         * @see @ref addFlags()
         */
        Configuration& clearFlags(Flags flags) {
            _flags &= ~flags;
            return *this;
        }

        /** @brief Disabled driver workarounds */
        Containers::ArrayView<const Containers::StringView> disabledWorkarounds() const;

        /**
         * @brief Add disabled driver workarounds
         * @return Reference to self (for method chaining)
         *
         * Accepts strings from the list at @ref opengl-workarounds. Unknown
         * workarounds are ignored with a warning. By default no workarounds
         * are disabled.
         *
         * Corresponds to the `--magnum-disable-workarounds`
         * @ref GL-Context-usage-command-line "command-line option".
         */
        Configuration& addDisabledWorkarounds(Containers::ArrayView<const Containers::StringView> workarounds);
        /** @overload */
        Configuration& addDisabledWorkarounds(std::initializer_list<Containers::StringView> workarounds);

        /** @brief Disabled extensions */
        Containers::ArrayView<const Extension> disabledExtensions() const;

        /**
         * @brief Add disabled extensions
         * @return Reference to self (for method chaining)
         *
         * By default no extensions are disabled, except for those disabled by
         * driver workarounds.
         *
         * Corresponds to the `--magnum-disable-extensions`
         * @ref GL-Context-usage-command-line "command-line option".
         */
        Configuration& addDisabledExtensions(Containers::ArrayView<const Extension> extensions);
        /** @overload */
        Configuration& addDisabledExtensions(std::initializer_list<Extension> extensions);
        /** @overload */
        template<class ...E> Configuration& addDisabledExtensions() {
            static_assert(Implementation::IsExtension<E...>::value, "expected only OpenGL extensions");
            return addDisabledExtensions({E{}...});
        }

    private:
        Flags _flags;
        Containers::Array<Containers::StringView> _disabledWorkarounds;
        Containers::Array<Extension> _disabledExtensions;
};

#ifndef DOXYGEN_GENERATING_OUTPUT
#define MAGNUM_GL_CONTEXT_CONFIGURATION_SUBCLASS_IMPLEMENTATION(Type)       \
    Type& addDisabledWorkarounds(Containers::ArrayView<const Containers::StringView> workarounds) { \
        GL::Context::Configuration::addDisabledWorkarounds(workarounds);    \
        return *this;                                                       \
    }                                                                       \
    Type& addDisabledWorkarounds(std::initializer_list<Containers::StringView> workarounds) { \
        GL::Context::Configuration::addDisabledWorkarounds(workarounds);    \
        return *this;                                                       \
    }                                                                       \
                                                                            \
    Type& addDisabledExtensions(Containers::ArrayView<const GL::Extension> extensions) { \
        GL::Context::Configuration::addDisabledExtensions(extensions);      \
        return *this;                                                       \
    }                                                                       \
    Type& addDisabledExtensions(std::initializer_list<GL::Extension> extensions) { \
        GL::Context::Configuration::addDisabledExtensions(extensions);      \
        return *this;                                                       \
    }                                                                       \
    template<class ...E> Type& addDisabledExtensions() {                    \
        GL::Context::Configuration::addDisabledExtensions<E...>();          \
        return *this;                                                       \
    }
#endif

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

}}

#endif
