#ifndef Magnum_Trade_AnySceneImporter_h
#define Magnum_Trade_AnySceneImporter_h
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
 * @brief Class @ref Magnum::Trade::AnySceneImporter
 */

#include "Magnum/Trade/AbstractImporter.h"
#include "MagnumPlugins/AnySceneImporter/configure.h"

#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef MAGNUM_ANYSCENEIMPORTER_BUILD_STATIC
    #ifdef AnySceneImporter_EXPORTS
        #define MAGNUM_ANYSCENEIMPORTER_EXPORT CORRADE_VISIBILITY_EXPORT
    #else
        #define MAGNUM_ANYSCENEIMPORTER_EXPORT CORRADE_VISIBILITY_IMPORT
    #endif
#else
    #define MAGNUM_ANYSCENEIMPORTER_EXPORT CORRADE_VISIBILITY_STATIC
#endif
#define MAGNUM_ANYSCENEIMPORTER_LOCAL CORRADE_VISIBILITY_LOCAL
#else
#define MAGNUM_ANYSCENEIMPORTER_EXPORT
#define MAGNUM_ANYSCENEIMPORTER_LOCAL
#endif

namespace Magnum { namespace Trade {

/**
@brief Any scene importer plugin

Detects file type based on file extension, loads corresponding plugin and then
tries to open the file with it. Supported formats:

-   3ds Max 3DS and ASE (`*.3ds`, `*.ase`), loaded with any plugin that
    provides `3dsImporter`
-   3D Manufacturing Format (`*.3mf`), loaded with any plugin that provides
    `3mfImporter`
-   AC3D (`*.ac`), loaded with any plugin that provides `Ac3dImporter`
-   Blender 3D (`*.blend`), loaded with any plugin that provides
    `BlenderImporter`
-   Biovision BVH (`*.bvh`), loaded with any plugin that provides `BvhImporter`
-   CharacterStudio Motion (`*.csm`), loaded with any plugin that provides
    `CsmImporter`
-   COLLADA (`*.dae`), loaded with any plugin that provides `ColladaImporter`
-   DirectX X (`*.x`), loaded with any plugin that provides `DirectXImporter`
-   AutoCAD DXF (`*.dxf`), loaded with any plugin that provides `DxfImporter`
-   Autodesk FBX (`*.fbx`), loaded with any plugin that provides `FbxImporter`
-   glTF (`*.gltf`, `*.glb`) and [VRM](https://vrm.dev/en/) (`*.vrm`), loaded
    with @ref GltfImporter or any other plugin that provides it
-   Industry Foundation Classes (IFC/Step) (`*.ifc`), loaded with any plugin
    that provides `IfcImporter`
-   Irrlicht Mesh and Scene (`*.irrmesh`, `*.irr`), loaded with any plugin that
    provides `IrrlichtImporter`
-   LightWave, LightWave Scene (`*.lwo`, `*.lws`), loaded with any plugin that
    provides `LightWaveImporter`
-   Modo (`*.lxo`), loaded with any plugin that provides `ModoImporter`
-   Milkshape 3D (`*.ms3d`), loaded with any plugin that provides
    `MilkshapeImporter`
-   Wavefront OBJ (`*.obj`), loaded with @ref ObjImporter or any other plugin
    that provides it
-   OGRE XML (`*.mesh.xml`), loaded with any plugin that provides
    `OgreImporter`. The `*.mesh` extension isn't recognized because it's used
    by [Meshwork](http://justsolve.archiveteam.org/wiki/Meshwork_model) as
    well.
-   OpenGEX (`*.ogex`), loaded with @ref OpenGexImporter or any other plugin
    that provides it
-   Stanford (`*.ply`), loaded with @ref StanfordImporter or any other plugin
    that provides it
-   Stereolitography (`*.stl`), loaded with any plugin that provides
    `StlImporter`
-   TrueSpace (`*.cob`, `*.scn`), loaded with any plugin that provides
    `TrueSpaceImporter`
-   [Universal Scene Description (USD)](https://en.wikipedia.org/wiki/Universal_Scene_Description)
    (`*.usd`, `*.usda`, `*.usdc`, `*.usdz`), loaded with any plugin that
    provides `UsdImporter`
-   Unreal (`*.3d`), loaded with any plugin that provides `UnrealImporter`
-   Valve Model (`*.smd`, `*.vta`), loaded with any plugin that provides
    `ValveImporter`
-   XGL (`*.xgl`, `*.zgl`), loaded with any plugin that provides `XglImporter`

@note
    Note that this list is not exhaustive, in particular the
    @ref AssimpImporter lists many more file extensions. Not all can be
    supported by this plugin because they're either very generic or used by
    multiple different formats, such as `*.mdl` used for both Quake I and 3D
    GameStudio. If file opening fails with this plugin, you can try directly
    with a concrete plugin such as the @ref AssimpImporter as a fallback.

Only loading from files is supported as the filename is used to detect the
format, however @ref ImporterFeature::FileCallback is supported as well.

@section Trade-AnySceneImporter-usage Usage

@m_class{m-note m-success}

@par
    This class is a plugin that's meant to be dynamically loaded and used
    through the base @ref AbstractImporter interface. See its documentation for
    introduction and usage examples.

This plugin depends on the @ref Trade library and is built if
`MAGNUM_WITH_ANYSCENEIMPORTER` is enabled when building Magnum. To use as a
dynamic plugin, load @cpp "AnySceneImporter" @ce via
@ref Corrade::PluginManager::Manager.

Additionally, if you're using Magnum as a CMake subproject, do the following:

@code{.cmake}
set(MAGNUM_WITH_ANYSCENEIMPORTER ON CACHE BOOL "" FORCE)
add_subdirectory(magnum EXCLUDE_FROM_ALL)

# So the dynamically loaded plugin gets built implicitly
add_dependencies(your-app Magnum::AnySceneImporter)
@endcode

To use as a static plugin or as a dependency of another plugin with CMake, you
need to request the `AnySceneImporter` component of the `Magnum` package in
CMake and link to the `Magnum::AnySceneImporter` target:

@code{.cmake}
find_package(Magnum REQUIRED AnySceneImporter)

# ...
target_link_libraries(your-app PRIVATE Magnum::AnySceneImporter)
@endcode

See @ref building, @ref cmake, @ref plugins and @ref file-formats for more
information.

@section Trade-AnySceneImporter-proxy Interface proxying and option propagation

On a call to @ref openFile(), a file format is detected from the extension and
a corresponding plugin is loaded. After that, flags set via @ref setFlags(),
file callbacks set via @ref setFileCallback() and options set through
@ref configuration() are propagated to the concrete implementation. A warning
is emitted in case an option set is not present in the default configuration of
the target plugin.

Calls to the @ref animation(), @ref scene(), @ref light(), @ref camera(),
@ref skin2D(), @ref skin3D(), @ref mesh(), @ref material(), @ref texture(),
@ref image1D(), @ref image2D(), @ref image3D() and corresponding
count-/name-related functions are then proxied to the concrete implementation.
The @ref close() function closes and discards the internally instantiated
plugin; @ref isOpened() works as usual.

While the @ref meshAttributeName(), @ref meshAttributeForName(),
@ref sceneFieldName() and @ref sceneFieldForName() APIs can be called without a
file opened, they return an empty string or an invalid attribute in that case.

Besides delegating the flags, the @ref AnySceneImporter itself recognizes
@ref ImporterFlag::Verbose, printing info about the concrete plugin being used
when the flag is enabled. @ref ImporterFlag::Quiet is recognized as well and
causes all warnings to be suppressed.
*/
class MAGNUM_ANYSCENEIMPORTER_EXPORT AnySceneImporter: public AbstractImporter {
    public:
        /** @brief Constructor with access to plugin manager */
        explicit AnySceneImporter(PluginManager::Manager<AbstractImporter>& manager);

