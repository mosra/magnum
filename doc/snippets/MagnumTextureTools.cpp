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

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/BitArray.h>
#include <Corrade/Containers/BitArrayView.h>
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/Utility/Algorithms.h>

#include "Magnum/Image.h"
#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Math/FunctionsBatch.h"
#include "Magnum/TextureTools/Atlas.h"

#define DOXYGEN_ELLIPSIS(...) __VA_ARGS__

using namespace Magnum;

int main() {
{
/* [AtlasLandfill-usage] */
Containers::ArrayView<const ImageView2D> images = DOXYGEN_ELLIPSIS({});
Containers::Array<Vector2i> offsets{NoInit, images.size()};
Containers::BitArray rotations{NoInit, images.size()};

/* Fill the atlas with an unbounded height */
TextureTools::AtlasLandfill atlas{{1024, 0}};
atlas.add(stridedArrayView(images).slice(&ImageView2D::size), offsets, rotations);

/* Copy the image data to the atlas, assuming all are RGBA8Unorm as well */
Image2D output{PixelFormat::RGBA8Unorm, atlas.filledSize(),
    Containers::Array<char>{ValueInit, std::size_t(atlas.filledSize().product())}};
Containers::StridedArrayView2D<Color4ub> dst = output.pixels<Color4ub>();
for(std::size_t i = 0; i != images.size(); ++i) {
    /* Rotate 90° counterclockwise if the image is rotated in the atlas */
    Containers::StridedArrayView2D<const Color4ub> src = rotations[i] ?
        images[i].pixels<Color4ub>().flipped<1>().transposed<0, 1>() :
        images[i].pixels<Color4ub>();
    Utility::copy(src, dst.sliceSize(
        {std::size_t(offsets[i].y()),
         std::size_t(offsets[i].x())}, src.size()));
}
/* [AtlasLandfill-usage] */
}

{
Containers::ArrayView<const ImageView2D> images;
Containers::Array<Vector2i> offsets{NoInit, images.size()};
TextureTools::AtlasLandfill atlas{{1024, 0}};
/* [AtlasLandfill-usage-no-rotation] */
atlas.clearFlags(TextureTools::AtlasLandfillFlag::RotatePortrait|
                 TextureTools::AtlasLandfillFlag::RotateLandscape)
     .add(stridedArrayView(images).slice(&ImageView2D::size), offsets);

/* Copy the image data to the atlas, assuming all are RGBA8Unorm as well */
Image2D output{PixelFormat::RGBA8Unorm, atlas.filledSize(),
    Containers::Array<char>{ValueInit, std::size_t(atlas.filledSize().product())}};
Containers::StridedArrayView2D<Color4ub> dst = output.pixels<Color4ub>();
for(std::size_t i = 0; i != images.size(); ++i) {
    Containers::StridedArrayView2D<const Color4ub> src = images[i].pixels<Color4ub>();
    Utility::copy(src, dst.sliceSize(
        {std::size_t(offsets[i].y()),
         std::size_t(offsets[i].x())}, src.size()));
}
/* [AtlasLandfill-usage-no-rotation] */
}

{
/* [AtlasLandfillArray-usage] */
Containers::ArrayView<const ImageView2D> images = DOXYGEN_ELLIPSIS({});
Containers::Array<Vector3i> offsets{NoInit, images.size()};
Containers::BitArray rotations{NoInit, images.size()};

/* Fill the atlas with an unbounded depth */
TextureTools::AtlasLandfillArray atlas{{1024, 1024, 0}};
atlas.add(stridedArrayView(images).slice(&ImageView2D::size), offsets, rotations);

/* Copy the image data to the atlas, assuming all are RGBA8Unorm as well */
Vector3i outputSize = atlas.filledSize();
Image3D output{PixelFormat::RGBA8Unorm, outputSize,
    Containers::Array<char>{ValueInit, std::size_t(outputSize.product())}};
Containers::StridedArrayView3D<Color4ub> dst = output.pixels<Color4ub>();
for(std::size_t i = 0; i != images.size(); ++i) {
    /* Rotate 90° counterclockwise if the image is rotated in the atlas */
    Containers::StridedArrayView3D<const Color4ub> src = rotations[i] ?
        images[i].pixels<Color4ub>().flipped<1>().transposed<0, 1>() :
        images[i].pixels<Color4ub>();
    Utility::copy(src, dst.sliceSize(
        {std::size_t(offsets[i].z()),
         std::size_t(offsets[i].y()),
         std::size_t(offsets[i].x())}, src.size()));
}
/* [AtlasLandfillArray-usage] */
}

{
/* [atlasArrayPowerOfTwo] */
Containers::ArrayView<const ImageView2D> input;
Containers::StridedArrayView1D<const Vector2i> sizes =
    stridedArrayView(input).slice(&ImageView2D::size);
Containers::Array<Vector3i> offsets{NoInit, input.size()};

/* Size the atlas based on the largest image and fill it */
Vector2i layerSize = Math::max(sizes);
Int layerCount = TextureTools::atlasArrayPowerOfTwo(layerSize, sizes, offsets);

/* Copy the image data to the atlas, assuming all are RGBA8Unorm as well */
Vector3i outputSize{layerSize, layerCount};
Image3D output{PixelFormat::RGBA8Unorm, outputSize,
    Containers::Array<char>{ValueInit, std::size_t(outputSize.product())}};
Containers::StridedArrayView3D<Color4ub> dst = output.pixels<Color4ub>();
for(std::size_t i = 0; i != input.size(); ++i) {
    Containers::StridedArrayView3D<const Color4ub> src = input[i].pixels<Color4ub>();
    Utility::copy(src, dst.sliceSize(
        {std::size_t(offsets[i].z()),
         std::size_t(offsets[i].y()),
         std::size_t(offsets[i].x())}, src.size()));
}
/* [atlasArrayPowerOfTwo] */
}
}
