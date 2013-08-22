#ifndef Magnum_Context_h
#define Magnum_Context_h
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

/** @file /Context.h
 * @brief Enum Magnum::Version, class Magnum::Context, Magnum::Extension, macro MAGNUM_ASSERT_VERSION_SUPPORTED(), MAGNUM_ASSERT_EXTENSION_SUPPORTED()
 */

#include <bitset>
#include <vector>
#include <Containers/EnumSet.h>

#include "Magnum.h"
#include "OpenGL.h"
#include "magnumVisibility.h"

namespace Magnum {

namespace Implementation {
    struct State;
}

/**
@brief OpenGL version

@see Context, MAGNUM_ASSERT_VERSION_SUPPORTED()
*/
enum class Version: Int {
    None = 0xFFFF,                  /**< @brief Unspecified */
    #ifndef MAGNUM_TARGET_GLES
    GL210 = 210,                    /**< @brief OpenGL 2.1 / GLSL 1.20 */
    GL300 = 300,                    /**< @brief OpenGL 3.0 / GLSL 1.30 */
    GL310 = 310,                    /**< @brief OpenGL 3.1 / GLSL 1.40 */
    GL320 = 320,                    /**< @brief OpenGL 3.2 / GLSL 1.50 */
    GL330 = 330,                    /**< @brief OpenGL 3.3, GLSL 3.30 */
    GL400 = 400,                    /**< @brief OpenGL 4.0, GLSL 4.00 */
    GL410 = 410,                    /**< @brief OpenGL 4.1, GLSL 4.10 */
    GL420 = 420,                    /**< @brief OpenGL 4.2, GLSL 4.20 */
    GL430 = 430,                    /**< @brief OpenGL 4.3, GLSL 4.30 */
    GL440 = 440,                    /**< @brief OpenGL 4.4, GLSL 4.40 */
    #endif

    /**
     * @brief OpenGL ES 2.0, GLSL ES 1.00
     *
     * All the functionality is present in OpenGL 4.2 (extension
     * @extension{ARB,ES2_compatibility}), so on desktop OpenGL this is
     * equivalent to @ref Version "Version::GL410".
     */
    #ifndef MAGNUM_TARGET_GLES
    GLES200 = 410,
    #else
    GLES200 = 200,
    #endif

    /**
     * @brief OpenGL ES 3.0, GLSL ES 3.00
     *
     * All the functionality is present in OpenGL 4.3 (extension
     * @extension{ARB,ES3_compatibility}), so on desktop OpenGL this is the
     * equivalent to @ref Version "Version::GL430".
     */
    #ifndef MAGNUM_TARGET_GLES
    GLES300 = 430
    #else
    GLES300 = 300
    #endif
};

/** @debugoperator{Magnum::Context} */
Debug MAGNUM_EXPORT operator<<(Debug debug, Version value);

/**
@brief Run-time information about OpenGL extension

Encapsulates runtime information about OpenGL extension, such as name string,
minimal required OpenGL version and version in which the extension was adopted
to core.

See also Extensions namespace, which contain compile-time information about
OpenGL extensions.
*/
class MAGNUM_EXPORT Extension {
    friend class Context;

    public:
        /** @brief All extensions for given OpenGL version */
        static const std::vector<Extension>& extensions(Version version);

        /** @brief Minimal version required by this extension */
        constexpr Version requiredVersion() const { return _requiredVersion; }

        /** @brief Version in which this extension was adopted to core */
        constexpr Version coreVersion() const { return _coreVersion; }

        /** @brief %Extension string */
        constexpr const char* string() const { return _string; }

    private:
        /* GCC 4.6 doesn't like const members, as std::vector doesn't have
           proper move semantic yet */
        std::size_t _index;
        Version _requiredVersion;
        Version _coreVersion;
        const char* _string;

        constexpr Extension(std::size_t index, Version requiredVersion, Version coreVersion, const char* string): _index(index), _requiredVersion(requiredVersion), _coreVersion(coreVersion), _string(string) {}
};

/**
@brief OpenGL context

Provides access to version and extension information. Instance available
through Context::current() is automatically created during construction of
*Application classes in Platform namespace so you can safely assume that the
instance is available during whole lifetime of *Application object.
@todo @extension{ATI,meminfo}, @extension{NVX,gpu_memory_info}, GPU temperature?
    (here or where?)
*/
class MAGNUM_EXPORT Context {
    Context(const Context&) = delete;
    Context(Context&&) = delete;
    Context& operator=(const Context&) = delete;
    Context& operator=(Context&&) = delete;

