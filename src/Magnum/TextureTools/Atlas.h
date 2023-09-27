#ifndef Magnum_TextureTools_Atlas_h
#define Magnum_TextureTools_Atlas_h
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

/** @file
 * @brief Class @ref Magnum::TextureTools::AtlasLandfill, @ref Magnum::TextureTools::AtlasLandfillArray, enum @ref Magnum::TextureTools::AtlasLandfillFlag, enum set @ref Magnum::TextureTools::AtlasLandfillFlags, function @ref Magnum::TextureTools::atlas(), @ref Magnum::TextureTools::atlasArrayPowerOfTwo()
 */

#include <Corrade/Containers/Pointer.h>
#include <Corrade/Utility/StlForwardVector.h>

#include "Magnum/Magnum.h"
#include "Magnum/Math/Vector2.h"
#include "Magnum/TextureTools/visibility.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <Corrade/Utility/Macros.h>
#endif

namespace Magnum { namespace TextureTools {

namespace Implementation {
    struct AtlasLandfillState;
}

/**
@brief Landfill texture atlas packer behavior flag
@m_since_latest

@see @ref AtlasLandfillFlags, @ref AtlasLandfill::setFlags(),
    @ref AtlasLandfill::addFlags(), @ref AtlasLandfill::clearFlags(),
    @ref AtlasLandfillArray::setFlags(), @ref AtlasLandfillArray::addFlags(),
    @ref AtlasLandfillArray::clearFlags()
*/
enum class AtlasLandfillFlag {
    /**
     * Rotate all textures to a portrait orientation. Only one of
     * @ref AtlasLandfillFlag::RotatePortrait and
     * @relativeref{AtlasLandfillFlag,RotateLandscape} can be set. If neither
     * is set, keeps the original orientation.
     */
    RotatePortrait = 1 << 0,

    /**
     * Rotate all textures to a landscape orientation. Only one of
     * @ref AtlasLandfillFlag::RotatePortrait and
     * @relativeref{AtlasLandfillFlag,RotateLandscape} can be set. If neither
     * is set, keeps the original orientation.
     */
    RotateLandscape = 1 << 1,

    /**
     * Sort same-height textures widest first. Only one of
     * @ref AtlasLandfillFlag::WidestFirst and
     * @relativeref{AtlasLandfillFlag,NarrowestFirst} can be set. If neither is
     * set, textures of the same height keep their original order.
     */
    WidestFirst = 1 << 2,

