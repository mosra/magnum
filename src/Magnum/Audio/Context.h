#ifndef Magnum_Audio_Context_h
#define Magnum_Audio_Context_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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
#include "Magnum/Audio/Extensions.h"
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
        /**
         * @brief HRTF status
         *
         * @see @ref hrtfStatus(), @ref isHrtfEnabled()
         * @requires_al_extension Extension @alc_extension{SOFTX,HRTF} or
         *      @alc_extension{SOFT,HRTF}
         */
        enum class HrtfStatus: ALenum {
            Disabled = ALC_HRTF_DISABLED_SOFT,  /**< HRTF is disabled */
            Enabled = ALC_HRTF_ENABLED_SOFT,    /**< HRTF is enabled */

            /**
             * HRTF is disabled because it is not allowed on the device. This
             * may be caused by invalid resource permissions, or an other user
             * configuration that disallows HRTF.
             * @requires_al_extension Extension @alc_extension{SOFT,HRTF}
             */
            Denied = ALC_HRTF_DENIED_SOFT,

            /**
             * HRTF is enabled because it must be used on the device. This may
             * be caused by a device that can only use HRTF, or other user
             * configuration that forces HRTF to be used.
             * @requires_al_extension Extension @alc_extension{SOFT,HRTF}
             */
            Required = ALC_HRTF_REQUIRED_SOFT,

            /**
             * HRTF is enabled automatically because the device reported
             * headphones.
             * @requires_al_extension Extension @alc_extension{SOFT,HRTF}
             */
            Detected = ALC_HRTF_HEADPHONES_DETECTED_SOFT,

            /**
             * The device does not support HRTF with the current format.
             * Typically this is caused by non-stereo output or an incompatible
             * output frequency.
             * @requires_al_extension Extension @alc_extension{SOFT,HRTF}
             */
            UnsupportedFormat = ALC_HRTF_UNSUPPORTED_FORMAT_SOFT
        };

        /**
         * @brief All device specifier strings
         *
         * @see @ref deviceSpecifierString(), @ref Configuration::setDeviceSpecifier()
         *      @fn_al{GetString} with @def_alc{DEVICE_SPECIFIER}
         */
        static std::vector<std::string> deviceSpecifierStrings();

        /**
         * @brief Whether there is any current context
         *
         * @see @ref current()
         */
        static bool hasCurrent();

        /**
         * @brief Current context
         *
         * Expect that there is current context.
         * @see @ref hasCurrent()
         */
        static Context& current();

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

        #if defined(MAGNUM_BUILD_DEPRECATED) && !defined(DOXYGEN_GENERATING_OUTPUT)
        CORRADE_DEPRECATED("Audio::Context::current() returns reference now") Context* operator->() { return this; }
        CORRADE_DEPRECATED("Audio::Context::current() returns reference now") operator Context*() { return this; }
        #endif

        /**
         * @brief Whether HRTFs (Head Related Transfer Functions) are enabled
         *
         * HRFTs may not be enabled/disabled in a running context. Instead
         * create a new @ref Context with HRFTs enabled or disabled.
         * @see @ref hrtfStatus(), @ref Audio::Context::Configuration::setHrtf(),
         *      @fn_alc{GetIntegerv} with @def_alc{HRTF_SOFT}
         * @requires_al_extension Extension @alc_extension{SOFTX,HRTF} or
         *      @alc_extension{SOFT,HRTF}
         */
        bool isHrtfEnabled() const;

        /**
         * @brief HRTF status
         *
         * @see @ref isHrtfEnabled(), @fn_alc{GetIntegerv} with
         *      @def_alc{HRTF_STATUS_SOFT}
         * @requires_al_extension Extension @alc_extension{SOFTX,HRTF} or
         *      @alc_extension{SOFT,HRTF}
         */
        HrtfStatus hrtfStatus() const;

        /**
         * @brief Hrtf specifier
         *
         * Name of the hrtf being used.
         *
         * @see @fn_al{GetString} with @def_alc{HRTF_SPECIFIER_SOFT}
         * @requires_al_extension @alc_extension{SOFT,HRTF}
         */
        std::string hrtfSpecifier() const {
            return alcGetString(_device, ALC_HRTF_SPECIFIER_SOFT);
        }

        /**
         * @brief Device specifier string
         *
         * @see @ref deviceSpecifierStrings(), @ref vendorString(), @ref rendererString(),
         *      @fn_al{GetString} with @def_alc{DEVICE_SPECIFIER}
         */
        std::string deviceSpecifierString() const { return alcGetString(_device, ALC_DEVICE_SPECIFIER); }

        /**
         * @brief Vendor string
         *
         * @see @ref deviceSpecifierString(), @ref rendererString(),
         *      @fn_al{GetString} with @def_al{VENDOR}
         */
        std::string vendorString() const { return alGetString(AL_VENDOR); }

        /**
         * @brief Renderer string
         *
         * @see @ref deviceSpecifierString(), @ref vendorString(),
         *      @fn_al{GetString} with @def_al{RENDERER}
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
         * if(Context::current().isExtensionSupported<Extensions::ALC::SOFTX::HRTF>()) {
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
        MAGNUM_AUDIO_LOCAL static Context* _current;

        /* Create a context with given configuration. Returns `true` on success.
         * @ref alcCreateContext(). */
        MAGNUM_AUDIO_LOCAL bool tryCreateContext(const Configuration& config);

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
         * @brief HRTF configuration
         *
         * @see @ref setHrtf()
         */
        enum class Hrtf: Byte {
            /** Default behavior depending on local OpenAL configuration */
            Default = 0,
            Enabled = 1,    /**< Eabled */
            Disabled = 2    /**< Disabled */
        };

        explicit Configuration();
        ~Configuration();

        /** @brief Device specifier */
        const std::string& deviceSpecifier() const { return _deviceSpecifier; }

        /**
         * @brief Set device specifier
         * @return Reference to self (for method chaining)
         *
         * If set to empty string (the default), default device specifier is
         * used.
         * @see @ref Context::deviceSpecifierStrings()
         */
        Configuration& setDeviceSpecifier(const std::string& specifier);
        Configuration& setDeviceSpecifier(std::string&& specifier); /**< @overload */

        /** @brief Sampling rate in Hz */
        Int frequency() const { return _frequency; }

        /**
         * @brief Set sampling rate
         * @return Reference to self (for method chaining)
         *
         * If set to `-1` (the default), system OpenAL configuration is used.
         */
        Configuration& setFrequency(Int hz) {
            _frequency = hz;
            return *this;
        }

        /** @brief HRTF configuration */
        Hrtf hrtf() const { return _hrtf; }

        /**
         * @brief Set HRTF configuration
         * @return Reference to self (for method chaining)
         *
         * If set to @ref Hrtf::Default (the default), system OpenAL
         * configuration is used.
         * @requires_al_extension Extension @alc_extension{SOFTX,HRTF} or
         *      @alc_extension{SOFT,HRTF}, otherwise the setting will be simply
         *      ignored
         */
        Configuration& setHrtf(Hrtf hrtf) {
            _hrtf = hrtf;
            return *this;
        }

        /** @brief Hint for how many mono sources to support */
        Int monoSourceCount() const { return _monoSources; }

        /**
         * @brief Set hint for how many mono sources to support
         * @return Reference to self (for method chaining)
         *
         * If set to `-1` (the default), no hint will be given to OpenAL.
         */
        Configuration& setMonoSourceCount(Int count) {
            _monoSources = count;
            return *this;
        }

        /** @brief Hint for how many stereo sources to support */
        Int stereoSourceCount() const { return _stereoSources; }

        /**
         * @brief Set hint for how many stereo sources to support
         * @return Reference to self (for method chaining)
         *
         * If set to `-1` (the default), no hint will be given to OpenAL.
         */
        Configuration& setStereoSourceCount(Int count) {
            _stereoSources = count;
            return *this;
        }

        /** @brief Refresh rate in Hz */
        Int refreshRate() const { return _refreshRate; }

        /**
         * @brief Set refresh rate
         * @return Reference to self (for method chaining)
         *
         * If set to `-1` (the default), system OpenAL configuration is used.
         */
        Configuration& setRefreshRate(Int hz) {
            _refreshRate = hz;
            return *this;
        }

    private:
        std::string _deviceSpecifier;

        Int _frequency{-1};
        Hrtf _hrtf{};

        Int _monoSources{-1};
        Int _stereoSources{-1};

        Int _refreshRate{-1};
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
#define MAGNUM_ASSERT_AUDIO_EXTENSION_SUPPORTED(extension)                  \
    do {                                                                    \
        if(!Magnum::Audio::Context::current().isExtensionSupported<extension>()) { \
            Corrade::Utility::Error() << "Magnum: required OpenAL extension" << extension::string() << "is not supported"; \
            std::abort();                                                   \
        }                                                                   \
    } while(0)
#endif

/** @debugoperatorclassenum{Magnum::Audio::Context,Magnum::Audio::Context::HrtfStatus} */
MAGNUM_AUDIO_EXPORT Debug& operator<<(Debug& debug, Context::HrtfStatus value);

inline bool Context::isHrtfEnabled() const {
    Int enabled;
    alcGetIntegerv(_device, ALC_HRTF_SOFT, 1, &enabled);
    return enabled == ALC_TRUE;
}

inline Context::HrtfStatus Context::hrtfStatus() const {
    if(!isExtensionSupported<Extensions::ALC::SOFT::HRTF>())
        return isHrtfEnabled() ? HrtfStatus::Enabled : HrtfStatus::Disabled;

    Int status;
    alcGetIntegerv(_device, ALC_HRTF_STATUS_SOFT, 1, &status);
    return Context::HrtfStatus(status);
}

}}

#endif
