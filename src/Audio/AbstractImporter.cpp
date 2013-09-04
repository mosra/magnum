/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

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

#include "AbstractImporter.h"

#include <fstream>
#include <Containers/Array.h>
#include <Utility/Assert.h>

namespace Magnum { namespace Audio {

AbstractImporter::AbstractImporter() = default;

AbstractImporter::AbstractImporter(PluginManager::AbstractManager* manager, std::string plugin): PluginManager::AbstractPlugin(manager, std::move(plugin)) {}

bool AbstractImporter::openData(Containers::ArrayReference<const unsigned char> data) {
    CORRADE_ASSERT(features() & Feature::OpenData,
        "Audio::AbstractImporter::openData(): feature not supported", nullptr);

    close();
    doOpenData(data);
    return isOpened();
}

void AbstractImporter::doOpenData(Containers::ArrayReference<const unsigned char>) {
    CORRADE_ASSERT(false, "Audio::AbstractImporter::openData(): feature advertised but not implemented", );
}

bool AbstractImporter::openFile(const std::string& filename) {
    close();
    doOpenFile(filename);
    return isOpened();
}

void AbstractImporter::doOpenFile(const std::string& filename) {
    CORRADE_ASSERT(features() & Feature::OpenData, "Audio::AbstractImporter::openFile(): not implemented", );

    /* Open file */
    std::ifstream in(filename.data(), std::ios::binary);
    if(!in.good()) {
        Error() << "Trade::AbstractImporter::openFile(): cannot open file" << filename;
        return;
    }

    /* Create array to hold file contents */
    in.seekg(0, std::ios::end);
    Containers::Array<unsigned char> data(in.tellg());

    /* Read data, close */
    in.seekg(0, std::ios::beg);
    in.read(reinterpret_cast<char*>(data.begin()), data.size());
    in.close();

    doOpenData(data);
}

void AbstractImporter::close() {
    if(isOpened()) {
        doClose();
        CORRADE_INTERNAL_ASSERT(!isOpened());
    }
}

Buffer::Format AbstractImporter::format() const {
    CORRADE_ASSERT(isOpened(), "Audio::AbstractImporter::format(): no file opened", Buffer::Format());
    return doFormat();
}

UnsignedInt AbstractImporter::frequency() const {
    CORRADE_ASSERT(isOpened(), "Audio::AbstractImporter::frequency(): no file opened", {});
    return doFrequency();
}

Containers::Array<unsigned char> AbstractImporter::data() {
    #ifndef CORRADE_GCC45_COMPATIBILITY
    CORRADE_ASSERT(isOpened(), "Audio::AbstractImporter::data(): no file opened", nullptr);
    #else
    CORRADE_ASSERT(isOpened(), "Audio::AbstractImporter::data(): no file opened", {});
    #endif
    return doData();
}

}}
