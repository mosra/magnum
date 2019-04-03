#ifndef Magnum_DebugTools_CompareImage_h
#define Magnum_DebugTools_CompareImage_h
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

/** @file
 * @brief Class @ref Magnum::DebugTools::CompareImage
 */

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/Pointer.h>
#include <Corrade/PluginManager/PluginManager.h>
#include <Corrade/TestSuite/Comparator.h>
#include <Corrade/Utility/StlForwardString.h>
#include <Corrade/Utility/StlForwardTuple.h>

#include "Magnum/Magnum.h"
#include "Magnum/Math/Vector2.h"
#include "Magnum/DebugTools/visibility.h"
#include "Magnum/Trade/Trade.h"

namespace Magnum { namespace DebugTools {

namespace Implementation {
    MAGNUM_DEBUGTOOLS_EXPORT std::tuple<Containers::Array<Float>, Float, Float> calculateImageDelta(const ImageView2D& actual, const ImageView2D& expected);

    MAGNUM_DEBUGTOOLS_EXPORT void printDeltaImage(Debug& out, Containers::ArrayView<const Float> delta, const Vector2i& size, Float max, Float maxThreshold, Float meanThreshold);

    MAGNUM_DEBUGTOOLS_EXPORT void printPixelDeltas(Debug& out, Containers::ArrayView<const Float> delta, const ImageView2D& actual, const ImageView2D& expected, Float maxThreshold, Float meanThreshold, std::size_t maxCount);
}

class CompareImage;
class CompareImageFile;
class CompareImageToFile;
class CompareFileToImage;

namespace Implementation {

class MAGNUM_DEBUGTOOLS_EXPORT ImageComparatorBase {
    public:
        explicit ImageComparatorBase(PluginManager::Manager<Trade::AbstractImporter>* manager, Float maxThreshold, Float meanThreshold);

        /*implicit*/ ImageComparatorBase(): ImageComparatorBase{nullptr, 0.0f, 0.0f} {}

        ~ImageComparatorBase();

        bool operator()(const ImageView2D& actual, const ImageView2D& expected);

        bool operator()(const std::string& actual, const std::string& expected);

        bool operator()(const std::string& actual, const ImageView2D& expected);

        bool operator()(const ImageView2D& actual, const std::string& expected);

        void printErrorMessage(Debug& out, const std::string& actual, const std::string& expected) const;

    private:
        class MAGNUM_DEBUGTOOLS_LOCAL FileState;

        enum class State: UnsignedByte;

        Containers::Pointer<FileState> _fileState;
        Float _maxThreshold, _meanThreshold;

        State _state{};
        const ImageView2D *_actualImage, *_expectedImage;
        Float _max, _mean;
        Containers::Array<Float> _delta;
};

}}}

#ifndef DOXYGEN_GENERATING_OUTPUT
/* If Doxygen sees this, all @ref Corrade::TestSuite links break (prolly
   because the namespace is undocumented in this project) */
