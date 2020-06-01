#ifndef Magnum_Text_AbstractGlyphCache_h
#define Magnum_Text_AbstractGlyphCache_h
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
 * @brief Class @ref Magnum::Text::AbstractGlyphCache
 * @m_since{2019,10}
 */

#include <vector>
#include <unordered_map>

#include "Magnum/Magnum.h"
#include "Magnum/Math/Range.h"
#include "Magnum/Text/visibility.h"

namespace Magnum { namespace Text {

/**
@brief Features supported by a particular glyph cache implementation
@m_since{2019,10}

@see @ref GlyphCacheFeatures, @ref AbstractGlyphCache::features()
*/
enum class GlyphCacheFeature: UnsignedByte {
    /**
     * Ability to download glyph cache data using
     * @ref AbstractGlyphCache::image(). May not be supported by glyph caches
     * on embedded platforms that don't have an ability to get texture data
     * back from a GPU.
     */
    ImageDownload = 1 << 0
};

/**
@brief Set of features supported by a glyph cache
@m_since{2019,10}

@see @ref AbstractGlyphCache::features()
*/
typedef Containers::EnumSet<GlyphCacheFeature> GlyphCacheFeatures;

CORRADE_ENUMSET_OPERATORS(GlyphCacheFeatures)

/**
@brief Base for glyph caches
@m_since{2019,10}

An API-agnostic base for glyph caches. See @ref GlyphCache and
@ref DistanceFieldGlyphCache for concrete implementations.

@section Text-AbstractGlyphCache-subclassing Subclassing

The subclass needs to implement the @ref doSetImage() function and manage the
glyph cache image. The public @ref setImage() function already does checking
for rectangle bounds so it's not needed to do it again on the implementation
side.
*/
class MAGNUM_TEXT_EXPORT AbstractGlyphCache {
    public:
        /**
         * @brief Constructor
         * @param size              Glyph cache texture size
         * @param padding           Padding around every glyph
         */
        explicit AbstractGlyphCache(const Vector2i& size, const Vector2i& padding = {});

        virtual ~AbstractGlyphCache();

        /** @brief Features supported by this glyph cache implementation */
        GlyphCacheFeatures features() const { return doFeatures(); }

        /** Glyph cache texture size */
        Vector2i textureSize() const { return _size; }

        /** @brief Glyph padding */
        Vector2i padding() const { return _padding; }

        /** @brief Count of glyphs in the cache */
        std::size_t glyphCount() const { return glyphs.size(); }

        /**
         * @brief Parameters of given glyph
         * @param glyph         Glyph ID
         *
         * First tuple element is glyph position relative to point on baseline,
         * second element is glyph region in texture atlas.
         *
         * Returned values include padding.
         *
         * If no glyph is found, glyph @cpp 0 @ce is returned, which is by
         * default on zero position and has zero region in texture atlas. You
         * can reset it to some meaningful value in @ref insert().
         * @see @ref padding()
         */
        std::pair<Vector2i, Range2Di> operator[](UnsignedInt glyph) const {
            auto it = glyphs.find(glyph);
            return it == glyphs.end() ? glyphs.at(0) : it->second;
        }

        /** @brief Iterator access to cache data */
        std::unordered_map<UnsignedInt, std::pair<Vector2i, Range2Di>>::const_iterator begin() const {
            return glyphs.begin();
        }

        /** @brief Iterator access to cache data */
        std::unordered_map<UnsignedInt, std::pair<Vector2i, Range2Di>>::const_iterator end() const {
            return glyphs.end();
        }

        /**
         * @brief Layout glyphs with given sizes to the cache
         *
         * Returns non-overlapping regions in cache texture to store glyphs.
         * The reserved space is reused on next call to @ref reserve() if no
         * glyph was stored there, use @ref insert() to store actual glyph on
         * given position and @ref setImage() to upload glyph image.
         *
         * Glyph @p sizes are expected to be without padding.
         *
         * @attention Cache size must be large enough to contain all rendered
         *      glyphs.
         * @see @ref padding()
         */
        std::vector<Range2Di> reserve(const std::vector<Vector2i>& sizes);

        /**
         * @brief Insert glyph to cache
         * @param glyph         Glyph ID
         * @param position      Position relative to point on baseline
         * @param rectangle     Region in texture atlas
         *
         * You can obtain unused non-overlapping regions with @ref reserve().
         * You can't overwrite already inserted glyph, however you can reset
         * glyph @cpp 0 @ce to some meaningful value.
         *
         * Glyph parameters are expected to be without padding.
         *
         * See also @ref setImage() to upload glyph image.
         * @see @ref padding()
         */
        void insert(UnsignedInt glyph, const Vector2i& position, const Range2Di& rectangle);

        /**
         * @brief Set cache image
         *
         * Uploads image for one or more glyphs to given offset in cache
         * texture. Calls @ref doSetImage(). The @p offset and
         * @ref ImageView::size() are expected tro be in bounds for
         * @ref textureSize().
         */
        void setImage(const Vector2i& offset, const ImageView2D& image);

        /**
         * @brief Download cache image
         *
         * Downloads the cache texture back. Calls @ref doImage(). Available
         * only if @ref GlyphCacheFeature::ImageDownload is supported.
         * @see @ref features()
         */
        Image2D image();

    private:
        /** @brief Implementation for @ref features() */
        virtual GlyphCacheFeatures doFeatures() const = 0;

        /**
         * @brief Implementation for @ref setImage()
         *
         * The @p offset and @ref ImageView::size() are guaranteed to be in
         * bounds for @ref textureSize().
         */
        virtual void doSetImage(const Vector2i& offset, const ImageView2D& image) = 0;

        /** @brief Implementation for @ref image() */
        virtual Image2D doImage();

        Vector2i _size, _padding;
        std::unordered_map<UnsignedInt, std::pair<Vector2i, Range2Di>> glyphs;
};

}}

#endif
