/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023 Vladimír Vondruš <mosra@centrum.cz>

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

#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/Containers/ArrayViewStl.h> /** @todo remove once std::vector is gone */

#include "Magnum/Image.h"
#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
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

    /* Rotations are not yet supported */
    TextureTools::AtlasLandfill atlas{_size};
    atlas.setPadding(_padding)
         .clearFlags(TextureTools::AtlasLandfillFlag::RotatePortrait|
                     TextureTools::AtlasLandfillFlag::RotateLandscape);

    /* Create the output array. Because the new atlas packer doesn't accept
       zero sizes, change those to be (1, 1) instead. A new interface will
       disallow them as well, but here we want to keep backwards
       compatibility. */
    std::vector<Range2Di> out(sizes.size());
    for(std::size_t i = 0; i != sizes.size(); ++i)
        out[i].max() = Math::max(sizes[i], Vector2i{1});

    /* The error message matches what the old TextureTools::atlas() did. Not
       great, but that's the interface we should stay compatible with. */
    if(!atlas.add(Containers::stridedArrayView(out).slice(&Range2Di::max),
                  Containers::stridedArrayView(out).slice(&Range2Di::min)))
    {
        Error{} << "Text::AbstractGlyphCache::reserve(): requested atlas size"
                << _size << "is too small to fit" << sizes.size()
                << "textures. Generated atlas will be empty.";
        return {};
    }

    /* Update the ranges max values to match the new offsets, undo the zero
       sizes being (1, 1) */
    for(std::size_t i = 0; i != sizes.size(); ++i)
        out[i].max() = out[i].min() + sizes[i];

    return out;
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
        "Text::AbstractGlyphCache::setImage():" << Range2Di::fromSize(offset, image.size()) << "out of range for texture size" << _size, );

    doSetImage(offset, image);
}

Image2D AbstractGlyphCache::image() {
    CORRADE_ASSERT(features() & GlyphCacheFeature::ImageDownload,
        "Text::AbstractGlyphCache::image(): feature not supported", Image2D{PixelFormat::R8Unorm});

    return doImage();
}

Image2D AbstractGlyphCache::doImage() {
    CORRADE_ASSERT_UNREACHABLE("Text::AbstractGlyphCache::image(): feature advertised but not implemented", Image2D{PixelFormat::R8Unorm});
}

}}
