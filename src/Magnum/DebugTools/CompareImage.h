#ifndef Magnum_DebugTools_CompareImage_h
#define Magnum_DebugTools_CompareImage_h
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

/** @file
 * @brief Class @ref Magnum::DebugTools::CompareImage
 */

#include <Corrade/Containers/Pointer.h>
#include <Corrade/PluginManager/PluginManager.h>
#include <Corrade/TestSuite/TestSuite.h>

#include "Magnum/Magnum.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/Math/Vector2.h"
#include "Magnum/DebugTools/visibility.h"
#include "Magnum/Trade/Trade.h"

#ifdef MAGNUM_BUILD_DEPRECATED
/* Some arguments used to be a std::string, so provide implicit conversion to a
   StringView */
#include "Corrade/Containers/StringStl.h"
#endif

namespace Magnum { namespace DebugTools {

namespace Implementation {
    MAGNUM_DEBUGTOOLS_EXPORT Containers::Triple<Containers::Array<Float>, Float, Float> calculateImageDelta(PixelFormat actualFormat, const Containers::StridedArrayView3D<const char>& actualPixels, const ImageView2D& expected);

    MAGNUM_DEBUGTOOLS_EXPORT void printDeltaImage(Debug& out, Containers::ArrayView<const Float> delta, const Vector2i& size, Float max, Float maxThreshold, Float meanThreshold);

    MAGNUM_DEBUGTOOLS_EXPORT void printPixelDeltas(Debug& out, Containers::ArrayView<const Float> delta, PixelFormat format, const Containers::StridedArrayView3D<const char>& actualPixels, const Containers::StridedArrayView3D<const char>& expectedPixels, Float maxThreshold, Float meanThreshold, std::size_t maxCount);
}

class CompareImage;
class CompareImageFile;
class CompareImageToFile;
class CompareFileToImage;

namespace Implementation {

/* Used by the pixel view comparators to try to guess a format matching the
   expected image */
template<class> constexpr PixelFormat pixelFormatFor(PixelFormat);

class MAGNUM_DEBUGTOOLS_EXPORT ImageComparatorBase {
    public:
        explicit ImageComparatorBase(PluginManager::Manager<Trade::AbstractImporter>* importerManager, PluginManager::Manager<Trade::AbstractImageConverter>* converterManager, Float maxThreshold, Float meanThreshold);

        /*implicit*/ ImageComparatorBase(): ImageComparatorBase{nullptr, nullptr, 0.0f, 0.0f} {}

        ~ImageComparatorBase();

        TestSuite::ComparisonStatusFlags operator()(const ImageView2D& actual, const ImageView2D& expected);

        TestSuite::ComparisonStatusFlags operator()(Containers::StringView actual, Containers::StringView expected);

        TestSuite::ComparisonStatusFlags operator()(Containers::StringView actual, const ImageView2D& expected);

        TestSuite::ComparisonStatusFlags operator()(const ImageView2D& actual, Containers::StringView expected);

        TestSuite::ComparisonStatusFlags compare(PixelFormat actualFormat, const Containers::StridedArrayView3D<const char>& actualPixels, const ImageView2D& expected);
        /* Used in templated CompareImage::operator(), the actual format gets
           matched to the format in the expected image */
        TestSuite::ComparisonStatusFlags compare(PixelFormat(*actualFormatFor)(PixelFormat), const Containers::StridedArrayView3D<const char>& actualPixels, const ImageView2D& expected);

        /* Used in templated CompareImageToFile::operator(). If actualFormatFor
           isn't nullptr, the actualFormat gets overriden by it once the
           expected image is loaded. */
        TestSuite::ComparisonStatusFlags compare(PixelFormat actualFormat, PixelFormat(*actualFormatFor)(PixelFormat), const Containers::StridedArrayView3D<const char>& actualPixels, Containers::StringView expected);

        void printMessage(TestSuite::ComparisonStatusFlags flags, Debug& out, Containers::StringView actual, Containers::StringView expected) const;