namespace Corrade { namespace TestSuite {

template<> class MAGNUM_DEBUGTOOLS_EXPORT Comparator<Magnum::DebugTools::CompareImage>: public Magnum::DebugTools::Implementation::ImageComparatorBase {
    public:
        explicit Comparator(Magnum::Float maxThreshold, Magnum::Float meanThreshold): Magnum::DebugTools::Implementation::ImageComparatorBase{nullptr, maxThreshold, meanThreshold} {}

        /*implicit*/ Comparator(): Comparator{0.0f, 0.0f} {}

        bool operator()(const Magnum::ImageView2D& actual, const Magnum::ImageView2D& expected) {
            return Magnum::DebugTools::Implementation::ImageComparatorBase::operator()(actual, expected);
        }
};

template<> class MAGNUM_DEBUGTOOLS_EXPORT Comparator<Magnum::DebugTools::CompareImageFile>: public Magnum::DebugTools::Implementation::ImageComparatorBase {
    public:
        explicit Comparator(PluginManager::Manager<Magnum::Trade::AbstractImporter>* manager, Magnum::Float maxThreshold, Magnum::Float meanThreshold): Magnum::DebugTools::Implementation::ImageComparatorBase{manager, maxThreshold, meanThreshold} {}

        /*implicit*/ Comparator(): Comparator{nullptr, 0.0f, 0.0f} {}

        bool operator()(const std::string& actual, const std::string& expected) {
            return Magnum::DebugTools::Implementation::ImageComparatorBase::operator()(actual, expected);
        }
};

template<> class MAGNUM_DEBUGTOOLS_EXPORT Comparator<Magnum::DebugTools::CompareImageToFile>: public Magnum::DebugTools::Implementation::ImageComparatorBase {
    public:
        explicit Comparator(PluginManager::Manager<Magnum::Trade::AbstractImporter>* manager, Magnum::Float maxThreshold, Magnum::Float meanThreshold): Magnum::DebugTools::Implementation::ImageComparatorBase{manager, maxThreshold, meanThreshold} {}

        /*implicit*/ Comparator(): Comparator{nullptr, 0.0f, 0.0f} {}

        bool operator()(const Magnum::ImageView2D& actual, const std::string& expected) {
            return Magnum::DebugTools::Implementation::ImageComparatorBase::operator()(actual, expected);
        }
};

template<> class MAGNUM_DEBUGTOOLS_EXPORT Comparator<Magnum::DebugTools::CompareFileToImage>: public Magnum::DebugTools::Implementation::ImageComparatorBase {
    public:
        explicit Comparator(PluginManager::Manager<Magnum::Trade::AbstractImporter>* manager, Magnum::Float maxThreshold, Magnum::Float meanThreshold): Magnum::DebugTools::Implementation::ImageComparatorBase{manager, maxThreshold, meanThreshold} {}

        /*implicit*/ Comparator(): Comparator{nullptr, 0.0f, 0.0f} {}

        bool operator()(const std::string& actual, const Magnum::ImageView2D& expected) {
            return Magnum::DebugTools::Implementation::ImageComparatorBase::operator()(actual, expected);
        }
};

}}
#endif

namespace Magnum { namespace DebugTools {

/**
@brief Image comparator

To be used with @ref Corrade::TestSuite.

@note This class is available only if Magnum is compiled with `WITH_TRADE`
    enabled (done by default). See @ref building-features for more information.

Basic use is really simple:

@snippet debugtools-compareimage.cpp 0

@note For comparing image files, there are also @ref CompareImageFile,
    @ref CompareImageToFile and @ref CompareFileToImage variants.

Based on actual images used, in case of commparison failure the comparator can
give for example the following result:

@include debugtools-compareimage.ansi

Supports the following formats:

-   @ref PixelFormat::RGBA8Unorm, @ref PixelFormat::RGBA16Unorm and their
    one-/two-/three-component versions
-   @ref PixelFormat::RGBA8Snorm, @ref PixelFormat::RGBA16Snorm and their
    one-/two-/three-component versions
-   @ref PixelFormat::RGBA8UI, @ref PixelFormat::RGBA16UI,
    @ref PixelFormat::RGBA32UI and their one-/two-/three-component versions
-   @ref PixelFormat::RGBA8I, @ref PixelFormat::RGBA16I,
    @ref PixelFormat::RGBA32I and their one-/two-/three-component versions
-   @ref PixelFormat::RGBA32F and its one-/two-/three-component versions

@ref PixelFormat::RGBA16F and other half-float formats are not supported at the
moment. Implementation-specific pixel formats can't be supported.

Supports all @ref PixelStorage parameters. The images don't need to have the
same pixel storage parameters, meaning you are able to compare different
subimages of a larger image as long as they have the same size.

The comparator first compares both images to have the same pixel format/type
combination and size. Each pixel is then first converted to @ref Magnum::Float "Float"
vector of corresponding channel count and then the per-pixel delta is
calculated as simple sum of per-channel deltas (where @f$ \boldsymbol{a} @f$ is
the actual pixel value, @f$ \boldsymbol{e} @f$ expected pixel value and @f$ c @f$
is channel count), with max and mean delta being taken over the whole picture. @f[

    \Delta_{\boldsymbol{p}} = \sum\limits_{i=1}^c \dfrac{a_i - e_i}{c}

@f]

The two parameters passed to the @ref CompareImage(Float, Float) constructor
are max and mean delta threshold. If the calculated values are above these
threshold, the comparison fails. In case of comparison failure the diagnostic
output contains calculated max/meanvalues, delta image visualization and a list
of top deltas. The delta image is an ASCII-art representation of the image
difference with each block being a maximum of pixel deltas in some area,
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
         * @brief Construct with implicit thresholds
         *
         * Equivalent to calling @ref CompareImage(Float, Float) with zero
         * values.
         */
        explicit CompareImage(): _c{0.0f, 0.0f} {}

