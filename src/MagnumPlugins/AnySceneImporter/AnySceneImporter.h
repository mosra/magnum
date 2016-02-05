#ifndef Magnum_Trade_AnySceneImporter_h
#define Magnum_Trade_AnySceneImporter_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016
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
 * @brief Class @ref Magnum::Trade::AnySceneImporter
 */

#include <Magnum/Trade/AbstractImporter.h>

namespace Magnum { namespace Trade {

/**
@brief Any scene importer plugin

Detects file type based on file extension, loads corresponding plugin and then
tries to open the file with it.

This plugin is built if `WITH_ANYSCENEIMPORTER` is enabled when building
Magnum Plugins. To use dynamic plugin, you need to load `AnySceneImporter`
plugin from `MAGNUM_PLUGINS_IMPORTER_DIR`. To use static plugin, you need to
request `AnySceneImporter` component of `MagnumPlugins` package in CMake and
link to `${MAGNUMPLUGINS_ANYSCENEIMPORTER_LIBRARIES}`. To use this as a
dependency of another plugin, you additionally need to add
`${MAGNUMPLUGINS_ANYSCENEIMPORTER_INCLUDE_DIRS}` to include path. See
@ref building-plugins, @ref cmake-plugins and @ref plugins for more
information.

Supported formats:

-   COLLADA (`*.dae`), loaded with @ref ColladaImporter or any other plugin
    that provides it
-   OBJ (`*.obj`), loaded with @ref ObjImporter or any other plugin that
    provides it
-   OpenGEX (`*.ogex`), loaded with @ref OpenGexImporter or any other plugin
    that provides it
-   Stanford (`*.ply`), loaded with @ref StanfordImporter or any other plugin
    that provides it

Only loading from files is supported.
*/
class AnySceneImporter: public AbstractImporter {
    public:
        /** @brief Constructor with access to plugin manager */
        explicit AnySceneImporter(PluginManager::Manager<AbstractImporter>& manager);

        /** @brief Plugin manager constructor */
        explicit AnySceneImporter(PluginManager::AbstractManager& manager, std::string plugin);

        ~AnySceneImporter();

    private:
        Features doFeatures() const override;
        bool doIsOpened() const override;
        void doClose() override;
        void doOpenFile(const std::string& filename) override;

        Int doDefaultScene() override;

        UnsignedInt doSceneCount() const override;
        Int doSceneForName(const std::string& name) override;
        std::string doSceneName(UnsignedInt id) override;
        std::optional<SceneData> doScene(UnsignedInt id) override;

        UnsignedInt doLightCount() const override;
        Int doLightForName(const std::string& name) override;
        std::string doLightName(UnsignedInt id) override;
        std::optional<LightData> doLight(UnsignedInt id) override;

        UnsignedInt doCameraCount() const override;
        Int doCameraForName(const std::string& name) override;
        std::string doCameraName(UnsignedInt id) override;
        std::optional<CameraData> doCamera(UnsignedInt id) override;

        UnsignedInt doObject2DCount() const override;
        Int doObject2DForName(const std::string& name) override;
        std::string doObject2DName(UnsignedInt id) override;
        std::unique_ptr<ObjectData2D> doObject2D(UnsignedInt id) override;

        UnsignedInt doObject3DCount() const override;
        Int doObject3DForName(const std::string& name) override;
        std::string doObject3DName(UnsignedInt id) override;
        std::unique_ptr<ObjectData3D> doObject3D(UnsignedInt id) override;

        UnsignedInt doMesh2DCount() const override;
        Int doMesh2DForName(const std::string& name) override;
        std::string doMesh2DName(UnsignedInt id) override;
        std::optional<MeshData2D> doMesh2D(UnsignedInt id) override;

        UnsignedInt doMesh3DCount() const override;
        Int doMesh3DForName(const std::string& name) override;
        std::string doMesh3DName(UnsignedInt id) override;
        std::optional<MeshData3D> doMesh3D(UnsignedInt id) override;

        UnsignedInt doMaterialCount() const override;
        Int doMaterialForName(const std::string& name) override;
        std::string doMaterialName(UnsignedInt id) override;
        std::unique_ptr<AbstractMaterialData> doMaterial(UnsignedInt id) override;

        UnsignedInt doTextureCount() const override;
        Int doTextureForName(const std::string& name) override;
        std::string doTextureName(UnsignedInt id) override;
        std::optional<TextureData> doTexture(UnsignedInt id) override;

        UnsignedInt doImage1DCount() const override;
        Int doImage1DForName(const std::string& name) override;
        std::string doImage1DName(UnsignedInt id) override;
        std::optional<ImageData1D> doImage1D(UnsignedInt id) override;

        UnsignedInt doImage2DCount() const override;
        Int doImage2DForName(const std::string& name) override;
        std::string doImage2DName(UnsignedInt id) override;
        std::optional<ImageData2D> doImage2D(UnsignedInt id) override;

        UnsignedInt doImage3DCount() const override;
        Int doImage3DForName(const std::string& name) override;
        std::string doImage3DName(UnsignedInt id) override;
        std::optional<ImageData3D> doImage3D(UnsignedInt id) override;

    private:
        std::unique_ptr<AbstractImporter> _in;
};

}}

#endif
