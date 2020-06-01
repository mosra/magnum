#ifndef Magnum_DebugTools_CompareImage_h
#define Magnum_DebugTools_CompareImage_h
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

/** @file
 * @brief Class @ref Magnum::DebugTools::CompareImage
 */

#include <Corrade/Containers/Pointer.h>
#include <Corrade/PluginManager/PluginManager.h>
#include <Corrade/TestSuite/TestSuite.h>
#include <Corrade/Utility/StlForwardString.h>
#include <Corrade/Utility/StlForwardTuple.h>

#include "Magnum/Magnum.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/Math/Vector2.h"
#include "Magnum/DebugTools/visibility.h"
#include "Magnum/Trade/Trade.h"

namespace Magnum { namespace DebugTools {

namespace Implementation {
    MAGNUM_DEBUGTOOLS_EXPORT std::tuple<Containers::Array<Float>, Float, Float> calculateImageDelta(PixelFormat actualFormat, const Containers::StridedArrayView3D<const char>& actualPixels, const ImageView2D& expected);

    MAGNUM_DEBUGTOOLS_EXPORT void printDeltaImage(Debug& out, Containers::ArrayView<const Float> delta, const Vector2i& size, Float max, Float maxThreshold, Float meanThreshold);

    MAGNUM_DEBUGTOOLS_EXPORT void printPixelDeltas(Debug& out, Containers::ArrayView<const Float> delta, PixelFormat format, const Containers::StridedArrayView3D<const char>& actualPixels, const Containers::StridedArrayView3D<const char>& expectedPixels, Float maxThreshold, Float meanThreshold, std::size_t maxCount);
}

class CompareImage;
class CompareImageFile;
class CompareImageToFile;
class CompareFileToImage;

namespace Implementation {

template<class> constexpr PixelFormat pixelFormatFor();

class MAGNUM_DEBUGTOOLS_EXPORT ImageComparatorBase {
    public:
        explicit ImageComparatorBase(PluginManager::Manager<Trade::AbstractImporter>* importerManager, PluginManager::Manager<Trade::AbstractImageConverter>* converterManager, Float maxThreshold, Float meanThreshold);

        /*implicit*/ ImageComparatorBase(): ImageComparatorBase{nullptr, nullptr, 0.0f, 0.0f} {}

        ~ImageComparatorBase();

        TestSuite::ComparisonStatusFlags operator()(const ImageView2D& actual, const ImageView2D& expected);

        TestSuite::ComparisonStatusFlags operator()(const std::string& actual, const std::string& expected);

        TestSuite::ComparisonStatusFlags operator()(const std::string& actual, const ImageView2D& expected);

        TestSuite::ComparisonStatusFlags operator()(const ImageView2D& actual, const std::string& expected);

        /* Used in templated CompareImage::operator() */
        TestSuite::ComparisonStatusFlags compare(PixelFormat actualFormat, const Containers::StridedArrayView3D<const char>& actualPixels, const ImageView2D& expected);

        /* Used in templated CompareImageToFile::operator() */
        TestSuite::ComparisonStatusFlags compare(PixelFormat actualFormat, const Containers::StridedArrayView3D<const char>& actualPixels, const std::string& expected);

        void printMessage(TestSuite::ComparisonStatusFlags flags, Debug& out, const std::string& actual, const std::string& expected) const;

        void saveDiagnostic(TestSuite::ComparisonStatusFlags flags, Utility::Debug& out, const std::string& path);

    private:
        class MAGNUM_DEBUGTOOLS_LOCAL State;
        Containers::Pointer<State> _state;
};

}}}

#ifndef DOXYGEN_GENERATING_OUTPUT
/* If Doxygen sees this, all @ref Corrade::TestSuite links break (prolly
   because the namespace is undocumented in this project) */
