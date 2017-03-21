/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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
#include <vector>

#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/Math/Functions.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Math/Algorithms/KahanSum.h"

namespace Magnum { namespace DebugTools { namespace Implementation {

namespace {

template<std::size_t size, class T> Math::Vector<size, T> pixelAt(const char* const pixels, const std::size_t stride, const Vector2i& pos) {
    return reinterpret_cast<const Math::Vector<size, T>*>(pixels + stride*pos.y())[pos.x()];
}

template<std::size_t size, class T> Float calculateImageDelta(const ImageView2D& actual, const ImageView2D& expected, std::vector<Float>& output) {
    CORRADE_INTERNAL_ASSERT(output.size() == std::size_t(expected.size().product()));

    /* Precalculate parameters for pixel access */
    Math::Vector2<std::size_t> dataOffset, dataSize;

    std::tie(dataOffset, dataSize, std::ignore) = actual.dataProperties();
    const char* const actualPixels = actual.data() + dataOffset.sum();
    const std::size_t actualStride = dataSize.x();

    std::tie(dataOffset, dataSize, std::ignore) = expected.dataProperties();
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

template<class T> Float calculateIntegerImageDelta(const ImageView2D& actual, const ImageView2D& expected, std::vector<Float>& output) {
    if(
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        expected.format() == PixelFormat::Red
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        || expected.format() == PixelFormat::RedInteger
        #else
        #ifndef MAGNUM_TARGET_WEBGL
        ||
        #endif
        expected.format() == PixelFormat::Luminance
        #endif
        )
        return calculateImageDelta<1, T>(actual, expected, output);
    else if(
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        expected.format() == PixelFormat::RG
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        || expected.format() == PixelFormat::RGInteger
        #else
        #ifndef MAGNUM_TARGET_WEBGL
        ||
        #endif
        expected.format() == PixelFormat::LuminanceAlpha
        #endif
        )
        return calculateImageDelta<2, T>(actual, expected, output);
    else if(expected.format() == PixelFormat::RGB
        #ifndef MAGNUM_TARGET_GLES2
        || expected.format() == PixelFormat::RGBInteger
        #endif
        )
        return calculateImageDelta<3, T>(actual, expected, output);
    else if(expected.format() == PixelFormat::RGBA
        #ifndef MAGNUM_TARGET_GLES2
        || expected.format() == PixelFormat::RGBAInteger
        #endif
        )
        return calculateImageDelta<4, T>(actual, expected, output);

    CORRADE_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
}

template<class T> Float calculateImageDelta(const ImageView2D& actual, const ImageView2D& expected, std::vector<Float>& output) {
    if(
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        expected.format() == PixelFormat::Red
        #endif
        #ifdef MAGNUM_TARGET_GLES2
        #ifndef MAGNUM_TARGET_WEBGL
        ||
        #endif
        expected.format() == PixelFormat::Luminance
        #endif
        )
        return calculateImageDelta<1, T>(actual, expected, output);
    else if(
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        expected.format() == PixelFormat::RG
        #endif
        #ifdef MAGNUM_TARGET_GLES2
        #ifndef MAGNUM_TARGET_WEBGL
        ||
        #endif
        expected.format() == PixelFormat::LuminanceAlpha
        #endif
        )
        return calculateImageDelta<2, T>(actual, expected, output);
    else if(expected.format() == PixelFormat::RGB)
        return calculateImageDelta<3, T>(actual, expected, output);
    else if(expected.format() == PixelFormat::RGBA)
        return calculateImageDelta<4, T>(actual, expected, output);

    CORRADE_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
}

}

std::tuple<std::vector<Float>, Float, Float> calculateImageDelta(const ImageView2D& actual, const ImageView2D& expected) {
    /* Calculate a delta image */
    std::vector<Float> delta(expected.size().product());

    Float max;
    if(expected.type() == PixelType::UnsignedByte)
        max = calculateIntegerImageDelta<UnsignedByte>(actual, expected, delta);
    else if(expected.type() == PixelType::UnsignedShort)
        max = calculateIntegerImageDelta<UnsignedShort>(actual, expected, delta);
    else if(expected.type() == PixelType::UnsignedInt)
        max = calculateIntegerImageDelta<UnsignedInt>(actual, expected, delta);
    #ifndef MAGNUM_TARGET_GLES2
    else if(expected.type() == PixelType::Byte)
        max = calculateIntegerImageDelta<Byte>(actual, expected, delta);
    else if(expected.type() == PixelType::Short)
        max = calculateIntegerImageDelta<Short>(actual, expected, delta);
    else if(expected.type() == PixelType::Int)
        max = calculateIntegerImageDelta<Int>(actual, expected, delta);
    #endif
    else if(expected.type() == PixelType::Float)
        max = calculateImageDelta<Float>(actual, expected, delta);
    else CORRADE_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */

    /* Calculate mean delta. Do it the special way so we don't lose
       precision -- that would result in having false negatives! */
    const Float mean = Math::Algorithms::kahanSum(delta.begin(), delta.end())/delta.size();

    return std::make_tuple(delta, max, mean);
}

namespace {
    /* Done by printing an white to black gradient using one of the online
       ASCII converters. Yes, I'm lazy. Another one could be " .,:;ox%#@". */
    const char Characters[] = " .,:~=+?7IZ$08DNM";
}

void printDeltaImage(Debug& out, const std::vector<Float>& deltas, const Vector2i& size, const Float max, const Float maxThreshold, const Float meanThreshold) {
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

template<class T> void printIntegerPixelAt(Debug& out, const char* const pixels, const std::size_t stride, const Vector2i& pos, const PixelFormat format) {
    if(
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        format == PixelFormat::Red
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        || format == PixelFormat::RedInteger
        #else
        #ifndef MAGNUM_TARGET_WEBGL
        ||
        #endif
        format == PixelFormat::Luminance
        #endif
        )
        out << pixelAt<1, T>(pixels, stride, pos);
    else if(
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        format == PixelFormat::RG
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        || format == PixelFormat::RGInteger
        #else
        #ifndef MAGNUM_TARGET_WEBGL
        ||
        #endif
        format == PixelFormat::LuminanceAlpha
        #endif
        )
        out << pixelAt<2, T>(pixels, stride, pos);
    /* Take the opportunity and print 8-bit colors in hex */
    else if(format == PixelFormat::RGB
        #ifndef MAGNUM_TARGET_GLES2
        || format == PixelFormat::RGBInteger
        #endif
        )
        out << Math::Color3<T>{pixelAt<3, T>(pixels, stride, pos)};
    else if(format == PixelFormat::RGBA
        #ifndef MAGNUM_TARGET_GLES2
        || format == PixelFormat::RGBAInteger
        #endif
        )
        out << Math::Color4<T>{pixelAt<4, T>(pixels, stride, pos)};
    else CORRADE_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
}

template<class T> void printPixelAt(Debug& out, const char* const pixels, const std::size_t stride, const Vector2i& pos, const PixelFormat format) {
    if(
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        format == PixelFormat::Red
        #endif
        #ifdef MAGNUM_TARGET_GLES2
        #ifndef MAGNUM_TARGET_WEBGL
        ||
        #endif
        format == PixelFormat::Luminance
        #endif
        )
        out << pixelAt<1, T>(pixels, stride, pos);
    else if(
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        format == PixelFormat::RG
        #endif
        #ifdef MAGNUM_TARGET_GLES2
        #ifndef MAGNUM_TARGET_WEBGL
        ||
        #endif
        format == PixelFormat::LuminanceAlpha
        #endif
        )
        out << pixelAt<2, T>(pixels, stride, pos);
    else if(format == PixelFormat::RGB)
        out << pixelAt<3, T>(pixels, stride, pos);
    else if(format == PixelFormat::RGBA)
        out << pixelAt<4, T>(pixels, stride, pos);
    else CORRADE_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
}

void printPixelAt(Debug& out, const char* const pixels, const std::size_t stride, const Vector2i& pos, const PixelFormat format, const PixelType type) {
    if(type == PixelType::UnsignedByte)
        printIntegerPixelAt<UnsignedByte>(out, pixels, stride, pos, format);
    else if(type == PixelType::UnsignedShort)
        printIntegerPixelAt<UnsignedShort>(out, pixels, stride, pos, format);
    else if(type == PixelType::UnsignedInt)
        printIntegerPixelAt<UnsignedInt>(out, pixels, stride, pos, format);
    #ifndef MAGNUM_TARGET_GLES2
    else if(type == PixelType::Byte)
        printIntegerPixelAt<Byte>(out, pixels, stride, pos, format);
    else if(type == PixelType::Short)
        printIntegerPixelAt<Short>(out, pixels, stride, pos, format);
    else if(type == PixelType::Int)
        printIntegerPixelAt<Int>(out, pixels, stride, pos, format);
    #endif
    else if(type == PixelType::Float)
        printPixelAt<Float>(out, pixels, stride, pos, format);
    else CORRADE_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
}

}

void printPixelDeltas(Debug& out, const std::vector<Float>& delta, const ImageView2D& actual, const ImageView2D& expected, const Float maxThreshold, const Float meanThreshold, std::size_t maxCount) {
    /* Precalculate parameters for pixel access */
    Math::Vector2<std::size_t> offset, size;

    std::tie(offset, size, std::ignore) = actual.dataProperties();
    const char* const actualPixels = actual.data() + offset.sum();
    const std::size_t actualStride = size.x();

    std::tie(offset, size, std::ignore) = expected.dataProperties();
    const char* const expectedPixels = expected.data() + offset.sum();
    const std::size_t expectedStride = size.x();

    /* Find first maxCount values above mean threshold and put them into a
       sorted map */
    std::multimap<Float, std::size_t> large;
    for(std::size_t i = 0; i != delta.size(); ++i) {
        /* GCC 4.7 std::multimap doesn't have emplace() */
        if(delta[i] > meanThreshold) large.insert({delta[i], i});
    }

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

        printPixelAt(out, actualPixels, actualStride, pos, expected.format(), expected.type());

        out << Debug::nospace << ", expected";

        printPixelAt(out, expectedPixels, expectedStride, pos, expected.format(), expected.type());

        out << "(Δ =" << Debug::boldColor(delta[it->second] > maxThreshold ?
            Debug::Color::Red : Debug::Color::Yellow) << delta[it->second]
            << Debug::nospace << Debug::resetColor << ")";
    }
}

}}}

