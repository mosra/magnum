#ifndef Magnum_Audio_Context_h
#define Magnum_Audio_Context_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2015 Jonathan Hale <squareys@googlemail.com>
    Copyright © 2019 Guillaume Jacquemin <williamjcm@users.noreply.github.com>

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
 * @brief Class @ref Magnum::Audio::Context, macro @ref MAGNUM_ASSERT_AUDIO_EXTENSION_SUPPORTED()
 */

#include <cstdlib>
#include <string>
#include <vector>
#include <bitset>
#include <al.h>
#include <alc.h>
#include <Corrade/Containers/EnumSet.h>
#include <Corrade/Utility/Debug.h>

#include "Magnum/Magnum.h"
#include "Magnum/Tags.h"
#include "Magnum/Audio/visibility.h"
#include "Magnum/Math/BoolVector.h"
#include "MagnumExternal/OpenAL/extensions.h"

namespace Magnum { namespace Audio {

namespace Implementation {
    enum: std::size_t { ExtensionCount = 16 };
}

/**
@brief Run-time information about OpenAL extension

Encapsulates runtime information about OpenAL extension, such as name string,
minimal required OpenAL version and version in which the extension was adopted
to core.

See also the @ref Audio::Extensions namespace, which contain compile-time
information about OpenAL extensions.
*/
class MAGNUM_AUDIO_EXPORT Extension {
    public:
        /** @brief All OpenAL extensions */
        static Containers::ArrayView<const Extension> extensions();

        /** @brief Internal unique extension index */
        constexpr std::size_t index() const { return _index; }

        /** @brief Extension string */
        constexpr const char* string() const { return _string; }

        #ifndef DOXYGEN_GENERATING_OUTPUT
        constexpr Extension(std::size_t index, const char* string): _index{index}, _string{string} {}
        #endif

    private:
        /* MSVC seems to have problems with const members */
        std::size_t _index;
        const char* _string;
};

/**
@brief OpenAL context

@section AL-Context-command-line Command-line options

The context is configurable through command-line options, that can be passed
for example from the `Platform::*Application` classes. Usage:

@code{.sh}
<application> [--magnum-help] [--magnum-disable-extensions LIST]
              [--magnum-log default|quiet|verbose] ...
@endcode

Arguments:

-   `...` --- main application arguments (see `-h` or `--help` for details)
-   `--magnum-help` --- display this help message and exit
-   `--magnum-disable-extensions LIST` --- API extensions to disable
    (environment: `MAGNUM_DISABLE_EXTENSIONS`)
-   `--magnum-log default|quiet|verbose` --- console logging
    (environment: `MAGNUM_LOG`) (default: `default`)

Note that all options are prefixed with `--magnum-` to avoid conflicts with
options passed to the application itself. Options that don't have this prefix
are completely ignored, see documentation of the
@ref Utility-Arguments-delegating "Utility::Arguments" class for details.
Moreover, `--magnum`-prefixed options unrelated to audio (such as those defined
by @ref GL-Context-command-line "GL::Context") are ignored as well. In order to
provide a complete help and command-line argument diagnostic, you should
instantiate this class *after* @ref GL::Context.
 */
class MAGNUM_AUDIO_EXPORT Context {
    public:
        /**
         * @brief HRTF status
         *
         * @see @ref hrtfStatus(), @ref isHrtfEnabled()
         * @m_enum_values_as_keywords
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
         *      @fn_alc{GetString} with @def_alc_keyword{DEVICE_SPECIFIER}
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
         * Parses command-line arguments, and creates OpenAL context with given
         * configuration.
         */
        explicit Context(const Configuration& configuration, Int argc, const char** argv): Context(NoCreate, argc, argv) { create(configuration); }

        /** @overload */
        explicit Context(const Configuration& configuration, Int argc, char** argv): Context(configuration, argc, const_cast<const char**>(argv)) {}

        /** @overload */
        explicit Context(const Configuration& configuration, Int argc, std::nullptr_t argv): Context(configuration, argc, static_cast<const char**>(argv)) {}

        /** @overload */
        explicit Context(const Configuration& configuration): Context(configuration, 0, nullptr) {}

        /** @overload */
        explicit Context(Int argc, const char** argv);

        /** @overload */
        explicit Context(Int argc, char** argv): Context(argc, const_cast<const char**>(argv)) {}

        /** @overload */
        explicit Context(Int argc, std::nullptr_t argv): Context(argc, static_cast<const char**>(argv)) {}

        /** @overload */
        explicit Context(): Context(0, nullptr) {}

        /**
         * @brief Construct without creating the underlying OpenAL context
         *
         * Useful in cases where you need to defer context creation to a later
         * time, for example to do a more involved configuration. Call
         * @ref create() or @ref tryCreate() to create the actual context.
         */
        explicit Context(NoCreateT, Int argc, const char** argv) noexcept;

        /** @overload */
        explicit Context(NoCreateT, Int argc, char** argv) noexcept: Context(NoCreate, argc, const_cast<const char**>(argv)) {}

        /** @overload */
        explicit Context(NoCreateT, Int argc, std::nullptr_t argv) noexcept: Context(NoCreate, argc, static_cast<const char**>(argv)) {}

        /** @overload */
        explicit Context(NoCreateT) noexcept: Context{NoCreate, 0, nullptr} {}

        /** @brief Copying is not allowed */
        Context(const Context&) = delete;

        /** @brief Move constructor */
        Context(Context&& other) noexcept;

        /**
         * @brief Destructor
         *
         * Destroys OpenAL context.
         */
        ~Context();

