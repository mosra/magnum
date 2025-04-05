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

#include <algorithm>
#include <random>
#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/BitArray.h>
#include <Corrade/Containers/BitArrayView.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/Containers/String.h>
#include <Corrade/Containers/StringIterable.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/Algorithms.h>
#include <Corrade/Utility/Path.h>

#include "Magnum/Image.h"
#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/DebugTools/ColorMap.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Math/PackingBatch.h"
#include "Magnum/Math/Range.h"
#include "Magnum/TextureTools/Atlas.h"
#include "Magnum/Trade/AbstractImageConverter.h"

#include "configure.h"

#ifdef __has_include
#if __has_include("AtlasTestFiles/stb_rect_pack.h")
#ifdef CORRADE_TARGET_GCC
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#endif
#include "AtlasTestFiles/stb_rect_pack.h"
#ifdef CORRADE_TARGET_GCC
#pragma GCC diagnostic pop
#endif

inline void stbSort(stbrp_rect* rects, std::size_t count, std::size_t, int(*compare)(const void*, const void*)) {
    std::sort(rects, rects + count, [compare](const stbrp_rect& a, const stbrp_rect& b) {
        /* It returns -1 or 1, and -1 is if a dimension is higher, which is
           descending, which is what should return true here */
        return compare(&a, &b) < 0;
    });
}

#define STB_RECT_PACK_IMPLEMENTATION
/* Comment this out to test with qsort instead (considerably slower as the
   comparator function call isn't inlined) */
#define STBRP_SORT stbSort
#ifdef CORRADE_TARGET_GCC
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#endif
#include "AtlasTestFiles/stb_rect_pack.h"
#ifdef CORRADE_TARGET_GCC
#pragma GCC diagnostic pop
#endif
#endif
#endif

namespace Magnum { namespace TextureTools { namespace Test { namespace {

struct AtlasBenchmark: TestSuite::Tester {
    explicit AtlasBenchmark();

    /* A bit chaotic here -- the benchmarkBegin() / benchmarkEnd() rely on the
       actual case filling _sizes and _filledArea before the
       CORRADE_BENCHMARK() ends. Then, the actual verification of the output
       (that there is exactly as many filled pixels as was in the input) is
       done by a custom CompareAtlasPacking comparator that's implemented
       below, and this comparator also produces details about the packing with
       --verbose and saves a TGA visualization with --save-diagnostic */

    void benchmarkBegin();
    std::uint64_t benchmarkEnd();

    void landfill();
    void stbRectPack();

