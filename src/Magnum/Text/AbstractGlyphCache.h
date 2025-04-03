#ifndef Magnum_Text_AbstractGlyphCache_h
#define Magnum_Text_AbstractGlyphCache_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
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
 * @brief Class @ref Magnum::Text::AbstractGlyphCache, enum @ref Magnum::Text::GlyphCacheFeature, enum set @ref Magnum::Text::GlyphCacheFeatures
 * @m_since{2019,10}
 */

#include <initializer_list>
#include <Corrade/Containers/EnumSet.h>
#include <Corrade/Containers/Pointer.h>

#include "Magnum/Magnum.h"
#include "Magnum/Text/Text.h"
#include "Magnum/Text/visibility.h"
#include "Magnum/TextureTools/TextureTools.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <utility> /* std::pair */
#include <Corrade/Utility/Macros.h>
#include <Corrade/Utility/StlForwardVector.h>
#endif

namespace Magnum { namespace Text {

/**
@brief Features supported by a particular glyph cache implementation
@m_since{2019,10}

@see @ref GlyphCacheFeatures, @ref AbstractGlyphCache::features()
*/
enum class GlyphCacheFeature: UnsignedByte {
    /**
     * The glyph cache processes the input image, potentially to a different
     * size or format.
     * @see @ref AbstractGlyphCache::processedFormat(),
     *      @relativeref{AbstractGlyphCache,processedSize()},
     *      @relativeref{AbstractGlyphCache,setProcessedImage()},
     *      @ref GlyphCacheFeature::ProcessedImageDownload
     * @m_since_latest
     */
    ImageProcessing = 1 << 0,

    /**
     * Ability to download processed image data using
     * @ref AbstractGlyphCache::processedImage(). May not be supported by glyph
     * caches on embedded platforms that don't have an ability to get texture
     * data back from a GPU. Implies @ref GlyphCacheFeature::ImageProcessing.
     * Glyph caches without @ref GlyphCacheFeature::ImageProcessing have the
     * image accessible always through @ref AbstractGlyphCache::image().
     * @m_since_latest
     */
    ProcessedImageDownload = ImageProcessing|(1 << 1),

