/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
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

#include <Corrade/Containers/ArrayView.h>
#include <Corrade/Utility/Arguments.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/Audio/Context.h"

namespace Magnum {

/** @page magnum-al-info Magnum OpenAL Info
@brief Displays information about Magnum OpenAL capabilities

@m_footernavigation
@m_keywords{magnum-al-info al-info}

@m_div{m-button m-primary} <a href="https://magnum.graphics/showcase/al-info/">@m_div{m-big}Live web version @m_enddiv @m_div{m-small} uses WebAssembly & WebAudio @m_enddiv </a> @m_enddiv

This utility is built if `MAGNUM_WITH_AL_INFO` is enabled when building Magnum.
To use this utility with CMake, you need to request the `al-info` component of
the `Magnum` package and use the `Magnum::al-info` target for example in a
custom command:

@code{.cmake}
find_package(Magnum REQUIRED al-info)

add_custom_command(OUTPUT ... COMMAND Magnum::al-info ...)
@endcode

See @ref building, @ref cmake and the @ref Audio namespace for more
information.

@section magnum-al-info-usage Usage

@code{.sh}
magnum-al-info [--magnum-...] [-h|--help] [-s|--short] [--extension-strings]
    [--frequency Hz] [--hrtf true|false] [--mono-source-count N]
    [--stereo-source-count N] [--refresh-rate Hz]
@endcode

Arguments:

-   `-h`,` --help` --- display this help message and exit
-   `-s`, `--short` --- display just essential info and exit
-   `--extension-strings` --- list all extension strings provided by the driver
    (implies `--short`)
-   `--frequency Hz` --- override OpenAL context frequency (default: `-1`)
-   `--hrtf true|false` --- override OpenAL HRTF configuration
-   `--mono-source-count N` --- override OpenAL mono source count (default:
    `-1`)
-   `--stereo-source-count N` --- override OpenAL stereo source count (default:
    `-1`)
-   `--refresh-rate Hz` --- override OpenAL refresh rate (default: `-1`)
-   `--magnum-...` --- engine-specific options (see
    @ref Audio-Context-command-line for details)

@subsection magnum-al-info-usage-emscripten Usage on Emscripten

When installing Magnum, point the `MAGNUM_DEPLOY_PREFIX` CMake variable to your
webserver location. The Magnum Info utility is then available in a `magnum-al-info/`
subdirectory of given location. You can pass all command-line arguments from
above to it via GET parameters. See the relevant section of the
@ref platforms-html5-environment "Emscripten guide" for more information.

@section magnum-al-info-example Example output

@code{.shell-session}

  +---------------------------------------------------------+
  |   Information about Magnum engine OpenAL capabilities   |
  +---------------------------------------------------------+

Audio renderer: OpenAL Soft by OpenAL Community
OpenAL version: 1.1 ALSOFT 1.23.1
Available devices:
    OpenAL Soft
Current device: OpenAL Soft

Frequency: 44100 Hz
HRTF: Audio::Context::HrtfStatus::Disabled
Mono source count: 255
Stereo source count: 1
Refresh rate: 50 Hz

Vendor extension support:
    ALC_ENUMERATION_EXT                                           SUPPORTED
    ALC_SOFTX_HRTF                                                   -
    ALC_SOFT_HRTF                                                 SUPPORTED
    AL_EXT_ALAW                                                   SUPPORTED
    ...
@endcode

*/

}

using namespace Magnum;

int main(const int argc, const char* const* const argv) {
    Utility::Arguments args;
    args.addBooleanOption('s', "short").setHelp("short", "display just essential info and exit")
        .addBooleanOption("extension-strings").setHelp("extension-strings", "list all extension strings provided by the driver (implies --short)")
        .addOption("frequency", "-1").setHelp("frequency", "override OpenAL context frequency", "Hz")
        .addOption("hrtf", "").setHelp("hrtf", "override OpenAL HRTF configuration", "true|false")
        .addOption("mono-source-count", "-1").setHelp("mono-source-count", "override OpenAL mono source count", "N")
        .addOption("stereo-source-count", "-1").setHelp("stereo-source-count", "override OpenAL stereo source count", "N")
        .addOption("refresh-rate", "-1").setHelp("refresh-rate", "override OpenAL refresh rate", "Hz")
        .addSkippedPrefix("magnum", "engine-specific options")
        .parse(argc, argv);

    Debug() << "";
    Debug() << "  +---------------------------------------------------------+";
    Debug() << "  |   Information about Magnum engine OpenAL capabilities   |";
    Debug() << "  +---------------------------------------------------------+";
    Debug() << "";

    Audio::Context::Configuration configuration;
    configuration
        .setFrequency(args.value<Int>("frequency"))
        .setMonoSourceCount(args.value<Int>("mono-source-count"))
        .setStereoSourceCount(args.value<Int>("stereo-source-count"))
        .setRefreshRate(args.value<Int>("refresh-rate"));
    if(!args.value("hrtf").empty())
        configuration.setHrtf(args.value<bool>("hrtf") ?
            Audio::Context::Configuration::Hrtf::Enabled :
            Audio::Context::Configuration::Hrtf::Disabled);

    Audio::Context c{configuration, argc, argv};
    Debug() << "Available devices:";
    for(const auto& device: Audio::Context::deviceSpecifierStrings())
        Debug() << "   " << device;
    Debug() << "Current device:" << c.deviceSpecifierString() << Debug::newline;

    Debug{} << "Frequency:" << c.frequency() << "Hz";
    {
        Debug d;
        d << "HRTF:" << c.hrtfStatus();
        if(c.isHrtfEnabled())
            d << Debug::nospace << "," << c.hrtfSpecifierString();
    }
    Debug{} << "Mono source count:" << c.monoSourceCount();
    Debug{} << "Stereo source count:" << c.stereoSourceCount();
    Debug{} << "Refresh rate:" << c.refreshRate() << "Hz" << Debug::newline;

    if(args.isSet("extension-strings")) {
        Debug() << "Extension strings:";
        /* Because printing all extensions on a single line isn't helpful. */
        for(Containers::StringView e: c.extensionStrings())
            Debug{} << "   " << e;
        return 0;
    }

    if(args.isSet("short")) return 0;

    Debug() << "Vendor extension support:";
    for(const auto& extension: Audio::Extension::extensions()) {
        std::string extensionName = extension.string();
        Debug d;
        d << "   " << extensionName << std::string(60-extensionName.size(), ' ');
        if(c.isExtensionSupported(extension))
            d << "SUPPORTED";
        else if(c.isExtensionDisabled(extension))
            d << " removed";
        else
            d << "   -";
    }
}
