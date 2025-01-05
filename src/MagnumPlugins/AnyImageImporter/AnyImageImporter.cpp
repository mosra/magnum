/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2021 Pablo Escobar <mail@rvrs.in>

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
#include <Corrade/Utility/Format.h>
#include <Corrade/Utility/Path.h>
#include <Corrade/Utility/String.h> /* lowercase() */

#include "Magnum/Trade/ImageData.h"
#include "MagnumPlugins/Implementation/propagateConfiguration.h"

namespace Magnum { namespace Trade {

using namespace Containers::Literals;

AnyImageImporter::AnyImageImporter(PluginManager::Manager<AbstractImporter>& manager): AbstractImporter{manager} {}

AnyImageImporter::AnyImageImporter(PluginManager::AbstractManager& manager, const Containers::StringView& plugin): AbstractImporter{manager, plugin} {}

AnyImageImporter::AnyImageImporter(AnyImageImporter&&) noexcept = default;

AnyImageImporter::~AnyImageImporter() = default;

ImporterFeatures AnyImageImporter::doFeatures() const {
    return ImporterFeature::OpenData|ImporterFeature::FileCallback;
}

bool AnyImageImporter::doIsOpened() const { return !!_in; }

void AnyImageImporter::doClose() {
    _in = nullptr;
}

void AnyImageImporter::doOpenFile(const Containers::StringView filename) {
    CORRADE_INTERNAL_ASSERT(manager());

    /* We don't detect any double extensions yet, so we can normalize just the
       extension. In case we eventually might, it'd have to be split() instead
       to save at least by normalizing just the filename and not the path. */
    const Containers::String normalizedExtension = Utility::String::lowercase(Utility::Path::splitExtension(filename).second());

    /* Detect the plugin from extension */
    Containers::StringView plugin;
    if(normalizedExtension == ".astc"_s)
        plugin = "AstcImporter"_s;
    else if(normalizedExtension == ".basis"_s)
        plugin = "BasisImporter"_s;
    else if(normalizedExtension == ".bmp"_s)
        plugin = "BmpImporter"_s;
    else if(normalizedExtension == ".dds"_s)
        plugin = "DdsImporter"_s;
    else if(normalizedExtension == ".exr"_s)
        plugin = "OpenExrImporter"_s;
    else if(normalizedExtension == ".gif"_s)
        plugin = "GifImporter"_s;
    else if(normalizedExtension == ".hdr"_s)
        plugin = "HdrImporter"_s;
    else if(normalizedExtension == ".ico"_s ||
            normalizedExtension == ".cur"_s)
        plugin = "IcoImporter"_s;
    else if(normalizedExtension == ".jpg"_s ||
            normalizedExtension == ".jpeg"_s ||
            normalizedExtension == ".jpe"_s)
        plugin = "JpegImporter"_s;
    else if(normalizedExtension == ".jp2"_s)
        plugin = "Jpeg2000Importer"_s;
    else if(normalizedExtension == ".ktx2"_s) {
        plugin = "KtxImporter"_s;

        /* KtxImporter delegates to BasisImporter in case the file is
           Basis-compressed, so that's a good default choice. However, if it
           isn't available, we should try delegating to BasisImporter instead,
           so people that have just Basis-compressed KTX files don't need to
           have KtxImporter as well.

           BasisImporter unfortunately can't handle non-Basis-compressed KTX
           files, so in case people have just BasisImporter and not
           KtxImporter, it'll fail, but with a clear message suggesting to use
           KtxImporter. If neither BasisImporter would be available, it'd fail
           too (complaining that KtxImporter isn't available), so the behavior
           is roughly the same.

           Further discussion and reasoning here:
           https://github.com/mosra/magnum-plugins/pull/112#discussion_r734976174 */
        if(manager()->loadState("KtxImporter"_s) == PluginManager::LoadState::NotFound &&
           manager()->loadState("BasisImporter"_s) != PluginManager::LoadState::NotFound) {
            if(flags() & ImporterFlag::Verbose)
                Debug{} << "Trade::AnyImageImporter::openFile(): KtxImporter not found, trying a fallback";
            plugin = "BasisImporter"_s;
        }
    } else if(normalizedExtension == ".mng"_s)
        plugin = "MngImporter"_s;
    else if(normalizedExtension == ".pbm"_s)
        plugin = "PbmImporter"_s;
    else if(normalizedExtension == ".pcx"_s)
        plugin = "PcxImporter"_s;
    else if(normalizedExtension == ".pgm"_s)
        plugin = "PgmImporter"_s;
    else if(normalizedExtension == ".pic"_s)
        plugin = "PicImporter"_s;
    else if(normalizedExtension == ".pnm"_s)
        plugin = "PnmImporter"_s;
    else if(normalizedExtension == ".png"_s)
        plugin = "PngImporter"_s;
    else if(normalizedExtension == ".ppm"_s)
        plugin = "PpmImporter"_s;
    else if(normalizedExtension == ".psd"_s)
        plugin = "PsdImporter"_s;
    else if(normalizedExtension == ".sgi"_s ||
            normalizedExtension == ".bw"_s ||
            normalizedExtension == ".rgb"_s ||
            normalizedExtension == ".rgba"_s)
        plugin = "SgiImporter"_s;
    else if(normalizedExtension == ".tif"_s ||
            normalizedExtension == ".tiff"_s)
        plugin = "TiffImporter"_s;
    else if(normalizedExtension == ".tga"_s ||
            normalizedExtension == ".vda"_s ||
            normalizedExtension == ".icb"_s ||
            normalizedExtension == ".vst"_s)
        plugin = "TgaImporter"_s;
    else if(normalizedExtension == ".vdb"_s)
        plugin = "OpenVdbImporter"_s;
    else if(normalizedExtension == ".webp"_s)
        plugin = "WebPImporter"_s;
    else {
        Error{} << "Trade::AnyImageImporter::openFile(): cannot determine the format of" << filename;
        return;
    }

    /* Try to load the plugin */
    if(!(manager()->load(plugin) & PluginManager::LoadState::Loaded)) {
        Error{} << "Trade::AnyImageImporter::openFile(): cannot load the" << plugin << "plugin";
        return;
    }

    const PluginManager::PluginMetadata* const metadata = manager()->metadata(plugin);
    CORRADE_INTERNAL_ASSERT(metadata);
    if(flags() & ImporterFlag::Verbose) {
        Debug d;
        d << "Trade::AnyImageImporter::openFile(): using" << plugin;
        if(plugin != metadata->name())
            d << "(provided by" << metadata->name() << Debug::nospace << ")";
    }

    /* Instantiate the plugin, propagate flags and the file callback, if set */
    Containers::Pointer<AbstractImporter> importer = static_cast<PluginManager::Manager<AbstractImporter>*>(manager())->instantiate(plugin);
    importer->setFlags(flags());
    if(fileCallback()) importer->setFileCallback(fileCallback(), fileCallbackUserData());

    /* Propagate configuration */
    Magnum::Implementation::propagateConfiguration("Trade::AnyImageImporter::openFile():", {}, metadata->name(), configuration(), importer->configuration(), !(flags() & ImporterFlag::Quiet));

    /* Try to open the file (error output should be printed by the plugin
       itself) */
    if(!importer->openFile(filename)) return;

    /* Success, save the instance */
    _in = Utility::move(importer);
}

void AnyImageImporter::doOpenData(Containers::Array<char>&& data, DataFlags) {
    using namespace Containers::Literals;

    CORRADE_INTERNAL_ASSERT(manager());

    /* So we can use the convenient hasPrefix() API */
    const Containers::ArrayView<const char> dataView = data;
    const Containers::StringView dataString = dataView;

    Containers::StringView plugin;
    /* https://stackoverflow.com/questions/22600678/determine-internal-format-of-given-astc-compressed-image-through-its-header
       unfortunately it being in LE means it's SCALABLE in reverse :) */
    if(dataString.hasPrefix("\x13\xAB\xA1\x5C"_s))
        plugin = "AstcImporter"_s;
    /* https://github.com/BinomialLLC/basis_universal/blob/7d784c728844c007d8c95d63231f7adcc0f65364/transcoder/basisu_file_headers.h#L78 */
    else if(dataString.hasPrefix("sB"_s))
        plugin = "BasisImporter"_s;
    /* https://en.wikipedia.org/wiki/BMP_file_format#Bitmap_file_header */
    else if(dataString.hasPrefix("BM"_s))
        plugin = "BmpImporter"_s;
    /* https://docs.microsoft.com/cs-cz/windows/desktop/direct3ddds/dx-graphics-dds-pguide */
    else if(dataString.hasPrefix("DDS "_s))
        plugin = "DdsImporter"_s;
    /* http://www.openexr.com/openexrfilelayout.pdf */
    else if(dataString.hasPrefix("\x76\x2f\x31\x01"_s))
        plugin = "OpenExrImporter"_s;
    /* https://en.wikipedia.org/wiki/Radiance_(software)#HDR_image_format and
       https://en.wikipedia.org/wiki/RGBE_image_format which lists also the \n
       at the end. There's also a RGBE signature that isn't mentioned on
       Wikipedia, at https://paulbourke.net/dataformats/pic/ or used by the
       file utility https://github.com/file/file/blob/0fa2c8c3e64c372d038d46969bafaaa09a13a87b/magic/Magdir/images#L2755-L2759
       but is used by https://www.graphics.cornell.edu/~bjw/rgbe/rgbe.c which
       is subsequently derived from in e.g. https://github.com/kopaka1822/ImageViewer/blob/5ec358cf5c3f818c0cc4c363f5ec0c61aa99d372/dependencies/hdr/rgbe.h#L210
       and stb_image recognizes that as well. */
    else if(dataString.hasPrefix("#?RADIANCE\n"_s) ||
            dataString.hasPrefix("#?RGBE\n"_s))
        plugin = "HdrImporter"_s;
    /* https://en.wikipedia.org/wiki/JPEG#Syntax_and_structure */
    else if(dataString.hasPrefix("\xff\xd8\xff"_s))
        plugin = "JpegImporter"_s;
    /* https://github.khronos.org/KTX-Specification/#_identifier */
    else if(dataString.hasPrefix("\xabKTX 20\xbb\r\n\x1a\n"_s)) {
        plugin = "KtxImporter"_s;

        /* Same logic as in doOpenFile() for *.ktx2 files, see above for more
           information */
        if(manager()->loadState("KtxImporter"_s) == PluginManager::LoadState::NotFound &&
           manager()->loadState("BasisImporter"_s) != PluginManager::LoadState::NotFound) {
            if(flags() & ImporterFlag::Verbose)
                Debug{} << "Trade::AnyImageImporter::openData(): KtxImporter not found, trying a fallback";
            plugin = "BasisImporter"_s;
        }
    /* https://en.wikipedia.org/wiki/Portable_Network_Graphics#File_header */
    } else if(dataString.hasPrefix("\x89PNG\x0d\x0a\x1a\x0a"_s))
        plugin = "PngImporter"_s;
    /* http://paulbourke.net/dataformats/tiff/,
       http://paulbourke.net/dataformats/tiff/tiff_summary.pdf */
    else if(dataString.hasPrefix("II\x2a\x00"_s) ||
            dataString.hasPrefix("MM\x00\x2a"_s))
        plugin = "TiffImporter"_s;
    /* https://developers.google.com/speed/webp/docs/riff_container#webp_file_header */
    else if(dataString.size() >= 12 &&
            dataString.slice(0,  4) == "RIFF"_s &&
            dataString.slice(8, 12) == "WEBP"_s)
        plugin = "WebPImporter"_s;
    /* https://github.com/file/file/blob/d04de269e0b06ccd0a7d1bf4974fed1d75be7d9e/magic/Magdir/images#L18-L22
       TGAs are a complete guesswork, so try after everything else fails. */
    else if([dataView]() {
            /* TGA header is 18 bytes */
            if(dataView.size() < 18) return false;

            /* Third byte (image type) must be one of these */
            if(dataView[2] != 1 && dataView[2] != 2  && dataView[2] != 3 &&
               dataView[2] != 9 && dataView[2] != 10 && dataView[2] != 11) return false;

            /* If image type is 1 or 9, second byte (colormap type) must be 1 */
            if((dataView[2] == 1 || dataView[2] == 9) && dataView[1] != 1) return false;

            /* ... and 0 otherwise */
            if(dataView[2] != 1 && dataView[2] != 9 && dataView[1] != 0) return false;

            /* Colormap index (unsigned short, byte 3+4) should be 0 */
            if(dataView[3] != 0 && dataView[4] != 0) return false;

            /* Probably TGA, heh. Or random memory. */
            return true;
        }()) plugin = "TgaImporter"_s;
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
        Error{} << "Trade::AnyImageImporter::openData(): cannot determine the format from signature 0x" << Debug::nospace << Utility::format("{:.8x}", signature).prefix(Math::min(data.size(), std::size_t{4})*2);
        return;
    }

