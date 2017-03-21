#ifndef Magnum_DebugTools_CompareImage_h
#define Magnum_DebugTools_CompareImage_h
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

/** @file
 * @brief Class @ref Magnum::DebugTools::CompareImage
 */

#include <vector>
#include <Corrade/TestSuite/Comparator.h>

#include "Magnum/Magnum.h"
#include "Magnum/Math/Vector2.h"
#include "Magnum/DebugTools/visibility.h"

namespace Magnum { namespace DebugTools {

namespace Implementation {
    MAGNUM_DEBUGTOOLS_EXPORT std::tuple<std::vector<Float>, Float, Float> calculateImageDelta(const ImageView2D& actual, const ImageView2D& expected);

    MAGNUM_DEBUGTOOLS_EXPORT void printDeltaImage(Debug& out, const std::vector<Float>& delta, const Vector2i& size, Float max, Float maxThreshold, Float meanThreshold);

    MAGNUM_DEBUGTOOLS_EXPORT void printPixelDeltas(Debug& out, const std::vector<Float>& delta, const ImageView2D& actual, const ImageView2D& expected, Float maxThreshold, Float meanThreshold, std::size_t maxCount);
}

class CompareImage;

}}

#ifndef DOXYGEN_GENERATING_OUTPUT
/* If Doxygen sees this, all @ref Corrade::TestSuite links break (prolly
   because the namespace is undocumented in this project) */
namespace Corrade { namespace TestSuite {

template<> class MAGNUM_DEBUGTOOLS_EXPORT Comparator<Magnum::DebugTools::CompareImage> {
    public:
        explicit Comparator(Magnum::Float maxThreshold, Magnum::Float meanThreshold);

        /*implicit*/ Comparator(): Comparator{0.0f, 0.0f} {}

        bool operator()(const Magnum::ImageView2D& actual, const Magnum::ImageView2D& expected);

        void printErrorMessage(Utility::Debug& out, const std::string& actual, const std::string& expected) const;

    private:
        enum class State {
            DifferentSize = 1,
            DifferentFormat,
            AboveThresholds,
            AboveMeanThreshold,
            AboveMaxThreshold
        };

        Magnum::Float _maxThreshold, _meanThreshold;

        State _state{};
        const Magnum::ImageView2D *_actualImage, *_expectedImage;
        Magnum::Float _max, _mean;
        std::vector<Magnum::Float> _delta;
};

}}
#endif

namespace Magnum { namespace DebugTools {

/**
@brief Image comparator

To be used with @ref Corrade::TestSuite. Basic use is really simple:

@snippet debugtools-compareimage.cpp 0

Based on actual images used, in case of commparison failure the comparator can
give for example the following result:

@image html debugtools-compareimage.png

Supports the following formats:

-   @ref PixelFormat::Red, @ref PixelFormat::RedInteger, @ref PixelFormat::RG,
    @ref PixelFormat::RGInteger, @ref PixelFormat::RGB, @ref PixelFormat::RGBInteger,
    @ref PixelFormat::RGBA and @ref PixelFormat::RGBAInteger with
    @ref PixelType::UnsignedByte, @ref PixelType::Byte, @ref PixelType::UnsignedShort,
    @ref PixelType::Short, @ref PixelType::UnsignedInt and @ref PixelType::Int
-   @ref PixelFormat::Red, @ref PixelFormat::RG, @ref PixelFormat::RGB and
    @ref PixelFormat::RGBA with @ref PixelType::Float

In OpenGL ES 2.0 and WebGL 1.0, @ref PixelFormat::Luminance and
@ref PixelFormat::LuminanceAlpha are also accepted in place of
@ref PixelFormat::Red and @ref PixelFormat::RG.

Supports all @ref PixelStorage parameters *except* non-default
@ref PixelStorage::swapBytes() values. The images don't need to have the same
pixel storage parameters, meaning you are able to compare different subimages
of a larger image as long as they have the same size.

The comparator first compares both images to have the same pixel format/type
combination and size. Each pixel is then first converted to @ref Magnum::Float "Float"
vector of corresponding channel count and then the per-pixel delta is
calculated as simple sum of per-channel deltas (where @f$ \boldsymbol{a} @f$ is
the actual pixel value, @f$ \boldsymbol{e} @f$ expected pixel value and @f$ c @f$
is channel count), with max and mean delta being taken over the whole picture. @f[

    \Delta_{\boldsymbol{p}} = \sum\limits_{i=1}^c \dfrac{a_i - e_i}{c}

@f]

The two parameters passed to the @ref CompareImage(Float, Float) "CompareImage(Float, Float)"
constructor are max and mean delta threshold. If the calculated values are
above these threshold, the comparison fails. In case of comparison failure the
diagnostic output contains calculated max/meanvalues, delta image visualization
and a list of top deltas. The delta image is an ASCII-art representation of the
image difference with each block being a maximum of pixel deltas in some area,
printed as characters of different perceived brightness. Blocks with delta over
the max threshold are colored red, blocks with delta over the mean threshold
are colored yellow. The delta list contains X,Y pixel position (with origin at
bottom left), actual and expected pixel value and calculated delta.
*/
class CompareImage {
    public:
        /**
         * @brief Constructor
         * @param maxThreshold  Max threshold. If any pixel has delta above
         *      this value, this comparison fails
         * @param meanThreshold Mean threshold. If mean delta over all pixels
         *      is above this value, the comparison fails
         */
        explicit CompareImage(Float maxThreshold, Float meanThreshold): _c{maxThreshold, meanThreshold} {}

        /**
         * @brief Implicit constructor
         *
         * Equivalent to calling the above with zero values.
         */
        explicit CompareImage(): CompareImage{0.0f, 0.0f} {}

        #ifndef DOXYGEN_GENERATING_OUTPUT
        Corrade::TestSuite::Comparator<CompareImage>& comparator() {
            return _c;
        }
        #endif

    private:
        Corrade::TestSuite::Comparator<CompareImage> _c;
};

}}

#endif
