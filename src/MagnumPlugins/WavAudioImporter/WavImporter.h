#ifndef Magnum_Audio_WavImporter_h
#define Magnum_Audio_WavImporter_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016
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

/** @file
 * @brief Class @ref Magnum::Audio::WavImporter
 */

#include <Corrade/Containers/Array.h>

#include "Magnum/Audio/AbstractImporter.h"

namespace Magnum { namespace Audio {

/**
@brief WAV importer plugin

Supports mono and stereo PCM files with 8 or 16 bits per channel. The files are
imported with @ref Buffer::Format::Mono8, @ref Buffer::Format::Mono16,
@ref Buffer::Format::Stereo8 or @ref Buffer::Format::Stereo16, respectively.

This plugin is built if `WITH_WAVAUDIOIMPORTER` is enabled when building
Magnum. To use dynamic plugin, you need to load `WavAudioImporter` plugin
from `MAGNUM_PLUGINS_AUDIOIMPORTER_DIR`. To use static plugin or use this as a
dependency of another plugin, you need to request `WavAudioImporter` component
of `Magnum` package in CMake and link to `${MAGNUM_WAVAUDIOIMPORTER_LIBRARIES}`.
See @ref building, @ref cmake and @ref plugins for more information.
*/
class WavImporter: public AbstractImporter {
    public:
        /** @brief Default constructor */
        explicit WavImporter();

        /** @brief Plugin manager constructor */
        explicit WavImporter(PluginManager::AbstractManager& manager, std::string plugin);

    private:
        Features doFeatures() const override;
        bool doIsOpened() const override;
        void doOpenData(Containers::ArrayView<const char> data) override;
        void doClose() override;

        Buffer::Format doFormat() const override;
        UnsignedInt doFrequency() const override;
        Containers::Array<char> doData() override;

        Containers::Array<char> _data;
        Buffer::Format _format;
        UnsignedInt _frequency;
};

}}

#endif
