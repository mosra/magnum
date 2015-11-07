#ifndef Magnum_Audio_Context_h
#define Magnum_Audio_Context_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2015 Jonathan Hale <squareys@googlemail.com>

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
 * @brief Class @ref Magnum::Audio::Context
 */

#include <string>
#include <vector>
#include <array>
#include <bitset>
#include <al.h>

#include <Corrade/Containers/EnumSet.h>

#include "Magnum/Audio/Audio.h"
#include "Magnum/Audio/Buffer.h"
#include "Magnum/Audio/visibility.h"

#ifndef DOXYGEN_GENERATING_OUTPUT
typedef struct ALCdevice_struct ALCdevice;
typedef struct ALCcontext_struct ALCcontext;
#endif

namespace Magnum { namespace Audio {

/**
@brief Run-time information about OpenAL extension

Encapsulates runtime information about OpenAL extension, such as name string,
minimal required OpenAL version and version in which the extension was adopted
to core.

See also @ref Audio::Extensions namespace, which contain compile-time information
about OpenAL extensions.
*/
class MAGNUM_AUDIO_EXPORT Extension {
    friend Context;

    public:
        /** @brief All OpenAL extensions */
        static const std::vector<Extension>& extensions();

        /** @brief Extension string */
        constexpr const char* string() const { return _string; }

    private:
        /* MSVC seems to have problems with const members */
        std::size_t _index;
        const char* _string;

        constexpr Extension(std::size_t index, const char* string): _index(index), _string(string) {}
};

/**
@brief OpenAL context
 */
class MAGNUM_AUDIO_EXPORT Context {
    public:
        /** @brief Current context */
        static Context* current() { return _current; }

        class Configuration;

        /**
         * @brief Constructor
         *
         * Creates OpenAL context with given configuration.
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        explicit Context(const Configuration& configuration = Configuration());
        #else
        explicit Context(const Configuration& configuration);
        explicit Context();
        #endif

        /**
         * @brief Destructor
         *
         * Destroys OpenAL context.
         */
        ~Context();

        /**
         * @brief Vendor string
         *
         * @see @ref rendererString(), @fn_al{GetString} with @def_al{VENDOR}
         */
        std::string vendorString() const { return alGetString(AL_VENDOR); }

        /**
         * @brief Renderer string
         *
         * @see @ref vendorString(), @fn_al{GetString} with @def_al{RENDERER}
         */
        std::string rendererString() const { return alGetString(AL_RENDERER); }

        /**
         * @brief Version string
         *
         * @see @fn_al{GetString} with @def_al{VERSION}
         */
        std::string versionString() const { return alGetString(AL_VERSION); }

        /**
         * @brief Extension strings
         *
         * The result is *not* cached, repeated queries will result in repeated
         * OpenAL calls. Note that this function returns list of all extensions
         * reported by the driver (even those not supported by Magnum), see
         * @ref supportedExtensions(), @ref Extension::extensions() or
         * @ref isExtensionSupported() for alternatives.
         * @see @fn_al{Get} with @def_al{NUM_EXTENSIONS}, @fn_al{GetString}
         *      with @def_al{EXTENSIONS}, @fn_alc{GetString} with
         *      @def_alc{EXTENSIONS}
         */
        std::vector<std::string> extensionStrings() const;

        /**
         * @brief Supported extensions
         *
         * The list contains only extensions from OpenAL versions newer than
         * the current.
         * @see @ref isExtensionSupported(), @ref Extension::extensions()
         */
        const std::vector<Extension>& supportedExtensions() const {
            return _supportedExtensions;
        }

        /**
         * @brief Whether given extension is supported
         *
         * Extensions usable with this function are listed in @ref Extensions
         * namespace in header @ref Extensions.h. Example usage:
         * @code
         * if(Context::current()->isExtensionSupported<Extensions::ALC::SOFTX::HRTF>()) {
         *     // amazing binaural audio
         * } else {
         *     // probably left/right stereo only
         * }
         * @endcode
         *
         * @see @ref isExtensionSupported(const Extension&) const,
         *      @ref MAGNUM_ASSERT_AUDIO_EXTENSION_SUPPORTED()
         */
        template<class T> bool isExtensionSupported() const {
            return _extensionStatus[T::Index];
        }

        /**
         * @brief Whether given extension is supported
         *
         * Can be used e.g. for listing extensions available on current
         * hardware, but for general usage prefer @ref isExtensionSupported() const,
         * as it does most operations in compile time.
         * @see @ref supportedExtensions(), @ref Extension::extensions(),
         *      @ref MAGNUM_ASSERT_AUDIO_EXTENSION_SUPPORTED()
         */
        bool isExtensionSupported(const Extension& extension) const {
            return _extensionStatus[extension._index];
        }

