#ifndef Magnum_TextureTools_Atlas_h
#define Magnum_TextureTools_Atlas_h
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
 * @brief Class @ref Magnum::TextureTools::AtlasLandfill, enum @ref Magnum::TextureTools::AtlasLandfillFlag, enum set @ref Magnum::TextureTools::AtlasLandfillFlags, function @ref Magnum::TextureTools::atlas(), @ref Magnum::TextureTools::atlasArrayPowerOfTwo()
 */

#include <initializer_list>
#include <Corrade/Containers/Pointer.h>

#include "Magnum/Magnum.h"
#include "Magnum/TextureTools/visibility.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include "Magnum/Math/Vector2.h"

#include <Corrade/Utility/Macros.h>
#include <Corrade/Utility/StlForwardVector.h>
#endif

namespace Magnum { namespace TextureTools {

namespace Implementation {
    struct AtlasLandfillState;
}

/**
@brief Landfill texture atlas packer behavior flag
@m_since_latest

@see @ref AtlasLandfillFlags, @ref AtlasLandfill::setFlags(),
    @ref AtlasLandfill::addFlags(), @ref AtlasLandfill::clearFlags()
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
    NarrowestFirst = 1 << 3,

    /**
     * By default, when reaching an edge, the next row is filled in reverse
     * direction only if the previous row ended lower than it started. If it
     * ended at the same height or higher, the next row is filled in the same
     * direction again in an attempt to level it out with decreasing heights.
     * Enabling this flag reverses the fill direction always.
     */
    ReverseDirectionAlways = 1 << 4
};

/** @debugoperatorenum{AtlasLandfillFlag} */
MAGNUM_TEXTURETOOLS_EXPORT Debug& operator<<(Debug& output, AtlasLandfillFlag value);

/**
@brief Landfill texture atlas packer behavior flags
@m_since_latest

@see @ref AtlasLandfill::setFlags(), @ref AtlasLandfill::addFlags(),
    @ref AtlasLandfill::clearFlags()
*/
typedef Containers::EnumSet<AtlasLandfillFlag> AtlasLandfillFlags;

CORRADE_ENUMSET_OPERATORS(AtlasLandfillFlags)

/** @debugoperatorenum{AtlasLandfillFlags} */
MAGNUM_TEXTURETOOLS_EXPORT Debug& operator<<(Debug& output, AtlasLandfillFlags value);

/**
@brief Landfill texture atlas packer
@m_since_latest

Keeps track of currently filled height at every pixel with the aim to fill the
available space bottom-up as evenly as possible. Packs to a 2D or a 2D array
texture with either the height or depth optionally unbounded. See also
@ref atlasArrayPowerOfTwo() for a variant that always provides optimal packing
for power-of-two sizes.

@htmlinclude atlas-landfill.svg

* *The Trash Algorithm.* Naming credit goes to [\@lacyyy](https://github.com/lacyyy).

@section TextureTools-AtlasLandfill-usage Example usage

The following snippets shows packing a list of images into an atlas with the
width set to 1024 and height unbounded. The algorithm by default makes all
images the same orientation as that significantly improves the layout
efficiency while not making any difference for texture mapping.

@snippet TextureTools.cpp AtlasLandfill-usage

Calculating a texture coordinate transformation matrix for a particular image
can then be done with @ref atlasTextureCoordinateTransformation(), see its
documentation for an example of how to calculate and apply the matrix to either
the mesh directly or to a material / shader.

If rotations are undesirable, for example if the resulting atlas is used by a
linear rasterizer later, they can be disabled by clearing appropriate
@ref AtlasLandfillFlags. The process can then also use the
@ref add(const Containers::StridedArrayView1D<const Vector2i>&, const Containers::StridedArrayView1D<Vector2i>&)
overload without the rotations argument.

@snippet TextureTools.cpp AtlasLandfill-usage-no-rotation

@subsection TextureTools-AtlasLandfill-usage-atlas Array atlas

The packing can be extended to a third dimension as well, in which case the
packing overflows to next slices instead of expanding to potentially unbounded
height.

@snippet TextureTools.cpp AtlasLandfill-usage-array

The layer has to be taken into an account in addition to the texture coordinate
transformation matrix calculated with @ref atlasTextureCoordinateTransformation(),
for example by adding a texture layer attribute to @ref Trade::MaterialData.

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
@cpp 0 @ce and the process continues again in the opposite direction, or the
same direction if the previous row ended higher than it started. With the
assumption that the texture sizes are uniformly distributed, this results in a
fairly leveled out height. The process is aborted if the atlas height is
bounded and the next item cannot fit there anymore.

The sort is performed using @ref std::stable_sort(), which is usually
@f$ \mathcal{O}(n \log{} n) @f$, the actual atlasing is a single
@f$ \mathcal{O}(n) @f$ operation. Memory complexity is
@f$ \mathcal{O}(n + wc) @f$ with @f$ n @f$ being a sorted copy of the input
size array and @f$ wc @f$ being a 16-bit integer for every pixel of atlas width
times filled atlas depth. Additionally @ref std::stable_sort() performs its own allocation.

@section TextureTools-AtlasLandfill-incremental Incremental population

It's possible to call @ref add() multiple times in order to incrementally fill
the atlas with new data as much as the atlas height (if bounded) allows. In an
ideal scenario, if the previous fill resulted in an uniform height the newly
added data will be added in an optimal way as well, but in practice calling
@ref add() with all data just once will always result in a more optimal
packing than an incremental one.

In case of an array atlas, the incremental process always starts from the first
slice, finding the first that can fit the first (sorted) item. Then it attempts
to place as many items as possible and on overflow continues searching for the
next slice that can fit the first remaining item. If all slices are exhausted,
adds a new one for as long as the depth (if bounded) allows.
*/
class MAGNUM_TEXTURETOOLS_EXPORT AtlasLandfill {
    public:
        /**
         * @brief Constructor
         *
         * The @p size is expected to have non-zero width, and height not
         * larger than 65536. If height is @cpp 0 @ce, depth is expected to be
         * @cpp 1 @ce and the height is treated as unbounded, i.e. @ref add()
         * never fails. Otherwise, if depth is @cpp 0 @ce, depth is treated as
         * unbounded.
         */
        explicit AtlasLandfill(const Vector3i& size);

