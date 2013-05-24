/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

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

#include "AbstractImporter.h"

#include <Utility/Assert.h>

namespace Magnum { namespace Trade {

AbstractImporter::AbstractImporter() = default;

AbstractImporter::AbstractImporter(Corrade::PluginManager::AbstractManager* manager, std::string plugin): AbstractPlugin(manager, std::move(plugin)) {}

bool AbstractImporter::openData(const void* const, const std::size_t) {
    CORRADE_ASSERT(features() & Feature::OpenData,
        "Trade::AbstractImporter::openData(): feature advertised but not implemented", nullptr);

    CORRADE_ASSERT(false, "Trade::AbstractImporter::openData(): feature not implemented", nullptr);
}

bool AbstractImporter::openFile(const std::string&) {
    CORRADE_ASSERT(features() & Feature::OpenFile,
        "Trade::AbstractImporter::openFile(): feature advertised but not implemented", nullptr);

    CORRADE_ASSERT(false, "Trade::AbstractImporter::openFile(): feature not implemented", nullptr);
}

Int AbstractImporter::sceneForName(const std::string&) { return -1; }
std::string AbstractImporter::sceneName(UnsignedInt) { return {}; }
SceneData* AbstractImporter::scene(UnsignedInt) { return nullptr; }
Int AbstractImporter::lightForName(const std::string&) { return -1; }
std::string AbstractImporter::lightName(UnsignedInt) { return {}; }
LightData* AbstractImporter::light(UnsignedInt) { return nullptr; }
Int AbstractImporter::cameraForName(const std::string&) { return -1; }
std::string AbstractImporter::cameraName(UnsignedInt) { return {}; }
CameraData* AbstractImporter::camera(UnsignedInt) { return nullptr; }
Int AbstractImporter::object2DForName(const std::string&) { return -1; }
std::string AbstractImporter::object2DName(UnsignedInt) { return {}; }
ObjectData2D* AbstractImporter::object2D(UnsignedInt) { return nullptr; }
Int AbstractImporter::object3DForName(const std::string&) { return -1; }
std::string AbstractImporter::object3DName(UnsignedInt) { return {}; }
ObjectData3D* AbstractImporter::object3D(UnsignedInt) { return nullptr; }
Int AbstractImporter::mesh2DForName(const std::string&) { return -1; }
std::string AbstractImporter::mesh2DName(UnsignedInt) { return {}; }
MeshData2D* AbstractImporter::mesh2D(UnsignedInt) { return nullptr; }
Int AbstractImporter::mesh3DForName(const std::string&) { return -1; }
std::string AbstractImporter::mesh3DName(UnsignedInt) { return {}; }
MeshData3D* AbstractImporter::mesh3D(UnsignedInt) { return nullptr; }
Int AbstractImporter::materialForName(const std::string&) { return -1; }
std::string AbstractImporter::materialName(UnsignedInt) { return {}; }
AbstractMaterialData* AbstractImporter::material(UnsignedInt) { return nullptr; }
Int AbstractImporter::textureForName(const std::string&) { return -1; }
std::string AbstractImporter::textureName(UnsignedInt) { return {}; }
TextureData* AbstractImporter::texture(UnsignedInt) { return nullptr; }
Int AbstractImporter::image1DForName(const std::string&) { return -1; }
std::string AbstractImporter::image1DName(UnsignedInt) { return {}; }
ImageData1D* AbstractImporter::image1D(UnsignedInt) { return nullptr; }
Int AbstractImporter::image2DForName(const std::string&) { return -1; }
std::string AbstractImporter::image2DName(UnsignedInt) { return {}; }
ImageData2D* AbstractImporter::image2D(UnsignedInt) { return nullptr; }
Int AbstractImporter::image3DForName(const std::string&) { return -1; }
std::string AbstractImporter::image3DName(UnsignedInt) { return {}; }
ImageData3D* AbstractImporter::image3D(UnsignedInt) { return nullptr; }

}}
