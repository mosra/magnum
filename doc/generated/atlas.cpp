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

#include <random>
#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/BitArray.h>
#include <Corrade/Containers/BitArrayView.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/Containers/StringStl.h> /** @todo remove once formatString() isn't used */
#include <Corrade/Utility/FormatStl.h> /** @todo remove once growable String exists */
#include <Corrade/Utility/Path.h>

#include "Magnum/Math/Color.h"
#include "Magnum/Math/PackingBatch.h"
#include "Magnum/Math/Range.h"
#include "Magnum/DebugTools/ColorMap.h"
#include "Magnum/TextureTools/Atlas.h"

using namespace Magnum;

int main() {
    /* For a random set of colors */
    std::random_device rd;
    std::uniform_int_distribution<Int> colorDist{0, 255};

    /* atlasArrayPowerOfTwo() */
    {
        constexpr Int maxSize = 7;
        constexpr Int padding = 4;
        constexpr Int layerPadding = 4;
        constexpr Float displaySizeDivisor = 1.0f;

        /* Random set of texture sizes would be stupid, so have a say on how
           many is actually generated */
        std::size_t sizeCount[]{
            0,
            0,
            9, /* 4 */
            7, /* 8 */
            4, /* 16 */
            5, /* 32 */
            6, /* 64 */
            2, /* 128 */
        };
        std::size_t count = 0;
        for(std::size_t i: sizeCount)
            count += i;

        struct Data {
            Vector2i size;
            Vector3i offset;
        };
        Containers::Array<Data> data{NoInit, count};
        Vector2i size;
        std::size_t currentSize = 0;
        std::size_t currentSizeCount = 0;
        for(std::size_t i = 0; i != count; ++i) {
            while(currentSizeCount >= sizeCount[currentSize]) {
                ++currentSize;
                currentSizeCount = 0;
            }

            ++currentSizeCount;

            data[i].size = Vector2i{1 << currentSize};

            if(i) size.x() += padding;
            size.x() += data[i].size.x();
            size.y() = Math::max(size.y(), data[i].size.y());
        }

        Int layerCount = TextureTools::atlasArrayPowerOfTwo(Vector2i{1 << maxSize},
            stridedArrayView(data).slice(&Data::size),
            stridedArrayView(data).slice(&Data::offset));

        Range2Di viewBox{{}, {layerCount*(1 << maxSize) + (layerCount - 1)*layerPadding, 1 << maxSize}};

        std::string out;
        Utility::formatInto(out, out.size(), R"(<svg class="m-image" style="width: {4}px; height: {5}px;" viewBox="{0} {1} {2} {3}" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
)",
            viewBox.left(), viewBox.bottom(), viewBox.sizeX(), viewBox.sizeY(), viewBox.sizeX()/displaySizeDivisor, viewBox.sizeY()/displaySizeDivisor);

        for(std::size_t i = 0; i != data.size(); ++i) {
            const Vector2i size = data[i].size;
            const Vector2i offset = data[i].offset.xy();
            const Int layer = data[i].offset.z();
            const Color4ub color = DebugTools::ColorMap::turbo()[colorDist(rd)];

            Utility::formatInto(out, out.size(), R"(  <rect x="{}" y="{}" width="{}" height="{}" style="fill:#{:.2x}{:.2x}{:.2x}"/>
)",
                layer*((1 << maxSize) + layerPadding) + offset.x(), (1 << maxSize) - size.y() - offset.y(), size.x(), size.y(), color.r(), color.g(), color.b());
        }

        Utility::formatInto(out, out.size(), R"(</svg>
)");

        CORRADE_INTERNAL_ASSERT_OUTPUT(Utility::Path::write("atlas-array-power-of-two.svg", Containers::StringView{out}));

    /* AtlasLandfill */
    } {
        constexpr Float displaySizeDivisor = 1.0f;

        Containers::Optional<Containers::Array<char>> sizeData = Utility::Path::read(Utility::Path::join(Utility::Path::path(__FILE__), "../../src/Magnum/TextureTools/Test/AtlasTestFiles/oxygen-glyphs.bin"));
        CORRADE_INTERNAL_ASSERT(sizeData);

        auto sizes16 = Containers::arrayCast<Vector2s>(*sizeData);
        Containers::Array<Vector2i> sizes{NoInit, sizes16.size()};
        Math::castInto(
            stridedArrayView(sizes16).slice(&Vector2s::data),
            stridedArrayView(sizes).slice(&Vector2i::data));

        TextureTools::AtlasLandfill atlas{{512, 512}};
        Containers::Array<Vector2i> offsets{NoInit, sizes.size()};
        Containers::BitArray rotations{NoInit, sizes.size()};
        CORRADE_INTERNAL_ASSERT(atlas.add(sizes, offsets, rotations));

        Range2Di viewBox{{}, atlas.filledSize().xy()};

        std::string out;
        Utility::formatInto(out, out.size(), R"(<svg class="m-image" style="width: {4}px; height: {5}px;" viewBox="{0} {1} {2} {3}" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
)",
            viewBox.left(), viewBox.bottom(), viewBox.sizeX(), viewBox.sizeY(), viewBox.sizeX()/displaySizeDivisor, viewBox.sizeY()/displaySizeDivisor);

        for(std::size_t i = 0; i != sizes.size(); ++i) {
            const Vector2i size = rotations[i] ? sizes[i].flipped() : sizes[i];
            const Vector2i offset = offsets[i];
            const Color4ub color = DebugTools::ColorMap::turbo()[colorDist(rd)];

            Utility::formatInto(out, out.size(), R"(  <rect x="{}" y="{}" width="{}" height="{}" style="fill:#{:.2x}{:.2x}{:.2x}"/>
)",
                offset.x(), viewBox.sizeY() - size.y() - offset.y(), size.x(), size.y(), color.r(), color.g(), color.b());
        }

        Utility::formatInto(out, out.size(), R"(</svg>
)");

        CORRADE_INTERNAL_ASSERT_OUTPUT(Utility::Path::write("atlas-landfill.svg", Containers::StringView{out}));
    }
}