    #ifdef MAGNUM_BUILD_DEPRECATED
    /**
     * @m_deprecated_since_latest Use
     *      @ref GlyphCacheFeature::ProcessedImageDownload instead.
     */
    ImageDownload CORRADE_DEPRECATED_ENUM("use ProcessedImageDownload instead") = ProcessedImageDownload
    #endif
};

/** @debugoperatorenum{GlyphCacheFeature} */
MAGNUM_TEXT_EXPORT Debug& operator<<(Debug& output, GlyphCacheFeature value);

/**
@brief Set of features supported by a glyph cache
@m_since{2019,10}

@see @ref AbstractGlyphCache::features()
*/
typedef Containers::EnumSet<GlyphCacheFeature> GlyphCacheFeatures;

CORRADE_ENUMSET_OPERATORS(GlyphCacheFeatures)

/** @debugoperatorenum{GlyphCacheFeatures} */
MAGNUM_TEXT_EXPORT Debug& operator<<(Debug& output, GlyphCacheFeatures value);

/**
@brief Base for glyph caches
@m_since{2019,10}

A GPU-API-agnostic base for glyph caches, supporting multiple fonts and both 2D
and 2D array textures. Provides a common interface for adding fonts, glyph
properties, uploading glyph data and retrieving glyph properties back, the
@ref GlyphCacheGL, @ref GlyphCacheArrayGL and @ref DistanceFieldGlyphCacheGL
subclasses then provide concrete implementations backed with an OpenGL texture.

@section Text-AbstractGlyphCache-usage Basic usage

A glyph cache is constructed through the concrete GPU-API-specific subclasses,
such as @ref GlyphCacheGL. Its constructor takes a @ref PixelFormat and desired
texture size. @ref PixelFormat::R8Unorm is the usual choice,
@ref PixelFormat::RGBA8Unorm is useful for emoji fonts or when arbitrary icon
data are put into the cache.

@snippet Text-gl.cpp AbstractGlyphCache-usage-construct

Afterwards, assuming there's an opened @ref AbstractFont instance and a known
set of glyphs to prerender, the high-level use involves just calling
@ref AbstractFont::fillGlyphCache(), which then does all packing and data
copying on its own. Note that depending on the font file, font plugin
implementation, font size and cache size, it may happen that the characters
won't all fit, which should be checked by the application:

@snippet Text.cpp AbstractGlyphCache-usage-fill

As long as the cache size allows, you can call
@ref AbstractFont::fillGlyphCache() multiple times with additional glyphs and
other fonts. See the @ref Text-AbstractFont-glyph-cache "AbstractFont documentation"
for more options for glyph cache filling. Finally, assuming a @ref RendererGL
is used with this cache for rendering the text, its
@relativeref{RendererGL,mesh()} can be then drawn using @ref Shaders::VectorGL,
together with binding @ref GlyphCacheGL::texture() for drawing:

@snippet Text-gl.cpp AbstractGlyphCache-usage-draw

@section Text-AbstractGlyphCache-filling Filling the glyph cache directly

This section describes low level usage of the glyph cache filling APIs, which
are useful mainly when implementing an @ref AbstractFont itself or when adding
arbitrary other image data to the cache. Let's say we want to fill the glyph
cache with a custom set of images that don't necessarily need to be a font per
se. Assuming the input images are stored in a simple array, and the goal is to
put them all together into the cache and reference them later simply by their
array indices.

@snippet Text.cpp AbstractGlyphCache-filling-images

@subsection Text-AbstractGlyphCache-filling-font Adding a font

As the cache supports multiple fonts, each glyph added to it needs to be
associated with a particular font. The @ref addFont() function takes an upper
bound on *glyph IDs* used in the font and optionally an identifier to associate
it with a concrete @ref AbstractFont instance to look it up later with
@ref findFont(). It returns a *font ID* that's subsequently used for adding and
querying glyphs. In our case the glyph IDs are simply indices into the array,
so the upper bound is the array size:

@snippet Text.cpp AbstractGlyphCache-filling-font

@subsection Text-AbstractGlyphCache-filling-atlas Reserving space in the glyph atlas

Each glyph cache contains an instance of @ref TextureTools::AtlasLandfill
packer, which is used to layout glyph data into the cache texture as well as
maintain the remaining free space when more glyphs get added. In this case
we'll ask it for best offsets corresponding to the input image sizes, and as we
created the cache as 2D, we can get 2D offsets back. To keep the example
simple, rotations are disabled as well, see the atlas packer class docs for
information about how to deal with them and achieve potentially better packing
efficiency.

@snippet Text.cpp AbstractGlyphCache-filling-atlas

On success, @ref TextureTools::AtlasLandfill::add() returns a range spanning
all added images, which we'll use later for GPU texture upload. In case of a
failure, triggering the assertion above, the cache size was picked too small or
there was already enough glyphs added that the new ones didn't fit. The
solution is then to either increase the cache size, turn the cache into an
array, or create a second cache for the new data. Depending on the input sizes
it's also possible to achieve a better packing efficiency by toggling various
@ref TextureTools::AtlasLandfillFlag values --- you can for example create
temporary instances aside, attempt packing with them, and then for filling the
glyph cache itself pick the set of flags that resulted in the smallest
@ref TextureTools::AtlasLandfill::filledSize().

@subsection Text-AbstractGlyphCache-filling-glyphs Adding glyphs

With the `offsets` array filled, everything is ready for adding images into the
cache with @ref addGlyph() and copying their data to respective locations
in the cache @ref image(). Together with input image sizes, the `offsets` are
used to form @relativeref{Magnum,Range2Di} instances. What is left at zeros in
this case is the third *glyph offset* argument, which describes how the glyph
image is positioned relative to the text layouting cursor (used for example for
letters *j* or *q* that reach below the baseline).

@snippet Text.cpp AbstractGlyphCache-filling-glyphs

Important is to call @ref flushImage() at the end, which makes the glyph cache
update its actual GPU-side texture based on what area of the image was updated.
In case of @ref DistanceFieldGlyphCacheGL for example it also triggers distance
field generation for given area.

If the images put into the cache are meant to be used with general meshes, the
@ref TextureTools::atlasTextureCoordinateTransformation() function can be used
to create an appropriate texture coordinate transformation matrix. See its
documentation for an example of calculating and applying the matrix to either
the mesh directly or to a material / shader.

@subsection Text-AbstractGlyphCache-filling-incremental Incremental population

As long as the cache size allows, it's possible to add more fonts with
additional glyphs. It's also possible to call @ref addGlyph() for any font that
was added previously, as long as the added glyph ID is within corresponding
@ref fontGlyphCount() bounds and given glyph ID wasn't added yet. That allows
for example a use case where the glyph cache is initially empty and glyphs are
rasterized to it only as needed by actually rendered text, which is especially
useful with large alphabets or when the set of used fonts is large.

However, note that packing the atlas with all glyphs from all fonts just once
will always result in a more optimal layout of the glyph data than adding the
glyphs incrementally.

@subsection Text-AbstractGlyphCache-filling-invalid-glyph Setting a custom invalid glyph

By default, to denote an invalid glyph, i.e. a glyph that isn't present in the
cache, a zero-area rectangle is used. This can be overriden with
@ref setInvalidGlyph(). Its usage is similar to @ref addGlyph(), in particular
you may want to reserve its space together with other glyphs to achieve best
packing.

Additionally, glyph ID @cpp 0 @ce in fonts is usually reserved for invalid
font-specific glyphs as well. The cache-global invalid glyph can thus be either
a special one or you can make it alias some other font-specific invalid glyph,
by calling @ref setInvalidGlyph() with the same arguments as @ref addGlyph()
for a font-specific glyph ID @cpp 0 @ce.

@section Text-AbstractGlyphCache-querying Querying glyph properties and glyph data

A glyph cache can be queried for ID of a particular font with @ref findFont(),
passing the @ref AbstractFont pointer to it. If no font with such pointer was
added, the function returns @relativeref{Corrade,Containers::NullOpt}. Then,
for a particular font ID a font-specific glyph ID can be queried with
@ref glyphId(). If no such glyph was added yet, the function returns
@cpp 0 @ce, i.e. the invalid glyph. The @ref glyph() function then directly
returns data for given glyph, or the invalid glyph data in case the glyph
wasn't found.

@snippet Text.cpp AbstractGlyphCache-querying

As text rendering is potentially happening very often, batch
@ref glyphIdsInto(), @ref glyphOffsets(), @ref glyphLayers() and
@ref glyphRectangles() APIs are provided as well to trim down the amount of
function calls and redundant lookups:

@snippet Text.cpp AbstractGlyphCache-querying-batch

For invalid glyphs it's the caller choice to either use the invalid glyph
as-is (as done above), leading to blank spaces in the text, or remember the
font-specific glyph IDs that resolved to @cpp 0 @ce with @ref glyphId(),
rasterize them to the cache in the next round, and then update the rendered
text again.

@section Text-AbstractGlyphCache-padding Glyph padding

Fonts commonly shape the glyphs to sub-pixel positions, the GPU rasterizer then
rounds the actual quads to nearest pixels, which then can lead to neighboring
pixels to leak into the rendered glyph or, conversely, the rendered glyph
having an edge cut. Because of that, by default the glyphs are padded with one
pixel on each side, which should prevent such artifacts even if rendering the
glyphs 2x supersampled.

If you have a pixel-perfect font that gets always shaped to whole pixel
positions, you can set it back to zero in the constructor. Or for example if
you can ensure that at least lines get placed on whole pixel positions and the
line advance is whole pixels as well, you can se it to zero in one dimension at
least, assuming neither @ref TextureTools::AtlasLandfillFlag::RotatePortrait
nor @relativeref{TextureTools::AtlasLandfillFlag,RotateLandscape} is enabled in
@ref atlas().

On the other hand, if you're rendering more than 2x supersampled or for example
using the cache for generating a distance field, you may want to increase the
padding even further.

@section Text-AbstractGlyphCache-subclassing Subclassing

A subclass needs to implement the @ref doFeatures() and @ref doSetImage()
functions. If the subclass does additional processing of the glyph cache image,
it should advertise that with @ref GlyphCacheFeature::ImageProcessing and
implement @ref doSetProcessedImage() as well. If it's desirable and possible to
download the processed image as well, it should  advertise
@ref GlyphCacheFeature::ProcessedImageDownload and implement
@ref doProcessedImage().

The public @ref flushImage() function already does checking for rectangle
bounds so it's not needed to do it again inside @ref doSetImage(), similarly
the bounds checking is done for @ref doSetProcessedImage().
*/
class MAGNUM_TEXT_EXPORT AbstractGlyphCache {
    public:
        /**
         * @brief Construct a 2D array glyph cache
         * @param format     Source image format
         * @param size       Source image size in pixels
         * @param padding    Padding around every glyph in pixelss. See
         *      @ref Text-AbstractGlyphCache-padding for more information about
         *      the default.
         * @m_since_latest
         *
         * The @p size is expected to be non-zero. If the implementation
         * advertises @ref GlyphCacheFeature::ImageProcessing, the
         * @ref processedFormat() and @ref processedSize() is the same as
         * @p format and @p size, use @ref AbstractGlyphCache(PixelFormat, const Vector3i&, PixelFormat, const Vector2i&, const Vector2i&)
         * to specify different values.
         * @see @ref AbstractGlyphCache(PixelFormat, const Vector2i&, const Vector2i&)
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        explicit AbstractGlyphCache(PixelFormat format, const Vector3i& size, const Vector2i& padding = Vector2i{1});
        #else
        /* To not need to include Vector */
        explicit AbstractGlyphCache(PixelFormat format, const Vector3i& size, const Vector2i& padding);
        explicit AbstractGlyphCache(PixelFormat format, const Vector3i& size);
        #endif