    /* Try to load the plugin */
    if(!(manager()->load(plugin) & PluginManager::LoadState::Loaded)) {
        Error{} << "Trade::AnyImageImporter::openData(): cannot load the" << plugin << "plugin";
        return;
    }

    const PluginManager::PluginMetadata* const metadata = manager()->metadata(plugin);
    CORRADE_INTERNAL_ASSERT(metadata);
    if(flags() & ImporterFlag::Verbose) {
        Debug d;
        d << "Trade::AnyImageImporter::openData(): using" << plugin;
        if(plugin != metadata->name())
            d << "(provided by" << metadata->name() << Debug::nospace << ")";
    }

    /* Instantiate the plugin, propagate flags. File callbacks not propagated
       here as no image importers currently load any extra files. */
    /** @todo revisit callbacks when that becomes true (such as loading XMP
        files accompanying RAWs) */
    Containers::Pointer<AbstractImporter> importer = static_cast<PluginManager::Manager<AbstractImporter>*>(manager())->instantiate(plugin);
    importer->setFlags(flags());

    /* Propagate configuration */
    Magnum::Implementation::propagateConfiguration("Trade::AnyImageImporter::openData():", {}, metadata->name(), configuration(), importer->configuration(), !(flags() & ImporterFlag::Quiet));