namespace Corrade {

namespace Containers {
    /* Forward-declaring this function to avoid the need to include
       the whole StridedArrayView */
    template<unsigned newDimensions, class U, unsigned dimensions, class T> StridedArrayView<newDimensions, U> arrayCast(const StridedArrayView<dimensions, T>&);
}

namespace TestSuite {

template<> class MAGNUM_DEBUGTOOLS_EXPORT Comparator<Magnum::DebugTools::CompareImage>: public Magnum::DebugTools::Implementation::ImageComparatorBase {
    public:
        explicit Comparator(Magnum::Float maxThreshold, Magnum::Float meanThreshold): Magnum::DebugTools::Implementation::ImageComparatorBase{nullptr, nullptr, maxThreshold, meanThreshold} {}

        /*implicit*/ Comparator(): Comparator{0.0f, 0.0f} {}

        ComparisonStatusFlags operator()(const Magnum::ImageView2D& actual, const Magnum::ImageView2D& expected) {
            return Magnum::DebugTools::Implementation::ImageComparatorBase::operator()(actual, expected);
        }

        template<class T> TestSuite::ComparisonStatusFlags operator()(const Containers::StridedArrayView2D<const T>& actualPixels, const Magnum::ImageView2D& expected) {
            /** @todo do some tryFindCompatibleFormat() here */
            return Magnum::DebugTools::Implementation::ImageComparatorBase::compare(
                Magnum::DebugTools::Implementation::pixelFormatFor<T>(),
                Containers::arrayCast<3, const char>(actualPixels), expected);
        }
};

template<> class MAGNUM_DEBUGTOOLS_EXPORT Comparator<Magnum::DebugTools::CompareImageFile>: public Magnum::DebugTools::Implementation::ImageComparatorBase {
    public:
        explicit Comparator(PluginManager::Manager<Magnum::Trade::AbstractImporter>* importerManager, PluginManager::Manager<Magnum::Trade::AbstractImageConverter>* converterManager, Magnum::Float maxThreshold, Magnum::Float meanThreshold): Magnum::DebugTools::Implementation::ImageComparatorBase{importerManager, converterManager, maxThreshold, meanThreshold} {}

        /*implicit*/ Comparator(): Comparator{nullptr, nullptr, 0.0f, 0.0f} {}

        ComparisonStatusFlags operator()(const std::string& actual, const std::string& expected) {
            return Magnum::DebugTools::Implementation::ImageComparatorBase::operator()(actual, expected);
        }
};

template<> class MAGNUM_DEBUGTOOLS_EXPORT Comparator<Magnum::DebugTools::CompareImageToFile>: public Magnum::DebugTools::Implementation::ImageComparatorBase {
    public:
        explicit Comparator(PluginManager::Manager<Magnum::Trade::AbstractImporter>* importerManager, PluginManager::Manager<Magnum::Trade::AbstractImageConverter>* converterManager, Magnum::Float maxThreshold, Magnum::Float meanThreshold): Magnum::DebugTools::Implementation::ImageComparatorBase{importerManager, converterManager, maxThreshold, meanThreshold} {}

        /*implicit*/ Comparator(): Comparator{nullptr, nullptr, 0.0f, 0.0f} {}

        ComparisonStatusFlags operator()(const Magnum::ImageView2D& actual, const std::string& expected) {
            return Magnum::DebugTools::Implementation::ImageComparatorBase::operator()(actual, expected);
        }

        template<class T> TestSuite::ComparisonStatusFlags operator()(const Containers::StridedArrayView2D<const T>& actualPixels, const std::string& expected) {
            /** @todo do some tryFindCompatibleFormat() here */
            return Magnum::DebugTools::Implementation::ImageComparatorBase::compare(
                Magnum::DebugTools::Implementation::pixelFormatFor<T>(),
                Containers::arrayCast<3, const char>(actualPixels), expected);
        }
};

template<> class MAGNUM_DEBUGTOOLS_EXPORT Comparator<Magnum::DebugTools::CompareFileToImage>: public Magnum::DebugTools::Implementation::ImageComparatorBase {
    public:
        explicit Comparator(PluginManager::Manager<Magnum::Trade::AbstractImporter>* importerManager, Magnum::Float maxThreshold, Magnum::Float meanThreshold): Magnum::DebugTools::Implementation::ImageComparatorBase{importerManager, nullptr, maxThreshold, meanThreshold} {}

        /*implicit*/ Comparator(): Comparator{nullptr, 0.0f, 0.0f} {}

        ComparisonStatusFlags operator()(const std::string& actual, const Magnum::ImageView2D& expected) {
            return Magnum::DebugTools::Implementation::ImageComparatorBase::operator()(actual, expected);
        }
};

namespace Implementation {

/* Explicit ComparatorTraits specialization because
   Comparator<CompareImage>::operator() is overloaded */
template<class T> struct ComparatorTraits<Magnum::DebugTools::CompareImage, Magnum::ImageView2D, T> {
    typedef Magnum::ImageView2D ActualType;
    typedef Magnum::ImageView2D ExpectedType;
};
template<class T> struct ComparatorTraits<Magnum::DebugTools::CompareImage, Magnum::Image2D, T>: ComparatorTraits<Magnum::DebugTools::CompareImage, Magnum::ImageView2D, T> {};
template<class T> struct ComparatorTraits<Magnum::DebugTools::CompareImage, Magnum::Trade::ImageData2D, T>: ComparatorTraits<Magnum::DebugTools::CompareImage, Magnum::ImageView2D, T> {};
template<class T, class U> struct ComparatorTraits<Magnum::DebugTools::CompareImage, Containers::StridedArrayView2D<T>, U> {
    typedef Containers::StridedArrayView2D<const T> ActualType;
    typedef Magnum::ImageView2D ExpectedType;
};

/* Explicit ComparatorTraits specialization because
   Comparator<CompareImageToFile>::operator() is overloaded */
template<class T> struct ComparatorTraits<Magnum::DebugTools::CompareImageToFile, Magnum::ImageView2D, T> {
    typedef Magnum::ImageView2D ActualType;
    typedef std::string ExpectedType;
};
template<class T> struct ComparatorTraits<Magnum::DebugTools::CompareImageToFile, Magnum::Image2D, T>: ComparatorTraits<Magnum::DebugTools::CompareImageToFile, Magnum::ImageView2D, T> {};
template<class T> struct ComparatorTraits<Magnum::DebugTools::CompareImageToFile, Magnum::Trade::ImageData2D, T>: ComparatorTraits<Magnum::DebugTools::CompareImageToFile, Magnum::ImageView2D, T> {};
template<class T, class U> struct ComparatorTraits<Magnum::DebugTools::CompareImageToFile, Containers::StridedArrayView2D<T>, U> {
    typedef Containers::StridedArrayView2D<const T> ActualType;
    typedef std::string ExpectedType;
};

}

}}
#endif

