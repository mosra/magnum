#ifndef Magnum_Trade_AnyImageImporter_h
#define Magnum_Trade_AnyImageImporter_h
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
 * @brief Class @ref Magnum::Trade::AnyImageImporter
 */

#include <Magnum/Trade/AbstractImporter.h>

#include "MagnumPlugins/AnyImageImporter/configure.h"

#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef MAGNUM_ANYIMAGEIMPORTER_BUILD_STATIC
    #if defined(AnyImageImporter_EXPORTS) || defined(AnyImageImporterObjects_EXPORTS)
        #define MAGNUM_ANYIMAGEIMPORTER_EXPORT CORRADE_VISIBILITY_EXPORT
    #else
        #define MAGNUM_ANYIMAGEIMPORTER_EXPORT CORRADE_VISIBILITY_IMPORT
    #endif
#else
    #define MAGNUM_ANYIMAGEIMPORTER_EXPORT CORRADE_VISIBILITY_STATIC
#endif
#define MAGNUM_ANYIMAGEIMPORTER_LOCAL CORRADE_VISIBILITY_LOCAL
#endif

namespace Magnum { namespace Trade {

/**
@brief Any image importer plugin

Detects file type based on file extension, loads corresponding plugin and then
tries to open the file with it.

This plugin is built if `WITH_ANYIMAGEIMPORTER` is enabled when building
Magnum Plugins. To use dynamic plugin, you need to load `AnyImageImporter`
plugin from `MAGNUM_PLUGINS_IMPORTER_DIR`. To use static plugin, you need to
request `AnyImageImporter` component of `MagnumPlugins` package in CMake and
link to `MagnumPlugins::AnyImageImporter` target. See @ref building-plugins,
@ref cmake-plugins and @ref plugins for more information.

Supported formats:

-   Windows Bitmap (`*.bmp`), loaded with any plugin that provides `BmpImporter`
-   DirectDraw Surface (`*.dds`), loaded with @ref DdsImporter or any other
    plugin that provides it
-   Graphics Interchange Format (`*.gif`), loaded with any plugin that provides
    `GifImporter`
-   OpenEXR (`*.exr`), loaded with any plugin that provides `OpenExrImporter`
-   Radiance HDR (`*.hdr`), loaded with any plugin that provides `HdrImporter`
-   JPEG (`*.jpg`, `*.jpe`, `*.jpeg`), loaded with @ref JpegImporter or any
    other plugin that provides it
-   JPEG 2000 (`*.jp2`), loaded with any plugin that provides
    `Jpeg2000Importer`
-   Multiple-image Network Graphics (`*.mng`), loaded with any plugin that
    provides `MngImporter`
-   Portable Bitmap (`*.pbm`), loaded with any plugin that provides `PbmImporter`
-   ZSoft PCX (`*.pcx`), loaded with any plugin that provides `PcxImporter`
-   Portable Graymap (`*.pgm`), loaded with any plugin that provides
    `PgmImporter`
-   Softimage PIC (`*.pic`), loaded with any plugin that provides `PicImporter`
-   Portable Anymap (`*.pnm`), loaded with any plugin that provides
    `PnmImporter`
-   Portable Network Graphics (`*.png`), loaded with @ref PngImporter or any
    other plugin that provides it
-   Portable Pixmap (`*.ppm`), loaded with any plugin that provides `PpmImporter`
-   Adobe Photoshop (`*.psd`), loaded with any plugin that provides `PsdImporter`
-   Silicon Graphics (`*.sgi`, `*.bw`, `*.rgb`, `*.rgba`), loaded with any
    plugin that provides `SgiImporter`
-   Tagged Image File Format (`*.tif`, `*.tiff`), loaded with any plugin that
    provides `TiffImporter`
-   Truevision TGA (`*.tga`, `*.vda`, `*.icb`, `*.vst`), loaded with
    @ref TgaImporter or any other plugin that provides it

Only loading from files is supported.
*/
class MAGNUM_ANYIMAGEIMPORTER_EXPORT AnyImageImporter: public AbstractImporter {
    public:
        /** @brief Constructor with access to plugin manager */
        explicit AnyImageImporter(PluginManager::Manager<AbstractImporter>& manager);

        /** @brief Plugin manager constructor */
        explicit AnyImageImporter(PluginManager::AbstractManager& manager, const std::string& plugin);

        ~AnyImageImporter();

    private:
        MAGNUM_ANYIMAGEIMPORTER_LOCAL Features doFeatures() const override;
        MAGNUM_ANYIMAGEIMPORTER_LOCAL bool doIsOpened() const override;
        MAGNUM_ANYIMAGEIMPORTER_LOCAL void doClose() override;
        MAGNUM_ANYIMAGEIMPORTER_LOCAL void doOpenFile(const std::string& filename) override;

        MAGNUM_ANYIMAGEIMPORTER_LOCAL UnsignedInt doImage2DCount() const override;
        MAGNUM_ANYIMAGEIMPORTER_LOCAL std::optional<ImageData2D> doImage2D(UnsignedInt id) override;

        std::unique_ptr<AbstractImporter> _in;
};

}}

#endif
