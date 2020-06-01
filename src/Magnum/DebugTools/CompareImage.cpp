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

#include "CompareImage.h"

#include <map>
#include <sstream>
#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/TestSuite/Comparator.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/Directory.h>

#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/Math/Functions.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Math/Algorithms/KahanSum.h"
#include "Magnum/Trade/AbstractImageConverter.h"
#include "Magnum/Trade/AbstractImporter.h"
#include "Magnum/Trade/ImageData.h"

namespace Magnum { namespace DebugTools { namespace Implementation {

namespace {

template<std::size_t size, class T> Float calculateImageDelta(const Containers::StridedArrayView2D<const Math::Vector<size, T>>& actual, const Containers::StridedArrayView2D<const Math::Vector<size, T>>& expected, const Containers::StridedArrayView2D<Float>& output) {
    CORRADE_INTERNAL_ASSERT(actual.size() == output.size());
    CORRADE_INTERNAL_ASSERT(output.size() == expected.size());

    /* Calculate deltas and maximal value of them */
    Float max{};
    for(std::size_t i = 0, iMax = expected.size()[0]; i != iMax; ++i) {
        Containers::StridedArrayView1D<const Math::Vector<size, T>> actualRow = actual[i];
        Containers::StridedArrayView1D<const Math::Vector<size, T>> expectedRow = expected[i];
        Containers::StridedArrayView1D<Float> outputRow = output[i];

        for(std::size_t j = 0, jMax = expectedRow.size(); j != jMax; ++j) {
            /* Explicitly convert from T to Float */
            auto actualPixel = Math::Vector<size, Float>(actualRow[j]);
            auto expectedPixel = Math::Vector<size, Float>(expectedRow[j]);

            /* First calculate a classic difference */
            Math::Vector<size, Float> diff = Math::abs(actualPixel - expectedPixel);

            /* Mark pixels that are NaN in both actual and expected pixels as
               having no difference */
            diff = Math::lerp(diff, {}, Math::isNan(actualPixel) & Math::isNan(expectedPixel));

            /* Then also mark pixels that are the same sign of infnity in both
               actual and expected pixel as having no difference */
            diff = Math::lerp(diff, {}, Math::isInf(actualPixel) & Math::isInf(expectedPixel) & Math::equal(actualPixel, expectedPixel));

            /* Calculate the difference and save it to the output image even
               with NaN and ±Inf (as the user should know) */
            outputRow[j] = diff.sum()/size;

            /* On the other hand, infs and NaNs should not contribute to the
               max delta -- because all other differences would be zero
               compared to them */
            max = Math::max(max, Math::lerp(diff, {}, Math::isNan(diff)|Math::isInf(diff)).sum()/size);
        }
    }

    return max;
}

}

std::tuple<Containers::Array<Float>, Float, Float> calculateImageDelta(const PixelFormat actualFormat, const Containers::StridedArrayView3D<const char>& actualPixels, const ImageView2D& expected) {
    /* Calculate a delta image */
    Containers::Array<Float> deltaData{Containers::NoInit,
        std::size_t(expected.size().product())};
    Containers::StridedArrayView2D<Float> delta{deltaData,
        {std::size_t(expected.size().y()), std::size_t(expected.size().x())}};

    CORRADE_INTERNAL_ASSERT(actualFormat == expected.format());
    #ifdef CORRADE_NO_ASSERT
    static_cast<void>(actualFormat);
    #endif
    CORRADE_ASSERT(!isPixelFormatImplementationSpecific(expected.format()),
        "DebugTools::CompareImage: can't compare implementation-specific pixel formats", {});

    #ifdef __GNUC__
    #pragma GCC diagnostic push
    #pragma GCC diagnostic error "-Wswitch"
    #endif
    Float max{Constants::nan()};
    switch(expected.format()) {
        #define _c(format, size, T)                                         \
            case PixelFormat::format:                                       \
                max = calculateImageDelta<size, T>(                         \
                    Containers::arrayCast<2, const Math::Vector<size, T>>(actualPixels), \
                    expected.pixels<Math::Vector<size, T>>(), delta);       \
                break;
        #define _d(first, second, size, T)                                  \
            case PixelFormat::first:                                        \
            case PixelFormat::second:                                       \
                max = calculateImageDelta<size, T>(                         \
                    Containers::arrayCast<2, const Math::Vector<size, T>>(actualPixels), \
                    expected.pixels<Math::Vector<size, T>>(), delta);       \
                break;
        #define _e(first, second, third, size, T)                           \
            case PixelFormat::first:                                        \
            case PixelFormat::second:                                       \
            case PixelFormat::third:                                        \
                max = calculateImageDelta<size, T>(                         \
                    Containers::arrayCast<2, const Math::Vector<size, T>>(actualPixels), \
                    expected.pixels<Math::Vector<size, T>>(), delta);       \
                break;
        /* LCOV_EXCL_START */
        _e(R8Unorm, R8Srgb, R8UI, 1, UnsignedByte)
        _e(RG8Unorm, RG8Srgb, RG8UI, 2, UnsignedByte)
        _e(RGB8Unorm, RGB8Srgb, RGB8UI, 3, UnsignedByte)
        _e(RGBA8Unorm, RGBA8Srgb, RGBA8UI, 4, UnsignedByte)
        _d(R8Snorm, R8I, 1, Byte)
        _d(RG8Snorm, RG8I, 2, Byte)
        _d(RGB8Snorm, RGB8I, 3, Byte)
        _d(RGBA8Snorm, RGBA8I, 4, Byte)
        _d(R16Unorm, R16UI, 1, UnsignedShort)
        _d(RG16Unorm, RG16UI, 2, UnsignedShort)
        _d(RGB16Unorm, RGB16UI, 3, UnsignedShort)
        _d(RGBA16Unorm, RGBA16UI, 4, UnsignedShort)
        _d(R16Snorm, R16I, 1, Short)
        _d(RG16Snorm, RG16I, 2, Short)
        _d(RGB16Snorm, RGB16I, 3, Short)
        _d(RGBA16Snorm, RGBA16I, 4, Short)
        _c(R32UI, 1, UnsignedInt)
        _c(RG32UI, 2, UnsignedInt)
        _c(RGB32UI, 3, UnsignedInt)
        _c(RGBA32UI, 4, UnsignedInt)
        _c(R32I, 1, Int)
        _c(RG32I, 2, Int)
        _c(RGB32I, 3, Int)
        _c(RGBA32I, 4, Int)
        _c(R32F, 1, Float)
        _c(RG32F, 2, Float)
        _c(RGB32F, 3, Float)
        _c(RGBA32F, 4, Float)
        /* LCOV_EXCL_STOP */
        #undef _e
        #undef _d
        #undef _c

        case PixelFormat::R16F:
        case PixelFormat::RG16F:
        case PixelFormat::RGB16F:
        case PixelFormat::RGBA16F:
            CORRADE_ASSERT_UNREACHABLE("DebugTools::CompareImage: half-float formats are not supported yet", {});
    }
    #ifdef __GNUC__
    #pragma GCC diagnostic pop
    #endif

    CORRADE_ASSERT(max == max,
        "DebugTools::CompareImage: unknown format" << expected.format(), {});

    /* Calculate mean delta. Do it the special way so we don't lose
       precision -- that would result in having false negatives! This
       *deliberately* leaves specials in. The `max` has them already filtered
       out so if this would filter them out as well, there would be nothing
       left that could cause the comparison to fail. */
    const Float mean = Math::Algorithms::kahanSum(deltaData.begin(), deltaData.end())/deltaData.size();

    return std::make_tuple(std::move(deltaData), max, mean);
}

namespace {
    /* Done by printing an white to black gradient using one of the online
       ASCII converters. Yes, I'm lazy. Another one could be " .,:;ox%#@". */
    constexpr char CharacterData[] = " .,:~=+?7IZ$08DNM";
    constexpr Containers::ArrayView<const char> Characters{CharacterData, sizeof(CharacterData) - 1};
}

void printDeltaImage(Debug& out, Containers::ArrayView<const Float> deltas, const Vector2i& size, const Float max, const Float maxThreshold, const Float meanThreshold) {
    CORRADE_INTERNAL_ASSERT(meanThreshold <= maxThreshold);

    /* At most 64 characters per line. The console fonts height is usually 2x
       the width, so there is twice the pixels per block */
    const Vector2i pixelsPerBlock{(size.x() + 63)/64, 2*((size.x() + 63)/64)};
    const Vector2i blockCount = (size + pixelsPerBlock - Vector2i{1})/pixelsPerBlock;

    for(std::int_fast32_t y = 0; y != blockCount.y(); ++y) {
        if(y) out << Debug::newline;
        out << "          |";

        for(std::int_fast32_t x = 0; x != blockCount.x(); ++x) {
            /* Going bottom-up so we don't flip the image upside down when
               printing */
            const Vector2i offset = Vector2i{Int(x), blockCount.y() - Int(y) - 1}*pixelsPerBlock;
            const Vector2i blockSize = Math::min(size - offset, Vector2i{pixelsPerBlock});

            Float blockMax{};
            for(std::int_fast32_t yb = 0; yb != blockSize.y(); ++yb) {
                for(std::int_fast32_t xb = 0; xb != blockSize.x(); ++xb) {
                    /* Propagating NaNs. The delta should never be negative --
                       but we need to test inversely in order to work correctly
                       for NaNs. */
                    const Float delta = deltas[(offset.y() + yb)*size.x() + offset.x() + xb];
                    CORRADE_INTERNAL_ASSERT(!(delta < 0.0f));
                    blockMax = Math::max(delta, blockMax);
                }
            }

            const char c = Math::isNan(blockMax) ? Characters.back() : Characters[Int(Math::round(Math::min(1.0f, blockMax/max)*(Characters.size() - 1)))];

            if(blockMax > maxThreshold)
                out << Debug::boldColor(Debug::Color::Red) << Debug::nospace << std::string{c} << Debug::resetColor;
            else if(blockMax > meanThreshold)
                out << Debug::boldColor(Debug::Color::Yellow) << Debug::nospace << std::string{c} << Debug::resetColor;
            else out << Debug::nospace << std::string{c};
        }

        out << Debug::nospace << "|";
    }
}

namespace {

void printPixelAt(Debug& out, const Containers::StridedArrayView3D<const char>& pixels, const Vector2i& pos, const PixelFormat format) {
    const char* const pixel = &pixels[pos.y()][pos.x()][0];

    #ifdef __GNUC__
    #pragma GCC diagnostic push
    #pragma GCC diagnostic error "-Wswitch"
    #endif
    switch(format) {
        #define _c(format, size, T)                                         \
            case PixelFormat::format:                                       \
                out << *reinterpret_cast<const Math::Vector<size, T>*>(pixel); \
                break;
        #define _d(first, second, size, T)                                  \
            case PixelFormat::first:                                        \
            case PixelFormat::second:                                       \
                out << *reinterpret_cast<const Math::Vector<size, T>*>(pixel); \
                break;
        #define _e(first, second, third, size, T)                           \
            case PixelFormat::first:                                        \
            case PixelFormat::second:                                       \
            case PixelFormat::third:                                       \
                out << *reinterpret_cast<const Math::Vector<size, T>*>(pixel); \
                break;
        /* LCOV_EXCL_START */
        _e(R8Unorm, R8Srgb, R8UI, 1, UnsignedByte)
        _e(RG8Unorm, RG8Srgb, RG8UI, 2, UnsignedByte)
        _c(RGB8UI, 3, UnsignedByte)
        _c(RGBA8UI, 4, UnsignedByte)
        /* RGB8Unorm, RGBA8Unorm, RGB8Srgb, RGBA8Srgb handled below */
        _d(R8Snorm, R8I, 1, Byte)
        _d(RG8Snorm, RG8I, 2, Byte)
        _d(RGB8Snorm, RGB8I, 3, Byte)
        _d(RGBA8Snorm, RGBA8I, 4, Byte)
        _d(R16Unorm, R16UI, 1, UnsignedShort)
        _d(RG16Unorm, RG16UI, 2, UnsignedShort)
        _d(RGB16Unorm, RGB16UI, 3, UnsignedShort)
        _d(RGBA16Unorm, RGBA16UI, 4, UnsignedShort)
        _d(R16Snorm, R16I, 1, Short)
        _d(RG16Snorm, RG16I, 2, Short)
        _d(RGB16Snorm, RGB16I, 3, Short)
        _d(RGBA16Snorm, RGBA16I, 4, Short)
        _c(R32UI, 1, UnsignedInt)
        _c(RG32UI, 2, UnsignedInt)
        _c(RGB32UI, 3, UnsignedInt)
        _c(RGBA32UI, 4, UnsignedInt)
        _c(R32I, 1, Int)
        _c(RG32I, 2, Int)
        _c(RGB32I, 3, Int)
        _c(RGBA32I, 4, Int)
        _c(R32F, 1, Float)
        _c(RG32F, 2, Float)
        _c(RGB32F, 3, Float)
        _c(RGBA32F, 4, Float)
        /* LCOV_EXCL_STOP */
        #undef _e
        #undef _d
        #undef _c

        /* Take the opportunity and print 8-bit colors in hex */
        case PixelFormat::RGB8Unorm:
        case PixelFormat::RGB8Srgb:
            out << *reinterpret_cast<const Color3ub*>(pixel);
            break;
        case PixelFormat::RGBA8Unorm:
        case PixelFormat::RGBA8Srgb:
            out << *reinterpret_cast<const Color4ub*>(pixel);
            break;

        case PixelFormat::R16F:
        case PixelFormat::RG16F:
        case PixelFormat::RGB16F:
        case PixelFormat::RGBA16F:
            /* Already handled by a printing assert before */
            CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
    }
    #ifdef __GNUC__
    #pragma GCC diagnostic pop
    #endif
}

}

void printPixelDeltas(Debug& out, Containers::ArrayView<const Float> delta, PixelFormat format, const Containers::StridedArrayView3D<const char>& actualPixels, const Containers::StridedArrayView3D<const char>& expectedPixels, const Float maxThreshold, const Float meanThreshold, std::size_t maxCount) {
    /* Find first maxCount values above mean threshold and put them into a
       sorted map. Need to reverse the condition in order to catch NaNs. */
    std::multimap<Float, std::size_t> large;
    for(std::size_t i = 0; i != delta.size(); ++i)
        if(!(delta[i] <= meanThreshold)) large.emplace(delta[i], i);

    /* If there's no outliers, don't print anything. This can happen only when
       --verbose is used. */
    if(large.empty()) return;

    /* If there are outliers, adding a newline to separate itself from the
       delta image -- calling code wouldn't know if we produce output or not,
       so it can't do that on its own. */
    out << Debug::newline;

    if(large.size() > maxCount)
        out << "        Top" << maxCount << "out of" << large.size() << "pixels above max/mean threshold:";
    else
        out << "        Pixels above max/mean threshold:";

    /* Print the values from largest to smallest. Branching on the done in the
       inner loop but that doesn't matter as we always print just ~10 values. */
    std::size_t count = 0;
    for(auto it = large.crbegin(); it != large.crend(); ++it) {
        if(++count > maxCount) break;

        Vector2i pos;
        std::tie(pos.y(), pos.x()) = Math::div(Int(it->second), Int(expectedPixels.size()[1]));
        out << Debug::newline << "          [" << Debug::nospace << pos.x()
            << Debug::nospace << "," << Debug::nospace << pos.y()
            << Debug::nospace << "]";

        printPixelAt(out, actualPixels, pos, format);

        out << Debug::nospace << ", expected";

        printPixelAt(out, expectedPixels, pos, format);

        out << "(Δ =" << Debug::boldColor(delta[it->second] > maxThreshold ?
            Debug::Color::Red : Debug::Color::Yellow) << delta[it->second]
            << Debug::nospace << Debug::resetColor << ")";
    }
}

enum class Result: UnsignedByte {
    PluginLoadFailed = 1,
    ActualImageLoadFailed,
    ExpectedImageLoadFailed,
    ActualImageIsCompressed,
    ExpectedImageIsCompressed,
    DifferentSize,
    DifferentFormat,
    AboveThresholds,
    AboveMeanThreshold,
    AboveMaxThreshold,
    VerboseMessage
};

class ImageComparatorBase::State {
    public:
        explicit State(PluginManager::Manager<Trade::AbstractImporter>* importerManager, PluginManager::Manager<Trade::AbstractImageConverter>* converterManager, Float maxThreshold, Float meanThreshold): _importerManager{importerManager}, _converterManager{converterManager}, maxThreshold{maxThreshold}, meanThreshold{meanThreshold} {}

