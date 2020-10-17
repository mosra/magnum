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

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/Utility/Algorithms.h>

#include "Magnum/PixelFormat.h"
#include "Magnum/ImageView.h"
#include "Magnum/DebugTools/ColorMap.h"
#include "Magnum/Trade/AbstractImageConverter.h"

using namespace Magnum;
using namespace Magnum::Math::Literals;

int main() {
    PluginManager::Manager<Trade::AbstractImageConverter> converterManager;
    Containers::Pointer<Trade::AbstractImageConverter> converter = converterManager.loadAndInstantiate("PngImageConverter");
    if(!converter) {
        Error() << "Cannot load image converter plugin";
        return 1;
    }

    for(auto& image: {
        std::make_pair(DebugTools::ColorMap::turbo(), "colormap-turbo.png"),
        std::make_pair(DebugTools::ColorMap::magma(), "colormap-magma.png"),
        std::make_pair(DebugTools::ColorMap::plasma(), "colormap-plasma.png"),
        std::make_pair(DebugTools::ColorMap::inferno(), "colormap-inferno.png"),
        std::make_pair(DebugTools::ColorMap::viridis(), "colormap-viridis.png"),
        std::make_pair(DebugTools::ColorMap::coolWarmSmooth(), "colormap-cool-warm-smooth.png"),
        std::make_pair(DebugTools::ColorMap::coolWarmBent(), "colormap-cool-warm-bent.png")
    }) {
        constexpr Vector2i OutputSize{256, 12};
        CORRADE_INTERNAL_ASSERT(image.first.size() == std::size_t(OutputSize.x()));

        Containers::Array<Vector3ub> data{Containers::NoInit, std::size_t(OutputSize.product())};
        Containers::StridedArrayView2D<const Vector3ub> src{image.first,
            {std::size_t(OutputSize.y()), std::size_t(OutputSize.x())}, {0, 3}};
        Containers::StridedArrayView2D<Vector3ub> dst{data,
            {std::size_t(OutputSize.y()), std::size_t(OutputSize.x())}};
        Utility::copy(src, dst);

        if(!converter->exportToFile(ImageView2D{PixelFormat::RGB8Unorm, OutputSize, data}, image.second))
            return 2;
    }
}