        /**
         * @brief Construct a non-array atlas
         *
         * Same as calling @ref AtlasLandfill with depth set to @cpp 1 @ce.
         */
        explicit AtlasLandfill(const Vector2i& size);

        /** @brief Copying is not allowed */
        AtlasLandfill(const AtlasLandfill&) = delete;

        /**
         * @brief Move constructor
         *
         * Performs a destructive move, i.e. the original object isn't usable
         * afterwards anymore.
         */
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
        Vector3i size() const;

        /**
         * @brief Currently filled size
         *
         * Width is always taken from @ref size().
         *
         * If @ref size() depth is @cpp 1 @ce, the returned depth is always
         * @cpp 1 @ce, height is @cpp 0 @ce initially, and at most the
         * height of @ref size() if it's bounded. It's calculated with a
         * @f$ \mathcal{O}(w) @f$ complexity, with @f$ w @f$ being the atlas
         * width.
         *
         * Otherwise, if @ref size() depth is not @cpp 1 @ce, the height is
         * taken from @ref size() and the depth is @cpp 0 @ce initially, and
         * at most @ref size() depth if the size is bounded.
         */
        Vector3i filledSize() const;

        /**
         * @brief Behavior flags
         *
         * Default is @ref AtlasLandfillFlag::RotatePortrait and
         * @relativeref{AtlasLandfillFlag,WidestFirst}.
         */
        AtlasLandfillFlags flags() const;

        /**
         * @brief Set behavior flags
         * @return Reference to self (for method chaining)
         *
         * Note that some flags are mutually exclusive, see documentation of
         * particular @ref AtlasLandfillFlag values for more information. Can
         * be called with different values before each particular @ref add().
         * @see @ref addFlags(), @ref clearFlags()
         */
        AtlasLandfill& setFlags(AtlasLandfillFlags flags);