        /**
         * @brief Construct a 2D glyph cache
         * @param format     Source image format
         * @param size       Source image size in pixels
         * @param padding    Padding around every glyph in pixels. See
         *      @ref Text-AbstractGlyphCache-padding for more information about
         *      the default.
         * @m_since_latest
         *
         * Equivalent to calling
         * @ref AbstractGlyphCache(PixelFormat, const Vector3i&, const Vector2i&)
         * with depth set to @cpp 1 @ce.
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        explicit AbstractGlyphCache(PixelFormat format, const Vector2i& size, const Vector2i& padding = Vector2i{1});
        #else
        /* To not need to include Vector */
        explicit AbstractGlyphCache(PixelFormat format, const Vector2i& size, const Vector2i& padding);
        explicit AbstractGlyphCache(PixelFormat format, const Vector2i& size);
        #endif

        /**
         * @brief Construct a 2D array glyph cache with a specific processed format and size
         * @param format            Source image format
         * @param size              Source image size in pixels
         * @param processedFormat   Processed image format
         * @param processedSize     Processed image size in pixels
         * @param padding    Padding around every glyph in pixelss. See
         *      @ref Text-AbstractGlyphCache-padding for more information about
         *      the default.
         * @m_since_latest
         *
         * The @p size and @p processedSize is expected to be non-zero, depth
         * of processed size is implicitly the same as in @p size. If the
         * implementation doesn't advertise @ref GlyphCacheFeature::ImageProcessing,
         * the @p processedFormat and @p processedSize are unused.
         * @see @ref AbstractGlyphCache(PixelFormat, const Vector3i&, const Vector2i&),
         *      @ref AbstractGlyphCache(PixelFormat, const Vector2i&, PixelFormat, const Vector2i&, const Vector2i&)
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        explicit AbstractGlyphCache(PixelFormat format, const Vector3i& size, PixelFormat processedFormat, const Vector2i& processedSize, const Vector2i& padding = Vector2i{1});
        #else
        /* To not need to include Vector */
        explicit AbstractGlyphCache(PixelFormat format, const Vector3i& size, PixelFormat processedFormat, const Vector2i& processedSize, const Vector2i& padding);
        explicit AbstractGlyphCache(PixelFormat format, const Vector3i& size, PixelFormat processedFormat, const Vector2i& processedSize);
        #endif

