#ifndef Magnum_Trade_AnyImageImporter_h
#define Magnum_Trade_AnyImageImporter_h
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
 * @brief Class @ref Magnum::Trade::AnyImageImporter
 */

#include "Magnum/Trade/AbstractImporter.h"
#include "MagnumPlugins/AnyImageImporter/configure.h"

#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef MAGNUM_ANYIMAGEIMPORTER_BUILD_STATIC
    #ifdef AnyImageImporter_EXPORTS
        #define MAGNUM_ANYIMAGEIMPORTER_EXPORT CORRADE_VISIBILITY_EXPORT
    #else
        #define MAGNUM_ANYIMAGEIMPORTER_EXPORT CORRADE_VISIBILITY_IMPORT
    #endif
#else
    #define MAGNUM_ANYIMAGEIMPORTER_EXPORT CORRADE_VISIBILITY_STATIC
#endif
#define MAGNUM_ANYIMAGEIMPORTER_LOCAL CORRADE_VISIBILITY_LOCAL
#else
#define MAGNUM_ANYIMAGEIMPORTER_EXPORT
#define MAGNUM_ANYIMAGEIMPORTER_LOCAL
#endif

namespace Magnum { namespace Trade {

/**
@brief Any image importer plugin

Detects file type based on file extension or a signature at the start of the
file, loads corresponding plugin and then tries to open the file with it.
Supported formats:

-   Adaptive Scalable Texture Compression (`*.astc` or data with corresponding
    signature), loaded with @ref AstcImporter or any other plugin that provides
    it
-   Basis Universal (`*.basis` or data with corresponding signature), loaded
    with @ref BasisImporter or any other plugin that provides it
-   Windows Bitmap (`*.bmp` or data with corresponding signature), loaded with
    any plugin that provides `BmpImporter`
-   DirectDraw Surface (`*.dds` or data with corresponding signature), loaded
    with @ref DdsImporter or any other plugin that provides it
-   Graphics Interchange Format (`*.gif`), loaded with any plugin that provides
    `GifImporter`
-   OpenEXR (`*.exr` or data with corresponding signature), loaded with
    @ref OpenExrImporter or any other plugin that provides it
-   Radiance HDR (`*.hdr` or data with corresponding signature), loaded with
    any plugin that provides `HdrImporter`
-   Windows icon/cursor (`*.ico`, `*.cur`), loaded with @ref IcoImporter or any
    other plugin that provides it
-   JPEG (`*.jpg`, `*.jpe`, `*.jpeg` or data with corresponding signature),
    loaded with @ref JpegImporter or any other plugin that provides it
-   JPEG 2000 (`*.jp2`), loaded with any plugin that provides
    `Jpeg2000Importer`
-   KTX2 (`*.ktx2` or data with corresponding signature), loaded with
    @ref KtxImporter or any other plugin that provides it. If not found,
    @ref BasisImporter is tried as a fallback.
-   Multiple-image Network Graphics (`*.mng`), loaded with any plugin that
    provides `MngImporter`
-   Portable Bitmap, Graymap, Pixmap, Anymap (`*.pbm`, `*.pgm`, `*.ppm`,
    `*.pnm`), loaded with any plugin that provides `PbmImporter`,
    `PgmImporter`, `PpmImporter` or `PnmImporter`, respectively
-   ZSoft PCX (`*.pcx`), loaded with any plugin that provides `PcxImporter`
-   Softimage PIC (`*.pic`), loaded with any plugin that provides `PicImporter`
-   Portable Network Graphics (`*.png` or data with corresponding signature),
    loaded with @ref PngImporter or any other plugin that provides it
-   Adobe Photoshop (`*.psd`), loaded with any plugin that provides `PsdImporter`
-   Silicon Graphics (`*.sgi`, `*.bw`, `*.rgb`, `*.rgba`), loaded with any
    plugin that provides `SgiImporter`
-   Tagged Image File Format (`*.tif`, `*.tiff` or data with corresponding
    signature), loaded with any plugin that provides `TiffImporter`
-   Truevision TGA (`*.tga`, `*.vda`, `*.icb`, `*.vst` or data with
    corresponding signature), loaded with @ref TgaImporter or any other plugin
    that provides it
-   OpenVDB (`*.vdb`), loaded with any plugin that provides `OpenVdbImporter`
-   WebP (`*.webp` or data with corresponding signature), loaded with
    @ref WebPImporter or any other plugin that provides it

Detecting file type through @ref openData() is supported only for a subset of
formats that are marked as such in the list above.
@ref ImporterFeature::FileCallback is supported as well.

@section Trade-AnyImageImporter-usage Usage

@m_class{m-note m-success}

@par
    This class is a plugin that's meant to be dynamically loaded and used
    through the base @ref AbstractImporter interface. See its documentation for
    introduction and usage examples.

This plugin depends on the @ref Trade library and is built if
`MAGNUM_WITH_ANYIMAGEIMPORTER` is enabled when building Magnum. To use as a
dynamic plugin, load @cpp "AnyImageImporter" @ce via
@ref Corrade::PluginManager::Manager.

Additionally, if you're using Magnum as a CMake subproject, do the following:

@code{.cmake}
set(MAGNUM_WITH_ANYIMAGEIMPORTER ON CACHE BOOL "" FORCE)
add_subdirectory(magnum EXCLUDE_FROM_ALL)

# So the dynamically loaded plugin gets built implicitly
add_dependencies(your-app Magnum::AnyImageImporter)
@endcode

To use as a static plugin or as a dependency of another plugin with CMake, you
need to request the `AnyImageImporter` component of the `Magnum` package and
link to the `Magnum::AnyImageImporter` target:

@code{.cmake}
find_package(Magnum REQUIRED AnyImageImporter)

# ...
target_link_libraries(your-app PRIVATE Magnum::AnyImageImporter)
@endcode

See @ref building, @ref cmake, @ref plugins and @ref file-formats for more
information.

@section Audio-AnyImageImporter-proxy Interface proxying and option propagation

On a call to @ref openFile() / @ref openData(), a file format is detected from
the extension / file signature and a corresponding plugin is loaded. After
that, flags set via @ref setFlags(), file callbacks set via
@ref setFileCallback() and options set through @ref configuration() are
propagated to the concrete implementation. A warning is emitted in case an
option set is not present in the default configuration of the target plugin.

Calls to the @ref image1DCount() / @ref image2DCount() / @ref image3DCount(),
@ref image1DLevelCount() / @ref image2DLevelCount() / @ref image3DLevelCount()
and @ref image1D() / @ref image2D() / @ref image3D() functions are then proxied
to the concrete implementation. The @ref close() function closes and discards
the internally instantiated plugin; @ref isOpened() works as usual.

Besides delegating the flags, the @ref AnyImageImporter itself recognizes
@ref ImporterFlag::Verbose, printing info about the concrete plugin being used
when the flag is enabled. @ref ImporterFlag::Quiet is recognized as well and
causes all warnings to be suppressed.
*/
class MAGNUM_ANYIMAGEIMPORTER_EXPORT AnyImageImporter: public AbstractImporter {
    public:
        /** @brief Constructor with access to plugin manager */
        explicit AnyImageImporter(PluginManager::Manager<AbstractImporter>& manager);

