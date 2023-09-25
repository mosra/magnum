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
 * @brief Function @ref Magnum::TextureTools::atlas(), @ref Magnum::TextureTools::atlasArrayPowerOfTwo()
 */

#include <Corrade/Utility/StlForwardVector.h>

#include "Magnum/Magnum.h"
#include "Magnum/Math/Vector2.h"
#include "Magnum/TextureTools/visibility.h"

namespace Magnum { namespace TextureTools {

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