namespace Magnum { namespace DebugTools {

/**
@brief Image comparator

To be used with @ref Corrade::TestSuite. Basic use is really simple:

@snippet debugtools-compareimage.cpp 0

@note For comparing image files, there are also @ref CompareImageFile,
    @ref CompareImageToFile and @ref CompareFileToImage variants.

Based on actual images used, in case of comparison failure the comparator can
give for example the following output:

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

Sometimes it's desirable to print the delta image even if the comparison passed
--- for example, to check that the thresholds aren't too high to hide real
issues. If the `--verbose` @ref TestSuite-Tester-command-line "command-line option"
is specified, every image comparison with a non-zero delta will print an
@cb{.ansi} [1;39mINFO @ce message in the same form as the error diagnostic
shown above.

@section DebugTools-CompareImage-specials Special floating-point values

For floating-point input, the comparator treats the values similarly to how
@ref Corrade::TestSuite::Comparator<float> behaves for scalars:

-   If both actual and expected channel value are NaN, they are treated as the
    same (with channel delta being 0).
-   If actual and expected channel value have the same sign of infinity, they
    are treated the same (with channel delta being 0).
-   Otherwise, the delta is calculated the usual way, with NaN and infinity
    values getting propagated according to floating-point rules. This means
    the final per-pixel @f$ \Delta_{\boldsymbol{p}} @f$ becomes either NaN or
    infinity.
-   When calculating the max value, NaN and infinity @f$ \Delta_{\boldsymbol{p}} @f$
    values are ignored. This is done in order to avoid a single infinity deltas
    causing all other deltas to be comparatively zero in the ASCII-art
    representation.
-   The mean value is calculated as usual, meaning that NaN or infinity in
    @f$ \Delta_{\boldsymbol{p}} @f$ "poison" the final value, reliably causing
    the comparison to fail.

For the ASCII-art representation, NaN and infinity @f$ \Delta_{\boldsymbol{p}} @f$
values are always treated as maximum difference.

@section DebugTools-CompareImage-pixels Comparing against pixel views

For added flexibility, it's possible to use a
@ref Corrade::Containers::StridedArrayView2D containing pixel data on the left
side of the comparison in both @ref CompareImage and @ref CompareImageToFile.
This type is commonly returned from @ref ImageView::pixels() and allows you to
do arbitrary operations on the viewed data --- for example, comparing pixel
data flipped upside down:

@snippet MagnumDebugTools.cpp CompareImage-pixels-flip

For a different scenario, imagine you're comparing data read from a framebuffer
to a ground truth image. On many systems, internal framebuffer storage has to
be four-component; however your if your ground truth image is just
three-component you can cast the pixel data to just a three-component type:

@snippet MagnumDebugTools-gl.cpp CompareImage-pixels-rgb

Currently, comparing against pixel views has a few inherent limitations --- it
has to be cast to one of Magnum scalar or vector types and the format is
then autodetected from the passed type, with normalized formats preferred. In
practice this means e.g. @ref Math::Vector2 "Math::Vector2<UnsignedByte>" will
be understood as @ref PixelFormat::RG8Unorm and there's currently no way to
interpret it as @ref PixelFormat::RG8UI, for example.
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

@section DebugTools-CompareImageFile-save-diagnostic Saving files for failed comparisons

The comparator supports the @ref TestSuite-Tester-save-diagnostic "--save-diagnostic option"
--- if the comparison fails, it saves actual image data to given directory with
a filename and format matching the expected file, using the
@ref Trade::AnyImageConverter "AnyImageConverter" plugin. For this to work,
both @ref Trade::AnyImageConverter "AnyImageConverter" and the concrete
converter plugin need to be available. You can use it to perform a manual data comparison with an external tool or for example to quickly update expected test
data --- point the option to the directory with expected test files and let the
test overwrite them with actual results. The @ref CompareImageToFile variant
supports the same; the @ref CompareImage / @ref CompareFileToImage variants
don't since the comparison is done against in-memory data and so producing a
file isn't that helpful as in the other two variants.
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
        explicit CompareImageFile(Float maxThreshold, Float meanThreshold): _c{nullptr, nullptr, maxThreshold, meanThreshold} {}

        /**
         * @brief Construct with an explicit plugin manager instance
         * @param importerManager   Image importer plugin manager instance
         * @param maxThreshold      Max threshold. If any pixel has delta above
         *      this value, this comparison fails
         * @param meanThreshold     Mean threshold. If mean delta over all
         *      pixels is above this value, the comparison fails
         */
        explicit CompareImageFile(PluginManager::Manager<Trade::AbstractImporter>& importerManager, Float maxThreshold, Float meanThreshold): _c{&importerManager, nullptr, maxThreshold, meanThreshold} {}