    private:
        Containers::ArrayView<Vector2i> _sizes;
        UnsignedInt _filledArea;
};

const struct {
    const char* name;
    const char* filename;
    const char* image;
    Vector2i size;
    Containers::Optional<AtlasLandfillFlags> flags;
} LandfillData[]{
    {"Oxygen.ttf, portrait, widest first",
        "oxygen-glyphs.bin",
        "oxygen-glyphs-landfill-portrait-widest-first.tga",
        {512, 256}, {}},
    {"Oxygen.ttf, portrait, narrowest first",
        "oxygen-glyphs.bin",
        "oxygen-glyphs-landfill-portrait-narrowest-first.tga",
        {512, 256},
        ~~AtlasLandfillFlag::NarrowestFirst},
    {"Oxygen.ttf, landscape, widest first",
        "oxygen-glyphs.bin",
        "oxygen-glyphs-landfill-landscape-widest-first.tga",
        {512, 256},
        AtlasLandfillFlag::RotateLandscape|AtlasLandfillFlag::WidestFirst},
    {"Oxygen.ttf, landscape, narrowest first",
        "oxygen-glyphs.bin",
        "oxygen-glyphs-landfill-landscape-narrowest-first.tga",
        {512, 256},
        AtlasLandfillFlag::RotateLandscape|AtlasLandfillFlag::NarrowestFirst},
    {"Noto Serif Tangut, portrait, widest first",
        "noto-serif-tangut-glyphs.bin",
        "noto-serif-tangut-glyphs-landfill-portrait-widest-first.tga",
        {2048, 800},
        {}},
    {"Noto Serif Tangut, portrait, narrowest first",
        "noto-serif-tangut-glyphs.bin",
        "noto-serif-tangut-glyphs-landfill-portrait-narrowest-first.tga",
        {2048, 800},
        ~~AtlasLandfillFlag::NarrowestFirst},
    {"Noto Serif Tangut, landscape, widest first",
        "noto-serif-tangut-glyphs.bin",
        "noto-serif-tangut-glyphs-landfill-landscape-widest-first.tga",
        {2048, 800},
        AtlasLandfillFlag::RotateLandscape|AtlasLandfillFlag::WidestFirst},
    {"Noto Serif Tangut, landscape, narrowest first",
        "noto-serif-tangut-glyphs.bin",
        "noto-serif-tangut-glyphs-landfill-landscape-narrowest-first.tga",
        {2048, 800},
        AtlasLandfillFlag::RotateLandscape|AtlasLandfillFlag::NarrowestFirst},
    {"Noto Serif Tangut, landscape, narrowest first, reverse always",
        "noto-serif-tangut-glyphs.bin",
        "noto-serif-tangut-glyphs-landfill-landscape-narrowest-first-reverse-always.tga",
        {2048, 800},
        AtlasLandfillFlag::RotateLandscape|AtlasLandfillFlag::NarrowestFirst|AtlasLandfillFlag::ReverseDirectionAlways},
    {"FP 102344349, landscape, widest first",
        "fp-102344349-textures.bin",
        "fp-102344349-textures-landfill-portrait-widest-first.tga",
        {2048, 2048}, AtlasLandfillFlag::RotateLandscape|AtlasLandfillFlag::WidestFirst},
    {"FP 103997718_171030855, portrait, widest first",
        "fp-103997718-171030855-textures.bin",
        "fp-103997718-171030855-textures-landfill-portrait-widest-first.tga",
        {8192, 8192}, {}},
};

const struct {
    const char* name;
    const char* filename;
    const char* image;
    Vector2i size;
    Int rotate;
    bool allowOOM;
} StbRectPackData[]{
    {"Oxygen.ttf",
        "oxygen-glyphs.bin",
        "oxygen-glyph-stb.tga",
        {512, 256}, 0, false},
    {"Oxygen.ttf, portrait",
        "oxygen-glyphs.bin",
        "oxygen-glyphs-stb-portrait.tga",
        {512, 256}, 1, false},
    {"Oxygen.ttf, landscape",
        "oxygen-glyphs.bin",
        "oxygen-glyphs-stb-lanscape.tga",
        {512, 256}, -1, false},
    {"Oxygen.ttf, allow OOM",
        "oxygen-glyphs.bin",
        "oxygen-glyph-stb.tga",
        {512, 256}, 0, true},
    {"Noto Serif Tangut",
        "noto-serif-tangut-glyphs.bin",
        "noto-serif-tangut-glyphs-stb.tga",
        {2048, 800}, 0, false},
    {"Noto Serif Tangut, portrait",
        "noto-serif-tangut-glyphs.bin",
        "noto-serif-tangut-glyphs-stb-portrait.tga",
        {2048, 800}, 1, false},
    {"Noto Serif Tangut, landscape",
        "noto-serif-tangut-glyphs.bin",
        "noto-serif-tangut-glyphs-stb-lanscape.tga",
        {2048, 800}, -1, false},
    {"Noto Serif Tangut, allow OOM",
        "noto-serif-tangut-glyphs.bin",
        "noto-serif-tangut-glyphs-stb.tga",
        {2048, 800}, 0, true},
    {"FP 102344349",
        "fp-102344349-textures.bin",
        "fp-102344349-textures-stb.tga",
        {2048, 2048}, 0, false},
    {"FP 103997718_171030855",
        "fp-103997718-171030855-textures.bin",
        "fp-103997718-171030855-textures-stb.tga",
        {8192, 8192}, 0, false},
};

AtlasBenchmark::AtlasBenchmark() {
    addCustomInstancedBenchmarks({&AtlasBenchmark::landfill}, 1,
        Containers::arraySize(LandfillData),
        &AtlasBenchmark::benchmarkBegin,
        &AtlasBenchmark::benchmarkEnd,
        BenchmarkUnits::PercentageThousandths);

    addCustomInstancedBenchmarks({&AtlasBenchmark::stbRectPack}, 1,
        Containers::arraySize(StbRectPackData),
        &AtlasBenchmark::benchmarkBegin,
        &AtlasBenchmark::benchmarkEnd,
        BenchmarkUnits::PercentageThousandths);

    /* Run all benchmarks again but with time measurement instead of
       efficiency */
    addInstancedBenchmarks({&AtlasBenchmark::landfill}, 5,
        Containers::arraySize(LandfillData));

    addInstancedBenchmarks({&AtlasBenchmark::stbRectPack}, 5,
        Containers::arraySize(StbRectPackData));
}

class CompareAtlasPacking;

}}}}

