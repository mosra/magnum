/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
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

#include "AbstractGlyphCache.h"

#include "Magnum/Image.h"
#include "Magnum/TextureTools/Atlas.h"

namespace Magnum { namespace Text {

AbstractGlyphCache::AbstractGlyphCache(const Vector2i& size, const Vector2i& padding): _size{size}, _padding{padding} {
    /* Default "Not Found" glyph. Can't do just `.insert({0, {}})` because
       that's ambiguous in C++17, due to a new insert(node_type&&) overload. */
    glyphs.insert({0, std::pair<Vector2i, Range2Di>{}});
}

AbstractGlyphCache::~AbstractGlyphCache() = default;

std::vector<Range2Di> AbstractGlyphCache::reserve(const std::vector<Vector2i>& sizes) {
    CORRADE_ASSERT((glyphs.size() == 1 && glyphs.at(0) == std::pair<Vector2i, Range2Di>()),
        "Text::AbstractGlyphCache::reserve(): reserving space in non-empty cache is not yet implemented", {});
    glyphs.reserve(glyphs.size() + sizes.size());
    return TextureTools::atlas(_size, sizes, _padding);
}

void AbstractGlyphCache::insert(const UnsignedInt glyph, const Vector2i& position, const Range2Di& rectangle) {
    const std::pair<Vector2i, Range2Di> glyphData = {position-_padding, rectangle.padded(_padding)};

    /* Overwriting "Not Found" glyph */
    if(glyph == 0) glyphs[0] = glyphData;

    /* Inserting new glyph */
    else CORRADE_INTERNAL_ASSERT_OUTPUT(glyphs.insert({glyph, glyphData}).second);
}

void AbstractGlyphCache::setImage(const Vector2i& offset, const ImageView2D& image) {
    CORRADE_ASSERT((offset >= Vector2i{} && offset + image.size() <= _size).all(),
        "Text::AbstractGlyphCache::setImage():" << Range2Di::fromSize(offset, image.size()) << "out of bounds for texture size" << _size, );

    doSetImage(offset, image);
}

Image2D AbstractGlyphCache::image() {
    CORRADE_ASSERT(features() & GlyphCacheFeature::ImageDownload,
        "Text::AbstractGlyphCache::image(): feature not supported", Image2D{{}});

    return doImage();
}

Image2D AbstractGlyphCache::doImage() {
    CORRADE_ASSERT(false, "Text::AbstractGlyphCache::image(): feature advertised but not implemented", Image2D{{}});
}

}}
