#ifndef Magnum_Text_Implementation_abstractGlyphCacheState_h
#define Magnum_Text_Implementation_abstractGlyphCacheState_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025, 2026
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

#include <Corrade/Containers/Array.h>

#include "Magnum/Image.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/Math/Vector2.h"
#include "Magnum/Text/AbstractGlyphCache.h"
#include "Magnum/TextureTools/Atlas.h"

namespace Magnum { namespace Text {

struct AbstractGlyphCache::State {
    explicit State(PixelFormat format, const Vector3i& size, PixelFormat processedFormat, const Vector2i& processedSize, const Vector2i& padding);

    /* GlyphCacheGL, GlyphCacheArrayGL, DistanceFieldGlyphCacheGL derive from
       this to avoid another state struct allocation or putting the extra state
       as members, inflating header size */
    virtual ~State();

    Image3D image;
    TextureTools::AtlasLandfill atlas;

    PixelFormat processedFormat;
    Vector2i processedSize;
    Vector2i padding;

    /* 0/4 bytes free */

    /* First element is glyph position relative to a point on the baseline,
       second layer in the texture atlas, third a region in the atlas
       slice. Index of the item is ID of the glyph in the cache, refered to
       from the fontGlyphMapping array. Index 0 is reserved for an invalid
       glyph. */
    Containers::Array<Containers::Triple<Vector2i, Int, Range2Di>> glyphs;
    /* `fontRanges[i]` to `fontRanges[i + 1]` is the range in
       `fontGlyphMapping` containing a mapping for glyphs from font `i`,
       `fontGlyphMapping[fontRanges[i]] + j` is then mapping from glyph ID `j`
       from font `i` to index in the `glyphs` array, or is 0 if given
       glyph isn't present in the cache (which then maps to the invalid
       glyph). */
    struct Font {
        UnsignedInt offset;
        /* 4 bytes free on 64b, but not so critical I think */
        const AbstractFont* pointer;
    };
    Containers::Array<Font> fonts;
    /* With an assumption that majority of font glyphs get put into a cache,
       this achieves O(1) mapping from a font ID + font-specific glyph ID pair
       to a cache-global glyph ID with far less overhead than a hashmap would,
       and much less memory used as well compared to storing a key, value and a
       hash for each mapping entry.

       Another assumption is that there's no more than 64k glyphs in total,
       which makes the mapping save half memory compared to storing 32-bit
       ints. 64K glyphs is enough to fill a 4K texture with 16x16 glyphs, which
       seems enough for now. It however might get reached at some point in
       practice, in which case the type would simply get changed to a 32-bit
       one (and the assertion in addGlyph() then removed). */
    Containers::Array<UnsignedShort> fontGlyphMapping;
};

}}

#endif
