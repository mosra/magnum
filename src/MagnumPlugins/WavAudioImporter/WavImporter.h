#ifndef Magnum_Audio_WavImporter_h
#define Magnum_Audio_WavImporter_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2016 Alice Margatroid <loveoverwhelming@gmail.com>

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
 * @brief Class @ref Magnum::Audio::WavImporter
 */

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/Optional.h>

#include "Magnum/Audio/AbstractImporter.h"

#include "MagnumPlugins/WavAudioImporter/configure.h"

#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef MAGNUM_WAVAUDIOIMPORTER_BUILD_STATIC
    #if defined(WavAudioImporter_EXPORTS) || defined(WavAudioImporterObjects_EXPORTS)
        #define MAGNUM_WAVAUDIOIMPORTER_EXPORT CORRADE_VISIBILITY_EXPORT
    #else
        #define MAGNUM_WAVAUDIOIMPORTER_EXPORT CORRADE_VISIBILITY_IMPORT
    #endif
#else
    #define MAGNUM_WAVAUDIOIMPORTER_EXPORT CORRADE_VISIBILITY_STATIC
#endif
#define MAGNUM_WAVAUDIOIMPORTER_LOCAL CORRADE_VISIBILITY_LOCAL
#else
#define MAGNUM_WAVAUDIOIMPORTER_EXPORT
#define MAGNUM_WAVAUDIOIMPORTER_LOCAL
#endif

namespace Magnum { namespace Audio {

/**
@brief WAV importer plugin

@m_keywords{WavAudioImporter}

Supports mono and stereo files of the following formats:

-   8 bit per channel PCM, imported as @ref BufferFormat::Mono8 and
    @ref BufferFormat::Stereo8
-   16 bit per channel PCM, imported as @ref BufferFormat::Mono16 and
    @ref BufferFormat::Stereo16
-   32-bit IEEE Float, imported as @ref BufferFormat::MonoFloat /
    @ref BufferFormat::StereoFloat
-   64-bit IEEE Float, imported as @ref BufferFormat::MonoDouble /
    @ref BufferFormat::StereoDouble
-   A-Law, imported as @ref BufferFormat::MonoALaw / @ref BufferFormat::StereoALaw
-   μ-Law, imported as @ref BufferFormat::MonoMuLaw / @ref BufferFormat::StereoMuLaw

Both Little-Endian files (with a `RIFF` header) and Big-Endian files (with
a `RIFX` header) are supported, data is converted to machine endian on import.

@section Audio-WavImporter-usage Usage

This plugin is built if `WITH_WAVAUDIOIMPORTER` is enabled when building
Magnum. To use dynamic plugin, load @cpp "WavAudioImporter" @ce
via @ref Corrade::PluginManager::Manager.

Additionally, if you're using Magnum as a CMake subproject, do the following:

@code{.cmake}
set(WITH_WAVAUDIOIMPORTER ON CACHE BOOL "" FORCE)
add_subdirectory(magnum EXCLUDE_FROM_ALL)

# So the dynamically loaded plugin gets built implicitly
add_dependencies(your-app Magnum::WavAudioImporter)
@endcode

To use as a static plugin or as a dependency of another plugin with CMake, you
need to request the `WavAudioImporter` component of the `Magnum` package and
link to the `Magnum::WavAudioImporter` target:

@code{.cmake}
find_package(Magnum REQUIRED WavAudioImporter)

# ...
target_link_libraries(your-app PRIVATE Magnum::WavAudioImporter)
@endcode

See @ref building, @ref cmake and @ref plugins for more information.

@section Audio-WavImporter-limitations Behavior and limitations

Multi-channel formats are not supported.
*/
class MAGNUM_WAVAUDIOIMPORTER_EXPORT WavImporter: public AbstractImporter {
    public:
        /** @brief Default constructor */
        explicit WavImporter();

        /** @brief Plugin manager constructor */
        explicit WavImporter(PluginManager::AbstractManager& manager, const std::string& plugin);

    private:
        MAGNUM_WAVAUDIOIMPORTER_LOCAL ImporterFeatures doFeatures() const override;
        MAGNUM_WAVAUDIOIMPORTER_LOCAL bool doIsOpened() const override;
        MAGNUM_WAVAUDIOIMPORTER_LOCAL void doOpenData(Containers::ArrayView<const char> data) override;
        MAGNUM_WAVAUDIOIMPORTER_LOCAL void doClose() override;

        MAGNUM_WAVAUDIOIMPORTER_LOCAL BufferFormat doFormat() const override;
        MAGNUM_WAVAUDIOIMPORTER_LOCAL UnsignedInt doFrequency() const override;
        MAGNUM_WAVAUDIOIMPORTER_LOCAL Containers::Array<char> doData() override;

        Containers::Optional<Containers::Array<char>> _data;
        BufferFormat _format;
        UnsignedInt _frequency;
};

}}

#endif