        #ifndef DOXYGEN_GENERATING_OUTPUT
        TestSuite::Comparator<CompareImage>& comparator() {
            return _c;
        }
        #endif

    private:
        TestSuite::Comparator<CompareImage> _c;
};

/**
@brief Image file comparator

Similar to @ref CompareImage, but comparing images loaded from files. Example
usage:

@snippet MagnumDebugTools.cpp CompareImageFile

By default, the comparator uses a local instance of
@ref Corrade::PluginManager::Manager to load image files. This might be
problematic if the code being tested also uses a plugin manager instance or if
you need to use a different plugin directory, for example. For such cases it's
possible to supply an external instance:

@snippet MagnumDebugTools.cpp CompareImageFile-manager

The comparator uses the @ref Trade::AnyImageImporter "AnyImageImporter" plugin,
which in turn delegates the import to some importer plugin matching the image
format(s). Both the @ref Trade::AnyImageImporter "AnyImageImporter" and the
concrete importer plugin(s) need to be available, otherwise the comparison
fails. An alternative way is manually skipping the test if the plugins are not
available:

@snippet MagnumDebugTools.cpp CompareImageFile-skip

See also @ref CompareImageToFile and @ref CompareFileToImage for comparing
in-memory images to image files and vice versa.
*/
class CompareImageFile {
    public:
        /**
         * @brief Constructor
         * @param maxThreshold  Max threshold. If any pixel has delta above
         *      this value, this comparison fails
         * @param meanThreshold Mean threshold. If mean delta over all pixels
         *      is above this value, the comparison fails
         */
        explicit CompareImageFile(Float maxThreshold, Float meanThreshold): _c{nullptr, maxThreshold, meanThreshold} {}

        /**
         * @brief Construct with an explicit plugin manager instance
         * @param manager       Image importer plugin manager instance used
         *      when comparing against images loaded from files
         * @param maxThreshold  Max threshold. If any pixel has delta above
         *      this value, this comparison fails
         * @param meanThreshold Mean threshold. If mean delta over all pixels
         *      is above this value, the comparison fails
         */
        explicit CompareImageFile(PluginManager::Manager<Trade::AbstractImporter>& manager, Float maxThreshold, Float meanThreshold): _c{&manager, maxThreshold, meanThreshold} {}

        /**
         * @brief Construct with an explicit plugin manager instance and implicit thresholds
         *
         * Equivalent to calling @ref CompareImageFile(PluginManager::Manager<Trade::AbstractImporter>&, Float, Float)
         * with zero values.
         */
        explicit CompareImageFile(PluginManager::Manager<Trade::AbstractImporter>& manager): _c{&manager, 0.0f, 0.0f} {}

        /**
         * @brief Construct with implicit thresholds
         *
         * Equivalent to calling @ref CompareImageFile(Float, Float) with zero
         * values.
         */
        explicit CompareImageFile(): _c{nullptr, 0.0f, 0.0f} {}

        #ifndef DOXYGEN_GENERATING_OUTPUT
        TestSuite::Comparator<CompareImageFile>& comparator() {
            return _c;
        }
        #endif