        /* Lazy-create the importer / converter if those weren't passed from
           the outside. The importer might not be used at all if we are
           comparing two image data (but in that case the FileState won't be
           created at all); the converter will get used only very rarely for
           the --save-failed option. Treat both the same lazy way to keep the
           code straightforward. */
        PluginManager::Manager<Trade::AbstractImporter>& importerManager() {
            if(!_importerManager) _importerManager = &_privateImporterManager.emplace();
            return *_importerManager;
        }
        PluginManager::Manager<Trade::AbstractImageConverter>& converterManager() {
            if(!_converterManager) _converterManager = &_privateConverterManager.emplace();
            return *_converterManager;
        }

    private:
        Containers::Optional<PluginManager::Manager<Trade::AbstractImporter>> _privateImporterManager;
        Containers::Optional<PluginManager::Manager<Trade::AbstractImageConverter>> _privateConverterManager;
        PluginManager::Manager<Trade::AbstractImporter>* _importerManager{};
        PluginManager::Manager<Trade::AbstractImageConverter>* _converterManager{};

    public:
        std::string actualFilename, expectedFilename;
        Containers::Optional<Trade::ImageData2D> actualImageData, expectedImageData;
        PixelFormat actualFormat;
        Containers::StridedArrayView3D<const char> actualPixels;
        Containers::Optional<ImageView2D> expectedImage;

