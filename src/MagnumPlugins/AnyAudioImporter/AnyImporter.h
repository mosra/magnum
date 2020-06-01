#ifndef Magnum_Trade_AnyImporter_h
#define Magnum_Trade_AnyImporter_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Class @ref Magnum::Audio::AnyImporter
 */

#include <Corrade/Containers/Pointer.h>

#include "Magnum/Audio/AbstractImporter.h"
#include "MagnumPlugins/AnyAudioImporter/configure.h"

#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef MAGNUM_ANYAUDIOIMPORTER_BUILD_STATIC
    #ifdef AnyAudioImporter_EXPORTS
        #define MAGNUM_ANYAUDIOIMPORTER_EXPORT CORRADE_VISIBILITY_EXPORT
    #else
        #define MAGNUM_ANYAUDIOIMPORTER_EXPORT CORRADE_VISIBILITY_IMPORT
    #endif
#else
    #define MAGNUM_ANYAUDIOIMPORTER_EXPORT CORRADE_VISIBILITY_STATIC
#endif
#define MAGNUM_ANYAUDIOIMPORTER_LOCAL CORRADE_VISIBILITY_LOCAL
#else
#define MAGNUM_ANYAUDIOIMPORTER_EXPORT
#define MAGNUM_ANYAUDIOIMPORTER_LOCAL
#endif

namespace Magnum { namespace Audio {

/**
@brief Any audio importer plugin

@m_keywords{AnyAudioImporter}

Detects file type based on file extension, loads corresponding plugin and then
tries to open the file with it. Supported formats:

-   AAC (`*.aac`), loaded with any plugin that provides `AacAudioImporter`
-   MP3 (`*.mp3`), loaded with any plugin that provides `Mp3AudioImporter`
-   OGG Vorbis (`*.ogg`), loaded with any plugin that provides
    `VorbisAudioImporter`
-   WAV (`*.wav`), loaded with @ref WavImporter "WavAudioImporter" or any other
    plugin that provides it
-   FLAC (`*.flac`), loaded with any plugin that provides `FlacAudioImporter`

Only loading from files is supported.

@section Audio-AnyImporter-usage Usage

This plugin depends on the @ref Audio library and is built if
`WITH_ANYAUDIOIMPORTER` is enabled when building Magnum. To use as a dynamic
plugin, load @cpp "AnyAudioImporter" @ce via
@ref Corrade::PluginManager::Manager.

Additionally, if you're using Magnum as a CMake subproject, do the following:

@code{.cmake}
set(WITH_ANYAUDIOIMPORTER ON CACHE BOOL "" FORCE)
add_subdirectory(magnum EXCLUDE_FROM_ALL)

# So the dynamically loaded plugin gets built implicitly
add_dependencies(your-app Magnum::AnyAudioImporter)
@endcode

To use as a static plugin or as a dependency of another plugin with CMake, you
need to request the `AnyAudioImporter` component of the `Magnum` package and
link to the `Magnum::AnyAudioImporter` target:

@code{.cmake}
find_package(Magnum REQUIRED AnyAudioImporter)

# ...
target_link_libraries(your-app PRIVATE Magnum::AnyAudioImporter)
@endcode

See @ref building, @ref cmake and @ref plugins for more information.
*/
class MAGNUM_ANYAUDIOIMPORTER_EXPORT AnyImporter: public AbstractImporter {
    public:
        /** @brief Constructor with access to plugin manager */
        explicit AnyImporter(PluginManager::Manager<AbstractImporter>& manager);

        /** @brief Plugin manager constructor */
        explicit AnyImporter(PluginManager::AbstractManager& manager, const std::string& plugin);

        ~AnyImporter();

    private:
        MAGNUM_ANYAUDIOIMPORTER_LOCAL ImporterFeatures doFeatures() const override;
        MAGNUM_ANYAUDIOIMPORTER_LOCAL bool doIsOpened() const override;
        MAGNUM_ANYAUDIOIMPORTER_LOCAL void doClose() override;
        MAGNUM_ANYAUDIOIMPORTER_LOCAL void doOpenFile(const std::string& filename) override;

        MAGNUM_ANYAUDIOIMPORTER_LOCAL BufferFormat doFormat() const override;
        MAGNUM_ANYAUDIOIMPORTER_LOCAL UnsignedInt doFrequency() const override;
        MAGNUM_ANYAUDIOIMPORTER_LOCAL Containers::Array<char> doData() override;

        Containers::Pointer<AbstractImporter> _in;
};

}}

#endif
