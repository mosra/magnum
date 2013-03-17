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

/** @file
 * @brief Enum Magnum::Version, class Magnum::Context, Magnum::Extension, macro MAGNUM_ASSERT_VERSION_SUPPORTED(), MAGNUM_ASSERT_EXTENSION_SUPPORTED()
 */

#include <bitset>
#include <vector>

#include "Magnum.h"
#include "OpenGL.h"
#include "magnumVisibility.h"

namespace Magnum {

#ifndef DOXYGEN_GENERATING_OUTPUT
namespace Implementation {
    struct State;
}
#endif

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
        inline constexpr Version requiredVersion() const { return _requiredVersion; }

        /** @brief Version in which this extension was adopted to core */
        inline constexpr Version coreVersion() const { return _coreVersion; }

        /** @brief %Extension string */
        inline constexpr const char* string() const { return _string; }

    private:
        /* GCC 4.6 doesn't like const members, as std::vector doesn't have
           proper move semantic yet */
        std::size_t _index;
        Version _requiredVersion;
        Version _coreVersion;
        const char* _string;

        inline constexpr Extension(std::size_t index, Version requiredVersion, Version coreVersion, const char* string): _index(index), _requiredVersion(requiredVersion), _coreVersion(coreVersion), _string(string) {}
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
         * @brief Constructor
         *
         * Constructed automatically, see class documentation for more
         * information.
         * @see @fn_gl{Get} with @def_gl{MAJOR_VERSION}, @def_gl{MINOR_VERSION},
         *      @fn_gl{GetString} with @def_gl{EXTENSIONS}
         */
        explicit Context();

        ~Context();

        /** @brief Current context */
        inline static Context* current() { return _current; }

        /**
         * @brief OpenGL version
         *
         * @see majorVersion(), minorVersion(), versionString(),
         *      shadingLanguageVersionString()
         */
        inline Version version() const { return _version; }

        /**
         * @brief Major OpenGL version (e.g. `4`)
         *
         * @see minorVersion(), version(), versionString(),
         *      shadingLanguageVersionString()
         */
        inline Int majorVersion() const { return _majorVersion; }

        /**
         * @brief Minor OpenGL version (e.g. `3`)
         *
         * @see majorVersion(), version(), versionString(),
         *      shadingLanguageVersionString()
         */
        inline Int minorVersion() const { return _minorVersion; }

        /**
         * @brief Vendor string
         *
         * @see rendererString(), @fn_gl{GetString} with @def_gl{VENDOR}
         */
        inline std::string vendorString() const {
            return reinterpret_cast<const char*>(glGetString(GL_VENDOR));
        }

        /**
         * @brief %Renderer string
         *
         * @see vendorString(), @fn_gl{GetString} with @def_gl{RENDERER}
         */
        inline std::string rendererString() const {
            return reinterpret_cast<const char*>(glGetString(GL_RENDERER));
        }

        /**
         * @brief Version string
         *
         * @see shadingLanguageVersionString(), version(), @fn_gl{GetString}
         *      with @def_gl{VERSION}
         */
        inline std::string versionString() const {
            return reinterpret_cast<const char*>(glGetString(GL_VERSION));
        }

        /**
         * @brief Shading language version string
         *
         * @see versionString(), version(), @fn_gl{GetString} with
         *      @def_gl{SHADING_LANGUAGE_VERSION}
         */
        inline std::string shadingLanguageVersionString() const {
            return reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));
        }

        /**
         * @brief Supported extensions
         *
         * The list contains only extensions from OpenGL versions newer than
         * the current.
         *
         * @see isExtensionSupported(), Extension::extensions()
         */
        inline const std::vector<Extension>& supportedExtensions() const {
            return _supportedExtensions;
        }

        /**
         * @brief Whether given OpenGL version is supported
         *
         * @see supportedVersion(), MAGNUM_ASSERT_VERSION_SUPPORTED()
         */
        inline bool isVersionSupported(Version version) const {
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
        template<class T> inline bool isExtensionSupported() const {
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
        inline bool isExtensionSupported(const Extension& extension) const {
            return isVersionSupported(extension._coreVersion) || (isVersionSupported(extension._requiredVersion) && extensionStatus[extension._index]);
        }

        #ifndef DOXYGEN_GENERATING_OUTPUT
        inline Implementation::State* state() { return _state; }
        #endif

    private:
        static Context* _current;

        Version _version;
        Int _majorVersion;
        Int _minorVersion;

        std::bitset<128> extensionStatus;
        std::vector<Extension> _supportedExtensions;

        Implementation::State* _state;
};

/** @hideinitializer
@brief Assert that given OpenGL version is supported
@param version      Version

Useful for initial checks on availability of required features.

By default, if assertion fails, an message is printed to error output and the
application exits with value `-3`. If `CORRADE_NO_ASSERT` is defined, this
macro does nothing. Example usage:
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
        if(!Context::current()->isVersionSupported(version)) {    \
            Corrade::Utility::Error() << "Magnum: required version" << version << "is not supported"; \
            std::exit(-3);                                                  \
        }                                                                   \
    } while(0)
#endif

/** @hideinitializer
@brief Assert that given OpenGL extension is supported
@param extension    Extension name (from @ref Magnum::Extensions "Extensions"
    namespace)

Useful for initial checks on availability of required features.

By default, if assertion fails, an message is printed to error output and the
application exits with value `-3`. If `CORRADE_NO_ASSERT` is defined, this
macro does nothing. Example usage:
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
        if(!Context::current()->isExtensionSupported<extension>()) { \
            Corrade::Utility::Error() << "Magnum: required extension" << extension::string() << "is not supported"; \
            std::exit(-3);                                                  \
        }                                                                   \
    } while(0)
#endif

}

#endif