#ifndef DOXYGEN_GENERATING_OUTPUT
/* If Doxygen sees this, all @ref Corrade::TestSuite links break (prolly
   because the namespace is undocumented in this project) */
namespace Corrade { namespace TestSuite {

using namespace Magnum;

Comparator<DebugTools::CompareImage>::Comparator(Float maxThreshold, Float meanThreshold): _maxThreshold{maxThreshold}, _meanThreshold{meanThreshold} {
    CORRADE_ASSERT(meanThreshold <= maxThreshold,
        "DebugTools::CompareImage: maxThreshold can't be smaller than meanThreshold", );
}

bool Comparator<DebugTools::CompareImage>::operator()(const ImageView2D& actual, const ImageView2D& expected) {
    _actualImage = &actual;
    _expectedImage = &expected;

    /* Verify that the images are the same */
    if(actual.size() != expected.size()) {
        _state = State::DifferentSize;
        return false;
    }
    if(actual.format() != expected.format() || actual.type() != expected.type()) {
        _state = State::DifferentFormat;
        return false;
    }

    /* Assert on unsupported format/storage */
    #ifndef CORRADE_NO_DEBUG
    const bool formatSupported = (
        (
            #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
            expected.format() == PixelFormat::Red ||
            expected.format() == PixelFormat::RG ||
            #endif
            #ifndef MAGNUM_TARGET_GLES2
            expected.format() == PixelFormat::RedInteger ||
            expected.format() == PixelFormat::RGInteger ||
            expected.format() == PixelFormat::RGBInteger ||
            expected.format() == PixelFormat::RGBAInteger ||
            #else
            expected.format() == PixelFormat::Luminance ||
            expected.format() == PixelFormat::LuminanceAlpha ||
            #endif
            expected.format() == PixelFormat::RGB ||
            expected.format() == PixelFormat::RGBA
        ) && (
            #ifndef MAGNUM_TARGET_GLES2
            expected.type() == PixelType::Byte ||
            expected.type() == PixelType::Short ||
            expected.type() == PixelType::Int ||
            #endif
            expected.type() == PixelType::UnsignedByte ||
            expected.type() == PixelType::UnsignedShort ||
            expected.type() == PixelType::UnsignedInt
        )) || ((
            #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
            expected.format() == PixelFormat::Red ||
            expected.format() == PixelFormat::RG ||
            #endif
            #ifdef MAGNUM_TARGET_GLES2
            expected.format() == PixelFormat::Luminance ||
            expected.format() == PixelFormat::LuminanceAlpha ||
            #endif
            expected.format() == PixelFormat::RGB ||
            expected.format() == PixelFormat::RGBA
        ) && expected.type() == PixelType::Float);
    CORRADE_ASSERT(
        formatSupported,
        "DebugTools::CompareImage: format" << expected.format() << Debug::nospace << "/" << expected.type() << "is not supported", {});
    #endif
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_ASSERT(!actual.storage().swapBytes() && !expected.storage().swapBytes(),
        "DebugTools::CompareImage: pixel storage with byte swap is not supported", {});
    #endif

    std::vector<Float> delta;
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

void Comparator<DebugTools::CompareImage>::printErrorMessage(Debug& out, const std::string& actual, const std::string& expected) const {
    out << "Images" << actual << "and" << expected << "have";
    if(_state == State::DifferentSize)
        out << "different size, actual" << _actualImage->size() << "but"
            << _expectedImage->size() << "expected.";
    else if(_state == State::DifferentFormat)
        out << "different format, actual" << _actualImage->format()
            << Debug::nospace << "/" << Debug::nospace << _actualImage->type()
            << "but" << _expectedImage->format() << Debug::nospace << "/"
            << Debug::nospace << _expectedImage->type() << "expected.";
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

}}
#endif