        Float maxThreshold, meanThreshold;
        Result result{};
        Float max{}, mean{};
        Containers::Array<Float> delta;
};

ImageComparatorBase::ImageComparatorBase(PluginManager::Manager<Trade::AbstractImporter>* importerManager, PluginManager::Manager<Trade::AbstractImageConverter>* converterManager, Float maxThreshold, Float meanThreshold): _state{Containers::InPlaceInit, importerManager, converterManager, maxThreshold, meanThreshold} {
    CORRADE_ASSERT(!Math::isNan(maxThreshold) && !Math::isInf(maxThreshold) &&
                   !Math::isNan(meanThreshold) && !Math::isInf(meanThreshold),
        "DebugTools::CompareImage: thresholds can't be NaN or infinity", );
    CORRADE_ASSERT(meanThreshold <= maxThreshold,
        "DebugTools::CompareImage: maxThreshold can't be smaller than meanThreshold", );
}

ImageComparatorBase::~ImageComparatorBase() = default;

TestSuite::ComparisonStatusFlags ImageComparatorBase::compare(const PixelFormat actualFormat, const Containers::StridedArrayView3D<const char>& actualPixels, const ImageView2D& expected) {
    /* The reference can be pointing to the storage, don't call the assignment
       on itself in that case */
    if(&_state->actualPixels != &actualPixels) {
        _state->actualFormat = actualFormat;
        _state->actualPixels = actualPixels;
    }
    if(!_state->expectedImage || &*_state->expectedImage != &expected)
        _state->expectedImage = expected;

    /* Verify that the images are the same */
    if(Vector2i{Int(actualPixels.size()[1]), Int(actualPixels.size()[0])} != expected.size()) {
        _state->result = Result::DifferentSize;
        return TestSuite::ComparisonStatusFlag::Failed;
    }
    if(actualFormat != expected.format()) {
        _state->result = Result::DifferentFormat;
        return TestSuite::ComparisonStatusFlag::Failed;
    }

    Containers::Array<Float> delta;
    std::tie(delta, _state->max, _state->mean) = DebugTools::Implementation::calculateImageDelta(actualFormat, actualPixels, expected);

    /* Verify the max/mean is never below zero so we didn't mess up when
       calculating specials. Note the inverted condition to catch NaNs in
       _mean. The max should OTOH be never special as it would make all other
       deltas become zero in comparison. */
    CORRADE_INTERNAL_ASSERT(!(_state->mean < 0.0f));
    CORRADE_INTERNAL_ASSERT(_state->max >= 0.0f && !Math::isInf(_state->max) && !Math::isNan(_state->max));

    /* If both values are not above threshold, success. If the values are
       above, save the delta. If the values are below thresholds but nonzero,
       we can provide optional message -- save the delta in that case too. */
    TestSuite::ComparisonStatusFlags flags = TestSuite::ComparisonStatusFlag::Failed;
    if(_state->max > _state->maxThreshold && !(_state->mean <= _state->meanThreshold))
        _state->result = Result::AboveThresholds;
    else if(_state->max > _state->maxThreshold)
        _state->result = Result::AboveMaxThreshold;
    /* Comparing this way in order to propely catch NaNs in mean values */
    else if(!(_state->mean <= _state->meanThreshold))
        _state->result = Result::AboveMeanThreshold;
    else if(_state->max > 0.0f || _state->mean > 0.0f) {
        _state->result = Result::VerboseMessage;
        flags = TestSuite::ComparisonStatusFlag::Verbose;
    } else return TestSuite::ComparisonStatusFlags{};

    /* Otherwise save the deltas and fail */
    _state->delta = std::move(delta);
    return flags;
}

TestSuite::ComparisonStatusFlags ImageComparatorBase::operator()(const ImageView2D& actual, const ImageView2D& expected) {
    return compare(actual.format(), actual.pixels(), expected);
}

TestSuite::ComparisonStatusFlags ImageComparatorBase::operator()(const std::string& actual, const std::string& expected) {
    _state->actualFilename = actual;
    _state->expectedFilename = expected;

    Containers::Pointer<Trade::AbstractImporter> importer;
    /* Can't load importer plugin. While we *could* save diagnostic in this
       case too, it would make no sense as it's a Schrödinger image at this
       point -- we have no idea if it's the same or not until we open it. */
    if(!(importer = _state->importerManager().loadAndInstantiate("AnyImageImporter"))) {
        _state->result = Result::PluginLoadFailed;
        return TestSuite::ComparisonStatusFlag::Failed;
    }

    /* Same here. We can't open the image for some reason (file missing? broken
       plugin?), so can't know if it's the same or not. */
    if(!importer->openFile(actual) || !(_state->actualImageData = importer->image2D(0))) {
        _state->result = Result::ActualImageLoadFailed;
        return TestSuite::ComparisonStatusFlag::Failed;
    }

    /* If the actual data are compressed, we won't be able to compare them
       (and probably neither save them back due to format mismatches). Don't
       provide diagnostic in that case. */
    if(_state->actualImageData->isCompressed()) {
        _state->result = Result::ActualImageIsCompressed;
        return TestSuite::ComparisonStatusFlag::Failed;
    }

    /* At this point we already know we successfully opened the actual file,
       so save also the view on its parsed contents to avoid it going out of
       scope. We're saving through an image converter, not the original file,
       see saveDiagnostic() for reasons why. */
    _state->actualFormat = _state->actualImageData->format();
    _state->actualPixels = _state->actualImageData->pixels();

    /* If the expected file can't be opened, we should still be able to save
       the actual as a diagnostic. This could get also used to generate ground
       truth data on the first-ever test run. */
    if(!importer->openFile(expected) || !(_state->expectedImageData = importer->image2D(0))) {
        _state->result = Result::ExpectedImageLoadFailed;
        return TestSuite::ComparisonStatusFlag::Failed|TestSuite::ComparisonStatusFlag::Diagnostic;
    }

    /* If the expected file is compressed, it's bad, but it doesn't mean we
       couldn't save the actual file either */
    if(_state->expectedImageData->isCompressed()) {
        _state->result = Result::ExpectedImageIsCompressed;
        return TestSuite::ComparisonStatusFlag::Failed|TestSuite::ComparisonStatusFlag::Diagnostic;
    }

    /* Save also a view on the expected image data and proxy to the actual data
       comparison. If comparison failed, offer to save a diagnostic. */
    _state->expectedImage.emplace(*_state->expectedImageData);
    TestSuite::ComparisonStatusFlags flags = compare(_state->actualFormat, _state->actualPixels, *_state->expectedImage);
    if(flags & TestSuite::ComparisonStatusFlag::Failed)
        flags |= TestSuite::ComparisonStatusFlag::Diagnostic;
    return flags;
}

TestSuite::ComparisonStatusFlags ImageComparatorBase::compare(const PixelFormat actualFormat, const Containers::StridedArrayView3D<const char>& actualPixels, const std::string& expected) {
    _state->expectedFilename = expected;

    Containers::Pointer<Trade::AbstractImporter> importer;
    /* Can't load importer plugin. While we *could* save diagnostic in this
       case too, it would make no sense as it's a Schrödinger image at this
       point -- we have no idea if it's the same or not until we open it. */
    if(!(importer = _state->importerManager().loadAndInstantiate("AnyImageImporter"))) {
        _state->result = Result::PluginLoadFailed;
        return TestSuite::ComparisonStatusFlag::Failed;
    }

    /* Save the actual image so saveDiagnostic() can reach the data even if we
       fail before the final data comparison (which does this as well). The
       reference can be pointing to the storage, don't call the assignment on
       itself in that case. */
    if(&_state->actualPixels != &actualPixels) {
        _state->actualFormat = actualFormat;
        _state->actualPixels = actualPixels;
    }

    /* If the expected file can't be opened, we should still be able to save
       the actual as a diagnostic. This could get also used to generate ground
       truth data on the first-ever test run. */
    if(!importer->openFile(expected) || !(_state->expectedImageData = importer->image2D(0))) {
        _state->result = Result::ExpectedImageLoadFailed;
        return TestSuite::ComparisonStatusFlag::Failed|TestSuite::ComparisonStatusFlag::Diagnostic;
    }

    /* If the expected file is compressed, it's bad, but it doesn't mean we
       couldn't save the actual file either */
    if(_state->expectedImageData->isCompressed()) {
        _state->result = Result::ExpectedImageIsCompressed;
        return TestSuite::ComparisonStatusFlag::Failed|TestSuite::ComparisonStatusFlag::Diagnostic;
    }

    /* Save a view on the expected image data and proxy to the actual data
       comparison. If comparison failed, offer to save a diagnostic. */
    _state->expectedImage.emplace(*_state->expectedImageData);
    TestSuite::ComparisonStatusFlags flags = compare(_state->actualFormat, _state->actualPixels, *_state->expectedImage);
    if(flags & TestSuite::ComparisonStatusFlag::Failed)
        flags |= TestSuite::ComparisonStatusFlag::Diagnostic;
    return flags;
}

TestSuite::ComparisonStatusFlags ImageComparatorBase::operator()(const ImageView2D& actual, const std::string& expected) {
    return compare(actual.format(), actual.pixels(), expected);
}

TestSuite::ComparisonStatusFlags ImageComparatorBase::operator()(const std::string& actual, const ImageView2D& expected) {
    _state->actualFilename = actual;

    /* Here we are comparing against a view, not a file, so we cannot save
       diagnostic in any case as we don't have the expected filename. This
       behavior is consistent with TestSuite::Compare::FileToString. */

    Containers::Pointer<Trade::AbstractImporter> importer;
    if(!(importer = _state->importerManager().loadAndInstantiate("AnyImageImporter"))) {
        _state->result = Result::PluginLoadFailed;
        return TestSuite::ComparisonStatusFlag::Failed;
    }

    if(!importer->openFile(actual) || !(_state->actualImageData = importer->image2D(0))) {
        _state->result = Result::ActualImageLoadFailed;
        return TestSuite::ComparisonStatusFlag::Failed;
    }

    if(_state->actualImageData->isCompressed()) {
        _state->result = Result::ActualImageIsCompressed;
        return TestSuite::ComparisonStatusFlag::Failed;
    }

    _state->actualFormat = _state->actualImageData->format();
    _state->actualPixels = _state->actualImageData->pixels();
    return compare(_state->actualFormat, _state->actualPixels, expected);
}

void ImageComparatorBase::printMessage(const TestSuite::ComparisonStatusFlags flags, Debug& out, const std::string& actual, const std::string& expected) const {
    if(_state->result == Result::PluginLoadFailed) {
        out << "AnyImageImporter plugin could not be loaded.";
        return;
    }
    if(_state->result == Result::ActualImageLoadFailed) {
        out << "Actual image" << actual << "(" << Debug::nospace << _state->actualFilename << Debug::nospace << ")" << "could not be loaded.";
        return;
    }
    if(_state->result == Result::ExpectedImageLoadFailed) {
        out << "Expected image" << expected << "(" << Debug::nospace << _state->expectedFilename << Debug::nospace << ")" << "could not be loaded.";
        return;
    }
    if(_state->result == Result::ActualImageIsCompressed) {
        out << "Actual image" << actual << "(" << Debug::nospace << _state->actualFilename << Debug::nospace << ")" << "is compressed, comparison not possible.";
        return;
    }
    if(_state->result == Result::ExpectedImageIsCompressed) {
        out << "Expected image" << expected << "(" << Debug::nospace << _state->expectedFilename << Debug::nospace << ")" << "is compressed, comparison not possible.";
        return;
    }

    out << "Images" << actual << "and" << expected << "have";
    if(_state->result == Result::DifferentSize)
        out << "different size, actual"
            << Vector2i{Int(_state->actualPixels.size()[1]),  Int(_state->actualPixels.size()[0])}
            << "but" << _state->expectedImage->size() << "expected.";
    else if(_state->result == Result::DifferentFormat)
        out << "different format, actual" << _state->actualFormat
            << "but" << _state->expectedImage->format() << "expected.";
    else {
        if(_state->result == Result::AboveThresholds)
            out << "both max and mean delta above threshold, actual"
                << _state->max << Debug::nospace << "/" << Debug::nospace << _state->mean
                << "but at most" << _state->maxThreshold << Debug::nospace << "/"
                << Debug::nospace << _state->meanThreshold << "expected.";
        else if(_state->result == Result::AboveMaxThreshold)
            out << "max delta above threshold, actual" << _state->max
                << "but at most" << _state->maxThreshold
                << "expected. Mean delta" << _state->mean << "is within threshold"
                << _state->meanThreshold << Debug::nospace << ".";
        else if(_state->result == Result::AboveMeanThreshold)
            out << "mean delta above threshold, actual" << _state->mean
                << "but at most" << _state->meanThreshold
                << "expected. Max delta" << _state->max << "is within threshold"
                << _state->maxThreshold << Debug::nospace << ".";
        else if(_state->result == Result::VerboseMessage) {
            CORRADE_INTERNAL_ASSERT(flags & TestSuite::ComparisonStatusFlag::Verbose);
            #ifdef CORRADE_NO_ASSERT
            static_cast<void>(flags);
            #endif
            out << "deltas" << _state->max << Debug::nospace << "/"
                << Debug::nospace << _state->mean << "below threshold"
                << _state->maxThreshold << Debug::nospace << "/"
                << Debug::nospace << _state->meanThreshold << Debug::nospace << ".";
        } else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */

        out << "Delta image:" << Debug::newline;
        DebugTools::Implementation::printDeltaImage(out, _state->delta, _state->expectedImage->size(), _state->max, _state->maxThreshold, _state->meanThreshold);
        CORRADE_INTERNAL_ASSERT(_state->actualFormat == _state->expectedImage->format());
        DebugTools::Implementation::printPixelDeltas(out, _state->delta, _state->actualFormat, _state->actualPixels, _state->expectedImage->pixels(), _state->maxThreshold, _state->meanThreshold, 10);
    }
}

void ImageComparatorBase::saveDiagnostic(TestSuite::ComparisonStatusFlags, Utility::Debug& out, const std::string& path) {
    /* Tightly pack the actual pixels into a new array and create an image from
       it -- the array view might have totally arbitrary strides that can't
       be represented in an Image */
    Containers::Array<char> data{_state->actualPixels.size()[0]*_state->actualPixels.size()[1]*_state->actualPixels.size()[2]};
    Containers::StridedArrayView3D<char> pixels{data, _state->actualPixels.size()};
    for(std::size_t i = 0, iMax = _state->actualPixels.size()[0]; i != iMax; ++i) {
        Containers::StridedArrayView2D<const char> inRow = _state->actualPixels[i];
        Containers::StridedArrayView2D<char> outRow = pixels[i];
        for(std::size_t j = 0, jMax = inRow.size()[0]; j != jMax; ++j) {
            Containers::StridedArrayView1D<const char> inPixel = inRow[j];
            Containers::StridedArrayView1D<char> outPixel = outRow[j];
            for(std::size_t k = 0, kMax = inPixel.size(); k != kMax; ++k)
                outPixel[k] = inPixel[k];
        }
    }

    const ImageView2D image{PixelStorage{}.setAlignment(1), _state->actualFormat, Vector2i{Int(pixels.size()[1]), Int(pixels.size()[0])}, data};
    const std::string filename = Utility::Directory::join(path, Utility::Directory::filename(_state->expectedFilename));

    /* Export the data the base view/view comparator saved. Ignore failures,
       we're in the middle of a fail anyway (and everything will print messages
       to the output nevertheless). */
    Containers::Pointer<Trade::AbstractImageConverter> converter = _state->converterManager().loadAndInstantiate("AnyImageConverter");
    if(converter && converter->exportToFile(image, filename))
        out << "->" << filename;
}

}}}
