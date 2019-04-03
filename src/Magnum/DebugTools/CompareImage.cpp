/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
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

#include "CompareImage.h"

#include <map>
#include <sstream>
#include <Corrade/Containers/Optional.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/Math/Functions.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Math/Algorithms/KahanSum.h"
#include "Magnum/Trade/AbstractImporter.h"
#include "Magnum/Trade/ImageData.h"

namespace Magnum { namespace DebugTools { namespace Implementation {

namespace {

template<std::size_t size, class T> Math::Vector<size, T> pixelAt(const char* const pixels, const std::size_t stride, const Vector2i& pos) {
    return reinterpret_cast<const Math::Vector<size, T>*>(pixels + stride*pos.y())[pos.x()];
}

template<std::size_t size, class T> Float calculateImageDelta(const ImageView2D& actual, const ImageView2D& expected, Containers::ArrayView<Float> output) {
    CORRADE_INTERNAL_ASSERT(output.size() == std::size_t(expected.size().product()));

    /* Precalculate parameters for pixel access */
    Math::Vector2<std::size_t> dataOffset, dataSize;

    std::tie(dataOffset, dataSize) = actual.dataProperties();
    const char* const actualPixels = actual.data() + dataOffset.sum();
    const std::size_t actualStride = dataSize.x();

    std::tie(dataOffset, dataSize) = expected.dataProperties();
    const char* const expectedPixels = expected.data() + dataOffset.sum();
    const std::size_t expectedStride = dataSize.x();

    /* Calculate deltas and maximal value of them */
    Float max{};
    for(std::int_fast32_t y = 0; y != expected.size().y(); ++y) {
        for(std::int_fast32_t x = 0; x != expected.size().x(); ++x) {
            Math::Vector<size, Float> actualPixel{pixelAt<size, T>(actualPixels, actualStride, {Int(x), Int(y)})};
            Math::Vector<size, Float> expectedPixel{pixelAt<size, T>(expectedPixels, expectedStride, {Int(x), Int(y)})};

            const Float value = (Math::abs(actualPixel - expectedPixel)).sum()/size;
            output[y*expected.size().x() + x] = value;
            max = Math::max(max, value);
        }
    }

    return max;
}

}

std::tuple<Containers::Array<Float>, Float, Float> calculateImageDelta(const ImageView2D& actual, const ImageView2D& expected) {
    /* Calculate a delta image */
    Containers::Array<Float> delta{Containers::NoInit, std::size_t(expected.size().product())};

    CORRADE_ASSERT(!isPixelFormatImplementationSpecific(expected.format()),
        "DebugTools::CompareImage: can't compare implementation-specific pixel formats", {});

    Float max{Constants::nan()};
    switch(expected.format()) {
        #define _c(format, size, T)                                         \
            case PixelFormat::format:                                       \
                max = calculateImageDelta<size, T>(actual, expected, delta); \
                break;
        #define _d(first, second, size, T)                                  \
            case PixelFormat::first:                                        \
            case PixelFormat::second:                                       \
                max = calculateImageDelta<size, T>(actual, expected, delta); \
                break;
        /* LCOV_EXCL_START */
        _d(R8Unorm, R8UI, 1, UnsignedByte)
        _d(RG8Unorm, RG8UI, 2, UnsignedByte)
        _d(RGB8Unorm, RGB8UI, 3, UnsignedByte)
        _d(RGBA8Unorm, RGBA8UI, 4, UnsignedByte)
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
        #undef _d
        #undef _c

        case PixelFormat::R16F:
        case PixelFormat::RG16F:
        case PixelFormat::RGB16F:
        case PixelFormat::RGBA16F:
            CORRADE_ASSERT(false,
                "DebugTools::CompareImage: half-float formats are not supported yet", {});
    }

    CORRADE_ASSERT(max == max,
        "DebugTools::CompareImage: unknown format" << expected.format(), {});

    /* Calculate mean delta. Do it the special way so we don't lose
       precision -- that would result in having false negatives! */
    const Float mean = Math::Algorithms::kahanSum(delta.begin(), delta.end())/delta.size();

    return std::make_tuple(std::move(delta), max, mean);
}

namespace {
    /* Done by printing an white to black gradient using one of the online
       ASCII converters. Yes, I'm lazy. Another one could be " .,:;ox%#@". */
    const char Characters[] = " .,:~=+?7IZ$08DNM";
}

void printDeltaImage(Debug& out, Containers::ArrayView<const Float> deltas, const Vector2i& size, const Float max, const Float maxThreshold, const Float meanThreshold) {
    CORRADE_INTERNAL_ASSERT(meanThreshold <= maxThreshold);

    /* At most 64 characters per line. The console fonts height is usually 2x
       the width, so there is twice the pixels per block */
    const Vector2i pixelsPerBlock{(size.x() + 63)/64, 2*((size.x() + 63)/64)};
    const Vector2i blockCount = (size + pixelsPerBlock - Vector2i{1})/pixelsPerBlock;

    for(std::int_fast32_t y = 0; y != blockCount.y(); ++y) {
        out << "          |";

        for(std::int_fast32_t x = 0; x != blockCount.x(); ++x) {
            /* Going bottom-up so we don't flip the image upside down when printing */
            const Vector2i offset = Vector2i{Int(x), blockCount.y() - Int(y) - 1}*pixelsPerBlock;
            const Vector2i blockSize = Math::min(size - offset, Vector2i{pixelsPerBlock});

            Float blockMax{};
            for(std::int_fast32_t yb = 0; yb != blockSize.y(); ++yb)
                for(std::int_fast32_t xb = 0; xb != blockSize.x(); ++xb)
                    blockMax = Math::max(blockMax, deltas[(offset.y() + yb)*size.x() + offset.x() + xb]);

            const char c = Characters[Int(Math::round(Math::min(blockMax/max, 1.0f)*(sizeof(Characters) - 2)))];

            if(blockMax > maxThreshold)
                out << Debug::boldColor(Debug::Color::Red) << Debug::nospace << std::string{c} << Debug::resetColor;
            else if(blockMax > meanThreshold)
                out << Debug::boldColor(Debug::Color::Yellow) << Debug::nospace << std::string{c} << Debug::resetColor;
            else out << Debug::nospace << std::string{c};
        }

        out << Debug::nospace << "|" << Debug::newline;
    }
}

namespace {

void printPixelAt(Debug& out, const char* const pixels, const std::size_t stride, const Vector2i& pos, const PixelFormat format) {
    switch(format) {
        #define _c(format, size, T)                                         \
            case PixelFormat::format:                                       \
                out << pixelAt<size, T>(pixels, stride, pos);               \
                break;
        #define _d(first, second, size, T)                                  \
            case PixelFormat::first:                                        \
            case PixelFormat::second:                                       \
                out << pixelAt<size, T>(pixels, stride, pos);               \
                break;
        /* LCOV_EXCL_START */
        _d(R8Unorm, R8UI, 1, UnsignedByte)
        _d(RG8Unorm, RG8UI, 2, UnsignedByte)
        _c(RGB8UI, 3, UnsignedByte)
        _c(RGBA8UI, 4, UnsignedByte)
        /* RGB8Unorm, RGBA8Unorm handled below */
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
        #undef _d
        #undef _c

        /* Take the opportunity and print 8-bit colors in hex */
        case PixelFormat::RGB8Unorm:
            out << Color3ub{pixelAt<3, UnsignedByte>(pixels, stride, pos)};
            break;
        case PixelFormat::RGBA8Unorm:
            out << Color4ub{pixelAt<4, UnsignedByte>(pixels, stride, pos)};
            break;

        case PixelFormat::R16F:
        case PixelFormat::RG16F:
        case PixelFormat::RGB16F:
        case PixelFormat::RGBA16F:
            /* Already handled by a printing assert before */
            CORRADE_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
    }
}

}

void printPixelDeltas(Debug& out, Containers::ArrayView<const Float> delta, const ImageView2D& actual, const ImageView2D& expected, const Float maxThreshold, const Float meanThreshold, std::size_t maxCount) {
    /* Precalculate parameters for pixel access */
    Math::Vector2<std::size_t> offset, size;

    std::tie(offset, size) = actual.dataProperties();
    const char* const actualPixels = actual.data() + offset.sum();
    const std::size_t actualStride = size.x();

    std::tie(offset, size) = expected.dataProperties();
    const char* const expectedPixels = expected.data() + offset.sum();
    const std::size_t expectedStride = size.x();

    /* Find first maxCount values above mean threshold and put them into a
       sorted map */
    std::multimap<Float, std::size_t> large;
    for(std::size_t i = 0; i != delta.size(); ++i)
        if(delta[i] > meanThreshold) large.emplace(delta[i], i);

    CORRADE_INTERNAL_ASSERT(!large.empty());

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
        std::tie(pos.y(), pos.x()) = Math::div(Int(it->second), expected.size().x());
        out << Debug::newline << "          [" << Debug::nospace << pos.x()
            << Debug::nospace << "," << Debug::nospace << pos.y()
            << Debug::nospace << "]";

        printPixelAt(out, actualPixels, actualStride, pos, expected.format());

        out << Debug::nospace << ", expected";

        printPixelAt(out, expectedPixels, expectedStride, pos, expected.format());

        out << "(Δ =" << Debug::boldColor(delta[it->second] > maxThreshold ?
            Debug::Color::Red : Debug::Color::Yellow) << delta[it->second]
            << Debug::nospace << Debug::resetColor << ")";
    }
}

enum class ImageComparatorBase::State: UnsignedByte {
    PluginLoadFailed = 1,
    ActualImageLoadFailed,
    ExpectedImageLoadFailed,
    ActualImageIsCompressed,
    ExpectedImageIsCompressed,
    DifferentSize,
    DifferentFormat,
    AboveThresholds,
    AboveMeanThreshold,
    AboveMaxThreshold
};

class ImageComparatorBase::FileState {
    public:
        explicit FileState(PluginManager::Manager<Trade::AbstractImporter>& manager): manager{&manager} {}