namespace Corrade { namespace TestSuite {

using namespace Magnum;

template<> class Comparator<TextureTools::Test::CompareAtlasPacking> {
    public:
        explicit Comparator(Containers::StringView filename, const Vector2i& filledSize): _filename{filename}, _image{PixelFormat::RGBA8Unorm, filledSize, Containers::Array<char>{ValueInit, std::size_t(filledSize.product())*4}} {}

        ComparisonStatusFlags operator()(const Containers::Pair<Containers::StridedArrayView1D<const Vector2i>, Containers::BitArrayView>& offsetsRotations, Containers::ArrayView<const Vector2i> sizes) {
            _count = sizes.size();
            Containers::StridedArrayView2D<Color4ub> pixels = _image.pixels<Color4ub>();

            /* Generate a random set of colors. Have the same set every time,
               so location of corresponding entries can be compared across
               different algorithms. */
            std::mt19937 rd;
            std::uniform_int_distribution<Int> colorDist{0, 255};

            /* Fill pixels where the items are placed */
            for(std::size_t i = 0; i != sizes.size(); ++i) {
                const Color4ub color[]{DebugTools::ColorMap::turbo()[colorDist(rd)]};
                const Containers::StridedArrayView2D<const Color4ub> src{color, {1, 1}};

                const Vector2i size = !offsetsRotations.second().isEmpty() && offsetsRotations.second()[i] ? sizes[i].flipped() : sizes[i];
                const Containers::StridedArrayView2D<Color4ub> dst =
                    pixels.sliceSize(
                        {std::size_t(offsetsRotations.first()[i].y()),
                         std::size_t(offsetsRotations.first()[i].x())},
                        {std::size_t(size.y()),
                         std::size_t(size.x())});

                Utility::copy(src.broadcasted<0>(dst.size()[0])
                                 .broadcasted<1>(dst.size()[1]), dst);
            }

            /* Calculate expected area of the input sizes */
            _expectedTotal = 0;
            for(const Vector2i& i: sizes)
                _expectedTotal += i.product();

            /* Calculate the actual filled area */
            _actualTotal = 0;
            for(Containers::StridedArrayView1D<const Color4ub> row: pixels)
                for(Color4ub pixel: row)
                    if(pixel != Color4ub{})
                        ++_actualTotal;

            return (_actualTotal == _expectedTotal ? ComparisonStatusFlags{} :
                ComparisonStatusFlag::Failed)|ComparisonStatusFlag::Diagnostic|ComparisonStatusFlag::Verbose;
        }

        void printMessage(ComparisonStatusFlags flags, Debug& out, const char* actual, const char* expected) const {
            if(flags & ComparisonStatusFlag::Failed)
                out << "Packing" << actual << "from" << expected << "was lossy," << _actualTotal << "filled pixels but expected" << _expectedTotal;
            else if(flags & ComparisonStatusFlag::Verbose)
                out << "Packed" << _count << "images into" << Debug::packed << _image.size();
            else CORRADE_INTERNAL_ASSERT_UNREACHABLE();
        }

        void saveDiagnostic(CORRADE_UNUSED ComparisonStatusFlags flags, Debug& out, Containers::StringView path) {
            CORRADE_INTERNAL_ASSERT(flags & ComparisonStatusFlag::Diagnostic);

            PluginManager::Manager<Trade::AbstractImageConverter> imageConverterManager;
            Containers::Pointer<Trade::AbstractImageConverter> imageConverter = imageConverterManager.loadAndInstantiate("TgaImageConverter");

            Containers::String filename = Utility::Path::join(path, _filename);
            if(imageConverter->convertToFile(_image, filename))
                out << "->" << filename;
        }

    private:
        Containers::StringView _filename;
        Image2D _image;
        UnsignedInt _count, _actualTotal, _expectedTotal;
};

}}

namespace Magnum { namespace TextureTools { namespace Test { namespace {

class CompareAtlasPacking {
    public:
        explicit CompareAtlasPacking(Containers::StringView filename, const Vector2i& filledSize): _c{filename, filledSize} {}

        TestSuite::Comparator<CompareAtlasPacking>& comparator() {
            return _c;
        }