        /**
         * @brief Construct with an explicit plugin manager instance and implicit thresholds
         *
         * Equivalent to calling @ref CompareImageFile(PluginManager::Manager<Trade::AbstractImporter>&, Float, Float)
         * with zero values.
         */
        explicit CompareImageFile(PluginManager::Manager<Trade::AbstractImporter>& importerManager): _c{&importerManager, nullptr, 0.0f, 0.0f} {}

        /**
         * @brief Construct with an explicit importer and converter plugin manager instance
         * @param importerManager   Image importer plugin manager instance
         * @param converterManager  Image converter plugin manager instance
         * @param maxThreshold      Max threshold. If any pixel has delta above
         *      this value, this comparison fails
         * @param meanThreshold     Mean threshold. If mean delta over all
         *      pixels is above this value, the comparison fails
         *
         * This variant is rarely usable outside of testing environment, as the
         * @p converterManager is only ever used when saving diagnostic for
         * failed * comparison when the `--save-diagnostic`
         * @ref TestSuite-Tester-save-diagnostic "command-line option" is
         * specified.
         */
        explicit CompareImageFile(PluginManager::Manager<Trade::AbstractImporter>& importerManager, PluginManager::Manager<Trade::AbstractImageConverter>& converterManager, Float maxThreshold, Float meanThreshold): _c{&importerManager, &converterManager, maxThreshold, meanThreshold} {}