        /**
         * @brief Construct a 2D glyph cache with a specific processed format and size
         * @param format            Source image format
         * @param size              Source image size in pixels
         * @param processedFormat   Processed image format
         * @param processedSize     Processed image size in pixels
         * @param padding    Padding around every glyph in pixels. See
         *      @ref Text-AbstractGlyphCache-padding for more information about
         *      the default.
         * @m_since_latest
         *
         * Equivalent to calling
         * @ref AbstractGlyphCache(PixelFormat, const Vector3i&, PixelFormat, const Vector2i&, const Vector2i&)
         * with @p size depth set to @cpp 1 @ce.
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        explicit AbstractGlyphCache(PixelFormat format, const Vector2i& size, PixelFormat processedFormat, const Vector2i& processedSize, const Vector2i& padding = Vector2i{1});
        #else
        /* To not need to include Vector */
        explicit AbstractGlyphCache(PixelFormat format, const Vector2i& size, PixelFormat processedFormat, const Vector2i& processedSize, const Vector2i& padding);
        explicit AbstractGlyphCache(PixelFormat format, const Vector2i& size, PixelFormat processedFormat, const Vector2i& processedSize);
        #endif

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief Construct a 2D glyph cache
         * @param size       Source image size in pixels
         * @param padding    Padding around every glyph in pixelss. See
         *      @ref Text-AbstractGlyphCache-padding for more information about
         *      the default.
         *
         * Calls @ref AbstractGlyphCache(PixelFormat, const Vector2i&, const Vector2i&)
         * with @p format set to @ref PixelFormat::R8Unorm.
         * @m_deprecated_since_latest Use @ref AbstractGlyphCache(PixelFormat, const Vector2i&, const Vector2i&)
         *      instead.
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        explicit AbstractGlyphCache(const Vector2i& size, const Vector2i& padding = Vector2i{1});
        #else
        /* To not need to include Vector */
        CORRADE_DEPRECATED("use AbstractGlyphCache(PixelFormat, const Vector2i&, const Vector2i&) instead") explicit AbstractGlyphCache(const Vector2i& size, const Vector2i& padding);
        CORRADE_DEPRECATED("use AbstractGlyphCache(PixelFormat, const Vector2i&, const Vector2i&) instead") explicit AbstractGlyphCache(const Vector2i& size);
        #endif
        #endif

        /**
         * @brief Construct without creating the internal state
         * @m_since_latest
         *
         * The constructed instance is equivalent to moved-from state, i.e. no
         * APIs can be safely called on the object. Useful in cases where you
         * will overwrite the instance later anyway. Move another object over
         * it to make it useful.
         *
         * Note that this is a low-level and a potentially dangerous API, see
         * the documentation of @ref NoCreate for alternatives.
         */
        explicit AbstractGlyphCache(NoCreateT) noexcept;

        /** @brief Copying is not allowed */
        AbstractGlyphCache(const AbstractGlyphCache&) = delete;

        /**
         * @brief Move constructor
         * @m_since_latest
         *
         * Performs a destructive move, i.e. the original object isn't usable
         * afterwards anymore.
         */
        AbstractGlyphCache(AbstractGlyphCache&&) noexcept;

        virtual ~AbstractGlyphCache();

        /** @brief Copying is not allowed */
        AbstractGlyphCache& operator=(const AbstractGlyphCache&) = delete;

        /**
         * @brief Move assignment
         * @m_since_latest
         */
        AbstractGlyphCache& operator=(AbstractGlyphCache&&) noexcept;

        /** @brief Features supported by this glyph cache implementation */
        GlyphCacheFeatures features() const { return doFeatures(); }

        /**
         * @brief Glyph cache format
         * @m_since_latest
         *
         * Corresponds to the format of the image view returned from
         * @ref image(). Note that if the implementation advertises
         * @ref GlyphCacheFeature::ImageProcessing, the format actually used
         * for rendering exposed in @ref processedFormat() may be different
         * from the input format.
         * @see @ref features(), @ref size()
         */
        PixelFormat format() const;

        /**
         * @brief Processed glyph cache format
         * @m_since_latest
         *
         * Corresponds to the format of the image view returned from
         * @ref processedImage(), if @ref GlyphCacheFeature::ImageProcessing is
         * supported.
         * @see @ref features(), @ref format(), @ref processedSize()
         */
        PixelFormat processedFormat() const;

        /**
         * @brief Glyph cache size
         * @m_since_latest
         *
         * Corresponds to the size of the image view returned from
         * @ref image(). Note that if the implementation advertises
         * @ref GlyphCacheFeature::ImageProcessing, the size actually used for
         * rendering exposed in @ref processedSize() may be different from
         * the input size.
         * @see @ref features(), @ref format()
         */
        Vector3i size() const;

        /**
         * @brief Processed glyph cache size
         * @m_since_latest
         *
         * Corresponds to the size of the image view returned from
         * @ref processedImage(), if @ref GlyphCacheFeature::ImageProcessing is
         * supported. The depth is always the same as in @ref size().
         * @see @ref features(), @ref processedFormat()
         */
        Vector3i processedSize() const;

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief 2D glyph cache texture size
         *
         * Can be called only if @ref size() depth is @cpp 1 @ce.
         * @m_deprecated_since_latest Use @ref size() instead.
         */
        CORRADE_DEPRECATED("use size() instead") Vector2i textureSize() const;
        #endif