    public:
        /**
         * @brief Context flag
         *
         * @see Flags, flags()
         */
        enum class Flag: GLint {
            #ifndef MAGNUM_TARGET_GLES3
            /**
             * Debug context
             * @requires_gl43 %Extension @es_extension{KHR,debug}
             * @requires_es_extension %Extension @es_extension{KHR,debug}
             */
            #ifndef MAGNUM_TARGET_GLES
            Debug = GL_CONTEXT_FLAG_DEBUG_BIT,
            #else
            Debug = GL_CONTEXT_FLAG_DEBUG_BIT_KHR,
            #endif
            #endif

            #ifndef MAGNUM_TARGET_GLES
            /**
             * Context with robust buffer access
             * @requires_extension %Extension @extension{EXT,robustness}
             * @requires_es_extension %Extension @es_extension{EXT,robustness}
             * @todo In ES available under glGetIntegerv(CONTEXT_ROBUST_ACCESS_EXT),
             *      how to make it compatible?
             */
            Robustness = GL_CONTEXT_FLAG_ROBUST_ACCESS_BIT_ARB
            #endif
        };

        /**
         * @brief Context flags
         *
         * @see flags()
         */
        typedef Containers::EnumSet<Flag, GLint> Flags;

        /**
         * @brief Constructor
         *
         * Constructed automatically, see class documentation for more
         * information.
         * @see @fn_gl{Get} with @def_gl{MAJOR_VERSION}, @def_gl{MINOR_VERSION},
         *      @def_gl{CONTEXT_FLAGS}, @def_gl{NUM_EXTENSIONS},
         *      @fn_gl{GetString} with @def_gl{EXTENSIONS}
         */
        explicit Context();

        ~Context();

        /** @brief Current context */
        static Context* current() { return _current; }

        /**
         * @brief OpenGL version
         *
         * @see majorVersion(), minorVersion(), versionString(),
         *      shadingLanguageVersionString()
         */
        Version version() const { return _version; }

        /**
         * @brief Major OpenGL version (e.g. `4`)
         *
         * @see minorVersion(), version(), versionString(),
         *      shadingLanguageVersionString()
         */
        Int majorVersion() const { return _majorVersion; }

        /**
         * @brief Minor OpenGL version (e.g. `3`)
         *
         * @see majorVersion(), version(), versionString(),
         *      shadingLanguageVersionString()
         */
        Int minorVersion() const { return _minorVersion; }

        /**
         * @brief Vendor string
         *
         * The result is *not* cached, repeated queries will result in repeated
         * OpenGL calls.
         * @see rendererString(), @fn_gl{GetString} with @def_gl{VENDOR}
         */
        std::string vendorString() const {
            return reinterpret_cast<const char*>(glGetString(GL_VENDOR));
        }

        /**
         * @brief %Renderer string
         *
         * The result is *not* cached, repeated queries will result in repeated
         * OpenGL calls.
         * @see vendorString(), @fn_gl{GetString} with @def_gl{RENDERER}
         */
        std::string rendererString() const {
            return reinterpret_cast<const char*>(glGetString(GL_RENDERER));
        }

        /**
         * @brief Version string
         *
         * The result is *not* cached, repeated queries will result in repeated
         * OpenGL calls.
         * @see shadingLanguageVersionString(), version(), @fn_gl{GetString}
         *      with @def_gl{VERSION}
         */
        std::string versionString() const {
            return reinterpret_cast<const char*>(glGetString(GL_VERSION));
        }