        /** @brief Plugin manager constructor */
        explicit AnyImageImporter(PluginManager::AbstractManager& manager, const Containers::StringView& plugin);

        /** @brief Copying is not allowed */
        AnyImageImporter(const AnyImageImporter&) = delete;

        /**
         * @brief Move constructor
         *
         * See @ref Corrade::PluginManager::AbstractPlugin::AbstractPlugin(AbstractPlugin&&)
         * for caveats.
        */
        AnyImageImporter(AnyImageImporter&&) noexcept;

        /** @brief Copying is not allowed */
        AnyImageImporter& operator=(const AnyImageImporter&) = delete;

        /** @brief Only move construction is allowed */
        AnyImageImporter& operator=(AnyImageImporter&&) = delete;

        ~AnyImageImporter();

    private:
        MAGNUM_ANYIMAGEIMPORTER_LOCAL ImporterFeatures doFeatures() const override;
        MAGNUM_ANYIMAGEIMPORTER_LOCAL bool doIsOpened() const override;
        MAGNUM_ANYIMAGEIMPORTER_LOCAL void doClose() override;
        MAGNUM_ANYIMAGEIMPORTER_LOCAL void doOpenFile(Containers::StringView filename) override;
        MAGNUM_ANYIMAGEIMPORTER_LOCAL void doOpenData(Containers::Array<char>&& data, DataFlags dataFlags) override;

        MAGNUM_ANYIMAGEIMPORTER_LOCAL UnsignedInt doImage1DCount() const override;
        MAGNUM_ANYIMAGEIMPORTER_LOCAL UnsignedInt doImage1DLevelCount(UnsignedInt id) override;
        MAGNUM_ANYIMAGEIMPORTER_LOCAL Containers::Optional<ImageData1D> doImage1D(UnsignedInt id, UnsignedInt level) override;

        MAGNUM_ANYIMAGEIMPORTER_LOCAL UnsignedInt doImage2DCount() const override;
        MAGNUM_ANYIMAGEIMPORTER_LOCAL UnsignedInt doImage2DLevelCount(UnsignedInt id) override;
        MAGNUM_ANYIMAGEIMPORTER_LOCAL Containers::Optional<ImageData2D> doImage2D(UnsignedInt id, UnsignedInt level) override;

        MAGNUM_ANYIMAGEIMPORTER_LOCAL UnsignedInt doImage3DCount() const override;
        MAGNUM_ANYIMAGEIMPORTER_LOCAL UnsignedInt doImage3DLevelCount(UnsignedInt id) override;
        MAGNUM_ANYIMAGEIMPORTER_LOCAL Containers::Optional<ImageData3D> doImage3D(UnsignedInt id, UnsignedInt level) override;

        Containers::Pointer<AbstractImporter> _in;
};

}}

#endif