        /**
         * @brief Glyph padding
         *
         * @see @ref Text-AbstractGlyphCache-padding
         */
        Vector2i padding() const;

        /**
         * @brief Count of fonts in the cache
         * @m_since_latest
         *
         * @see @ref addFont(), @ref glyphCount()
         */
        UnsignedInt fontCount() const;

        /**
         * @brief Count of all glyphs added to the cache
         *
         * The returned count is a sum across all fonts present in the cache.
         * It's not possible to query count of added glyphs for a just single
         * font, the @ref fontGlyphCount() query returns an upper bound for a
         * font-specific glyph ID.
         * @see @ref addGlyph(), @ref fontCount()
         */
        UnsignedInt glyphCount() const;

        /**
         * @brief Atlas packer instance
         * @m_since_latest
         *
         * Meant to be used to reserve space in the atlas texture for
         * to-be-added glyphs. After that call @ref addGlyph() to associate the
         * reserved space with actual glyph properties, copy the corresponding
         * glyph data to appropriate sub-ranges in @ref image() and reflect
         * the updates to the GPU-side data with @ref flushImage().
         *
         * The atlas packer is initially configured to match @ref size() and
         * @ref padding() and the
         * @ref TextureTools::AtlasLandfillFlag::RotatePortrait and
         * @relativeref{TextureTools::AtlasLandfillFlag,RotateLandscape} flags
         * are cleared. Everything else is left at defaults. See the class
         * documentation for more information.
         */
        TextureTools::AtlasLandfill& atlas();

        /**
         * @overload
         * @m_since_latest
         */
        const TextureTools::AtlasLandfill& atlas() const;

        /**
         * @brief Set a cache-global invalid glyph
         * @param offset        Offset of the rendered glyph relative to a
         *      point on the baseline
         * @param layer         Layer in the atlas
         * @param rectangle     Rectangle in the atlas without padding applied
         * @return Cache-global glyph ID
         * @m_since_latest
         *
         * Defines properties of glyph with ID @cpp 0 @ce, i.e. a cache-global
         * invalid glyph. By default the glyph is empty.
         *
         * The @p layer and @p rectangle is expected to be in bounds for
         * @ref size(). Usually the @p rectangle would match an offset + size
         * reserved earlier in the @ref atlas() packer, but doesn't have to. If
         * not, it's the caller responsibility to ensure the atlas packer has
         * up-to-date information about used area in the atlas in case
         * incremental filling of the cache is desired.
         *
         * Copy the corresponding glyph data to appropriate sub-ranges in
         * @ref image(). After the glyphs are copied, call @ref flushImage() to
         * reflect the updates to the GPU-side data.
         */
        void setInvalidGlyph(const Vector2i& offset, Int layer, const Range2Di& rectangle);

        /**
         * @brief Set a cache-global invalid glyph in a 2D glyph cache
         * @m_since_latest
         *
         * Equivalent to calling @ref setInvalidGlyph(const Vector2i&, Int, const Range2Di&)
         * with @p layer set to @cpp 0 @ce. Can be called only if @ref size()
         * depth is @cpp 1 @ce.
         */
        void setInvalidGlyph(const Vector2i& offset, const Range2Di& rectangle);

        /**
         * @brief Add a font
         * @param glyphCount    Upper bound on glyph IDs present in the font,
         *      or the value of @ref AbstractFont::glyphCount()
         * @param pointer       Font instance for later lookup via
         *      @ref findFont(). Use @cpp nullptr @ce if not associated with
         *      any particular font instance.
         * @m_since_latest
         *
         * Returns font ID that's subsequently used to identify the font in
         * @ref addGlyph() and @ref glyph(). The @p pointer is expected to be
         * either @cpp nullptr @ce or unique across all added fonts but apart
         * from that isn't accessed in any way.
         */
        UnsignedInt addFont(UnsignedInt glyphCount, const AbstractFont* pointer = nullptr);

        /**
         * @brief Upper bound on glyph IDs present in given font
         * @param fontId        Font ID returned by @ref addFont()
         * @m_since_latest
         *
         * The @p fontId is expected to be less than @ref fontCount(). Note
         * that this query doesn't return an actual number of glyphs added for
         * given font but an upper bound on their IDs.
         */
        UnsignedInt fontGlyphCount(UnsignedInt fontId) const;

        /**
         * @brief Unique font identifier
         * @param fontId        Font ID returned by @ref addFont()
         * @m_since_latest
         *
         * The @p fontId is expected to be less than @ref fontCount(). The
         * returned pointer isn't guaranteed to point to anything meaningful.
         */
        const AbstractFont* fontPointer(UnsignedInt fontId) const;

        /**
         * @brief Find a font ID for a font instance
         * @m_since_latest
         *
         * Returns a font ID if a pointer matching @p font was used in an
         * earlier @ref addFont() call, @relativeref{Corrade,Containers::NullOpt}
         * otherwise. The lookup is done with an @f$ \mathcal{O}(n) @f$
         * complexity with @f$ n @f$ being @ref fontCount().
         */
        Containers::Optional<UnsignedInt> findFont(const AbstractFont& font) const;

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief Reserve space for given glyph sizes in the cache
         *
         * Calls @ref addFont() with glyph count set to size of the @p sizes
         * vector and then @ref TextureTools::AtlasLandfill::add() to reserve
         * the sizes. For backwards compatibility only, can be called just
         * once.
         * @m_deprecated_since_latest Use @ref atlas() and
         *      @ref TextureTools::AtlasLandfill::add() instead.
         */
        CORRADE_DEPRECATED("use atlas() and TextureTools::AtlasLandfill::add() instead") std::vector<Range2Di> reserve(const std::vector<Vector2i>& sizes);
        #endif

