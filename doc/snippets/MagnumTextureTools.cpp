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