        /** @brief Copying is not allowed */
        Context& operator=(const Context&) = delete;

        /** @brief Move assignment is not allowed */
        Context& operator=(Context&& other) = delete;

        /**
         * @brief Complete the context setup and exit on failure
         *
         * Finalizes the setup after the class was created using
         * @ref Context(NoCreateT). If any error occurs, a message is printed
         * to error output and the application exits. See @ref tryCreate() for
         * an alternative.
         */
        void create(const Configuration& configuration);

        /**
         * @brief Complete the context setup
         *
         * Unlike @ref create() just prints a message to error output and
         * returns `false` on error.
         */
        bool tryCreate(const Configuration& configuration);

        /**
         * @brief Whether HRTFs (Head Related Transfer Functions) are enabled
         *
         * HRFTs may not be enabled/disabled in a running context. Instead
         * create a new @ref Context with HRFTs enabled or disabled.
         * @see @ref hrtfStatus(), @ref Audio::Context::Configuration::setHrtf(),
         *      @fn_alc{GetIntegerv} with @def_alc_keyword{HRTF_SOFT}
         * @requires_al_extension Extension @alc_extension{SOFTX,HRTF} or
         *      @alc_extension{SOFT,HRTF}
         */
        bool isHrtfEnabled() const;

        /**
         * @brief HRTF status
         *
         * @see @ref isHrtfEnabled(), @fn_alc{GetIntegerv} with
         *      @def_alc_keyword{HRTF_STATUS_SOFT}
         * @requires_al_extension Extension @alc_extension{SOFTX,HRTF} or
         *      @alc_extension{SOFT,HRTF}
         */
        HrtfStatus hrtfStatus() const;

        /**
         * @brief HRTF specifier
         *
         * Name of the HRTF being used.
         * @see @fn_al{GetString} with @def_alc_keyword{HRTF_SPECIFIER_SOFT}
         * @requires_al_extension @alc_extension{SOFT,HRTF}
         */
        std::string hrtfSpecifierString() const;

        /**
         * @brief Device specifier string
         *
         * @see @ref deviceSpecifierStrings(), @ref vendorString(), @ref rendererString(),
         *      @fn_al{GetString} with @def_alc_keyword{DEVICE_SPECIFIER}
         */
        std::string deviceSpecifierString() const;

        /**
         * @brief Vendor string
         *
         * @see @ref deviceSpecifierString(), @ref rendererString(),
         *      @fn_al{GetString} with @def_al_keyword{VENDOR}
         */
        std::string vendorString() const;

        /**
         * @brief Renderer string
         *
         * @see @ref deviceSpecifierString(), @ref vendorString(),
         *      @fn_al{GetString} with @def_al_keyword{RENDERER}
         */
        std::string rendererString() const;

        /**
         * @brief Version string
         *
         * @see @fn_al{GetString} with @def_al_keyword{VERSION}
         */
        std::string versionString() const;

        /**
         * @brief Extension strings
         *
         * The result is *not* cached, repeated queries will result in repeated
         * OpenAL calls. Note that this function returns list of all extensions
         * reported by the driver (even those not supported by Magnum), see
         * @ref supportedExtensions(), @ref Extension::extensions() or
         * @ref isExtensionSupported() for alternatives.
         * @see @fn_al{Get} with @def_al_keyword{NUM_EXTENSIONS},
         *      @fn_al{GetString} with @def_al_keyword{EXTENSIONS},
         *      @fn_alc{GetString} with @def_alc_keyword{EXTENSIONS}
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
         *
         * @snippet MagnumAudio.cpp Context-isExtensionSupported
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
            return _extensionStatus[extension.index()];
        }

        /**
         * @brief Whether given extension is disabled
         *
         * Can be used for detecting driver bug workarounds. Disabled
         * extensions return @cpp false @ce in @ref isExtensionSupported() even
         * if they are advertised as being supported by the driver.
         */
        template<class T> bool isExtensionDisabled() const {
            return _disabledExtensions[T::Index];
        }

        /**
         * @brief Whether given extension is disabled
         *
         * Can be used e.g. for listing extensions available on current
         * hardware, but for general usage prefer @ref isExtensionDisabled() const,
         * as it does most operations in compile time.
         */
        bool isExtensionDisabled(const Extension& extension) const {
            return _disabledExtensions[extension.index()];
        }

    private:
        bool _displayInitializationLog;

        ALCdevice* _device;
        ALCcontext* _context;

        Math::BoolVector<Implementation::ExtensionCount> _extensionStatus;
        Math::BoolVector<Implementation::ExtensionCount> _disabledExtensions;
        std::vector<Extension> _supportedExtensions;
        std::vector<std::string> _disabledExtensionStrings;
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
         * If set to @cpp -1 @ce (the default), system OpenAL configuration is
         * used.
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
         * If set to @ref Hrtf "Hrtf::Default" (the default), system OpenAL
         * configuration is used.
         * @todoc the fuck doxygen why can't you link to Hrtf::Default?!
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
         * If set to @cpp -1 @ce (the default), no hint will be given to
         * OpenAL.
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
         * If set to @cpp -1 @ce (the default), no hint will be given to
         * OpenAL.
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
         * If set to @cpp -1 @ce (the default), system OpenAL configuration is
         * used.
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

@snippet MagnumAudio.cpp MAGNUM_ASSERT_AUDIO_EXTENSION_SUPPORTED

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

/** @debugoperatorclassenum{Context,Context::HrtfStatus} */
MAGNUM_AUDIO_EXPORT Debug& operator<<(Debug& debug, Context::HrtfStatus value);

}}

#endif