        /**
         * @brief Add a glyph
         * @param fontId        Font ID returned by @ref addFont()
         * @param fontGlyphId   Glyph ID in given font
         * @param offset        Offset of the rendered glyph relative to a
         *      point on the baseline
         * @param layer         Layer in the atlas
         * @param rectangle     Rectangle in the atlas without padding applied
         * @return Cache-global glyph ID
         * @m_since_latest
         *
         * The @p fontId is expected to be less than @ref fontCount(),
         * @p fontGlyphId then less than the glyph count passed in the
         * @ref addFont() call and an ID that haven't been added yet, and
         * @p layer and @p rectangle in bounds for @ref size(). Usually the
         * @p rectangle would match an offset + size reserved earlier in the
         * @ref atlas() packer, but doesn't have to. If not, it's the caller
         * responsibility to ensure the atlas packer has up-to-date information
         * about used area in the atlas in case incremental filling of the
         * cache is desired.
         *
         * Copy the corresponding glyph data to appropriate sub-ranges in
         * @ref image(). After the glyphs are copied, call @ref flushImage() to
         * reflect the updates to the GPU-side data.
         *
         * The returned glyph ID can be passed directly to @ref glyph() to
         * retrieve its properties, the same ID can be also queried by passing
         * the @p fontId and @p fontGlyphId to @ref glyphId(). Due to how the
         * internal glyph ID mapping is implemented, there can be at most 65536
         * glyphs added including the implicit invalid one.
         */
        UnsignedInt addGlyph(UnsignedInt fontId, UnsignedInt fontGlyphId, const Vector2i& offset, Int layer, const Range2Di& rectangle);

        /**
         * @brief Add a glyph to a 2D glyph cache
         *
         * Equivalent to calling @ref addGlyph(UnsignedInt, UnsignedInt, const Vector2i&, Int, const Range2Di&)
         * with @p layer set to @cpp 0 @ce. Can be called only if @ref size()
         * depth is @cpp 1 @ce.
         */
        UnsignedInt addGlyph(UnsignedInt fontId, UnsignedInt fontGlyphId, const Vector2i& offset, const Range2Di& rectangle);

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief Add a glyph
         * @param glyph         Glyph ID
         * @param offset        Offset relative to point on baseline
         * @param rectangle     Region in texture atlas
         *
         * Calls either @ref setInvalidGlyph() or @ref addGlyph() with
         * @p fontId set to @cpp 0 @ce. If no font is added yet, adds it, if
         * it's added expands its glyph count as necessary. Cannot be called if
         * there's more than one font.
         * @m_deprecated_since_latest Use @ref setInvalidGlyph(),
         *      @ref addFont() and @ref addGlyph() instead.
         */
        CORRADE_DEPRECATED("use addFont() and addGlyph() instead") void insert(UnsignedInt glyph, const Vector2i& offset, const Range2Di& rectangle);
        #endif

        /**
         * @brief Glyph cache image
         * @m_since_latest
         *
         * The view is of @ref format() and @ref size(), and is initially
         * zero-filled. For every @ref addGlyph() copy the corresponding glyph
         * data to appropriate sub-ranges of the image. After the glyphs are
         * copied, call @ref flushImage() to reflect the updates to the
         * GPU-side data.
         *
         * If the glyph cache has @ref GlyphCacheFeature::ImageProcessing set,
         * the actual image used for rendering is different. Use
         * @ref processedImage() to download it.
         * @see @ref ImageView::pixels(), @ref Utility::copy(),
         *      @ref processedImage()
         */
        MutableImageView3D image();

        /**
         * @overload
         * @m_since_latest
         */
        ImageView3D image() const;

        /**
         * @brief Flush glyph cache image updates
         * @m_since_latest
         *
         * Call after copying glyph data to @ref image() in order to reflect
         * the updates to the GPU-side data. The @p range is expected to be in
         * bounds for @ref size(). You can use @ref Math::join() on rectangles
         * passed to @ref addGlyph() to calculate the area that spans all
         * glyphs that were added.
         *
         * The function assumes the @p range excludes @ref padding(). The image
         * data get copied to the GPU including the padding to make sure the
         * padded glyph area doesn't contain leftovers of uninitialized GPU
         * memory.
         */
        void flushImage(const Range3Di& range);

        /**
         * @overload
         * @m_since_latest
         */
        void flushImage(Int layer, const Range2Di& range);

        /**
         * @brief Flush 2D glyph cache image updates
         * @m_since_latest
         *
         * Equivalent to calling @ref flushImage(const Range3Di&) with depth
         * offset @cpp 0 @ce and depth size @cpp 1 @ce. Can be called only if
         * @ref size() depth is @cpp 1 @ce.
         */
        void flushImage(const Range2Di& range);

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief Set cache image
         *
         * Uploads image for one or more glyphs to given offset in cache
         * texture and calls @ref flushImage(). The @p offset and
         * @ref ImageView::size() are expected to be in bounds for @ref size().
         * Can be called only if @ref size() depth is @cpp 1 @ce.
         * @m_deprecated_since_latest Copy the glyph data to slices of
         *      @ref image() instead and call @ref flushImage() afterwards.
         */
        CORRADE_DEPRECATED("copy data to image() instead") void setImage(const Vector2i& offset, const ImageView2D& image);
        #endif