        /**
         * @brief Add behavior flags
         * @return Reference to self (for method chaining)
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
         * @return Reference to self (for method chaining)
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
         * @return Reference to self (for method chaining)
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
        AtlasLandfill& setPadding(const Vector2i& padding);

        /**
         * @brief Add textures to the atlas
         * @param[in]  sizes        Texture sizes
         * @param[out] offsets      Resulting offsets in the atlas
         * @param[out] rotations    Which textures got rotated
         * @return Range spanning all added items including padding or
         *      @relativeref{Corrade,Containers::NullOpt} if they didn't fit
         *
         * The @p sizes, @p offsets and @p rotations views are expected to have
         * the same size. The @p sizes are all expected to be not larger than
         * @ref size() after appying padding and then a rotation based on
         * @ref AtlasLandfillFlag::RotatePortrait or
         * @relativeref{AtlasLandfillFlag,RotateLandscape} being set. If
         * neither @relativeref{AtlasLandfillFlag,RotatePortrait} nor
         * @relativeref{AtlasLandfillFlag,RotateLandscape} is set, the
         * @p rotations view can be also empty or you can use the
         * @ref add(const Containers::StridedArrayView1D<const Vector2i>&, const Containers::StridedArrayView1D<Vector2i>&)
         * overload. The resulting @p offsets always point to the original
         * (potentially rotated) sizes without padding applied.
         *
         * Items with zero width or height don't contribute to the layout in
         * any way if padding is zero, but are still sorted, rotated and placed
         * relative to others. If padding is non-zero, items with zero width or
         * height are treated as any others to make sure they don't overlap
         * other items.
         *
         * On success updates @ref filledSize() and returns a range spanning
         * all added items including padding, which can be used for example to
         * perform a partial GPU texture upload. If @ref size() is bounded, can
         * return @relativeref{Corrade,Containers::NullOpt} if the items didn't
         * fit, in which case the internals and contents of @p offsets and
         * @p rotations are left in an undefined state. For an unbounded
         * @ref size() the function never fails.
         * @see @ref setFlags(), @ref setPadding()
         */
        Containers::Optional<Range3Di> add(const Containers::StridedArrayView1D<const Vector2i>& sizes, const Containers::StridedArrayView1D<Vector3i>& offsets, Containers::MutableBitArrayView rotations);
        /** @overload */
        Containers::Optional<Range3Di> add(std::initializer_list<Vector2i> sizes, const Containers::StridedArrayView1D<Vector3i>& offsets, Containers::MutableBitArrayView rotations);

        /**
         * @brief Add textures to the atlas with rotations disabled
         *
         * Equivalent to calling @ref add(const Containers::StridedArrayView1D<const Vector2i>&, const Containers::StridedArrayView1D<Vector3i>&, Containers::MutableBitArrayView)
         * with the @p rotations view being empty. Can be called only if
         * neither @ref AtlasLandfillFlag::RotatePortrait nor
         * @relativeref{AtlasLandfillFlag,RotateLandscape} is set.
         * @see @ref clearFlags()
         */
        Containers::Optional<Range3Di> add(const Containers::StridedArrayView1D<const Vector2i>& sizes, const Containers::StridedArrayView1D<Vector3i>& offsets);
        /** @overload */
        Containers::Optional<Range3Di> add(std::initializer_list<Vector2i> sizes, const Containers::StridedArrayView1D<Vector3i>& offsets);

        /**
         * @brief Add textures to a non-array atlas
         *
         * Like @ref add(const Containers::StridedArrayView1D<const Vector2i>&, const Containers::StridedArrayView1D<Vector3i>&, Containers::MutableBitArrayView),
         * but omitting the third dimension. Can be called only if @ref size()
         * depth is @cpp 1 @ce.
         */
        Containers::Optional<Range2Di> add(const Containers::StridedArrayView1D<const Vector2i>& sizes, const Containers::StridedArrayView1D<Vector2i>& offsets, Containers::MutableBitArrayView rotations);
        /** @overload */
        Containers::Optional<Range2Di> add(std::initializer_list<Vector2i> sizes, const Containers::StridedArrayView1D<Vector2i>& offsets, Containers::MutableBitArrayView rotations);

        /**
         * @brief Add textures to a non-array atlas with rotations disabled
         *
         * Equivalent to calling @ref add(const Containers::StridedArrayView1D<const Vector2i>&, const Containers::StridedArrayView1D<Vector2i>&, Containers::MutableBitArrayView)
         * with the @p rotations view being empty. Can be called only if
         * @ref size() depth is @cpp 1 @ce and neither
         * @ref AtlasLandfillFlag::RotatePortrait nor
         * @relativeref{AtlasLandfillFlag,RotateLandscape} is set.
         * @see @ref clearFlags()
         */
        Containers::Optional<Range2Di> add(const Containers::StridedArrayView1D<const Vector2i>& sizes, const Containers::StridedArrayView1D<Vector2i>& offsets);
        /** @overload */
        Containers::Optional<Range2Di> add(std::initializer_list<Vector2i> sizes, const Containers::StridedArrayView1D<Vector2i>& offsets);