        /**
         * @brief Construct with implicit thresholds
         *
         * Equivalent to calling @ref CompareImageFile(Float, Float) with zero
         * values.
         */
        explicit CompareImageFile(): _c{nullptr, nullptr, 0.0f, 0.0f} {}

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

A combination of @ref CompareImage and @ref CompareImageFile, which allows to
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
        explicit CompareImageToFile(Float maxThreshold, Float meanThreshold): _c{nullptr, nullptr, maxThreshold, meanThreshold} {}

        /**
         * @brief Construct with an explicit plugin manager instance
         *
         * See @ref CompareImageFile::CompareImageFile(PluginManager::Manager<Trade::AbstractImporter>&, Float, Float)
         * for more information.
         */
        explicit CompareImageToFile(PluginManager::Manager<Trade::AbstractImporter>& importerManager, Float maxThreshold, Float meanThreshold): _c{&importerManager, nullptr, maxThreshold, meanThreshold} {}

        /**
         * @brief Construct with an explicit plugin manager instance and implicit thresholds
         *
         * Equivalent to calling @ref CompareImageToFile(PluginManager::Manager<Trade::AbstractImporter>&, Float, Float)
         * with zero values.
         */
        explicit CompareImageToFile(PluginManager::Manager<Trade::AbstractImporter>& importerManager): _c{&importerManager, nullptr, 0.0f, 0.0f} {}

        /**
         * @brief Construct with an explicit importer and converter plugin manager instance
         *
         * See @ref CompareImageFile::CompareImageFile(PluginManager::Manager<Trade::AbstractImporter>&, PluginManager::Manager<Trade::AbstractImageConverter>&, Float, Float)
         * for more information.
         */
        explicit CompareImageToFile(PluginManager::Manager<Trade::AbstractImporter>& importerManager, PluginManager::Manager<Trade::AbstractImageConverter>& imageConverterManager, Float maxThreshold, Float meanThreshold): _c{&importerManager, &imageConverterManager, maxThreshold, meanThreshold} {}