    /**
     * Sort same-height textures narrowest first. Only one of
     * @ref AtlasLandfillFlag::WidestFirst and
     * @relativeref{AtlasLandfillFlag,NarrowestFirst} can be set. If neither is
     * set, textures of the same height keep their original order.
     */
    NarrowestFirst = 1 << 3
};

/** @debugoperatorenum{AtlasLandfillFlag} */
MAGNUM_TEXTURETOOLS_EXPORT Debug& operator<<(Debug& output, AtlasLandfillFlag value);

/**
@brief Landfill texture atlas packer behavior flags
@m_since_latest

@see @ref Flags, @ref AtlasLandfill::setFlags(), @ref AtlasLandfill::addFlags(),
    @ref AtlasLandfill::clearFlags(), @ref AtlasLandfillArray::setFlags(),
    @ref AtlasLandfillArray::addFlags(), @ref AtlasLandfillArray::clearFlags()
*/
typedef Containers::EnumSet<AtlasLandfillFlag> AtlasLandfillFlags;

CORRADE_ENUMSET_OPERATORS(AtlasLandfillFlags)

/** @debugoperatorenum{AtlasLandfillFlags} */
MAGNUM_TEXTURETOOLS_EXPORT Debug& operator<<(Debug& output, AtlasLandfillFlags value);

/**
@brief Landfill texture atlas packer
@m_since_latest

Keeps track of currently filled height at every pixel with the aim to fill the
available space bottom-up as evenly as possible. Packs to a 2D texture with the
height optionally unbounded. See @ref AtlasLandfillArray for a variant that
works with 2D texture arrays, and @ref atlasArrayPowerOfTwo() for a variant
that always provides optimal packing for power-of-two sizes.

@htmlinclude atlas-landfill.svg

* *The Trash Algorithm.* Naming credit goes to [\@lacyyy](https://github.com/lacyyy).

@section TextureTools-AtlasLandfill-usage Example usage

The following snippets shows packing a list of images into an atlas with the
width set to 1024 and height unbounded. The algorithm by default makes all
images the same orientation as that significantly improves the layout
efficiency while not making any difference for texture mapping.

@snippet MagnumTextureTools.cpp AtlasLandfill-usage

If rotations are undesirable, for example if the resulting atlas is used by a
linear rasterizer later, they can be disabled by clearing appropriate
@ref AtlasLandfillFlags. The process can then also use the
@ref add(const Containers::StridedArrayView1D<const Vector2i>&, const Containers::StridedArrayView1D<Vector2i>&)
overload without the rotations argument.

@snippet MagnumTextureTools.cpp AtlasLandfill-usage-no-rotation

@section TextureTools-AtlasLandfill-process Packing process

On every @ref add(), the algorithm first makes all sizes the same orientation
depending on @ref AtlasLandfillFlag::RotatePortrait or
@relativeref{AtlasLandfillFlag,RotateLandscape} being set and sorts the sizes
highest first and then depending on @ref AtlasLandfillFlag::WidestFirst or
@relativeref{AtlasLandfillFlag,NarrowestFirst} being set.

A per-pixel array of currently filled `heights`, initially all @cpp 0 @ce, and
a horizontal insertion `cursor`, initially @cpp 0 @ce, is maintained. An item
of given `size` gets placed at a `height` that's
@cpp max(heights[cursor], heights[cursor + size.x]) @ce, this range gets then
set to `height + size.y` and the cursor is updated to `cursor + size.x`. If
cursor reaches the edge that an item cannot fit there anymore, it's reset to
@cpp 0 @ce and the process continues again in the opposite direction. With the
assumption that the texture sizes are uniformly distributed, this results in a
fairly leveled out height. The process is aborted if the atlas height is
bounded and the next item cannot fit there anymore.

The sort is performed using @ref std::stable_sort(), which is usually
@f$ \mathcal{O}(n \log{} n) @f$, the actual atlasing is a single
@f$ \mathcal{O}(n) @f$ operation. Memory complexity is
@f$ \mathcal{O}(n + w) @f$ with @f$ n @f$ being a sorted copy of the input size
array and @f$ w @f$ being a 16-bit integer for every pixel of atlas width,
additionally @ref std::stable_sort() performs its own allocation.

@section TextureTools-AtlasLandfill-incremental Incremental population

It's possible to call @ref add() multiple times in order to incrementally fill
the atlas with new data as much as the atlas height (if bounded) allows. In an
ideal scenario, if the previous fill resulted in an uniform height the newly
added data will be added in an optimal way as well, but in practice calling
@ref add() with all data just once will always result in a more optimal
packing than an incremental one.
*/
class MAGNUM_TEXTURETOOLS_EXPORT AtlasLandfill {
    public:
        /**
         * @brief Constructor
         *
         * The @p size is expected to have non-zero width, and height not
         * larger than 65536. If height is zero, the dimension is treated as
         * unbounded, i.e. @ref add() never fails.
         */
        explicit AtlasLandfill(const Vector2i& size);

        /** @brief Copying is not allowed */
        AtlasLandfill(const AtlasLandfill&) = delete;

        /** @brief Move constructor */
        AtlasLandfill(AtlasLandfill&&) noexcept;

        ~AtlasLandfill();

        /** @brief Copying is not allowed */
        AtlasLandfill& operator=(const AtlasLandfill&) = delete;

        /** @brief Move assignment */
        AtlasLandfill& operator=(AtlasLandfill&&) noexcept;

        /**
         * @brief Atlas size specified in constructor
         *
         * @see @ref filledSize()
         */
        Vector2i size() const;

        /**
         * @brief Currently filled size
         *
         * Width is always taken from @ref size(). The height is @cpp 0 @ce
         * initially, and at most the height of @ref size() if it's bounded.
         * The size is calculated with a @f$ \mathcal{O}(w) @f$ complexity,
         * with @f$ w @f$ being the atlas width.
         */
        Vector2i filledSize() const;

