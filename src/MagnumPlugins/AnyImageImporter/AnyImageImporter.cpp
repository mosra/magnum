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

#include "AnyImageImporter.h"

#include <Corrade/Containers/Optional.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Utility/Assert.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/String.h>

#include "Magnum/Trade/ImageData.h"

namespace Magnum { namespace Trade {

AnyImageImporter::AnyImageImporter(PluginManager::Manager<AbstractImporter>& manager): AbstractImporter{manager} {}

AnyImageImporter::AnyImageImporter(PluginManager::AbstractManager& manager, const std::string& plugin): AbstractImporter{manager, plugin} {}

AnyImageImporter::~AnyImageImporter() = default;

auto AnyImageImporter::doFeatures() const -> Features { return Feature::OpenData; }

bool AnyImageImporter::doIsOpened() const { return !!_in; }

void AnyImageImporter::doClose() {
    _in = nullptr;
}

void AnyImageImporter::doOpenFile(const std::string& filename) {
    CORRADE_INTERNAL_ASSERT(manager());

    /** @todo lowercase only the extension, once Directory::split() is done */
    const std::string normalized = Utility::String::lowercase(filename);

    /* Detect type from extension */
    std::string plugin;
    if(Utility::String::endsWith(normalized, ".bmp"))
        plugin = "BmpImporter";
    else if(Utility::String::endsWith(normalized, ".dds"))
        plugin = "DdsImporter";
    else if(Utility::String::endsWith(normalized, ".exr"))
        plugin = "OpenExrImporter";
    else if(Utility::String::endsWith(normalized, ".gif"))
        plugin = "GifImporter";
    else if(Utility::String::endsWith(normalized, ".hdr"))
        plugin = "HdrImporter";
    else if(Utility::String::endsWith(normalized, ".jpg") ||
            Utility::String::endsWith(normalized, ".jpeg") ||
            Utility::String::endsWith(normalized, ".jpe"))
        plugin = "JpegImporter";
    else if(Utility::String::endsWith(normalized, ".jp2"))
        plugin = "Jpeg2000Importer";
    else if(Utility::String::endsWith(normalized, ".mng"))
        plugin = "MngImporter";
    else if(Utility::String::endsWith(normalized, ".pbm"))
        plugin = "PbmImporter";
    else if(Utility::String::endsWith(normalized, ".pcx"))
        plugin = "PcxImporter";
    else if(Utility::String::endsWith(normalized, ".pgm"))
        plugin = "PgmImporter";
    else if(Utility::String::endsWith(normalized, ".pic"))
        plugin = "PicImporter";
    else if(Utility::String::endsWith(normalized, ".pnm"))
        plugin = "PnmImporter";
    else if(Utility::String::endsWith(normalized, ".png"))
        plugin = "PngImporter";
    else if(Utility::String::endsWith(normalized, ".ppm"))
        plugin = "PpmImporter";
    else if(Utility::String::endsWith(normalized, ".psd"))
        plugin = "PsdImporter";
    else if(Utility::String::endsWith(normalized, ".sgi") ||
            Utility::String::endsWith(normalized, ".bw") ||
            Utility::String::endsWith(normalized, ".rgb") ||
            Utility::String::endsWith(normalized, ".rgba"))
        plugin = "SgiImporter";
    else if(Utility::String::endsWith(normalized, ".tif") ||
            Utility::String::endsWith(normalized, ".tiff"))
        plugin = "TiffImporter";
    else if(Utility::String::endsWith(normalized, ".tga") ||
            Utility::String::endsWith(normalized, ".vda") ||
            Utility::String::endsWith(normalized, ".icb") ||
            Utility::String::endsWith(normalized, ".vst"))
        plugin = "TgaImporter";
    else {
        Error() << "Trade::AnyImageImporter::openFile(): cannot determine type of file" << filename;
        return;
    }

    /* Try to load the plugin */
    if(!(manager()->load(plugin) & PluginManager::LoadState::Loaded)) {
        Error() << "Trade::AnyImageImporter::openFile(): cannot load" << plugin << "plugin";
        return;
    }

    /* Try to open the file (error output should be printed by the plugin
       itself) */
    Containers::Pointer<AbstractImporter> importer = static_cast<PluginManager::Manager<AbstractImporter>*>(manager())->instantiate(plugin);
    if(!importer->openFile(filename)) return;

    /* Success, save the instance */
    _in = std::move(importer);
}

void AnyImageImporter::doOpenData(Containers::ArrayView<const char> data) {
    CORRADE_INTERNAL_ASSERT(manager());

    std::string plugin;
    /* https://docs.microsoft.com/cs-cz/windows/desktop/direct3ddds/dx-graphics-dds-pguide */
    if(Utility::String::viewBeginsWith(data, "DDS "))
        plugin = "DdsImporter";
    /* http://www.openexr.com/openexrfilelayout.pdf */
    else if(Utility::String::viewBeginsWith(data, "\x76\x2f\x31\x01"))
        plugin = "OpenExrImporter";
    /* https://en.wikipedia.org/wiki/Radiance_(software)#HDR_image_format */
    else if(Utility::String::viewBeginsWith(data, "#?RADIANCE"))
        plugin = "HdrImporter";
    /* https://en.wikipedia.org/wiki/JPEG#Syntax_and_structure */
    else if(Utility::String::viewBeginsWith(data, "\xff\xd8\xff"))
        plugin = "JpegImporter";
    /* https://en.wikipedia.org/wiki/Portable_Network_Graphics#File_header */
    else if(Utility::String::viewBeginsWith(data, "\x89PNG\x0d\x0a\x1a\x0a"))
        plugin = "PngImporter";
    /* https://github.com/file/file/blob/d04de269e0b06ccd0a7d1bf4974fed1d75be7d9e/magic/Magdir/images#L18-L22
       TGAs are a complete guesswork, so try after everything else fails. */
    else if([data]() {
            /* TGA header is 18 bytes */
            if(data.size() < 18) return false;

            /* Third byte (image type) must be one of these */
            if(data[2] != 1 && data[2] != 2  && data[2] != 3 &&
               data[2] != 9 && data[2] != 10 && data[2] != 11) return false;

            /* If image type is 1 or 9, second byte (colormap type) must be 1 */
            if((data[2] == 1 || data[2] == 9) && data[1] != 1) return false;

            /* ... and 0 otherwise */
            if(data[2] != 1 && data[2] != 9 && data[1] != 0) return false;

            /* Colormap index (unsigned short, byte 3+4) should be 0 */
            if(data[3] != 0 && data[4] != 0) return false;

            /* Probably TGA, heh. Or random memory. */
            return true;
        }()) plugin = "TgaImporter";
    else if(!data.size()) {
        Error{} << "Trade::AnyImageImporter::openData(): file is empty";
        return;
    } else {
        std::uint32_t signature = data[0] << 24;
        if(data.size() > 1) signature |= data[1] << 16;
        if(data.size() > 2) signature |= data[2] << 8;
        if(data.size() > 3) signature |= data[3];
        Error() << "Trade::AnyImageImporter::openData(): cannot determine type from signature" << reinterpret_cast<void*>(signature);
        return;
    }

    /* Try to load the plugin */
    if(!(manager()->load(plugin) & PluginManager::LoadState::Loaded)) {
        Error() << "Trade::AnyImageImporter::openData(): cannot load" << plugin << "plugin";
        return;
    }

    /* Try to open the file (error output should be printed by the plugin
       itself) */
    Containers::Pointer<AbstractImporter> importer = static_cast<PluginManager::Manager<AbstractImporter>*>(manager())->instantiate(plugin);
    if(!importer->openData(data)) return;

    /* Success, save the instance */
    _in = std::move(importer);
}

UnsignedInt AnyImageImporter::doImage2DCount() const { return _in->image2DCount(); }

Containers::Optional<ImageData2D> AnyImageImporter::doImage2D(const UnsignedInt id) { return _in->image2D(id); }

}}

CORRADE_PLUGIN_REGISTER(AnyImageImporter, Magnum::Trade::AnyImageImporter,
    "cz.mosra.magnum.Trade.AbstractImporter/0.3")
