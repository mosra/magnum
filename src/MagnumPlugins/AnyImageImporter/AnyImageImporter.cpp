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

#include "AnyImageImporter.h"

#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/StringView.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/PluginManager/PluginMetadata.h>
#include <Corrade/Utility/Assert.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/FormatStl.h>
#include <Corrade/Utility/String.h>

#include "Magnum/Trade/ImageData.h"

namespace Magnum { namespace Trade {

AnyImageImporter::AnyImageImporter(PluginManager::Manager<AbstractImporter>& manager): AbstractImporter{manager} {}

AnyImageImporter::AnyImageImporter(PluginManager::AbstractManager& manager, const std::string& plugin): AbstractImporter{manager, plugin} {}

AnyImageImporter::AnyImageImporter(AnyImageImporter&&) noexcept = default;

AnyImageImporter::~AnyImageImporter() = default;

ImporterFeatures AnyImageImporter::doFeatures() const { return ImporterFeature::OpenData; }

bool AnyImageImporter::doIsOpened() const { return !!_in; }

void AnyImageImporter::doClose() {
    _in = nullptr;
}

void AnyImageImporter::doOpenFile(const std::string& filename) {
    CORRADE_INTERNAL_ASSERT(manager());

    /** @todo lowercase only the extension, once Directory::split() is done */
    const std::string normalized = Utility::String::lowercase(filename);

    /* Detect the plugin from extension */
    std::string plugin;
    if(Utility::String::endsWith(normalized, ".basis"))
        plugin = "BasisImporter";
    else if(Utility::String::endsWith(normalized, ".bmp"))
        plugin = "BmpImporter";
    else if(Utility::String::endsWith(normalized, ".dds"))
        plugin = "DdsImporter";
    else if(Utility::String::endsWith(normalized, ".exr"))
        plugin = "OpenExrImporter";
    else if(Utility::String::endsWith(normalized, ".gif"))
        plugin = "GifImporter";
    else if(Utility::String::endsWith(normalized, ".hdr"))
        plugin = "HdrImporter";
    else if(Utility::String::endsWith(normalized, ".ico") ||
            Utility::String::endsWith(normalized, ".cur"))
        plugin = "IcoImporter";
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
        Error{} << "Trade::AnyImageImporter::openFile(): cannot determine the format of" << filename;
        return;
    }
    if(flags() & ImporterFlag::Verbose) {
        Debug d;
        d << "Trade::AnyImageImporter::openFile(): using" << plugin;
        PluginManager::PluginMetadata* metadata = manager()->metadata(plugin);
        CORRADE_INTERNAL_ASSERT(metadata);
        if(plugin != metadata->name())
            d << "(provided by" << metadata->name() << Debug::nospace << ")";
    }

    /* Try to load the plugin */
    if(!(manager()->load(plugin) & PluginManager::LoadState::Loaded)) {
        Error{} << "Trade::AnyImageImporter::openFile(): cannot load the" << plugin << "plugin";
        return;
    }

    /* Instantiate the plugin, propagate flags */
    Containers::Pointer<AbstractImporter> importer = static_cast<PluginManager::Manager<AbstractImporter>*>(manager())->instantiate(plugin);
    importer->setFlags(flags());

    /* Try to open the file (error output should be printed by the plugin
       itself) */
    if(!importer->openFile(filename)) return;

