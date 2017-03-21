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

#include "AnyImageImporter.h"

#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Utility/Assert.h>
#include <Corrade/Utility/String.h>
#include <Magnum/Trade/ImageData.h>

namespace Magnum { namespace Trade {

AnyImageImporter::AnyImageImporter(PluginManager::Manager<AbstractImporter>& manager): AbstractImporter{manager} {}

AnyImageImporter::AnyImageImporter(PluginManager::AbstractManager& manager, const std::string& plugin): AbstractImporter{manager, plugin} {}

AnyImageImporter::~AnyImageImporter() = default;

auto AnyImageImporter::doFeatures() const -> Features { return {}; }

bool AnyImageImporter::doIsOpened() const { return !!_in; }

void AnyImageImporter::doClose() {
    _in = nullptr;
}

void AnyImageImporter::doOpenFile(const std::string& filename) {
    CORRADE_INTERNAL_ASSERT(manager());

    /* Detect type from extension */
    std::string plugin;
    if(Utility::String::endsWith(filename, ".bmp"))
        plugin = "BmpImporter";
    else if(Utility::String::endsWith(filename, ".dds"))
        plugin = "DdsImporter";
    else if(Utility::String::endsWith(filename, ".exr"))
        plugin = "OpenExrImporter";
    else if(Utility::String::endsWith(filename, ".gif"))
        plugin = "GifImporter";
    else if(Utility::String::endsWith(filename, ".hdr"))
        plugin = "HdrImporter";
    else if(Utility::String::endsWith(filename, ".jpg") ||
            Utility::String::endsWith(filename, ".jpeg") ||
            Utility::String::endsWith(filename, ".jpe") )
        plugin = "JpegImporter";
    else if(Utility::String::endsWith(filename, ".jp2"))
        plugin = "Jpeg2000Importer";
    else if(Utility::String::endsWith(filename, ".mng"))
        plugin = "MngImporter";
    else if(Utility::String::endsWith(filename, ".pbm"))
        plugin = "PbmImporter";
    else if(Utility::String::endsWith(filename, ".pcx"))
        plugin = "PcxImporter";
    else if(Utility::String::endsWith(filename, ".pgm"))
        plugin = "PgmImporter";
    else if(Utility::String::endsWith(filename, ".pic"))
        plugin = "PicImporter";
    else if(Utility::String::endsWith(filename, ".pnm"))
        plugin = "PnmImporter";
    else if(Utility::String::endsWith(filename, ".png"))
        plugin = "PngImporter";
    else if(Utility::String::endsWith(filename, ".ppm"))
        plugin = "PpmImporter";
    else if(Utility::String::endsWith(filename, ".psd"))
        plugin = "PsdImporter";
    else if(Utility::String::endsWith(filename, ".sgi") ||
            Utility::String::endsWith(filename, ".bw") ||
            Utility::String::endsWith(filename, ".rgb") ||
            Utility::String::endsWith(filename, ".rgba"))
        plugin = "SgiImporter";
    else if(Utility::String::endsWith(filename, ".tif") ||
            Utility::String::endsWith(filename, ".tiff"))
        plugin = "TiffImporter";
    else if(Utility::String::endsWith(filename, ".tga") ||
            Utility::String::endsWith(filename, ".vda") ||
            Utility::String::endsWith(filename, ".icb") ||
            Utility::String::endsWith(filename, ".vst"))
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
    std::unique_ptr<AbstractImporter> importer = static_cast<PluginManager::Manager<AbstractImporter>*>(manager())->instance(plugin);
    if(!importer->openFile(filename)) return;

    /* Success, save the instance */
    _in = std::move(importer);
}

UnsignedInt AnyImageImporter::doImage2DCount() const { return _in->image2DCount(); }

std::optional<ImageData2D> AnyImageImporter::doImage2D(const UnsignedInt id) { return _in->image2D(id); }

}}