        /**
         * @brief Behavior flags
         *
         * Default is @ref AtlasLandfillFlag::RotatePortrait and
         * @relativeref{AtlasLandfillFlag,WidestFirst}.
         */
        AtlasLandfillFlags flags() const;

        /**
         * @brief Set behavior flags
         *
         * Can be called with different values before each particular
         * @ref add().
         * @see @ref addFlags(), @ref clearFlags()
         */
        AtlasLandfill& setFlags(AtlasLandfillFlags flags);

        /**
         * @brief Add behavior flags
         *
         * Calls @ref setFlags() with the existing flags ORed with @p flags.
         * Useful for preserving the defaults.
         * @see @ref clearFlags()
         */
        AtlasLandfill& addFlags(AtlasLandfillFlags flags) {
            return setFlags(this->flags()|flags);
        }

        /**
         * @brief Clear behavior flags
         *
         * Calls @ref setFlags() with the existing flags ANDed with the inverse
         * of @p flags. Useful for preserving the defaults.
         * @see @ref addFlags()
         */
        AtlasLandfill& clearFlags(AtlasLandfillFlags flags) {
            return setFlags(this->flags() & ~flags);
        }

        /**
         * @brief Padding around each texture
         *
         * Default is a zero vector.
         */
        Vector2i padding() const;

        /**
         * @brief Set padding around each texture
         *
         * Sizes are extended with twice the padding value before placement but
         * the returned offsets are without padding again. In order to have
         * @ref AtlasLandfillFlag::RotatePortrait and
         * @relativeref{AtlasLandfillFlag,RotateLandscape} work well also
         * with non-uniform padding, the padding is applied *before* a
         * potential rotation. I.e., the horizontal padding value is always
         * applied on input image width independently on how it's rotated
         * after. If you need different behavior, disable rotations with
         * @ref clearFlags() or pre-pad the input sizes directly instead of
         * using this function.
         *
         * Can be called with different values before each particular
         * @ref add().
         */
        AtlasLandfill& setPadding(const Vector2i& padding);

        /**
         * @brief Add textures to the atlas
         * @param[in]  sizes        Texture sizes
         * @param[out] offsets      Resulting offsets in the atlas
         * @param[out] rotations    Which textures got rotated
         *
         * The @p sizes, @p offsets and @p rotations views are expected to have
         * the same size. The @p sizes are all expected to be non-zero and not
         * larger than @ref size() after appying padding and then a rotation
         * based on @ref AtlasLandfillFlag::RotatePortrait or
         * @relativeref{AtlasLandfillFlag,RotateLandscape} being set. If
         * neither @relativeref{AtlasLandfillFlag,RotatePortrait} nor
         * @relativeref{AtlasLandfillFlag,RotateLandscape} is set, the
         * @p rotations view can be also empty or you can use the
         * @ref add(const Containers::StridedArrayView1D<const Vector2i>&, const Containers::StridedArrayView1D<Vector2i>&)
         * overload. The @p offsets always point to the original potentially
         * rotated sizes without padding applied.
         *
         * On success returns @cpp true @ce and updates @ref filledSize(). If
         * @ref size() is bounded, can return @cpp false @ce if the items
         * didn't fit, in which case the internals and contents of @p offsets
         * and @p rotations are left in an undefined state. For an unbounded
         * @ref size() returns @cpp true @ce always.
         * @see @ref setFlags(), @ref setPadding()
         */
        bool add(const Containers::StridedArrayView1D<const Vector2i>& sizes, const Containers::StridedArrayView1D<Vector2i>& offsets, Containers::MutableBitArrayView rotations);

        /** @overload */
        bool add(std::initializer_list<Vector2i> sizes, const Containers::StridedArrayView1D<Vector2i>& offsets, Containers::MutableBitArrayView rotations);

