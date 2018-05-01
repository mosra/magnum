#ifndef Magnum_Text_GlyphCache_h
#define Magnum_Text_GlyphCache_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
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
 * @brief Class @ref Magnum::Text::GlyphCache
 */

#include <vector>
#include <unordered_map>

#include "Magnum/Math/Range.h"
#include "Magnum/GL/Texture.h"
#include "Magnum/Text/visibility.h"

namespace Magnum { namespace Text {

/**
@brief Glyph cache

Contains font glyphs prerendered into texture atlas.

@section Text-GlyphCache-usage Usage

Create GlyphCache object with sufficient size and then call
@ref AbstractFont::createGlyphCache() to fill it with glyphs.

@snippet MagnumText.cpp GlyphCache-usage

See @ref Renderer for information about text rendering.
@todo Some way for Font to negotiate or check internal texture format
@todo Default glyph 0 with rect 0 0 0 0 will result in negative dimensions when
    nonzero padding is removed
*/
class MAGNUM_TEXT_EXPORT GlyphCache {
    public:
        /**
         * @brief Constructor
         * @param internalFormat    Internal texture format
         * @param originalSize      Unscaled glyph cache texture size
         * @param size              Actual glyph cache texture size
         * @param padding           Padding around every glyph
         *
         * All glyphs parameters are saved relative to @p originalSize,
         * although the actual glyph cache texture has @p size. Glyph
         * @p padding can be used to account for e.g. glyph shadows.
         */
        explicit GlyphCache(GL::TextureFormat internalFormat, const Vector2i& originalSize, const Vector2i& size, const Vector2i& padding);

        /**
         * @brief Constructor
         *
         * Same as calling the above with @p originalSize and @p size the same.
         */
        explicit GlyphCache(GL::TextureFormat internalFormat, const Vector2i& size, const Vector2i& padding = {});

        /**
         * @brief Constructor
         *
         * Sets internal texture format to red channel only. On desktop OpenGL
         * requires @gl_extension{ARB,texture_rg} (also part of OpenGL ES 3.0 and
         * WebGL 2), on ES2 unconditionally uses @ref GL::TextureFormat::Luminance.
         * This is done for consistency with @ref GL::pixelFormat(), which
         * unconditionally returns @ref GL::PixelFormat::Luminance for
         * @ref PixelFormat::R8Unorm. See
         * @ref GlyphCache(GL::TextureFormat, const Vector2i&, const Vector2i&)
         * for an alternative.
         */
        explicit GlyphCache(const Vector2i& originalSize, const Vector2i& size, const Vector2i& padding);

        /**
         * @brief Constructor
         *
         * Same as calling the above with @p originalSize and @p size the same.
         */
        explicit GlyphCache(const Vector2i& size, const Vector2i& padding = {});

        virtual ~GlyphCache();

        /**
         * @brief Cache size
         *
         * Size of unscaled glyph cache texture.
         */
        Vector2i textureSize() const { return _size; }

        /** @brief Glyph padding */
        Vector2i padding() const { return _padding; }

        /** @brief Count of glyphs in the cache */
        std::size_t glyphCount() const { return glyphs.size(); }

        /** @brief Cache texture */
        GL::Texture2D& texture() { return _texture; }

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
         * texture.
         */
        virtual void setImage(const Vector2i& offset, const ImageView2D& image);

    private:
        void MAGNUM_LOCAL initialize(GL::TextureFormat internalFormat, const Vector2i& size);

        Vector2i _size, _padding;
        GL::Texture2D _texture;

        std::unordered_map<UnsignedInt, std::pair<Vector2i, Range2Di>> glyphs;
};

}}

#endif