        /**
         * @brief Shading language version string
         *
         * The result is *not* cached, repeated queries will result in repeated
         * OpenGL calls.
         * @see versionString(), version(), @fn_gl{GetString} with
         *      @def_gl{SHADING_LANGUAGE_VERSION}
         */
        std::string shadingLanguageVersionString() const {
            return reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));
        }

        /**
         * @brief Shading language version strings
         *
         * The result is *not* cached, repeated queries will result in repeated
         * OpenGL calls.
         * @see versionString(), version(), @fn_gl{Get} with @def_gl{NUM_SHADING_LANGUAGE_VERSIONS},
         *      @fn_gl{GetString} with @def_gl{SHADING_LANGUAGE_VERSION}
         */
        std::vector<std::string> shadingLanguageVersionStrings() const;

        /** @brief Context flags */
        Flags flags() const { return _flags; }

        /**
         * @brief Supported extensions
         *
         * The list contains only extensions from OpenGL versions newer than
         * the current.
         *
         * @see isExtensionSupported(), Extension::extensions()
         */
        const std::vector<Extension>& supportedExtensions() const {
            return _supportedExtensions;
        }

        /**
         * @brief Whether given OpenGL version is supported
         *
         * @see supportedVersion(), MAGNUM_ASSERT_VERSION_SUPPORTED()
         */
        bool isVersionSupported(Version version) const {
            #ifndef CORRADE_GCC44_COMPATIBILITY
            return _version >= version;
            #else
            return static_cast<GLint>(_version) >= static_cast<GLint>(version);
            #endif
        }

        /**
         * @brief Get supported OpenGL version
         *
         * Returns first supported OpenGL version from passed list. Convenient
         * equivalent to subsequent isVersionSupported() calls, e.g.:
         * @code
         * Version v = isVersionSupported(Version::GL330) ? Version::GL330 : Version::GL210;
         * Version v = supportedVersion({Version::GL330, Version::GL210});
         * @endcode
         *
         * If no version from the list is supported, returns lowest available
         * OpenGL version (@ref Version "Version::GL210" for desktop OpenGL,
         * @ref Version "Version::GLES200" for OpenGL ES).
         */
        Version supportedVersion(std::initializer_list<Version> versions) const;

        /**
         * @brief Whether given extension is supported
         *
         * %Extensions usable with this function are listed in Extensions
         * namespace in header Extensions.h. Example usage:
         * @code
         * if(Context::current()->isExtensionSupported<Extensions::GL::ARB::tessellation_shader>()) {
         *     // draw fancy detailed model
         * } else {
         *     // texture fallback
         * }
         * @endcode
         *
         * @see isExtensionSupported(const Extension&) const,
         *      MAGNUM_ASSERT_EXTENSION_SUPPORTED()
         */
        template<class T> bool isExtensionSupported() const {
            return isVersionSupported(T::coreVersion()) || (isVersionSupported(T::requiredVersion()) && extensionStatus[T::Index]);
        }

        /**
         * @brief Whether given extension is supported
         *
         * Can be used e.g. for listing extensions available on current
         * hardware, but for general usage prefer isExtensionSupported() const,
         * as it does most operations in compile time.
         *
         * @see supportedExtensions(), Extension::extensions(),
         *      MAGNUM_ASSERT_EXTENSION_SUPPORTED()
         */
        bool isExtensionSupported(const Extension& extension) const {
            return isVersionSupported(extension._coreVersion) || (isVersionSupported(extension._requiredVersion) && extensionStatus[extension._index]);
        }

        #ifndef DOXYGEN_GENERATING_OUTPUT
        Implementation::State& state() { return *_state; }
        #endif

    private:
        static Context* _current;

        Version _version;
        Int _majorVersion;
        Int _minorVersion;
        Flags _flags;

        std::bitset<160> extensionStatus;
        std::vector<Extension> _supportedExtensions;

        Implementation::State* _state;
};

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
    MAGNUM_ASSERT_EXTENSION_SUPPORTED(), CORRADE_ASSERT(),
    CORRADE_INTERNAL_ASSERT()
*/
#ifdef CORRADE_NO_ASSERT
#define MAGNUM_ASSERT_VERSION_SUPPORTED(version) do {} while(0)
#else
#define MAGNUM_ASSERT_VERSION_SUPPORTED(version)                            \
    do {                                                                    \
        if(!Magnum::Context::current()->isVersionSupported(version)) {      \
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
    MAGNUM_ASSERT_VERSION_SUPPORTED(), CORRADE_ASSERT(),
    CORRADE_INTERNAL_ASSERT()
*/
#ifdef CORRADE_NO_ASSERT
#define MAGNUM_ASSERT_EXTENSION_SUPPORTED(extension) do {} while(0)
#else
#define MAGNUM_ASSERT_EXTENSION_SUPPORTED(extension)                        \
    do {                                                                    \
        if(!Magnum::Context::current()->isExtensionSupported<extension>()) { \
            Corrade::Utility::Error() << "Magnum: required extension" << extension::string() << "is not supported"; \
            std::abort();                                                   \
        }                                                                   \
    } while(0)
#endif

}

#endif