        /** @brief Plugin manager constructor */
        explicit AnySceneImporter(PluginManager::AbstractManager& manager, const Containers::StringView& plugin);

        ~AnySceneImporter();

    private:
        MAGNUM_ANYSCENEIMPORTER_LOCAL ImporterFeatures doFeatures() const override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL bool doIsOpened() const override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL void doClose() override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL void doOpenFile(Containers::StringView filename) override;

        MAGNUM_ANYSCENEIMPORTER_LOCAL UnsignedInt doAnimationCount() const override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL Containers::String doAnimationName(UnsignedInt id) override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL Int doAnimationForName(Containers::StringView name) override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL Containers::Optional<AnimationData> doAnimation(UnsignedInt id) override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL AnimationTrackTarget doAnimationTrackTargetForName(Containers::StringView name) override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL Containers::String doAnimationTrackTargetName(AnimationTrackTarget name) override;

        MAGNUM_ANYSCENEIMPORTER_LOCAL Int doDefaultScene() const override;

        MAGNUM_ANYSCENEIMPORTER_LOCAL UnsignedInt doSceneCount() const override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL UnsignedLong doObjectCount() const override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL Int doSceneForName(Containers::StringView name) override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL Long doObjectForName(Containers::StringView name) override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL Containers::String doSceneName(UnsignedInt id) override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL Containers::String doObjectName(UnsignedLong id) override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL Containers::Optional<SceneData> doScene(UnsignedInt id) override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL SceneField doSceneFieldForName(Containers::StringView name) override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL Containers::String doSceneFieldName(SceneField name) override;

        MAGNUM_ANYSCENEIMPORTER_LOCAL UnsignedInt doLightCount() const override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL Int doLightForName(Containers::StringView name) override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL Containers::String doLightName(UnsignedInt id) override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL Containers::Optional<LightData> doLight(UnsignedInt id) override;

        MAGNUM_ANYSCENEIMPORTER_LOCAL UnsignedInt doCameraCount() const override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL Int doCameraForName(Containers::StringView name) override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL Containers::String doCameraName(UnsignedInt id) override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL Containers::Optional<CameraData> doCamera(UnsignedInt id) override;