        explicit FileState(): _privateManager{Containers::InPlaceInit}, manager{&*_privateManager} {}

    private:
        Containers::Optional<PluginManager::Manager<Trade::AbstractImporter>> _privateManager;

    public:
        PluginManager::Manager<Trade::AbstractImporter>* manager;
        std::string actualFilename, expectedFilename;
        Containers::Optional<Trade::ImageData2D> actualImageData, expectedImageData;
        /** @todo could at least the views have a NoCreate constructor? */
        Containers::Optional<ImageView2D> actualImage, expectedImage;
};

ImageComparatorBase::ImageComparatorBase(PluginManager::Manager<Trade::AbstractImporter>* manager, Float maxThreshold, Float meanThreshold): _maxThreshold{maxThreshold}, _meanThreshold{meanThreshold}, _max{}, _mean{} {
    if(manager) _fileState.reset(new FileState{*manager});

    CORRADE_ASSERT(meanThreshold <= maxThreshold,
        "DebugTools::CompareImage: maxThreshold can't be smaller than meanThreshold", );
}

ImageComparatorBase::~ImageComparatorBase() = default;

bool ImageComparatorBase::operator()(const ImageView2D& actual, const ImageView2D& expected) {
    _actualImage = &actual;
    _expectedImage = &expected;

    /* Verify that the images are the same */
    if(actual.size() != expected.size()) {
        _state = State::DifferentSize;
        return false;
    }
    if(actual.format() != expected.format()) {
        _state = State::DifferentFormat;
        return false;
    }

    Containers::Array<Float> delta;
    std::tie(delta, _max, _mean) = DebugTools::Implementation::calculateImageDelta(actual, expected);

    /* If both values are not above threshold, success */
    if(_max > _maxThreshold && _mean > _meanThreshold)
        _state = State::AboveThresholds;
    else if(_max > _maxThreshold)
        _state = State::AboveMaxThreshold;
    else if(_mean > _meanThreshold)
        _state = State::AboveMeanThreshold;
    else return true;

    /* Otherwise save the deltas and fail */
    _delta = std::move(delta);
    return false;
}

bool ImageComparatorBase::operator()(const std::string& actual, const std::string& expected) {
    if(!_fileState) _fileState.reset(new FileState);

    _fileState->actualFilename = actual;
    _fileState->expectedFilename = expected;

    Containers::Pointer<Trade::AbstractImporter> importer;
    if(!(importer = _fileState->manager->loadAndInstantiate("AnyImageImporter"))) {
        _state = State::PluginLoadFailed;
        return false;
    }

    if(!importer->openFile(actual) || !(_fileState->actualImageData = importer->image2D(0))) {
        _state = State::ActualImageLoadFailed;
        return false;
    }

    if(!importer->openFile(expected) || !(_fileState->expectedImageData = importer->image2D(0))) {
        _state = State::ExpectedImageLoadFailed;
        return false;
    }

    if(_fileState->actualImageData->isCompressed()) {
        _state = State::ActualImageIsCompressed;
        return false;
    }

    if(_fileState->expectedImageData->isCompressed()) {
        _state = State::ExpectedImageIsCompressed;
        return false;
    }

    _fileState->actualImage.emplace(*_fileState->actualImageData);
    _fileState->expectedImage.emplace(*_fileState->expectedImageData);
    return operator()(*_fileState->actualImage, *_fileState->expectedImage);
}

bool ImageComparatorBase::operator()(const ImageView2D& actual, const std::string& expected) {
    if(!_fileState) _fileState.reset(new FileState);

    _fileState->expectedFilename = expected;

    Containers::Pointer<Trade::AbstractImporter> importer;
    if(!(importer = _fileState->manager->loadAndInstantiate("AnyImageImporter"))) {
        _state = State::PluginLoadFailed;
        return false;
    }

    if(!importer->openFile(expected) || !(_fileState->expectedImageData = importer->image2D(0))) {
        _state = State::ExpectedImageLoadFailed;
        return false;
    }

    if(_fileState->expectedImageData->isCompressed()) {
        _state = State::ExpectedImageIsCompressed;
        return false;
    }

    _fileState->expectedImage.emplace(*_fileState->expectedImageData);
    return operator()(actual, *_fileState->expectedImage);
}

bool ImageComparatorBase::operator()(const std::string& actual, const ImageView2D& expected) {
    if(!_fileState) _fileState.reset(new FileState);

    _fileState->actualFilename = actual;

    Containers::Pointer<Trade::AbstractImporter> importer;
    if(!(importer = _fileState->manager->loadAndInstantiate("AnyImageImporter"))) {
        _state = State::PluginLoadFailed;
        return false;
    }

    if(!importer->openFile(actual) || !(_fileState->actualImageData = importer->image2D(0))) {
        _state = State::ActualImageLoadFailed;
        return false;
    }

    if(_fileState->actualImageData->isCompressed()) {
        _state = State::ActualImageIsCompressed;
        return false;
    }

    _fileState->actualImage.emplace(*_fileState->actualImageData);
    return operator()(*_fileState->actualImage, expected);
}

void ImageComparatorBase::printErrorMessage(Debug& out, const std::string& actual, const std::string& expected) const {
    if(_state == State::PluginLoadFailed) {
        out << "AnyImageImporter plugin could not be loaded.";
        return;
    }
    if(_state == State::ActualImageLoadFailed) {
        out << "Actual image" << actual << "(" << Debug::nospace << _fileState->actualFilename << Debug::nospace << ")" << "could not be loaded.";
        return;
    }
    if(_state == State::ExpectedImageLoadFailed) {
        out << "Expected image" << expected << "(" << Debug::nospace << _fileState->expectedFilename << Debug::nospace << ")" << "could not be loaded.";
        return;
    }
    if(_state == State::ActualImageIsCompressed) {
        out << "Actual image" << actual << "(" << Debug::nospace << _fileState->actualFilename << Debug::nospace << ")" << "is compressed, comparison not possible.";
        return;
    }
    if(_state == State::ExpectedImageIsCompressed) {
        out << "Expected image" << expected << "(" << Debug::nospace << _fileState->expectedFilename << Debug::nospace << ")" << "is compressed, comparison not possible.";
        return;
    }

    out << "Images" << actual << "and" << expected << "have";
    if(_state == State::DifferentSize)
        out << "different size, actual" << _actualImage->size() << "but"
            << _expectedImage->size() << "expected.";
    else if(_state == State::DifferentFormat)
        out << "different format, actual" << _actualImage->format() << "but"
            << _expectedImage->format() << "expected.";
    else {
        if(_state == State::AboveThresholds)
            out << "both max and mean delta above threshold, actual"
                << _max << Debug::nospace << "/" << Debug::nospace << _mean
                << "but at most" << _maxThreshold << Debug::nospace << "/"
                << Debug::nospace << _meanThreshold << "expected.";
        else if(_state == State::AboveMaxThreshold)
            out << "max delta above threshold, actual" << _max
                << "but at most" << _maxThreshold
                << "expected. Mean delta" << _mean << "is below threshold"
                << _meanThreshold << Debug::nospace << ".";
        else if(_state == State::AboveMeanThreshold)
            out << "mean delta above threshold, actual" << _mean
                << "but at most" << _meanThreshold
                << "expected. Max delta" << _max << "is below threshold"
                << _maxThreshold << Debug::nospace << ".";
        else CORRADE_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */

        out << "Delta image:" << Debug::newline;
        DebugTools::Implementation::printDeltaImage(out, _delta, _expectedImage->size(), _max, _maxThreshold, _meanThreshold);
        DebugTools::Implementation::printPixelDeltas(out, _delta, *_actualImage, *_expectedImage, _maxThreshold, _meanThreshold, 10);
    }
}

}}}