    /* Success, save the instance */
    _in = std::move(importer);
}

void AnyImageImporter::doOpenData(Containers::ArrayView<const char> data) {
    using namespace Containers::Literals;

    CORRADE_INTERNAL_ASSERT(manager());

    /* So we can use the convenient hasSuffix() API */
    const Containers::StringView dataString = data;

    std::string plugin;
    /* https://github.com/BinomialLLC/basis_universal/blob/7d784c728844c007d8c95d63231f7adcc0f65364/transcoder/basisu_file_headers.h#L78 */
    if(dataString.hasPrefix("sB"_s))
        plugin = "BasisImporter";
    /* https://docs.microsoft.com/cs-cz/windows/desktop/direct3ddds/dx-graphics-dds-pguide */
    else if(dataString.hasPrefix("DDS "_s))
        plugin = "DdsImporter";
    /* http://www.openexr.com/openexrfilelayout.pdf */
    else if(dataString.hasPrefix("\x76\x2f\x31\x01"_s))
        plugin = "OpenExrImporter";
    /* https://en.wikipedia.org/wiki/Radiance_(software)#HDR_image_format */
    else if(dataString.hasPrefix("#?RADIANCE"_s))
        plugin = "HdrImporter";
    /* https://en.wikipedia.org/wiki/JPEG#Syntax_and_structure */
    else if(dataString.hasPrefix("\xff\xd8\xff"_s))
        plugin = "JpegImporter";
    /* https://en.wikipedia.org/wiki/Portable_Network_Graphics#File_header */
    else if(dataString.hasPrefix("\x89PNG\x0d\x0a\x1a\x0a"_s))
        plugin = "PngImporter";
    /* http://paulbourke.net/dataformats/tiff/,
       http://paulbourke.net/dataformats/tiff/tiff_summary.pdf */
    else if(dataString.hasPrefix("II\x2a\x00"_s) ||
            dataString.hasPrefix("MM\x00\x2a"_s))
        plugin = "TiffImporter";
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
        /* FFS so much casting to avoid implicit sign extension ruining
           everything */
        UnsignedInt signature = UnsignedInt(UnsignedByte(data[0])) << 24;
        if(data.size() > 1) signature |= UnsignedInt(UnsignedByte(data[1])) << 16;
        if(data.size() > 2) signature |= UnsignedInt(UnsignedByte(data[2])) << 8;
        if(data.size() > 3) signature |= UnsignedInt(UnsignedByte(data[3]));
        /* If there's less than four bytes, cut the rest away */
        Error{} << "Trade::AnyImageImporter::openData(): cannot determine the format from signature 0x" << Debug::nospace << Utility::formatString("{:.8x}", signature).substr(0, data.size() < 4 ? data.size()*2 : std::string::npos);
        return;
    }

    /* Try to load the plugin */
    if(!(manager()->load(plugin) & PluginManager::LoadState::Loaded)) {
        Error{} << "Trade::AnyImageImporter::openData(): cannot load the" << plugin << "plugin";
        return;
    }
    if(flags() & ImporterFlag::Verbose) {
        Debug d;
        d << "Trade::AnyImageImporter::openData(): using" << plugin;
        PluginManager::PluginMetadata* metadata = manager()->metadata(plugin);
        CORRADE_INTERNAL_ASSERT(metadata);
        if(plugin != metadata->name())
            d << "(provided by" << metadata->name() << Debug::nospace << ")";
    }

    /* Instantiate the plugin, propagate flags */
    Containers::Pointer<AbstractImporter> importer = static_cast<PluginManager::Manager<AbstractImporter>*>(manager())->instantiate(plugin);
    importer->setFlags(flags());

    /* Try to open the file (error output should be printed by the plugin
       itself) */
    if(!importer->openData(data)) return;

    /* Success, save the instance */
    _in = std::move(importer);
}

UnsignedInt AnyImageImporter::doImage2DCount() const { return _in->image2DCount(); }

UnsignedInt AnyImageImporter::doImage2DLevelCount(UnsignedInt id) { return _in->image2DLevelCount(id); }

Containers::Optional<ImageData2D> AnyImageImporter::doImage2D(const UnsignedInt id, const UnsignedInt level) { return _in->image2D(id, level); }

}}

CORRADE_PLUGIN_REGISTER(AnyImageImporter, Magnum::Trade::AnyImageImporter,
    "cz.mosra.magnum.Trade.AbstractImporter/0.3.3")