        #ifdef MAGNUM_BUILD_DEPRECATED
        MAGNUM_ANYSCENEIMPORTER_LOCAL UnsignedInt doObject2DCount() const override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL Int doObject2DForName(const std::string& name) override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL std::string doObject2DName(UnsignedInt id) override;
        CORRADE_IGNORE_DEPRECATED_PUSH
        MAGNUM_ANYSCENEIMPORTER_LOCAL Containers::Pointer<ObjectData2D> doObject2D(UnsignedInt id) override;
        CORRADE_IGNORE_DEPRECATED_POP

        MAGNUM_ANYSCENEIMPORTER_LOCAL UnsignedInt doObject3DCount() const override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL Int doObject3DForName(const std::string& name) override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL std::string doObject3DName(UnsignedInt id) override;
        CORRADE_IGNORE_DEPRECATED_PUSH
        MAGNUM_ANYSCENEIMPORTER_LOCAL Containers::Pointer<ObjectData3D> doObject3D(UnsignedInt id) override;
        CORRADE_IGNORE_DEPRECATED_POP
        #endif

        MAGNUM_ANYSCENEIMPORTER_LOCAL UnsignedInt doSkin2DCount() const override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL Int doSkin2DForName(Containers::StringView name) override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL Containers::String doSkin2DName(UnsignedInt id) override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL Containers::Optional<SkinData2D> doSkin2D(UnsignedInt id) override;

        MAGNUM_ANYSCENEIMPORTER_LOCAL UnsignedInt doSkin3DCount() const override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL Int doSkin3DForName(Containers::StringView name) override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL Containers::String doSkin3DName(UnsignedInt id) override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL Containers::Optional<SkinData3D> doSkin3D(UnsignedInt id) override;

        MAGNUM_ANYSCENEIMPORTER_LOCAL UnsignedInt doMeshCount() const override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL UnsignedInt doMeshLevelCount(UnsignedInt id) override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL Int doMeshForName(Containers::StringView name) override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL Containers::String doMeshName(UnsignedInt id) override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL Containers::Optional<MeshData> doMesh(UnsignedInt id, UnsignedInt level) override;

        MAGNUM_ANYSCENEIMPORTER_LOCAL MeshAttribute doMeshAttributeForName(Containers::StringView name) override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL Containers::String doMeshAttributeName(MeshAttribute id) override;

        #ifdef MAGNUM_BUILD_DEPRECATED
        MAGNUM_ANYSCENEIMPORTER_LOCAL UnsignedInt doMesh2DCount() const override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL Int doMesh2DForName(const std::string& name) override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL std::string doMesh2DName(UnsignedInt id) override;
        CORRADE_IGNORE_DEPRECATED_PUSH
        MAGNUM_ANYSCENEIMPORTER_LOCAL Containers::Optional<MeshData2D> doMesh2D(UnsignedInt id) override;
        CORRADE_IGNORE_DEPRECATED_POP

        MAGNUM_ANYSCENEIMPORTER_LOCAL UnsignedInt doMesh3DCount() const override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL Int doMesh3DForName(const std::string& name) override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL std::string doMesh3DName(UnsignedInt id) override;
        CORRADE_IGNORE_DEPRECATED_PUSH
        MAGNUM_ANYSCENEIMPORTER_LOCAL Containers::Optional<MeshData3D> doMesh3D(UnsignedInt id) override;
        CORRADE_IGNORE_DEPRECATED_POP
        #endif

        MAGNUM_ANYSCENEIMPORTER_LOCAL UnsignedInt doMaterialCount() const override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL Int doMaterialForName(Containers::StringView name) override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL Containers::String doMaterialName(UnsignedInt id) override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL Containers::Optional<MaterialData> doMaterial(UnsignedInt id) override;

        MAGNUM_ANYSCENEIMPORTER_LOCAL UnsignedInt doTextureCount() const override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL Int doTextureForName(Containers::StringView name) override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL Containers::String doTextureName(UnsignedInt id) override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL Containers::Optional<TextureData> doTexture(UnsignedInt id) override;

        MAGNUM_ANYSCENEIMPORTER_LOCAL UnsignedInt doImage1DCount() const override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL UnsignedInt doImage1DLevelCount(UnsignedInt id) override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL Int doImage1DForName(Containers::StringView name) override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL Containers::String doImage1DName(UnsignedInt id) override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL Containers::Optional<ImageData1D> doImage1D(UnsignedInt id, UnsignedInt level) override;

        MAGNUM_ANYSCENEIMPORTER_LOCAL UnsignedInt doImage2DCount() const override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL UnsignedInt doImage2DLevelCount(UnsignedInt id) override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL Int doImage2DForName(Containers::StringView name) override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL Containers::String doImage2DName(UnsignedInt id) override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL Containers::Optional<ImageData2D> doImage2D(UnsignedInt id, UnsignedInt level) override;

        MAGNUM_ANYSCENEIMPORTER_LOCAL UnsignedInt doImage3DCount() const override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL UnsignedInt doImage3DLevelCount(UnsignedInt id) override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL Int doImage3DForName(Containers::StringView name) override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL Containers::String doImage3DName(UnsignedInt id) override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL Containers::Optional<ImageData3D> doImage3D(UnsignedInt id, UnsignedInt level) override;

        Containers::Pointer<AbstractImporter> _in;
};

}}

#endif
