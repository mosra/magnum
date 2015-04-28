/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015
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

#include "Texture.h"

#include "Magnum/Context.h"
#include "Magnum/Extensions.h"

#ifndef MAGNUM_TARGET_GLES
#include "Magnum/BufferImage.h"
#include "Magnum/Image.h"
#endif

#include "Implementation/maxTextureSize.h"
#include "Implementation/State.h"
#include "Implementation/TextureState.h"

namespace Magnum {

namespace Implementation {

template<UnsignedInt dimensions> typename DimensionTraits<dimensions, Int>::VectorType maxTextureSize() {
    return typename DimensionTraits<dimensions, Int>::VectorType{Implementation::maxTextureSideSize()};
}

#ifndef MAGNUM_TARGET_GLES
template MAGNUM_EXPORT Math::Vector<1, Int> maxTextureSize<1>();
#endif
template MAGNUM_EXPORT Vector2i maxTextureSize<2>();

template<> MAGNUM_EXPORT Vector3i maxTextureSize<3>() {
    #ifdef MAGNUM_TARGET_GLES2
    if(!Context::current()->isExtensionSupported<Extensions::GL::OES::texture_3D>())
        return {};
    #endif
    return {Vector2i(Implementation::maxTextureSideSize()), Implementation::max3DTextureDepth()};
}

}

#ifndef MAGNUM_TARGET_GLES
template<UnsignedInt dimensions> Image<dimensions> Texture<dimensions>::image(const Int level, Image<dimensions>&& image) {
    this->image(level, image);
    return std::move(image);
}

#ifndef DOXYGEN_GENERATING_OUTPUT
template MAGNUM_EXPORT Image<1> Texture<1>::image(Int, Image<1>&&);
template MAGNUM_EXPORT Image<2> Texture<2>::image(Int, Image<2>&&);
template MAGNUM_EXPORT Image<3> Texture<3>::image(Int, Image<3>&&);
#endif

template<UnsignedInt dimensions> BufferImage<dimensions> Texture<dimensions>::image(const Int level, BufferImage<dimensions>&& image, const BufferUsage usage) {
    this->image(level, image, usage);
    return std::move(image);
}

#ifndef DOXYGEN_GENERATING_OUTPUT
template MAGNUM_EXPORT BufferImage<1> Texture<1>::image(Int, BufferImage<1>&&, BufferUsage);
template MAGNUM_EXPORT BufferImage<2> Texture<2>::image(Int, BufferImage<2>&&, BufferUsage);
template MAGNUM_EXPORT BufferImage<3> Texture<3>::image(Int, BufferImage<3>&&, BufferUsage);
#endif

template<UnsignedInt dimensions> Image<dimensions> Texture<dimensions>::subImage(const Int level, const RangeTypeFor<dimensions, Int>& range, Image<dimensions>&& image) {
    this->subImage(level, range, image);
    return std::move(image);
}

#ifndef DOXYGEN_GENERATING_OUTPUT
template MAGNUM_EXPORT Image<1> Texture<1>::subImage(Int, const Range1Di&, Image<1>&&);
template MAGNUM_EXPORT Image<2> Texture<2>::subImage(Int, const Range2Di&, Image<2>&&);
template MAGNUM_EXPORT Image<3> Texture<3>::subImage(Int, const Range3Di&, Image<3>&&);
#endif

template<UnsignedInt dimensions> BufferImage<dimensions> Texture<dimensions>::subImage(const Int level, const RangeTypeFor<dimensions, Int>& range, BufferImage<dimensions>&& image, const BufferUsage usage) {
    this->subImage(level, range, image, usage);
    return std::move(image);
}

#ifndef DOXYGEN_GENERATING_OUTPUT
template MAGNUM_EXPORT BufferImage<1> Texture<1>::subImage(Int, const Range1Di&, BufferImage<1>&&, BufferUsage);
template MAGNUM_EXPORT BufferImage<2> Texture<2>::subImage(Int, const Range2Di&, BufferImage<2>&&, BufferUsage);
template MAGNUM_EXPORT BufferImage<3> Texture<3>::subImage(Int, const Range3Di&, BufferImage<3>&&, BufferUsage);
#endif
#endif

}