        void saveDiagnostic(TestSuite::ComparisonStatusFlags flags, Utility::Debug& out, Containers::StringView path);

    private:
        class State;
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
            return Magnum::DebugTools::Implementation::ImageComparatorBase::compare(
                Magnum::DebugTools::Implementation::pixelFormatFor<T>,
                Containers::arrayCast<3, const char>(actualPixels), expected);
        }
};

template<> class MAGNUM_DEBUGTOOLS_EXPORT Comparator<Magnum::DebugTools::CompareImageFile>: public Magnum::DebugTools::Implementation::ImageComparatorBase {
    public:
        explicit Comparator(PluginManager::Manager<Magnum::Trade::AbstractImporter>* importerManager, PluginManager::Manager<Magnum::Trade::AbstractImageConverter>* converterManager, Magnum::Float maxThreshold, Magnum::Float meanThreshold): Magnum::DebugTools::Implementation::ImageComparatorBase{importerManager, converterManager, maxThreshold, meanThreshold} {}

        /*implicit*/ Comparator(): Comparator{nullptr, nullptr, 0.0f, 0.0f} {}

        ComparisonStatusFlags operator()(Containers::StringView actual, const Containers::StringView expected);
};

template<> class MAGNUM_DEBUGTOOLS_EXPORT Comparator<Magnum::DebugTools::CompareImageToFile>: public Magnum::DebugTools::Implementation::ImageComparatorBase {
    public:
        explicit Comparator(PluginManager::Manager<Magnum::Trade::AbstractImporter>* importerManager, PluginManager::Manager<Magnum::Trade::AbstractImageConverter>* converterManager, Magnum::Float maxThreshold, Magnum::Float meanThreshold): Magnum::DebugTools::Implementation::ImageComparatorBase{importerManager, converterManager, maxThreshold, meanThreshold} {}

        /*implicit*/ Comparator(): Comparator{nullptr, nullptr, 0.0f, 0.0f} {}

        ComparisonStatusFlags operator()(const Magnum::ImageView2D& actual, Containers::StringView expected);

        /* Unlike other functions, this one is using const StringView& to avoid
           having to include it -- it's not needed when comparing just images
           alone, not files */
        template<class T> TestSuite::ComparisonStatusFlags operator()(const Containers::StridedArrayView2D<const T>& actualPixels, const Containers::StringView& expected) {
            return Magnum::DebugTools::Implementation::ImageComparatorBase::compare(
                Magnum::DebugTools::Implementation::pixelFormatFor<T>(Magnum::PixelFormat{}),
                Magnum::DebugTools::Implementation::pixelFormatFor<T>,
                Containers::arrayCast<3, const char>(actualPixels), expected);
        }
};

template<> class MAGNUM_DEBUGTOOLS_EXPORT Comparator<Magnum::DebugTools::CompareFileToImage>: public Magnum::DebugTools::Implementation::ImageComparatorBase {
    public:
        explicit Comparator(PluginManager::Manager<Magnum::Trade::AbstractImporter>* importerManager, Magnum::Float maxThreshold, Magnum::Float meanThreshold): Magnum::DebugTools::Implementation::ImageComparatorBase{importerManager, nullptr, maxThreshold, meanThreshold} {}

        /*implicit*/ Comparator(): Comparator{nullptr, 0.0f, 0.0f} {}

        ComparisonStatusFlags operator()(Containers::StringView actual, const Magnum::ImageView2D& expected);
};

namespace Implementation {

/* Explicit ComparatorTraits specialization because
   Comparator<CompareImage>::operator() is overloaded */
template<class T, class U> struct ComparatorTraits<Magnum::DebugTools::CompareImage, Magnum::ImageView<2, U>, T> {
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
template<class T, class U> struct ComparatorTraits<Magnum::DebugTools::CompareImageToFile, Magnum::ImageView<2, U>, T> {
    typedef Magnum::ImageView2D ActualType;
    typedef Containers::StringView ExpectedType;
};
template<class T> struct ComparatorTraits<Magnum::DebugTools::CompareImageToFile, Magnum::Image2D, T>: ComparatorTraits<Magnum::DebugTools::CompareImageToFile, Magnum::ImageView2D, T> {};
template<class T> struct ComparatorTraits<Magnum::DebugTools::CompareImageToFile, Magnum::Trade::ImageData2D, T>: ComparatorTraits<Magnum::DebugTools::CompareImageToFile, Magnum::ImageView2D, T> {};
template<class T, class U> struct ComparatorTraits<Magnum::DebugTools::CompareImageToFile, Containers::StridedArrayView2D<T>, U> {
    typedef Containers::StridedArrayView2D<const T> ActualType;
    typedef Containers::StringView ExpectedType;
};

}

}}
#endif

namespace Magnum { namespace DebugTools {

/**
@brief Image comparator for @ref Corrade::TestSuite

The simplest way to use the comparator is by passing it to
@ref CORRADE_COMPARE_AS() along with an actual and expected image, as shown
below. That will compare image sizes, pixel formats and pixel data for equality
taking into account pixel storage parameters of each image, without requiring
you to manually go through all relevant properties and looping over pixel data:

@snippet debugtools-compareimage.cpp basic

@note For comparing image files, there are also @ref CompareImageFile,
    @ref CompareImageToFile and @ref CompareFileToImage variants.

Where the comparator actually shines, however, is comparing with some delta ---
since images produced by real-world hardware, algorithms and lossy compression
schemes are rarely exactly bit-equal. Using @ref CORRADE_COMPARE_WITH() instead
to be able to pass parameters to the constructor, it takes a max delta for a
single pixel and mean delta averaged over all pixels:

@snippet debugtools-compareimage.cpp delta

Based on actual images used, in case of a comparison failure the comparator can
give for example the following output:

@m_class{m-console-wrap}

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
-   @ref PixelFormat::RGBA16F and its one-/two-/three-component versions
-   @ref PixelFormat::RGBA32F and its one-/two-/three-component versions

Packed depth/stencil formats are not supported at the moment, however you can
work around that by making separate depth/stencil pixel views and
@ref DebugTools-CompareImage-pixels "comparing the views" to a
depth/stencil-only ground truth images. Implementation-specific pixel formats
can't be supported.

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

@snippet DebugTools.cpp CompareImage-pixels-flip

For a different scenario, imagine you're comparing data read from a framebuffer
to a ground truth image. On many systems, internal framebuffer storage has to
be four-component; however your if your ground truth image is three-component
you can slice the pixel data to just the RGB channels:

@snippet DebugTools-gl.cpp CompareImage-pixels-rgb

The pixel views are expected to be cast to one of Magnum scalar or vector
types. The format is then autodetected from the passed type. For types that map
to more than one @ref PixelFormat, such as @relativeref{Magnum,Vector3ub} that
can be @ref PixelFormat::RGB8Unorm, @relativeref{PixelFormat,RGB8Srgb} or
@relativeref{PixelFormat,RGB8UI}, an attempt is made to match the pixel format
of the expected image if possible. If not possible, such as comparing a
@relativeref{Magnum,Vector3ub} view to a @ref PixelFormat::RGB8Snorm, the
comparison fails the same way as if comparing two views of different pixel
formats. Byte and short color types always autodetect to normalized (sRGB)
pixel formats, never to integer formats.

@see @ref CompareMaterial
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
@brief Image file comparator for @ref Corrade::TestSuite

Similar to @ref CompareImage, but comparing images loaded from files. Example
usage:

@snippet DebugTools.cpp CompareImageFile

By default, the comparator uses a local instance of
@ref Corrade::PluginManager::Manager to load image files. This might be
problematic if the code being tested also uses a plugin manager instance or if
you need to use a different plugin directory, for example. For such cases it's
possible to supply an external instance:

@snippet DebugTools.cpp CompareImageFile-manager

The comparator uses the @ref Trade::AnyImageImporter "AnyImageImporter" plugin,
which in turn delegates the import to some importer plugin matching the image
format(s). Both the @ref Trade::AnyImageImporter "AnyImageImporter" and the
concrete importer plugin(s) need to be available, otherwise the comparison
fails. An alternative way is manually skipping the test if the plugins are not
available:

@snippet DebugTools.cpp CompareImageFile-skip

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
         * @brief Construct with an explicit importer and converter plugin manager instance and implicit thresholds
         * @m_since_latest
         *
         * Equivalent to calling @ref CompareImageFile(PluginManager::Manager<Trade::AbstractImporter>&, PluginManager::Manager<Trade::AbstractImageConverter>&, Float, Float)
         * with zero values.
         */
        explicit CompareImageFile(PluginManager::Manager<Trade::AbstractImporter>& importerManager, PluginManager::Manager<Trade::AbstractImageConverter>& imageConverterManager): _c{&importerManager, &imageConverterManager, 0.0f, 0.0f} {}

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
@brief Image-to-file comparator for @ref Corrade::TestSuite

A combination of @ref CompareImage and @ref CompareImageFile, which allows to
compare an in-memory image to a image file. See their documentation for more
information. Example usage:

@snippet DebugTools.cpp CompareImageToFile

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
         * @brief Construct with an explicit importer and converter plugin manager instance and implicit thresholds
         * @m_since_latest
         *
         * Equivalent to calling @ref CompareImageToFile(PluginManager::Manager<Trade::AbstractImporter>&, PluginManager::Manager<Trade::AbstractImageConverter>&, Float, Float)
         * with zero values.
         */
        explicit CompareImageToFile(PluginManager::Manager<Trade::AbstractImporter>& importerManager, PluginManager::Manager<Trade::AbstractImageConverter>& imageConverterManager): _c{&importerManager, &imageConverterManager, 0.0f, 0.0f} {}

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
@brief File-to-image comparator for @ref Corrade::TestSuite

A combination of @ref CompareImage and @ref CompareImageFile, which allows to
compare an image file to an in-memory image. See their documentation for more
information. Example usage:

@snippet DebugTools.cpp CompareFileToImage

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

/* One-component types. The Vector<1, T> types aren't used that often so they
   delegate to the T variants, the tests use Vector<1, T> to cover all
   variants. */
template<> constexpr PixelFormat pixelFormatFor<UnsignedByte>(PixelFormat expected) {
    /* Attempt to match Srgb or Unorm if the expected image has it */
    return expected == PixelFormat::R8Srgb ? PixelFormat::R8Srgb :
        expected == PixelFormat::R8Unorm ? PixelFormat::R8Unorm :
            PixelFormat::R8UI;
}
template<> constexpr PixelFormat pixelFormatFor<Math::Vector<1, UnsignedByte>>(PixelFormat expected) {
    return pixelFormatFor<UnsignedByte>(expected);
}
template<> constexpr PixelFormat pixelFormatFor<Byte>(PixelFormat expected) {
    /* Attempt to match Snorm if the expected image has it */
    return expected == PixelFormat::R8Snorm ? PixelFormat::R8Snorm :
        PixelFormat::R8I;
}
template<> constexpr PixelFormat pixelFormatFor<Math::Vector<1, Byte>>(PixelFormat expected) {
    return pixelFormatFor<Byte>(expected);
}
template<> constexpr PixelFormat pixelFormatFor<UnsignedShort>(PixelFormat expected) {
    /* Attempt to match Unorm if the expected image has it */
    return expected == PixelFormat::R16Unorm ? PixelFormat::R16Unorm :
        PixelFormat::R16UI;
}
template<> constexpr PixelFormat pixelFormatFor<Math::Vector<1, UnsignedShort>>(PixelFormat expected) {
    return pixelFormatFor<UnsignedShort>(expected);
}
template<> constexpr PixelFormat pixelFormatFor<Short>(PixelFormat expected) {
    /* Attempt to match Snorm if the expected image has it */
    return expected == PixelFormat::R16Snorm ? PixelFormat::R16Snorm :
        PixelFormat::R16I;
}
template<> constexpr PixelFormat pixelFormatFor<Math::Vector<1, Short>>(PixelFormat expected) {
    return pixelFormatFor<Short>(expected);
}
template<> constexpr PixelFormat pixelFormatFor<UnsignedInt>(PixelFormat) {
    return PixelFormat::R32UI;
}
template<> constexpr PixelFormat pixelFormatFor<Math::Vector<1, UnsignedInt>>(PixelFormat expected) {
    return pixelFormatFor<UnsignedInt>(expected);
}
template<> constexpr PixelFormat pixelFormatFor<Int>(PixelFormat) {
    return PixelFormat::R32I;
}
template<> constexpr PixelFormat pixelFormatFor<Math::Vector<1, Int>>(PixelFormat expected) {
    return pixelFormatFor<Int>(expected);
}
template<> constexpr PixelFormat pixelFormatFor<Half>(PixelFormat) {
    return PixelFormat::R16F;
}
template<> constexpr PixelFormat pixelFormatFor<Math::Vector<1, Half>>(PixelFormat expected) {
    return pixelFormatFor<Half>(expected);
}
template<> constexpr PixelFormat pixelFormatFor<Float>(PixelFormat) {
    return PixelFormat::R32F;
}
template<> constexpr PixelFormat pixelFormatFor<Math::Vector<1, Float>>(PixelFormat expected) {
    return pixelFormatFor<Float>(expected);
}

/* Two-component types. The Vector<2, T> types aren't used that often so they
   delegate to the Vector2<T> variants, the tests use Vector<2, T> to cover all
   variants. */
template<> constexpr PixelFormat pixelFormatFor<Math::Vector2<UnsignedByte>>(PixelFormat expected) {
    /* Attempt to match Srgb or Unorm if the expected image has it */
    return expected == PixelFormat::RG8Srgb ? PixelFormat::RG8Srgb :
        expected == PixelFormat::RG8Unorm ? PixelFormat::RG8Unorm :
            PixelFormat::RG8UI;
}
template<> constexpr PixelFormat pixelFormatFor<Math::Vector<2, UnsignedByte>>(PixelFormat expected) {
    return pixelFormatFor<Math::Vector2<UnsignedByte>>(expected);
}
template<> constexpr PixelFormat pixelFormatFor<Math::Vector2<Byte>>(PixelFormat expected) {
    /* Attempt to match Snorm if the expected image has it */
    return expected == PixelFormat::RG8Snorm ? PixelFormat::RG8Snorm :
        PixelFormat::RG8I;
}
template<> constexpr PixelFormat pixelFormatFor<Math::Vector<2, Byte>>(PixelFormat expected) {
    return pixelFormatFor<Math::Vector2<Byte>>(expected);
}
template<> constexpr PixelFormat pixelFormatFor<Math::Vector2<UnsignedShort>>(PixelFormat expected) {
    /* Attempt to match Unorm if the expected image has it */
    return expected == PixelFormat::RG16Unorm ? PixelFormat::RG16Unorm :
        PixelFormat::RG16UI;
}
template<> constexpr PixelFormat pixelFormatFor<Math::Vector<2, UnsignedShort>>(PixelFormat expected) {
    return pixelFormatFor<Math::Vector2<UnsignedShort>>(expected);
}
template<> constexpr PixelFormat pixelFormatFor<Math::Vector2<Short>>(PixelFormat expected) {
    /* Attempt to match Snorm if the expected image has it */
    return expected == PixelFormat::RG16Snorm ? PixelFormat::RG16Snorm :
        PixelFormat::RG16I;
}
template<> constexpr PixelFormat pixelFormatFor<Math::Vector<2, Short>>(PixelFormat expected) {
    return pixelFormatFor<Math::Vector2<Short>>(expected);
}
template<> constexpr PixelFormat pixelFormatFor<Math::Vector2<UnsignedInt>>(PixelFormat) {
    return PixelFormat::RG32UI;
}
template<> constexpr PixelFormat pixelFormatFor<Math::Vector<2, UnsignedInt>>(PixelFormat expected) {
    return pixelFormatFor<Math::Vector2<UnsignedInt>>(expected);
}
template<> constexpr PixelFormat pixelFormatFor<Math::Vector2<Int>>(PixelFormat) {
    return PixelFormat::RG32I;
}
template<> constexpr PixelFormat pixelFormatFor<Math::Vector<2, Int>>(PixelFormat expected) {
    return pixelFormatFor<Math::Vector2<Int>>(expected);
}
template<> constexpr PixelFormat pixelFormatFor<Math::Vector2<Half>>(PixelFormat) {
    return PixelFormat::RG16F;
}
template<> constexpr PixelFormat pixelFormatFor<Math::Vector<2, Half>>(PixelFormat expected) {
    return pixelFormatFor<Math::Vector2<Half>>(expected);
}
template<> constexpr PixelFormat pixelFormatFor<Math::Vector2<Float>>(PixelFormat) {
    return PixelFormat::RG32F;
}
template<> constexpr PixelFormat pixelFormatFor<Math::Vector<2, Float>>(PixelFormat expected) {
    return pixelFormatFor<Math::Vector2<Float>>(expected);
}

/* Three-component types. The Vector<3, T> types aren't used that often so they
   delegate to the Vector3<T> variants, the tests use Vector<3, T> to cover all
   variants. */
template<> constexpr PixelFormat pixelFormatFor<Math::Vector3<UnsignedByte>>(PixelFormat expected) {
    /* Attempt to match Srgb or Unorm if the expected image has it */
    return expected == PixelFormat::RGB8Srgb ? PixelFormat::RGB8Srgb :
        expected == PixelFormat::RGB8Unorm ? PixelFormat::RGB8Unorm :
            PixelFormat::RGB8UI;
}
template<> constexpr PixelFormat pixelFormatFor<Math::Vector<3, UnsignedByte>>(PixelFormat expected) {
    return pixelFormatFor<Math::Vector3<UnsignedByte>>(expected);
}
template<> constexpr PixelFormat pixelFormatFor<Math::Color3<UnsignedByte>>(PixelFormat expected) {
    /* Attempt to match Srgb if the expected image has it. No integer fallback
       for color types. */
    return expected == PixelFormat::RGB8Srgb ? PixelFormat::RGB8Srgb :
        PixelFormat::RGB8Unorm;
}
template<> constexpr PixelFormat pixelFormatFor<Math::Vector3<Byte>>(PixelFormat expected) {
    /* Attempt to match Snorm if the expected image has it */
    return expected == PixelFormat::RGB8Snorm ? PixelFormat::RGB8Snorm :
        PixelFormat::RGB8I;
}
template<> constexpr PixelFormat pixelFormatFor<Math::Vector<3, Byte>>(PixelFormat expected) {
    return pixelFormatFor<Math::Vector3<Byte>>(expected);
}
template<> constexpr PixelFormat pixelFormatFor<Math::Color3<Byte>>(PixelFormat) {
    /* No integer fallback for colors */
    return PixelFormat::RGB8Snorm;
}
template<> constexpr PixelFormat pixelFormatFor<Math::Vector3<UnsignedShort>>(PixelFormat expected) {
    /* Attempt to match Unorm if the expected image has it */
    return expected == PixelFormat::RGB16Unorm ? PixelFormat::RGB16Unorm :
        PixelFormat::RGB16UI;
}
template<> constexpr PixelFormat pixelFormatFor<Math::Vector<3, UnsignedShort>>(PixelFormat expected) {
    return pixelFormatFor<Math::Vector3<UnsignedShort>>(expected);
}
template<> constexpr PixelFormat pixelFormatFor<Math::Color3<UnsignedShort>>(PixelFormat) {
    return PixelFormat::RGB16Unorm;
}
template<> constexpr PixelFormat pixelFormatFor<Math::Vector3<Short>>(PixelFormat expected) {
    /* Attempt to match Snorm if the expected image has it */
    return expected == PixelFormat::RGB16Snorm ? PixelFormat::RGB16Snorm :
        PixelFormat::RGB16I;
}
template<> constexpr PixelFormat pixelFormatFor<Math::Vector<3, Short>>(PixelFormat expected) {
    return pixelFormatFor<Math::Vector3<Short>>(expected);
}
template<> constexpr PixelFormat pixelFormatFor<Math::Color3<Short>>(PixelFormat) {
    return PixelFormat::RGB16Snorm;
}
template<> constexpr PixelFormat pixelFormatFor<Math::Vector3<UnsignedInt>>(PixelFormat) {
    return PixelFormat::RGB32UI;
}
template<> constexpr PixelFormat pixelFormatFor<Math::Vector<3, UnsignedInt>>(PixelFormat expected) {
    return pixelFormatFor<Math::Vector3<UnsignedInt>>(expected);
}
/* Skipping Math::Color3<UnsignedInt>, as integer colors should always match
   normalized types */
template<> constexpr PixelFormat pixelFormatFor<Math::Vector3<Int>>(PixelFormat) {
    return PixelFormat::RGB32I;
}
template<> constexpr PixelFormat pixelFormatFor<Math::Vector<3, Int>>(PixelFormat expected) {
    return pixelFormatFor<Math::Vector3<Int>>(expected);
}
/* Skipping Math::Color3<Int>, as integer colors should always match normalized
   types */
template<> constexpr PixelFormat pixelFormatFor<Math::Vector3<Half>>(PixelFormat) {
    return PixelFormat::RGB16F;
}
template<> constexpr PixelFormat pixelFormatFor<Math::Vector<3, Half>>(PixelFormat expected) {
    return pixelFormatFor<Math::Vector3<Half>>(expected);
}
template<> constexpr PixelFormat pixelFormatFor<Math::Color3<Half>>(PixelFormat) {
    return PixelFormat::RGB16F;
}
template<> constexpr PixelFormat pixelFormatFor<Math::Vector3<Float>>(PixelFormat) {
    return PixelFormat::RGB32F;
}
template<> constexpr PixelFormat pixelFormatFor<Math::Vector<3, Float>>(PixelFormat expected) {
    return pixelFormatFor<Math::Vector3<Float>>(expected);
}
template<> constexpr PixelFormat pixelFormatFor<Math::Color3<Float>>(PixelFormat) {
    return PixelFormat::RGB32F;
}

/* Four-component types. The Vector<4, T> types aren't used that often so they
   delegate to the Vector4<T> variants, the tests use Vector<4, T> to cover all
   variants. */
template<> constexpr PixelFormat pixelFormatFor<Math::Vector4<UnsignedByte>>(PixelFormat expected) {
    /* Attempt to match Srgb or Unorm if the expected image has it */
    return expected == PixelFormat::RGBA8Srgb ? PixelFormat::RGBA8Srgb :
        expected == PixelFormat::RGBA8Unorm ? PixelFormat::RGBA8Unorm :
            PixelFormat::RGBA8UI;
}
template<> constexpr PixelFormat pixelFormatFor<Math::Vector<4, UnsignedByte>>(PixelFormat expected) {
    return pixelFormatFor<Math::Vector4<UnsignedByte>>(expected);
}
template<> constexpr PixelFormat pixelFormatFor<Math::Color4<UnsignedByte>>(PixelFormat expected) {
    /* Attempt to match Srgb if the expected image has it. No integer fallback
       for color types. */
    return expected == PixelFormat::RGBA8Srgb ? PixelFormat::RGBA8Srgb :
        PixelFormat::RGBA8Unorm;
}
template<> constexpr PixelFormat pixelFormatFor<Math::Vector4<Byte>>(PixelFormat expected) {
    /* Attempt to match Snorm if the expected image has it */
    return expected == PixelFormat::RGBA8Snorm ? PixelFormat::RGBA8Snorm :
        PixelFormat::RGBA8I;
}
template<> constexpr PixelFormat pixelFormatFor<Math::Vector<4, Byte>>(PixelFormat expected) {
    return pixelFormatFor<Math::Vector4<Byte>>(expected);
}
template<> constexpr PixelFormat pixelFormatFor<Math::Color4<Byte>>(PixelFormat) {
    /* No integer fallback for colors */
    return PixelFormat::RGBA8Snorm;
}
template<> constexpr PixelFormat pixelFormatFor<Math::Vector4<UnsignedShort>>(PixelFormat expected) {
    /* Attempt to match Unorm if the expected image has it */
    return expected == PixelFormat::RGBA16Unorm ? PixelFormat::RGBA16Unorm :
        PixelFormat::RGBA16UI;
}
template<> constexpr PixelFormat pixelFormatFor<Math::Vector<4, UnsignedShort>>(PixelFormat expected) {
    return pixelFormatFor<Math::Vector4<UnsignedShort>>(expected);
}
template<> constexpr PixelFormat pixelFormatFor<Math::Color4<UnsignedShort>>(PixelFormat) {
    /* No integer fallback for colors */
    return PixelFormat::RGBA16Unorm;
}
template<> constexpr PixelFormat pixelFormatFor<Math::Vector4<Short>>(PixelFormat expected) {
    /* Attempt to match Snorm if the expected image has it */
    return expected == PixelFormat::RGBA16Snorm ? PixelFormat::RGBA16Snorm :
        PixelFormat::RGBA16I;
}
template<> constexpr PixelFormat pixelFormatFor<Math::Vector<4, Short>>(PixelFormat expected) {
    return pixelFormatFor<Math::Vector4<Short>>(expected);
}
template<> constexpr PixelFormat pixelFormatFor<Math::Color4<Short>>(PixelFormat) {
    /* No integer fallback for colors */
    return PixelFormat::RGBA16Snorm;
}
template<> constexpr PixelFormat pixelFormatFor<Math::Vector4<UnsignedInt>>(PixelFormat) {
    return PixelFormat::RGBA32UI;
}
template<> constexpr PixelFormat pixelFormatFor<Math::Vector<4, UnsignedInt>>(PixelFormat expected) {
    return pixelFormatFor<Math::Vector4<UnsignedInt>>(expected);
}
/* Skipping Math::Color4<UnsignedInt>, as integer colors should always match
   normalized types */
template<> constexpr PixelFormat pixelFormatFor<Math::Vector4<Int>>(PixelFormat) {
    return PixelFormat::RGBA32I;
}
template<> constexpr PixelFormat pixelFormatFor<Math::Vector<4, Int>>(PixelFormat expected) {
    return pixelFormatFor<Math::Vector4<Int>>(expected);
}
/* Skipping Math::Color4<Int>, as integer colors should always match normalized
   types */
template<> constexpr PixelFormat pixelFormatFor<Math::Vector4<Half>>(PixelFormat) {
    return PixelFormat::RGBA16F;
}
template<> constexpr PixelFormat pixelFormatFor<Math::Vector<4, Half>>(PixelFormat expected) {
    return pixelFormatFor<Math::Vector4<Half>>(expected);
}
template<> constexpr PixelFormat pixelFormatFor<Math::Color4<Half>>(PixelFormat) {
    return PixelFormat::RGBA16F;
}
template<> constexpr PixelFormat pixelFormatFor<Math::Vector4<Float>>(PixelFormat) {
    return PixelFormat::RGBA32F;
}
template<> constexpr PixelFormat pixelFormatFor<Math::Vector<4, Float>>(PixelFormat expected) {
    return pixelFormatFor<Math::Vector4<Float>>(expected);
}
template<> constexpr PixelFormat pixelFormatFor<Math::Color4<Float>>(PixelFormat) {
    return PixelFormat::RGBA32F;
}

}

}}

#endif