        /**
         * @brief Add textures to the atlas with rotations disabled
         *
         * Equivalent to calling @ref add(const Containers::StridedArrayView1D<const Vector2i>&, const Containers::StridedArrayView1D<Vector2i>&, Containers::MutableBitArrayView)
         * with the @p rotations view being empty. Can be called only if
         * neither @ref AtlasLandfillFlag::RotatePortrait nor
         * @relativeref{AtlasLandfillFlag,RotateLandscape} is set.
         * @see @ref clearFlags()
         */
        bool add(const Containers::StridedArrayView1D<const Vector2i>& sizes, const Containers::StridedArrayView1D<Vector2i>& offsets);

        /** @overload */
        bool add(std::initializer_list<Vector2i> sizes, const Containers::StridedArrayView1D<Vector2i>& offsets);

    private:
        Containers::Pointer<Implementation::AtlasLandfillState> _state;
};

/**
@brief Landfill texture atlas packer
@m_since_latest

Extends @ref AtlasLandfill to a third dimension. Instead of expanding to an
unbounded height, on overflow a new texture slice is made. See also
@ref atlasArrayPowerOfTwo() for a variant that always provides optimal packing
for power-of-two sizes.

@section TextureTools-AtlasLandfillArray-usage Example usage

Compared to the @ref TextureTools-AtlasLandfill-usage "2D usage" it's extended
to three dimensions:

@snippet MagnumTextureTools.cpp AtlasLandfillArray-usage

@section TextureTools-AtlasLandfillArray-process Packing process

Apart from expanding to new slices on height overflow, the underlying process
is @ref TextureTools-AtlasLandfill-process "the same as in AtlasLandfill".

In this case, memory complexity is @f$ \mathcal{O}(n + wd) @f$ with @f$ n @f$
being a sorted copy of the input size array and @f$ wd @f$ being a 16-bit
integer for every pixel of atlas width times atlas depth.

@section TextureTools-AtlasLandfillArray-incremental Incremental population

Compared to the @ref TextureTools-AtlasLandfill-incremental "2D incremental population",
the incremental process always starts from the first slice, finding the first
that can fit the first (sorted) item. Then it attempts to place as many items
as possible and on overflow continues searching for the next slice that can fit
the first remaining item. If all slices are exhausted, adds a new one.
*/
class MAGNUM_TEXTURETOOLS_EXPORT AtlasLandfillArray {
    public:
        /**
         * @brief Constructor
         *
         * The @p size has to have non-zero width and height. If depth is
         * @cpp 0 @ce, the dimension is treated as unbounded, i.e. @ref add()
         * never fails. If depth is @cpp 1 @ce, behaves the same as
         * @ref AtlasLandfillArray with a bounded height.
         */
        explicit AtlasLandfillArray(const Vector3i& size);

        /** @brief Copying is not allowed */
        AtlasLandfillArray(const AtlasLandfillArray&) = delete;

        /** @brief Move constructor */
        AtlasLandfillArray(AtlasLandfillArray&&) noexcept;

        ~AtlasLandfillArray();

        /** @brief Copying is not allowed */
        AtlasLandfillArray& operator=(const AtlasLandfillArray&) = delete;

        /** @brief Move assignment */
        AtlasLandfillArray& operator=(AtlasLandfillArray&&) noexcept;

        /**
         * @brief Atlas size specified in constructor
         *
         * @see @ref filledSize()
         */
        Vector3i size() const;

        /**
         * @brief Currently filled size
         *
         * Width and height is always taken from @ref size(). The depth is
         * @cpp 0 @ce initially, and at most @ref size() depth if the size is
         * bounded.
         */
        Vector3i filledSize() const;

        /** @brief Behavior flags */
        AtlasLandfillFlags flags() const;

        /**
         * @brief Set behavior flags
         *
         * Can be called with different values before each particular
         * @ref add(). Default is @ref AtlasLandfillFlag::RotatePortrait.
         * @see @ref addFlags(), @ref clearFlags()
         */
        AtlasLandfillArray& setFlags(AtlasLandfillFlags flags);

        /**
         * @brief Add behavior flags
         *
         * Calls @ref setFlags() with the existing flags ORed with @p flags.
         * Useful for preserving the defaults.
         * @see @ref clearFlags()
         */
        AtlasLandfillArray& addFlags(AtlasLandfillFlags flags) {
            return setFlags(this->flags()|flags);
        }