    private:
        TestSuite::Comparator<CompareImageFile> _c;
};

/**
@brief Image-to-file comparator

A combination of @ref CompareImage and @ref CompareImageToFile, which allows to
compare an in-memory image to a image file. See their documentation for more
information. Example usage:

@snippet MagnumDebugTools.cpp CompareImageToFile

@see @ref CompareFileToImage
*/
class CompareImageToFile {
    public:
        /**
         * @brief Constructor
         *
         * See @ref CompareImageFile::CompareImageFile(Float, Float) for more
         * information.
         */
        explicit CompareImageToFile(Float maxThreshold, Float meanThreshold): _c{nullptr, maxThreshold, meanThreshold} {}

        /**
         * @brief Construct with an explicit plugin manager instance
         *
         * See @ref CompareImageFile::CompareImageFile(PluginManager::Manager<Trade::AbstractImporter>&, Float, Float)
         * for more information.
         */
        explicit CompareImageToFile(PluginManager::Manager<Trade::AbstractImporter>& manager, Float maxThreshold, Float meanThreshold): _c{&manager, maxThreshold, meanThreshold} {}

        /**
         * @brief Construct with an explicit plugin manager instance and implicit thresholds
         *
         * Equivalent to calling @ref CompareImageToFile(PluginManager::Manager<Trade::AbstractImporter>&, Float, Float)
         * with zero values.
         */
        explicit CompareImageToFile(PluginManager::Manager<Trade::AbstractImporter>& manager): _c{&manager, 0.0f, 0.0f} {}

        /**
         * @brief Implicit constructor
         *
         * Equivalent to calling @ref CompareImageToFile(Float, Float) with zero
         * values.
         */
        explicit CompareImageToFile(): _c{nullptr, 0.0f, 0.0f} {}

        #ifndef DOXYGEN_GENERATING_OUTPUT
        TestSuite::Comparator<CompareImageToFile>& comparator() {
            return _c;
        }
        #endif

    private:
        TestSuite::Comparator<CompareImageToFile> _c;
};

/**
@brief File-to-image comparator

A combination of @ref CompareImage and @ref CompareImageToFile, which allows to
compare an image file to an in-memory image. See their documentation for more
information. Example usage:

@snippet MagnumDebugTools.cpp CompareFileToImage

@see @ref CompareImageToFile
*/
class CompareFileToImage {
    public:
        /**
         * @brief Constructor
         *
         * See @ref CompareImageFile::CompareImageFile(Float, Float) for more
         * information.
         */
        explicit CompareFileToImage(Float maxThreshold, Float meanThreshold): _c{nullptr, maxThreshold, meanThreshold} {}

        /**
         * @brief Construct with an explicit plugin manager instance
         *
         * See @ref CompareImageFile::CompareImageFile(PluginManager::Manager<Trade::AbstractImporter>&, Float, Float)
         * for more information.
         */
        explicit CompareFileToImage(PluginManager::Manager<Trade::AbstractImporter>& manager, Float maxThreshold, Float meanThreshold): _c{&manager, maxThreshold, meanThreshold} {}

        /**
         * @brief Construct with an explicit plugin manager instance and implicit thresholds
         *
         * Equivalent to calling @ref CompareFileToImage(PluginManager::Manager<Trade::AbstractImporter>&, Float, Float)
         * with zero values.
         */
        explicit CompareFileToImage(PluginManager::Manager<Trade::AbstractImporter>& manager): _c{&manager, 0.0f, 0.0f} {}

        /**
         * @brief Implicit constructor
         *
         * Equivalent to calling @ref CompareFileToImage(Float, Float) with zero
         * values.
         */
        explicit CompareFileToImage(): _c{nullptr, 0.0f, 0.0f} {}

        #ifndef DOXYGEN_GENERATING_OUTPUT
        TestSuite::Comparator<CompareFileToImage>& comparator() {
            return _c;
        }
        #endif

    private:
        TestSuite::Comparator<CompareFileToImage> _c;
};

}}

#endif
