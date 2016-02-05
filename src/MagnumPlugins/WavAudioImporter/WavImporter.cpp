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

#include "WavImporter.h"

#include <Corrade/Utility/Assert.h>
#include <Corrade/Utility/Debug.h>
#include <Corrade/Utility/Endianness.h>

#include "MagnumPlugins/WavAudioImporter/WavHeader.h"

namespace Magnum { namespace Audio {

WavImporter::WavImporter() = default;

WavImporter::WavImporter(PluginManager::AbstractManager& manager, std::string plugin): AbstractImporter(manager, std::move(plugin)) {}

auto WavImporter::doFeatures() const -> Features { return Feature::OpenData; }

bool WavImporter::doIsOpened() const { return _data; }

void WavImporter::doOpenData(Containers::ArrayView<const char> data) {
    /* Check file size */
    if(data.size() < sizeof(WavHeader)) {
        Error() << "Audio::WavImporter::openData(): the file is too short:" << data.size() << "bytes";
        return;
    }

    /* Get header contents and fix endianness */
    WavHeader header(*reinterpret_cast<const WavHeader*>(data.begin()));
    Utility::Endianness::littleEndianInPlace(header.chunkSize,
        header.subChunk1Size, header.audioFormat, header.numChannels,
        header.sampleRate, header.byteRate, header.blockAlign,
        header.bitsPerSample, header.subChunk2Size);

    /* Check file signature */
    if(std::strncmp(header.chunkId, "RIFF", 4) != 0 ||
       std::strncmp(header.format, "WAVE", 4) != 0 ||
       std::strncmp(header.subChunk1Id, "fmt ", 4) != 0 ||
       std::strncmp(header.subChunk2Id, "data", 4) != 0) {
        Error() << "Audio::WavImporter::openData(): the file signature is invalid";
        return;
    }

    /* Check file size */
    if(header.chunkSize + 8 != data.size()) {
        Error() << "Audio::WavImporter::openData(): the file has improper size, expected"
                << header.chunkSize + 8 << "but got" << data.size();
        return;
    }

    /* Check PCM format */
    if(header.audioFormat != 1) {
        Error() << "Audio::WavImporter::openData(): unsupported audio format" << header.audioFormat;
        return;
    }

    /* Verify more things */
    if(header.subChunk1Size != 16 ||
       header.subChunk2Size + 44 != data.size() ||
       header.blockAlign != header.numChannels*header.bitsPerSample/8 ||
       header.byteRate != header.sampleRate*header.blockAlign) {
        Error() << "Audio::WavImporter::openData(): the file is corrupted";
        return;
    }

    /* Decide about format */
    if(header.numChannels == 1 && header.bitsPerSample == 8)
        _format = Buffer::Format::Mono8;
    else if(header.numChannels == 1 && header.bitsPerSample == 16)
        _format = Buffer::Format::Mono16;
    else if(header.numChannels == 2 && header.bitsPerSample == 8)
        _format = Buffer::Format::Stereo8;
    else if(header.numChannels == 2 && header.bitsPerSample == 16)
        _format = Buffer::Format::Stereo16;
    else {
        Error() << "Audio::WavImporter::openData(): unsupported channel count"
                << header.numChannels << "with" << header.bitsPerSample
                << "bits per sample";
        return;
    }

    /* Save frequency */
    _frequency = header.sampleRate;

    /** @todo Convert the data from little endian too */
    CORRADE_INTERNAL_ASSERT(!Utility::Endianness::isBigEndian());

    /* Copy the data */
    _data = Containers::Array<char>(header.subChunk2Size);
    std::copy(data.begin()+sizeof(WavHeader), data.end(), _data.begin());
    return;
}

void WavImporter::doClose() { _data = nullptr; }

Buffer::Format WavImporter::doFormat() const { return _format; }

UnsignedInt WavImporter::doFrequency() const { return _frequency; }

Containers::Array<char> WavImporter::doData() {
    Containers::Array<char> copy(_data.size());
    std::copy(_data.begin(), _data.end(), copy.begin());
    return copy;
}

}}