        /**
         * @brief Clear behavior flags
         *
         * Calls @ref setFlags() with the existing flags ANDed with the inverse
         * of @p flags. Useful for preserving the defaults.
         * @see @ref addFlags()
         */
        AtlasLandfillArray& clearFlags(AtlasLandfillFlags flags) {
            return setFlags(this->flags() & ~flags);
        }

        /**
         * @brief Padding around each texture
         *
         * Default is a zero vector.
         */
        Vector2i padding() const;

        /**
         * @brief Set padding around each texture
         *
         * Sizes are extended with twice the padding value before placement but
         * the returned offsets are without padding again. The third dimension
         * isn't treated in any special way. In order to have
         * @ref AtlasLandfillFlag::RotatePortrait and
         * @relativeref{AtlasLandfillFlag,RotateLandscape} work well also
         * with non-uniform padding, the padding is applied *before* a
         * potential rotation. I.e., the horizontal padding value is always
         * applied on input image width independently on how it's rotated
         * after. If you need different behavior, disable rotations with
         * @ref clearFlags() or pre-pad the input sizes directly instead of
         * using this function.
         *
         * Can be called with different values before each particular
         * @ref add().
         */
        AtlasLandfillArray& setPadding(const Vector2i& padding);

        /**
         * @brief Add textures to the atlas
         * @param[in]  sizes        Texture sizes
         * @param[out] offsets      Resulting offsets in the atlas
         * @param[out] rotations    Which textures got rotated
         *
         * The @p sizes, @p offsets and @p rotations views are expected to have
         * the same size. The @p sizes are all expected to be non-zero and not
         * larger than @ref size() after applying padding and then a rotation
         * based on @ref AtlasLandfillFlag::RotatePortrait or
         * @relativeref{AtlasLandfillFlag,RotateLandscape} being set. If
         * neither @relativeref{AtlasLandfillFlag,RotatePortrait} nor
         * @relativeref{AtlasLandfillFlag,RotateLandscape} is set, the
         * @p rotations view can be also empty or you can use the
         * @ref add(const Containers::StridedArrayView1D<const Vector2i>&, const Containers::StridedArrayView1D<Vector3i>&)
         * overload. The @p offsets always point to the original potentially
         * rotated sizes without padding applied.
         *
         * On success returns @cpp true @ce and updates @ref filledSize(). If
         * @ref size() is bounded, can return @cpp false @ce if the items
         * didn't fit, in which case the internals and contents of @p offsets
         * and @p rotations are left in an undefined state. For an unbounded
         * @ref size() returns @cpp true @ce always.
         * @see @ref setFlags(), @ref setPadding()
         */
        bool add(const Containers::StridedArrayView1D<const Vector2i>& sizes, const Containers::StridedArrayView1D<Vector3i>& offsets, Containers::MutableBitArrayView rotations);

        /** @overload */
        bool add(std::initializer_list<Vector2i> sizes, const Containers::StridedArrayView1D<Vector3i>& offsets, Containers::MutableBitArrayView rotations);

        /**
         * @brief Add textures to the atlas with rotations disabled
         *
         * Equivalent to calling @ref add(const Containers::StridedArrayView1D<const Vector2i>&, const Containers::StridedArrayView1D<Vector3i>&, Containers::MutableBitArrayView)
         * with the @p rotations view being empty. Can be called only if
         * neither @ref AtlasLandfillFlag::RotatePortrait nor
         * @relativeref{AtlasLandfillFlag,RotateLandscape} is set.
         * @see @ref clearFlags()
         */
        bool add(const Containers::StridedArrayView1D<const Vector2i>& sizes, const Containers::StridedArrayView1D<Vector3i>& offsets);

        /** @overload */
        bool add(std::initializer_list<Vector2i> sizes, const Containers::StridedArrayView1D<Vector3i>& offsets);