        /**
         * @brief Download processed cache image
         * @m_since_latest
         *
         * If the glyph cache has @ref GlyphCacheFeature::ImageProcessing set,
         * the actual image used for rendering is different from @ref image()
         * and has potentially a different size or format. Expects that
         * @ref GlyphCacheFeature::ProcessedImageDownload is supported. For a
         * glyph cache without @ref GlyphCacheFeature::ImageProcessing you can
         * get the image directly through @ref image().
         * @see @ref features(), @ref processedFormat(), @ref processedSize()
         */
        Image3D processedImage();

        /**
         * @brief Set processed cache image
         * @m_since_latest
         *
         * Expects that the implementation supports
         * @ref GlyphCacheFeature::ImageProcessing. The @ref ImageView::format()
         * is expected to match @ref processedFormat(), the @p offset and
         * @ref ImageView::size() are expected to be in bounds for
         * @ref processedSize().
         * @see @ref features(), @ref processedImage()
         */
        void setProcessedImage(const Vector3i& offset, const ImageView3D& image);

        /**
         * @brief Set 2D processed cache image
         * @m_since_latest
         *
         * Equivalent to calling
         * @ref setProcessedImage(const Vector3i&, const ImageView3D&) with
         * @p offset depth @cpp 0 @ce and @p image depth @cpp 1 @ce. Can be
         * called only if @ref size() depth (and thus also @ref processedSize()
         * depth) is @cpp 1 @ce.
         */
        void setProcessedImage(const Vector2i& offset, const ImageView2D& image);

        /**
         * @brief Query a cache-global glyph ID from a font-local glyph ID
         * @param fontId        Font ID returned by @ref addFont()
         * @param fontGlyphId   Glyph ID in given font
         * @m_since_latest
         *
         * The @p fontId is expected to be less than @ref fontCount(),
         * @p fontGlyphId then less than the glyph count passed in the
         * @ref addFont() call. The returned ID can be then used to index the
         * @ref glyphOffsets() const, @ref glyphLayers() const and
         * @ref glyphRectangles() const views, alternatively you can use
         * @ref glyph(UnsignedInt, UnsignedInt) const to get properties of a
         * single glyph directly.
         *
         * If @ref addGlyph() wasn't called for given
         * @p fontId and @p fontGlyphId yet, returns @cpp 0 @ce, i.e. the
         * cache-global invalid glyph index.
         *
         * The lookup is done with an @f$ \mathcal{O}(1) @f$ complexity.
         * @see @ref glyphIdsInto()
         */
        UnsignedInt glyphId(UnsignedInt fontId, UnsignedInt fontGlyphId) const;

        /**
         * @brief Query cache-global glyph IDs from font-local glyph IDs
         * @param[in]  fontId           Font ID returned by @ref addFont()
         * @param[in]  fontGlyphIds     Glyph IDs in given font
         * @param[out] glyphIds         Resulting cache-global glyph IDs
         *
         * A batch variant of @ref glyphId(), mainly meant to be used with the
         * output of @ref AbstractShaper::glyphIdsInto() to then index the
         * @ref glyphOffsets() const, @ref glyphLayers() const and
         * @ref glyphRectangles() const views.
         *
         * The @p fontId is expected to be less than @ref fontCount(), all
         * @p fontGlyphIds items then less than the glyph count passed in the
         * @ref addFont() call. The @p fontGlyphIds and @p glyphIds views are
         * expected to have the same size. Glyphs for which @ref addGlyph()
         * wasn't called yet have the corresponding @p glyphIds item set to
         * @cpp 0 @ce.
         *
         * The lookup is done with an @f$ \mathcal{O}(n) @f$ complexity with
         * @f$ n @f$ being size of the @p fontGlyphIds array.
         */
        void glyphIdsInto(UnsignedInt fontId, const Containers::StridedArrayView1D<const UnsignedInt>& fontGlyphIds, const Containers::StridedArrayView1D<UnsignedInt>& glyphIds) const;

        /**
         * @overload
         * @m_since_latest
         */
        void glyphIdsInto(UnsignedInt fontId, std::initializer_list<UnsignedInt> fontGlyphIds, const Containers::StridedArrayView1D<UnsignedInt>& glyphIds) const;

        /**
         * @brief Positions of all glyphs in the cache relative to a point on the baseline
         * @m_since_latest
         *
         * The offsets are including @ref padding(). Size of the returned view
         * is the same as @ref glyphCount(). Use @ref glyphId() or
         * @ref glyphIdsInto() to map from per-font glyph IDs to indices in
         * this array. The first item is the position of the cache-global
         * invalid glyph.
         *
         * The returned view is only guaranteed to be valid until the next
         * @ref addGlyph() call.
         * @see @ref glyphLayers() const, @ref glyphRectangles() const,
         *      @ref glyph()
         */
        Containers::StridedArrayView1D<const Vector2i> glyphOffsets() const;