    private:
        Containers::Pointer<Implementation::AtlasLandfillState> _state;
};

#ifdef MAGNUM_BUILD_DEPRECATED
/**
@brief Pack textures into a texture atlas
@param atlasSize    Size of the resulting atlas
@param sizes        Sizes of all textures in the atlas
@param padding      Padding around each texture

Packs many small textures into one larger. If the textures cannot be packed
into required size, an empty vector is returned.

Padding is added twice to each size and the atlas is laid out so the padding
don't overlap. Returned sizes are the same as original sizes, i.e. without the
padding.

@m_deprecated_since_latest Use the @ref AtlasLandfill class, which has a vastly
    better packing efficiency, supports incremental packing and doesn't force
    the caller to use a @ref std::vector.
*/
MAGNUM_TEXTURETOOLS_EXPORT CORRADE_DEPRECATED("use the AtlasLandfill class instead") std::vector<Range2Di> atlas(const Vector2i& atlasSize, const std::vector<Vector2i>& sizes, const Vector2i& padding = {});
#endif

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

Example usage is shown below. Calculating a texture coordinate transformation
matrix for a particular image can then be done with
@ref atlasTextureCoordinateTransformation(), see its documentation for how to
calculate and apply the matrix to either the mesh directly or to a material /
shader.

@snippet TextureTools.cpp atlasArrayPowerOfTwo

The algorithm first sorts the textures by size using @ref std::stable_sort(),
which is usually @f$ \mathcal{O}(n \log{} n) @f$, and then performs the actual
atlasing in a single @f$ \mathcal{O}(n) @f$ operation. Memory complexity is
@f$ \mathcal{O}(n) @f$ with @f$ n @f$ being a sorted copy of the input size
array, additionally @ref std::stable_sort() performs its own allocation. See
the [Zero-waste single-pass packing of power-of-two textures](https://blog.magnum.graphics/backstage/pot-array-packing/)
article for a detailed description of the algorithm.

See the @ref AtlasLandfill class for an alternative that isn't restricted to
power-of-two sizes and can be used in an incremental way but doesn't always
produce optimal packing.
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

/**
@brief Calculate a texture coordinate transformation matrix for an atlas-packed item
@m_since_latest

Together with @ref atlasTextureCoordinateTransformationRotatedCounterClockwise()
or @ref atlasTextureCoordinateTransformationRotatedClockwise() meant be used to
adjust mesh texture coordinate attributes after packing textures with
@ref AtlasLandfill or @ref atlasArrayPowerOfTwo(). Expects that @p size and
@p offset fit into the @p atlasSize, the rotated variants expect that @p size
with coordinates flipped and @p offset fit into the @p atlasSize.

With a concrete `atlasSize`, `sizes` being the input sizes passed to
@ref AtlasLandfill::add() (i.e., without any potential rotations applied yet),
and `offsets` and `rotations` being the output, the usage is as follows:

@snippet TextureTools.cpp atlasTextureCoordinateTransformation

The resulting matrix can be then directly used to adjust texture coordinates,
like below with @ref MeshTools::transformTextureCoordinates2DInPlace() on a
@link Trade::MeshData @endlink:

@snippet TextureTools.cpp atlasTextureCoordinateTransformation-meshdata

Alternatively, for example in cases where a single mesh is used with several different textures, the transformation can be applied at draw time, such as
with @ref Shaders::FlatGL::setTextureMatrix(). In case there's already a
texture transformation matrix being applied when drawing, the new
transformation has to happen *after*, so multiplied from the left side. For
example with a @ref Trade::MaterialData that contains a
@link Trade::MaterialAttribute::TextureMatrix @endlink:

@snippet TextureTools.cpp atlasTextureCoordinateTransformation-materialdata
*/
MAGNUM_TEXTURETOOLS_EXPORT Matrix3 atlasTextureCoordinateTransformation(const Vector2i& atlasSize, const Vector2i& size, const Vector2i& offset);

/**
@brief Calculate a texture coordinate transformation matrix for an atlas-packed item rotated counterclockwise
@m_since_latest

Like @ref atlasTextureCoordinateTransformation(), but swaps X and Y of @p size
and produces a matrix that rotates the texture coordinates 90°
counterclockwise. The lower left corner of the input becomes a lower right
corner. See @ref atlasTextureCoordinateTransformationRotatedClockwise() for a
clockwise variant.
*/
MAGNUM_TEXTURETOOLS_EXPORT Matrix3 atlasTextureCoordinateTransformationRotatedCounterClockwise(const Vector2i& atlasSize, const Vector2i& size, const Vector2i& offset);

/**
@brief Calculate a texture coordinate transformation matrix for an atlas-packed item rotated clockwise
@m_since_latest

Like @ref atlasTextureCoordinateTransformation(), but swaps X and Y of @p size
and produces a matrix that rotates the texture coordinates 90° clockwise. The lower left corner of the input becomes an upper left corner. See
@ref atlasTextureCoordinateTransformationRotatedClockwise() for a
counterclockwise variant.
*/
MAGNUM_TEXTURETOOLS_EXPORT Matrix3 atlasTextureCoordinateTransformationRotatedClockwise(const Vector2i& atlasSize, const Vector2i& size, const Vector2i& offset);

}}

#endif
