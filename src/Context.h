#ifndef Magnum_Context_h
#define Magnum_Context_h
/*
    Copyright © 2010, 2011, 2012 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

/** @file
 * @brief Enum Version, class Magnum::Context, Magnum::Extension
 */

#include <bitset>
#include <vector>

#include "Magnum.h"

#include "magnumVisibility.h"

namespace Magnum {

/** @brief OpenGL version */
enum class Version: GLint {
    None = 0,                       /**< @brief Unspecified */
    #ifndef MAGNUM_TARGET_GLES
    GL210 = 210,                    /**< @brief OpenGL 2.1 / GLSL 1.20 */
    GL300 = 300,                    /**< @brief OpenGL 3.0 / GLSL 1.30 */
    GL310 = 310,                    /**< @brief OpenGL 3.1 / GLSL 1.40 */
    GL320 = 320,                    /**< @brief OpenGL 3.2 / GLSL 1.50 */
    GL330 = 330,                    /**< @brief OpenGL 3.3, GLSL 3.30 */
    GL400 = 400,                    /**< @brief OpenGL 4.0, GLSL 4.00 */
    GL410 = 410,                    /**< @brief OpenGL 4.1, GLSL 4.10 */
    GL420 = 420,                    /**< @brief OpenGL 4.2, GLSL 4.20 */
    GL430 = 430                     /**< @brief OpenGL 4.3, GLSL 4.30 */
    #else
    GLES200 = 200,                  /**< @brief OpenGL ES 2.0, GLSL ES 1.00 */
    GLES300 = 300                   /**< @brief OpenGL ES 3.0, GLSL ES 3.00 */
    #endif
};

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

Provides access to version and extension information.
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
         * @see @fn_gl{Get} with @def_gl{MAJOR_VERSION}, @def_gl{MINOR_VERSION},
         *      @fn_gl{GetString} with @def_gl{EXTENSIONS}
         */
        Context();
        ~Context();

        /** @brief Current context */
        inline static Context* current() { return _current; }

        /** @brief OpenGL version */
        inline Version version() const { return _version; }

        /** @brief Major OpenGL version (e.g. `4`) */
        inline GLint majorVersion() const { return _majorVersion; }

        /** @brief Minor OpenGL version (e.g. `3`) */
        inline GLint minorVersion() const { return _minorVersion; }

        /**
         * @brief Vendor string
         *
         * @see @fn_gl{GetString} with @def_gl{VENDOR}
         */
        inline std::string vendorString() const {
            return reinterpret_cast<const char*>(glGetString(GL_VENDOR));
        }

        /**
         * @brief Renderer string
         *
         * @see @fn_gl{GetString} with @def_gl{RENDERER}
         */
        inline std::string rendererString() const {
            return reinterpret_cast<const char*>(glGetString(GL_RENDERER));
        }

        /**
         * @brief Version string
         *
         * @see @fn_gl{GetString} with @def_gl{VERSION}
         */
        inline std::string versionString() const {
            return reinterpret_cast<const char*>(glGetString(GL_VERSION));
        }

        /**
         * @brief Shading language version string
         *
         * @see @fn_gl{GetString} with @def_gl{SHADING_LANGUAGE_VERSION}
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

        /** @brief Whether given OpenGL version is supported */
        inline bool isVersionSupported(Version version) const {
            return _version >= version;
        }

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
         * @see isExtensionSupported(const Extension&) const
         */
        template<class T> inline bool isExtensionSupported() const {
            return _version >= T::coreVersion() || (_version >= T::requiredVersion() && extensionStatus[T::Index]);
        }

        /**
         * @brief Whether given extension is supported
         *
         * Can be used e.g. for listing extensions available on current
         * hardware, but for general usage prefer isExtensionSupported() const,
         * as it does most operations in compile time.
         *
         * @see supportedExtensions(), Extension::extensions()
         */
        inline bool isExtensionSupported(const Extension& extension) const {
            return _version >= extension._coreVersion || (_version >= extension._requiredVersion && extensionStatus[extension._index]);
        }

    private:
        static Context* _current;

        Version _version;
        GLint _majorVersion;
        GLint _minorVersion;

        std::bitset<128> extensionStatus;
        std::vector<Extension> _supportedExtensions;
};

}

#endif