    /* Try to open the file (error output should be printed by the plugin
       itself) */
    if(!importer->openData(data)) return;

    /* Success, save the instance */
    _in = Utility::move(importer);
}

UnsignedInt AnyImageImporter::doImage1DCount() const { return _in->image1DCount(); }

UnsignedInt AnyImageImporter::doImage1DLevelCount(UnsignedInt id) { return _in->image1DLevelCount(id); }

Containers::Optional<ImageData1D> AnyImageImporter::doImage1D(const UnsignedInt id, const UnsignedInt level) { return _in->image1D(id, level); }

UnsignedInt AnyImageImporter::doImage2DCount() const { return _in->image2DCount(); }

UnsignedInt AnyImageImporter::doImage2DLevelCount(UnsignedInt id) { return _in->image2DLevelCount(id); }

Containers::Optional<ImageData2D> AnyImageImporter::doImage2D(const UnsignedInt id, const UnsignedInt level) { return _in->image2D(id, level); }

UnsignedInt AnyImageImporter::doImage3DCount() const { return _in->image3DCount(); }

UnsignedInt AnyImageImporter::doImage3DLevelCount(UnsignedInt id) { return _in->image3DLevelCount(id); }

Containers::Optional<ImageData3D> AnyImageImporter::doImage3D(const UnsignedInt id, const UnsignedInt level) { return _in->image3D(id, level); }

}}

CORRADE_PLUGIN_REGISTER(AnyImageImporter, Magnum::Trade::AnyImageImporter,
    MAGNUM_TRADE_ABSTRACTIMPORTER_PLUGIN_INTERFACE)