        /**
         * @brief Layers of all glyphs in the cache atlas
         * @m_since_latest
         *
         * Size of the returned view is the same as @ref glyphCount(). Use
         * @ref glyphId() or @ref glyphIdsInto() to map from per-font glyph IDs
         * to indices in this array. The first item is the layer of the
         * cache-global invalid glyph. All values are guaranteed to be less
         * than @ref size() depth.
         *
         * The returned view is only guaranteed to be valid until the next
         * @ref addGlyph() call.
         * @see @ref glyphOffsets() const, @ref glyphRectangles() const,
         *      @ref glyph()
         */
        Containers::StridedArrayView1D<const Int> glyphLayers() const;

        /**
         * @brief Rectangles of all glyphs in the cache atlas
         * @m_since_latest
         *
         * The rectangles are including @ref padding(). Size of the returned
         * view is the same as @ref glyphCount(). Use @ref glyphId() or
         * @ref glyphIdsInto() to map from per-font glyph IDs to indices in
         * this array. The first item is the layer of the cache-global invalid
         * glyph. All values are guaranteed to fit into @ref size() width and
         * height.
         *
         * The returned view is only guaranteed to be valid until the next
         * @ref addGlyph() call.
         * @see @ref glyphOffsets() const, @ref glyphLayers() const,
         *      @ref glyph()
         */
        Containers::StridedArrayView1D<const Range2Di> glyphRectangles() const;

        /**
         * @brief Properties of given glyph ID in given font
         * @param fontId        Font ID returned by @ref addFont()
         * @param fontGlyphId   Glyph ID in given font
         * @m_since_latest
         *
         * Returns offset of the rendered glyph relative to a point on the
         * baseline, layer and rectangle in the atlas. The offset and rectangle
         * are including @ref padding(). The @p fontId is expected to be less
         * than @ref fontCount(), @p fontGlyphId then less than the glyph count
         * passed in the @ref addFont() call.
         *
         * The lookup is done with an @f$ \mathcal{O}(1) @f$ complexity.
         */
        Containers::Triple<Vector2i, Int, Range2Di> glyph(UnsignedInt fontId, UnsignedInt fontGlyphId) const;

        /**
         * @brief Properties of given cache-global glyph ID
         * @param glyphId       Cache-global glyph ID
         * @m_since_latest
         *
         * Returns offset of the rendered glyph relative to a point on the
         * baseline, layer and rectangle in the atlas. The offset and rectangle
         * are including @ref padding(). The @p glyphId is expected to be less
         * than @ref glyphCount().
         */
        Containers::Triple<Vector2i, Int, Range2Di> glyph(UnsignedInt glyphId) const;

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @brief Properties of given glyph
         * @param glyphId       Glyph ID
         *
         * Calls @ref glyph() with @p fontId set to @cpp 0 @ce, returns its
         * output with the layer index ignored.
         * @m_deprecated_since_latest Use @ref glyph() instead.
         */
        CORRADE_DEPRECATED("use glyph() instead") std::pair<Vector2i, Range2Di> operator[](UnsignedInt glyphId) const;
        #endif

    private:
        /** @brief Implementation for @ref features() */
        virtual GlyphCacheFeatures doFeatures() const = 0;

        /**
         * @brief Set a 3D glyph cache image
         * @m_since_latest
         *
         * Called from @ref flushImage() with a slice of @ref image(). The
         * @p offset and @ref ImageView::size() are guaranteed to be in bounds
         * for @ref size(). For a glyph cache with @ref size() depth being
         * @cpp 1 @ce default implementation delegates to
         * @ref doSetImage(const Vector2i&, const ImageView2D&). Implement
         * either this or the other overload.
         */
        virtual void doSetImage(const Vector3i& offset, const ImageView3D& image);

        /**
         * @brief Set a 2D glyph cache image
         *
         * Delegated to from the default implementation of
         * @ref doSetImage(const Vector3i&, const ImageView3D&) if @ref size()
         * depth is @cpp 1 @ce. The @p offset and @ref ImageView::size() are
         * guaranteed to be in bounds for @ref size(). Implement either this or
         * the other overload.
         */
        virtual void doSetImage(const Vector2i& offset, const ImageView2D& image);

        /**
         * @brief Implementation for @ref processedImage()
         * @m_since_latest
         */
        virtual Image3D doProcessedImage();

        /**
         * @brief Implementation for @ref setProcessedImage()
         * @m_since_latest
         *
         * The @p offset and @ref ImageView::size() are guaranteed to be in
         * bounds for @ref processedSize(). For a glyph cache with @ref size()
         * depth (and thus also @ref processedSize() depth) being @cpp 1 @ce
         * default implementation delegates to
         * @ref doSetProcessedImage(const Vector2i&, const ImageView2D&).
         * Implement either this or the other overload.
         */
        virtual void doSetProcessedImage(const Vector3i& offset, const ImageView3D& image);

        /**
         * @brief Implementation for @ref setProcessedImage()
         * @m_since_latest
         *
         * Delegated to from the default implementation of
         * @ref doSetProcessedImage(const Vector3i&, const ImageView3D&) if
         * @ref size() depth (and thus also @ref processedSize() depth) is
         * @cpp 1 @ce. The @p offset and @ref ImageView::size() are guaranteed
         * to be in bounds for @ref processedSize(). Implement either this or
         * the other overload.
         */
        virtual void doSetProcessedImage(const Vector2i& offset, const ImageView2D& image);

        struct State;
        Containers::Pointer<State> _state;
};

}}

#endif
