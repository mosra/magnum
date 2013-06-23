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

#include "AbstractFont.h"

#include <fstream>
#include <Containers/Array.h>
#include <Utility/Unicode.h>

namespace Magnum { namespace Text {

AbstractFont::AbstractFont(): _size(0.0f) {}

AbstractFont::AbstractFont(PluginManager::AbstractManager* manager, std::string plugin): AbstractPlugin(manager, std::move(plugin)), _size(0.0f) {}

bool AbstractFont::openData(const std::vector<std::pair<std::string, Containers::ArrayReference<const unsigned char>>>& data, const Float size) {
    CORRADE_ASSERT(features() & Feature::OpenData,
        "Text::AbstractFont::openData(): feature not supported", false);
    CORRADE_ASSERT(!data.empty(),
        "Text::AbstractFont::openData(): no data passed", false);

    close();
    doOpenData(data, size);
    return isOpened();
}

void AbstractFont::doOpenData(const std::vector<std::pair<std::string, Containers::ArrayReference<const unsigned char>>>& data, const Float size) {
    CORRADE_ASSERT(!(features() & Feature::MultiFile),
        "Text::AbstractFont::openData(): feature advertised but not implemented", );
    CORRADE_ASSERT(data.size() == 1,
        "Text::AbstractFont::openData(): expected just one file for single-file format", );

    close();
    doOpenSingleData(data[0].second, size);
}

bool AbstractFont::openSingleData(const Containers::ArrayReference<const unsigned char> data, const Float size) {
    CORRADE_ASSERT(features() & Feature::OpenData,
        "Text::AbstractFont::openSingleData(): feature not supported", false);
    CORRADE_ASSERT(!(features() & Feature::MultiFile),
        "Text::AbstractFont::openSingleData(): the format is not single-file", false);

    close();
    doOpenSingleData(data, size);
    return isOpened();
}

void AbstractFont::doOpenSingleData(Containers::ArrayReference<const unsigned char>, Float) {
    CORRADE_ASSERT(false, "Text::AbstractFont::openSingleData(): feature advertised but not implemented", );
}

bool AbstractFont::openFile(const std::string& filename, const Float size) {
    close();
    doOpenFile(filename, size);
    return isOpened();
}

void AbstractFont::doOpenFile(const std::string& filename, const Float size) {
    CORRADE_ASSERT(features() & Feature::OpenData && !(features() & Feature::MultiFile),
        "Text::AbstractFont::openFile(): not implemented", );

    /* Open file */
    std::ifstream in(filename.data(), std::ios::binary);
    if(!in.good()) {
        Error() << "Trade::AbstractFont::openFile(): cannot open file" << filename;
        return;
    }

    /* Create array to hold file contents */
    in.seekg(0, std::ios::end);
    Containers::Array<unsigned char> data(in.tellg());

    /* Read data, close */
    in.seekg(0, std::ios::beg);
    in.read(reinterpret_cast<char*>(data.begin()), data.size());
    in.close();

    doOpenSingleData(data, size);
}

void AbstractFont::close() {
    if(isOpened()) doClose();
}

void AbstractFont::createGlyphCache(GlyphCache* const cache, const std::string& characters) {
    CORRADE_ASSERT(isOpened(), "Text::AbstractFont::createGlyphCache(): no font opened", );

    doCreateGlyphCache(cache, Utility::Unicode::utf32(characters));
}

AbstractLayouter* AbstractFont::layout(const GlyphCache* const cache, const Float size, const std::string& text) {
    CORRADE_ASSERT(isOpened(), "Text::AbstractFont::layout(): no font opened", nullptr);

    return doLayout(cache, size, text);
}

AbstractLayouter::AbstractLayouter(): _glyphCount(0) {}

AbstractLayouter::~AbstractLayouter() {}

}}