        /**
         * @brief Implicit constructor
         *
         * Equivalent to calling @ref CompareImageToFile(Float, Float) with zero
         * values.
         */
        explicit CompareImageToFile(): _c{nullptr, nullptr, 0.0f, 0.0f} {}

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

A combination of @ref CompareImage and @ref CompareImageFile, which allows to
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

namespace Implementation {

/* LCOV_EXCL_START */
/* One-component types */
template<> constexpr PixelFormat pixelFormatFor<UnsignedByte>() { return PixelFormat::R8Unorm; }
template<> constexpr PixelFormat pixelFormatFor<Math::Vector<1, UnsignedByte>>() { return PixelFormat::R8Unorm; }
template<> constexpr PixelFormat pixelFormatFor<Byte>() { return PixelFormat::R8Snorm; }
template<> constexpr PixelFormat pixelFormatFor<Math::Vector<1, Byte>>() { return PixelFormat::R8Snorm; }
template<> constexpr PixelFormat pixelFormatFor<UnsignedShort>() { return PixelFormat::R16Unorm; }
template<> constexpr PixelFormat pixelFormatFor<Math::Vector<1, UnsignedShort>>() { return PixelFormat::R16Unorm; }
template<> constexpr PixelFormat pixelFormatFor<Short>() { return PixelFormat::R16Snorm; }
template<> constexpr PixelFormat pixelFormatFor<Math::Vector<1, Short>>() { return PixelFormat::R16Snorm; }
template<> constexpr PixelFormat pixelFormatFor<UnsignedInt>() { return PixelFormat::R32UI; }
template<> constexpr PixelFormat pixelFormatFor<Math::Vector<1, UnsignedInt>>() { return PixelFormat::R32UI; }
template<> constexpr PixelFormat pixelFormatFor<Int>() { return PixelFormat::R32I; }
template<> constexpr PixelFormat pixelFormatFor<Math::Vector<1, Int>>() { return PixelFormat::R32I; }
template<> constexpr PixelFormat pixelFormatFor<Float>() { return PixelFormat::R32F; }
template<> constexpr PixelFormat pixelFormatFor<Math::Vector<1, Float>>() { return PixelFormat::R32F; }

/* Two-component types */
template<> constexpr PixelFormat pixelFormatFor<Math::Vector<2, UnsignedByte>>() { return PixelFormat::RG8Unorm; }
template<> constexpr PixelFormat pixelFormatFor<Math::Vector2<UnsignedByte>>() { return PixelFormat::RG8Unorm; }
template<> constexpr PixelFormat pixelFormatFor<Math::Vector<2, Byte>>() { return PixelFormat::RG8Snorm; }
template<> constexpr PixelFormat pixelFormatFor<Math::Vector2<Byte>>() { return PixelFormat::RG8Snorm; }
template<> constexpr PixelFormat pixelFormatFor<Math::Vector<2, UnsignedShort>>() { return PixelFormat::RG16Unorm; }
template<> constexpr PixelFormat pixelFormatFor<Math::Vector2<UnsignedShort>>() { return PixelFormat::RG16Unorm; }
template<> constexpr PixelFormat pixelFormatFor<Math::Vector<2, Short>>() { return PixelFormat::RG16Snorm; }
template<> constexpr PixelFormat pixelFormatFor<Math::Vector2<Short>>() { return PixelFormat::RG16Snorm; }
template<> constexpr PixelFormat pixelFormatFor<Math::Vector<2, UnsignedInt>>() { return PixelFormat::RG32UI; }
template<> constexpr PixelFormat pixelFormatFor<Math::Vector2<UnsignedInt>>() { return PixelFormat::RG32UI; }
template<> constexpr PixelFormat pixelFormatFor<Math::Vector<2, Int>>() { return PixelFormat::RG32I; }
template<> constexpr PixelFormat pixelFormatFor<Math::Vector2<Int>>() { return PixelFormat::RG32I; }
template<> constexpr PixelFormat pixelFormatFor<Math::Vector<2, Float>>() { return PixelFormat::RG32F; }
template<> constexpr PixelFormat pixelFormatFor<Math::Vector2<Float>>() { return PixelFormat::RG32F; }

/* Three-component types */
template<> constexpr PixelFormat pixelFormatFor<Math::Vector<3, UnsignedByte>>() { return PixelFormat::RGB8Unorm; }
template<> constexpr PixelFormat pixelFormatFor<Math::Vector3<UnsignedByte>>() { return PixelFormat::RGB8Unorm; }
template<> constexpr PixelFormat pixelFormatFor<Math::Color3<UnsignedByte>>() { return PixelFormat::RGB8Unorm; }
template<> constexpr PixelFormat pixelFormatFor<Math::Vector<3, Byte>>() { return PixelFormat::RGB8Snorm; }
template<> constexpr PixelFormat pixelFormatFor<Math::Vector3<Byte>>() { return PixelFormat::RGB8Snorm; }
template<> constexpr PixelFormat pixelFormatFor<Math::Color3<Byte>>() { return PixelFormat::RGB8Snorm; }
template<> constexpr PixelFormat pixelFormatFor<Math::Vector<3, UnsignedShort>>() { return PixelFormat::RGB16Unorm; }
template<> constexpr PixelFormat pixelFormatFor<Math::Vector3<UnsignedShort>>() { return PixelFormat::RGB16Unorm; }
template<> constexpr PixelFormat pixelFormatFor<Math::Color3<UnsignedShort>>() { return PixelFormat::RGB16Unorm; }
template<> constexpr PixelFormat pixelFormatFor<Math::Vector<3, Short>>() { return PixelFormat::RGB16Snorm; }
template<> constexpr PixelFormat pixelFormatFor<Math::Vector3<Short>>() { return PixelFormat::RGB16Snorm; }
template<> constexpr PixelFormat pixelFormatFor<Math::Color3<Short>>() { return PixelFormat::RGB16Snorm; }
template<> constexpr PixelFormat pixelFormatFor<Math::Vector<3, UnsignedInt>>() { return PixelFormat::RGB32UI; }
template<> constexpr PixelFormat pixelFormatFor<Math::Vector3<UnsignedInt>>() { return PixelFormat::RGB32UI; }
/* Skipping Math::Color3<UnsignedInt>, as that isn't much used */
template<> constexpr PixelFormat pixelFormatFor<Math::Vector<3, Int>>() { return PixelFormat::RGB32I; }
template<> constexpr PixelFormat pixelFormatFor<Math::Vector3<Int>>() { return PixelFormat::RGB32I; }
/* Skipping Math::Color3<Int>, as that isn't much used */
template<> constexpr PixelFormat pixelFormatFor<Math::Vector<3, Float>>() { return PixelFormat::RGB32F; }
template<> constexpr PixelFormat pixelFormatFor<Math::Vector3<Float>>() { return PixelFormat::RGB32F; }

/* Four-component types */
template<> constexpr PixelFormat pixelFormatFor<Math::Vector<4, UnsignedByte>>() { return PixelFormat::RGBA8Unorm; }
template<> constexpr PixelFormat pixelFormatFor<Math::Vector4<UnsignedByte>>() { return PixelFormat::RGBA8Unorm; }
template<> constexpr PixelFormat pixelFormatFor<Math::Color4<UnsignedByte>>() { return PixelFormat::RGBA8Unorm; }
template<> constexpr PixelFormat pixelFormatFor<Math::Vector<4, Byte>>() { return PixelFormat::RGBA8Snorm; }
template<> constexpr PixelFormat pixelFormatFor<Math::Vector4<Byte>>() { return PixelFormat::RGBA8Snorm; }
template<> constexpr PixelFormat pixelFormatFor<Math::Color4<Byte>>() { return PixelFormat::RGBA8Snorm; }
template<> constexpr PixelFormat pixelFormatFor<Math::Vector<4, UnsignedShort>>() { return PixelFormat::RGBA16Unorm; }
template<> constexpr PixelFormat pixelFormatFor<Math::Vector4<UnsignedShort>>() { return PixelFormat::RGBA16Unorm; }
template<> constexpr PixelFormat pixelFormatFor<Math::Color4<UnsignedShort>>() { return PixelFormat::RGBA16Unorm; }
template<> constexpr PixelFormat pixelFormatFor<Math::Vector<4, Short>>() { return PixelFormat::RGBA16Snorm; }
template<> constexpr PixelFormat pixelFormatFor<Math::Vector4<Short>>() { return PixelFormat::RGBA16Snorm; }
template<> constexpr PixelFormat pixelFormatFor<Math::Color4<Short>>() { return PixelFormat::RGBA16Snorm; }
template<> constexpr PixelFormat pixelFormatFor<Math::Vector<4, UnsignedInt>>() { return PixelFormat::RGBA32UI; }
template<> constexpr PixelFormat pixelFormatFor<Math::Vector4<UnsignedInt>>() { return PixelFormat::RGBA32UI; }
/* Skipping Math::Color4<UnsignedInt>, as that isn't much used */
template<> constexpr PixelFormat pixelFormatFor<Math::Vector<4, Int>>() { return PixelFormat::RGBA32I; }
template<> constexpr PixelFormat pixelFormatFor<Math::Vector4<Int>>() { return PixelFormat::RGBA32I; }
/* Skipping Math::Color4<Int>, as that isn't much used */
template<> constexpr PixelFormat pixelFormatFor<Math::Vector<4, Float>>() { return PixelFormat::RGBA32F; }
template<> constexpr PixelFormat pixelFormatFor<Math::Vector4<Float>>() { return PixelFormat::RGBA32F; }
template<> constexpr PixelFormat pixelFormatFor<Math::Color4<Float>>() { return PixelFormat::RGBA32F; }
/* LCOV_EXCL_STOP */

}

}}

#endif