    private:
        Containers::Pointer<Implementation::AtlasLandfillState> _state;
};

/**
@brief Pack textures into texture atlas
@param atlasSize    Size of resulting atlas
@param sizes        Sizes of all textures in the atlas
@param padding      Padding around each texture

Packs many small textures into one larger. If the textures cannot be packed
into required size, empty vector is returned.

Padding is added twice to each size and the atlas is laid out so the padding
don't overlap. Returned sizes are the same as original sizes, i.e. without the
padding.
*/
std::vector<Range2Di> MAGNUM_TEXTURETOOLS_EXPORT atlas(const Vector2i& atlasSize, const std::vector<Vector2i>& sizes, const Vector2i& padding = Vector2i());

/**
@brief Pack square power-of-two textures into a texture atlas array
@param[in]  layerSize   Size of a single layer in the texture atlas
@param[in]  sizes       Sizes of all textures in the atlas
@param[out] offsets     Resulting offsets in the atlas
@return Total layer count
@m_since_latest

The @p sizes and @p offsets views are expected to have the same size. The
@p layerSize is expected to be non-zero, square and power-of-two. All items in
@p sizes are expected to be non-zero, square, power-of-two and not larger than
@p layerSize. With such constraints the packing is optimal with no wasted space
in all but the last layer. Setting @p layerSize to the size of the largest
texture in the set will lead to the least wasted space in the last layer.

@htmlinclude atlas-array-power-of-two.svg

Example usage is shown below.

@snippet MagnumTextureTools.cpp atlasArrayPowerOfTwo

The algorithm first sorts the textures by size using @ref std::stable_sort(),
which is usually @f$ \mathcal{O}(n \log{} n) @f$, and then performs the actual
atlasing in a single @f$ \mathcal{O}(n) @f$ operation. Memory complexity is
@f$ \mathcal{O}(n) @f$ with @f$ n @f$ being a sorted copy of the input size
array, additionally @ref std::stable_sort() performs its own allocation. See
the [Zero-waste single-pass packing of power-of-two textures](https://blog.magnum.graphics/backstage/pot-array-packing/)
article for a detailed description of the algorithm.

See the @ref AtlasLandfill and @ref AtlasLandfillArray classes for an
alternative that isn't restricted to power-of-two sizes and can be used in an
incremental way but doesn't always produce optimal packing.
*/
MAGNUM_TEXTURETOOLS_EXPORT Int atlasArrayPowerOfTwo(const Vector2i& layerSize, const Containers::StridedArrayView1D<const Vector2i>& sizes, const Containers::StridedArrayView1D<Vector3i>& offsets);

/**
 * @overload
 * @m_since_latest
 */
MAGNUM_TEXTURETOOLS_EXPORT Int atlasArrayPowerOfTwo(const Vector2i& layerSize, std::initializer_list<Vector2i> sizes, const Containers::StridedArrayView1D<Vector3i>& offsets);

#ifdef MAGNUM_BUILD_DEPRECATED
/**
@brief @copybrief atlasArrayPowerOfTwo(const Vector2i&, const Containers::StridedArrayView1D<const Vector2i>&, const Containers::StridedArrayView1D<Vector3i>&)
@m_deprecated_since_latest Use @ref atlasArrayPowerOfTwo(const Vector2i&, const Containers::StridedArrayView1D<const Vector2i>&, const Containers::StridedArrayView1D<Vector3i>&)
    instead.
*/
MAGNUM_TEXTURETOOLS_EXPORT CORRADE_DEPRECATED("use the overload taking offsets as an output view instead") Containers::Pair<Int, Containers::Array<Vector3i>> atlasArrayPowerOfTwo(const Vector2i& layerSize, const Containers::StridedArrayView1D<const Vector2i>& sizes);

/**
@overload
@m_deprecated_since_latest Use @ref atlasArrayPowerOfTwo(const Vector2i&, std::initializer_list<Vector2i>, const Containers::StridedArrayView1D<Vector3i>&)
    instead.
*/
MAGNUM_TEXTURETOOLS_EXPORT CORRADE_DEPRECATED("use the overload taking offsets as an output view instead") Containers::Pair<Int, Containers::Array<Vector3i>> atlasArrayPowerOfTwo(const Vector2i& layerSize, std::initializer_list<Vector2i> sizes);
#endif

}}

#endif
