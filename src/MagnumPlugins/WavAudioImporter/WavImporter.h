#ifndef Magnum_Audio_WavImporter_h
#define Magnum_Audio_WavImporter_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
              Vladimír Vondruš <mosra@centrum.cz>
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
#endif

namespace Magnum { namespace Audio {

/**
@brief WAV importer plugin

Supports mono and stereo files of the following formats:

-   8 bit per channel PCM, imported as @ref Buffer::Format::Mono8 and @ref Buffer::Format::Stereo8
-   16 bit per channel PCM, imported as @ref Buffer::Format::Mono16 and @ref Buffer::Format::Stereo16
-   32-bit IEEE Float, imported as @ref Buffer::Format::MonoFloat / @ref Buffer::Format::StereoFloat
-   64-bit IEEE Float, imported as @ref Buffer::Format::MonoDouble / @ref Buffer::Format::StereoDouble
-   A-Law, imported as @ref Buffer::Format::MonoALaw / @ref Buffer::Format::StereoALaw
-   μ-Law, imported as @ref Buffer::Format::MonoMuLaw / @ref Buffer::Format::StereoMuLaw

Multi-channel formats are not supported.

This plugin is built if `WITH_WAVAUDIOIMPORTER` is enabled when building
Magnum. To use dynamic plugin, you need to load `WavAudioImporter` plugin
from `MAGNUM_PLUGINS_AUDIOIMPORTER_DIR`. To use static plugin or use this as a
dependency of another plugin, you need to request `WavAudioImporter` component
of `Magnum` package in CMake and link to `Magnum::WavAudioImporter` target. See
@ref building, @ref cmake and @ref plugins for more information.
*/
class MAGNUM_WAVAUDIOIMPORTER_EXPORT WavImporter: public AbstractImporter {
    public:
        /** @brief Default constructor */
        explicit WavImporter();

        /** @brief Plugin manager constructor */
        explicit WavImporter(PluginManager::AbstractManager& manager, const std::string& plugin);

    private:
        MAGNUM_WAVAUDIOIMPORTER_LOCAL Features doFeatures() const override;
        MAGNUM_WAVAUDIOIMPORTER_LOCAL bool doIsOpened() const override;
        MAGNUM_WAVAUDIOIMPORTER_LOCAL void doOpenData(Containers::ArrayView<const char> data) override;
        MAGNUM_WAVAUDIOIMPORTER_LOCAL void doClose() override;

        MAGNUM_WAVAUDIOIMPORTER_LOCAL Buffer::Format doFormat() const override;
        MAGNUM_WAVAUDIOIMPORTER_LOCAL UnsignedInt doFrequency() const override;
        MAGNUM_WAVAUDIOIMPORTER_LOCAL Containers::Array<char> doData() override;

        Containers::Array<char> _data;
        Buffer::Format _format;
        UnsignedInt _frequency;
};

}}

#endif