    private:
        TestSuite::Comparator<CompareAtlasPacking> _c;
};

void AtlasBenchmark::benchmarkBegin() {
    setBenchmarkName("efficiency");
    _filledArea = 0;
}

std::uint64_t AtlasBenchmark::benchmarkEnd() {
    /* If the test failed, exit early as continuing would cause a division by
       zero. */
    if(!_filledArea) return {};

    UnsignedInt total = 0;
    for(const Vector2i& i: _sizes)
        total += i.product();

    return total*100000ull/_filledArea;
}

void AtlasBenchmark::landfill() {
    auto&& data = LandfillData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Containers::Optional<Containers::Array<char>> sizeData = Utility::Path::read(Utility::Path::join({TEXTURETOOLS_TEST_DIR, "AtlasTestFiles", data.filename}));
    CORRADE_VERIFY(sizeData);

    auto sizes16 = Containers::arrayCast<Vector2s>(*sizeData);
    Containers::Array<Vector2i> sizes{NoInit, sizes16.size()};
    Math::castInto(
        stridedArrayView(sizes16).slice(&Vector2s::data),
        stridedArrayView(sizes).slice(&Vector2i::data));
    _sizes = sizes;

    AtlasLandfill atlas{data.size};
    if(data.flags)
        atlas.setFlags(*data.flags);

    Containers::Array<Vector2i> offsets{NoInit, _sizes.size()};
    Containers::BitArray flips{NoInit, _sizes.size()};
    CORRADE_BENCHMARK(1) {
        CORRADE_VERIFY(atlas.add(_sizes, offsets, flips));
        _filledArea = atlas.filledSize().product();
    }

    CORRADE_COMPARE_WITH(
        Containers::pair(Containers::StridedArrayView1D<const Vector2i>{offsets}, Containers::BitArrayView{flips}),
        _sizes,
        (CompareAtlasPacking{data.image, atlas.filledSize().xy()}));
}

void AtlasBenchmark::stbRectPack() {
    auto&& data = StbRectPackData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifdef STB_RECT_PACK_VERSION
    Containers::Optional<Containers::Array<char>> sizeData = Utility::Path::read(Utility::Path::join({TEXTURETOOLS_TEST_DIR, "AtlasTestFiles", data.filename}));
    CORRADE_VERIFY(sizeData);

    auto sizes16 = Containers::arrayCast<Vector2s>(*sizeData);
    Containers::Array<Vector2i> sizes{NoInit, sizes16.size()};
    Math::castInto(
        stridedArrayView(sizes16).slice(&Vector2s::data),
        stridedArrayView(sizes).slice(&Vector2i::data));
    _sizes = sizes;

    if(data.rotate) for(Vector2i& size: _sizes) {
        if((data.rotate < 0 && size.x() < size.y()) ||
           (data.rotate > 0 && size.x() > size.y()))
            size = size.flipped();
    }

    stbrp_context ctx;
    Containers::Array<stbrp_node> nodes{NoInit, _sizes.size()};
    stbrp_init_target(&ctx, data.size.x(), data.size.y(), nodes.data(), nodes.size());
    stbrp_setup_allow_out_of_mem(&ctx, data.allowOOM);

    struct MyRect {
        int:32;
        Vector2i size;
        Vector2i offset;
        int:32;
    };
    static_assert(sizeof(MyRect) == sizeof(stbrp_rect), "failed to fake a rect struct");

    Containers::Array<MyRect> rects{NoInit, _sizes.size()};
    Utility::copy(_sizes, stridedArrayView(rects).slice(&MyRect::size));

    Int height = 0;
    CORRADE_BENCHMARK(1) {
        CORRADE_VERIFY(stbrp_pack_rects(&ctx, reinterpret_cast<stbrp_rect*>(rects.data()), rects.size()));
        for(const MyRect& i: rects)
            height = Math::max(i.size.y() + i.offset.y(), height);
        _filledArea = height*data.size.x();
    }

    Vector2i filledSize{data.size.x(), height};

    CORRADE_COMPARE_WITH(
        Containers::pair(Containers::StridedArrayView1D<const MyRect>{rects}.slice(&MyRect::offset), Containers::BitArrayView{}),
        _sizes,
        (CompareAtlasPacking{data.image, filledSize}));
    #else
    CORRADE_SKIP("stb_rect_pack.h not found, place it next to the test to benchmark it");
    #endif
}

}}}}

CORRADE_TEST_MAIN(Magnum::TextureTools::Test::AtlasBenchmark)