    private:
        static Context* _current;

        /* Create a context with given configuration. Returns `true` on success.
         * @ref alcCreateContext(). */
        bool tryCreateContext(const Configuration& config);

        ALCdevice* _device;
        ALCcontext* _context;

        std::bitset<64> _extensionStatus;
        std::vector<Extension> _supportedExtensions;
};

/**
@brief OpenAL context configuration

@see @ref Context()
*/
class MAGNUM_AUDIO_EXPORT Context::Configuration {
    public:
        /**
         * @brief Enum for boolean values with a driver specific default
         * value
         */
        enum class EnabledState: Byte {
            Default = 0,
            Enabled = 1,
            Disabled = 2
        };

        /** @brief Constructor */
        explicit Configuration():
            _frequency(44100),
            _enableHrtf(),
            _monoSources(-1),
            _stereoSources(-1),
            _refreshRate(-1)
        {}

        /** @brief Sampling rate in Hz */
        Int frequency() const { return _frequency; }

        /**
         * @brief Set sampling rate (in Hz)
         * @return Reference to self (for method chaining)
         *
         * Default is `44100`.
         */
        Configuration& setFrequency(Int freq) {
            _frequency = freq;
            return *this;
        }

        /**
         * @brief Whether to use hrtfs
         * @requires_alc_extension for HRTFs, extension @alc_extension{SOFTX,HRTF}
         *      or @alc_extension{SOFT,HRTF}
         */
        EnabledState isHrtfEnabled() const { return _enableHrtf; }

        /**
         * @brief Set whether to use hrtfs
         *
         * Defaults to local OpenAL configuration or false.
         * @requires_alc_extension for HRTFs otherwise setting will be ignored,
         *      extension @alc_extension{SOFTX,HRTF} or
         *      @alc_extension{SOFT,HRTF}
         */
        Configuration& setHrtfEnabled(EnabledState hrtf) {
            _enableHrtf = hrtf;
            return *this;
        }

        /**
         * @brief Hint for how mono sources to support
         *
         * Returns `-1`, if no hint was set.
         */
        Int monoSourcesCount() const { return _monoSources; }

        /**
         * @brief Set hint for how mono sources to support
         * @return Reference to self (for method chaining)
         *
         * If set to `-1` (the default), no hint will be given to OpenAL.
         */
        Configuration& setMonoSourcesCount(Int sources) {
            _monoSources = sources;
            return *this;
        }

        /**
         * @brief Hint for how stereo sources to support
         *
         * Returns `-1`, if no hint was set.
         */
        Int stereoSourcesCount() const { return _stereoSources; }

        /**
         * @brief Set hint for how stereo sources to support
         * @return Reference to self (for method chaining)
         *
         * If set to `-1` (the default), no hint will be given to OpenAL.
         */
        Configuration& setStereoSourcesCount(Int sources) {
            _stereoSources = sources;
            return *this;
        }

        /** @brief Rate at which the OpenAL device is refreshed (in Hz) */
        Int refreshRate() const { return _refreshRate; }

        /**
         * @brief Set rate at which the OpenAL device is refreshed (in Hz)
         * @return Reference to self (for method chaining)
         */
        Configuration& setRefreshRate(Int rate) {
            _refreshRate = rate;
            return *this;
        }

    private:
        Int _frequency;
        EnabledState _enableHrtf;

        Int _monoSources;
        Int _stereoSources;

        Int _refreshRate;
};


/** @hideinitializer
@brief Assert that given OpenAL extension is supported
@param extension    Extension name (from
    @ref Magnum::Audio::Extensions "Audio::Extensions" namespace)

Useful for initial checks on availability of required features.

By default, if assertion fails, an message is printed to error output and the
application aborts. If `CORRADE_NO_ASSERT` is defined, this macro does nothing.
Example usage:
@code
MAGNUM_ASSERT_AUDIO_EXTENSION_SUPPORTED(Extensions::ALC::SOFTX::HRTF);
@endcode

@see @ref Magnum::Audio::Context::isExtensionSupported() "Audio::Context::isExtensionSupported()",
    @ref CORRADE_ASSERT(), @ref CORRADE_INTERNAL_ASSERT()
*/
#ifdef CORRADE_NO_ASSERT
#define MAGNUM_ASSERT_AUDIO_EXTENSION_SUPPORTED(extension) do {} while(0)
#else
#define MAGNUM_ASSERT_AUDIO_EXTENSION_SUPPORTED(extension)                        \
    do {                                                                    \
        if(!Magnum::Audio::Context::current()->isExtensionSupported<extension>()) { \
            Corrade::Utility::Error() << "Magnum: required OpenAL extension" << extension::string() << "is not supported"; \
            std::abort();                                                   \
        }                                                                   \
    } while(0)
#endif

}}

#endif
