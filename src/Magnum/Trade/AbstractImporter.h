#ifndef Magnum_Trade_AbstractImporter_h
#define Magnum_Trade_AbstractImporter_h
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
 * @brief Class @ref Magnum::Trade::AbstractImporter
 */

#include <memory>
#include <Corrade/Containers/EnumSet.h>
#include <Corrade/PluginManager/AbstractManagingPlugin.h>

#include "Magnum/Magnum.h"
#include "Magnum/visibility.h"
#include "Magnum/Trade/Trade.h"
#include "MagnumExternal/Optional/optional.hpp"

namespace Magnum { namespace Trade {

/**
@brief Base for importer plugins

Provides interface for importing 2D/3D scene, mesh, material, texture and image
data. See @ref plugins for more information and `*Importer` classes in
@ref Trade namespace for available importer plugins.

## Subclassing

Plugin implements function @ref doFeatures(), @ref doIsOpened(), one of or both
@ref doOpenData() and @ref doOpenFile() functions, function @ref doClose() and
one or more tuples of data access functions, based on which features are
supported in given format.

For multi-data formats file opening shouldn't take long, all parsing should
be done in data parsing functions, because the user might want to import only
some data. This is obviously not the case for single-data formats like images,
as the file contains all data user wants to import.

You don't need to do most of the redundant sanity checks, these things are
checked by the implementation:

-   Functions @ref doOpenData() and @ref doOpenFile() are called after the
    previous file was closed, function @ref doClose() is called only if there
    is any file opened.
-   Function @ref doOpenData() is called only if @ref Feature::OpenData is
    supported.
-   All `do*()` implementations working on opened file are called only if there
    is any file opened.
-   All `do*()` implementations taking data ID as parameter are called only if
    the ID is from valid range.

Plugin interface string is `"cz.mosra.magnum.Trade.AbstractImporter/0.3"`.

@attention @ref Corrade::Containers::Array instances returned from the plugin
    should *not* use anything else than the default deleter, otherwise this can
    cause dangling function pointer call on array destruction if the plugin
    gets unloaded before the array is destroyed.

@todo How to handle casting from std::unique_ptr<> in more convenient way?
*/
class MAGNUM_EXPORT AbstractImporter: public PluginManager::AbstractManagingPlugin<AbstractImporter> {
    CORRADE_PLUGIN_INTERFACE("cz.mosra.magnum.Trade.AbstractImporter/0.3")

    public:
        /**
         * @brief Features supported by this importer
         *
         * @see @ref Features, @ref features()
         */
        enum class Feature: UnsignedByte {
            /** Opening files from raw data using @ref openData() */
            OpenData = 1 << 0
        };

        /** @brief Set of features supported by this importer */
        typedef Containers::EnumSet<Feature> Features;

        /** @brief Default constructor */
        explicit AbstractImporter();

        /** @brief Constructor with access to plugin manager */
        explicit AbstractImporter(PluginManager::Manager<AbstractImporter>& manager);

        /** @brief Plugin manager constructor */
        explicit AbstractImporter(PluginManager::AbstractManager& manager, const std::string& plugin);

        /** @brief Features supported by this importer */
        Features features() const { return doFeatures(); }

        /** @brief Whether any file is opened */
        bool isOpened() const { return doIsOpened(); }

        /**
         * @brief Open raw data
         *
         * Closes previous file, if it was opened, and tries to open given
         * file. Available only if @ref Feature::OpenData is supported. Returns
         * `true` on success, `false` otherwise.
         * @see @ref features(), @ref openFile()
         */
        bool openData(Containers::ArrayView<const char> data);

        /**
         * @brief Open file
         *
         * Closes previous file, if it was opened, and tries to open given
         * file. Returns `true` on success, `false` otherwise.
         * @see @ref features(), @ref openData()
         */
        bool openFile(const std::string& filename);

        /** @brief Close file */
        void close();

        /** @{ @name Data accessors
         * Each function tuple provides access to given data.
         */

        /**
         * @brief Default scene
         *
         * When there is more than one scene, returns ID of the default one.
         * If there is no default scene, returns `-1`.
         *
         * @note The function is not const, because the value will probably
         *      be lazy-populated.
         */
        Int defaultScene();

        /** @brief Scene count */
        UnsignedInt sceneCount() const;

        /**
         * @brief Scene ID for given name
         *
         * If no scene for given name exists, returns `-1`.
         * @see @ref sceneName()
         */
        Int sceneForName(const std::string& name);

        /**
         * @brief Scene name
         * @param id        Scene ID, from range [0, @ref sceneCount()).
         *
         * @see @ref sceneForName()
         */
        std::string sceneName(UnsignedInt id);

        /**
         * @brief Scene
         * @param id        Scene ID, from range [0, @ref sceneCount()).
         *
         * Returns given scene or `std::nullopt` if import failed.
         */
        std::optional<SceneData> scene(UnsignedInt id);

        /** @brief Light count */
        UnsignedInt lightCount() const;

        /**
         * @brief Light ID for given name
         *
         * If no light for given name exists, returns `-1`.
         * @see @ref lightName()
         */
        Int lightForName(const std::string& name);

        /**
         * @brief Light name
         * @param id        Light ID, from range [0, @ref lightCount()).
         *
         * @see @ref lightForName()
         */
        std::string lightName(UnsignedInt id);

        /**
         * @brief Light
         * @param id        Light ID, from range [0, @ref lightCount()).
         *
         * Returns given light or `std::nullopt` if importing failed.
         */
        std::optional<LightData> light(UnsignedInt id);

        /** @brief Camera count */
        UnsignedInt cameraCount() const;

        /**
         * @brief Camera ID for given name
         *
         * If no camera for given name exists, returns `-1`.
         * @see @ref cameraName()
         */
        Int cameraForName(const std::string& name);

        /**
         * @brief Camera name
         * @param id        Camera ID, from range [0, @ref cameraCount()).
         *
         * @see @ref cameraForName()
         */
        std::string cameraName(UnsignedInt id);

        /**
         * @brief Camera
         * @param id        Camera ID, from range [0, @ref cameraCount()).
         *
         * Returns given camera or `std::nullopt` if importing failed.
         */
        std::optional<CameraData> camera(UnsignedInt id);

        /** @brief Two-dimensional object count */
        UnsignedInt object2DCount() const;

        /**
         * @brief Two-dimensional object ID for given name
         *
         * If no scene for given name exists, returns `-1`.
         * @see @ref object2DName()
         */
        Int object2DForName(const std::string& name);

        /**
         * @brief Two-dimensional object name
         * @param id        Object ID, from range [0, @ref object2DCount()).
         *
         * @see @ref object2DForName()
         */
        std::string object2DName(UnsignedInt id);

        /**
         * @brief Two-dimensional object
         * @param id        Object ID, from range [0, @ref object2DCount()).
         *
         * Returns given object or `nullptr` if importing failed.
         */
        std::unique_ptr<ObjectData2D> object2D(UnsignedInt id);

        /** @brief Three-dimensional object count */
        UnsignedInt object3DCount() const;

        /**
         * @brief Three-dimensional object ID for given name
         *
         * If no scene for given name exists, returns `-1`.
         * @see @ref object3DName()
         */
        Int object3DForName(const std::string& name);

        /**
         * @brief Three-dimensional object name
         * @param id        Object ID, from range [0, @ref object3DCount()).
         *
         * @see @ref object3DForName()
         */
        std::string object3DName(UnsignedInt id);

        /**
         * @brief Three-dimensional object
         * @param id        Object ID, from range [0, @ref object3DCount()).
         *
         * Returns given object or `nullptr` if importing failed.
         */
        std::unique_ptr<ObjectData3D> object3D(UnsignedInt id);

        /** @brief Two-dimensional mesh count */
        UnsignedInt mesh2DCount() const;

        /**
         * @brief Two-dimensional mesh ID for given name
         *
         * If no mesh for given name exists, returns `-1`.
         * @see @ref mesh2DName()
         */
        Int mesh2DForName(const std::string& name);

        /**
         * @brief Two-dimensional mesh name
         * @param id        Mesh ID, from range [0, @ref mesh2DCount()).
         *
         * @see @ref mesh2DForName()
         */
        std::string mesh2DName(UnsignedInt id);

        /**
         * @brief Two-dimensional mesh
         * @param id        Mesh ID, from range [0, @ref mesh2DCount()).
         *
         * Returns given mesh or `std::nullopt` if importing failed.
         */
        std::optional<MeshData2D> mesh2D(UnsignedInt id);

        /** @brief Three-dimensional mesh count */
        UnsignedInt mesh3DCount() const;

        /**
         * @brief Three-dimensional mesh ID for given name
         *
         * If no mesh for given name exists, returns `-1`.
         * @see @ref mesh3DName()
         */
        Int mesh3DForName(const std::string& name);

        /**
         * @brief Three-dimensional mesh name
         * @param id        Mesh ID, from range [0, @ref mesh3DCount()).
         *
         * @see @ref mesh3DForName()
         */
        std::string mesh3DName(UnsignedInt id);

        /**
         * @brief Three-dimensional mesh
         * @param id        Mesh ID, from range [0, @ref mesh3DCount()).
         *
         * Returns given mesh or `std::nullopt` if importing failed.
         */
        std::optional<MeshData3D> mesh3D(UnsignedInt id);

        /** @brief Material count */
        UnsignedInt materialCount() const;

        /**
         * @brief Material ID for given name
         *
         * If no material for given name exists, returns `-1`.
         * @see @ref materialName()
         */
        Int materialForName(const std::string& name);

        /**
         * @brief Material name
         * @param id        Material ID, from range [0, @ref materialCount()).
         *
         * @see @ref materialForName()
         */
        std::string materialName(UnsignedInt id);

        /**
         * @brief Material
         * @param id        Material ID, from range [0, @ref materialCount()).
         *
         * Returns given material or `nullptr` if importing failed.
         */
        std::unique_ptr<AbstractMaterialData> material(UnsignedInt id);

        /** @brief Texture count */
        UnsignedInt textureCount() const;

        /**
         * @brief Texture ID for given name
         *
         * If no texture for given name exists, returns `-1`.
         * @see @ref textureName()
         */
        Int textureForName(const std::string& name);

        /**
         * @brief Texture name
         * @param id        Texture ID, from range [0, @ref textureCount()).
         *
         * @see @ref textureForName()
         */
        std::string textureName(UnsignedInt id);

        /**
         * @brief Texture
         * @param id        Texture ID, from range [0, @ref textureCount()).
         *
         * Returns given texture or `std::nullopt` if importing failed.
         */
        std::optional<TextureData> texture(UnsignedInt id);

        /** @brief One-dimensional image count */
        UnsignedInt image1DCount() const;

        /**
         * @brief One-dimensional image ID for given name
         *
         * If no image for given name exists, returns `-1`.
         * @see @ref image1DName()
         */
        Int image1DForName(const std::string& name);

        /**
         * @brief One-dimensional image name
         * @param id        Image ID, from range [0, @ref image1DCount()).
         *
         * @see @ref image1DForName()
         */
        std::string image1DName(UnsignedInt id);

        /**
         * @brief One-dimensional image
         * @param id        Image ID, from range [0, @ref image1DCount()).
         *
         * Returns given image or `std::nullopt` if importing failed.
         */
        std::optional<ImageData1D> image1D(UnsignedInt id);

        /** @brief Two-dimensional image count */
        UnsignedInt image2DCount() const;

        /**
         * @brief Two-dimensional image ID for given name
         *
         * If no image for given name exists, returns `-1`.
         * @see @ref image2DName()
         */
        Int image2DForName(const std::string& name);

        /**
         * @brief Two-dimensional image name
         * @param id        Image ID, from range [0, @ref image2DCount()).
         *
         * @see @ref image2DForName()
         */
        std::string image2DName(UnsignedInt id);

        /**
         * @brief Two-dimensional image
         * @param id        Image ID, from range [0, @ref image2DCount()).
         *
         * Returns given image or `std::nullopt` if importing failed.
         */
        std::optional<ImageData2D> image2D(UnsignedInt id);

        /** @brief Three-dimensional image count */
        UnsignedInt image3DCount() const;

        /**
         * @brief Three-dimensional image ID for given name
         *
         * If no image for given name exists, returns `-1`.
         * @see @ref image3DName()
         */
        Int image3DForName(const std::string& name);

        /**
         * @brief Three-dimensional image name
         * @param id        Image ID, from range [0, @ref image3DCount()).
         *
         * @see @ref image3DForName()
         */
        std::string image3DName(UnsignedInt id);

        /**
         * @brief Three-dimensional image
         * @param id        Image ID, from range [0, @ref image3DCount()).
         *
         * Returns given image or `std::nullopt` if importing failed.
         */
        std::optional<ImageData3D> image3D(UnsignedInt id);

        /*@}*/

        /**
         * @brief Plugin-specific access to internal importer state
         *
         * The importer might provide access to its internal data structures
         * for currently opened document through this function. See
         * documentation of particular plugin for more information about
         * returned type and contents. Returns `nullptr` by default.
         * @see @ref AbstractMaterialData::importerState(),
         *      @ref CameraData::importerState(), @ref ImageData::importerState(),
         *      @ref MeshData2D::importerState(), @ref MeshData3D::importerState(),
         *      @ref ObjectData2D::importerState(), @ref ObjectData3D::importerState(),
         *      @ref SceneData::importerState(), @ref TextureData::importerState()
         */
        const void* importerState() const;

    protected:
        /**
         * @brief Implementation for @ref openFile()
         *
         * If @ref Feature::OpenData is supported, default implementation opens
         * the file and calls @ref doOpenData() with its contents. It is
         * allowed to call this function from your @ref doOpenFile()
         * implementation.
         */
        virtual void doOpenFile(const std::string& filename);

    #ifndef DOXYGEN_GENERATING_OUTPUT
    private:
    #else
    protected:
    #endif
        /** @brief Implementation for @ref features() */
        virtual Features doFeatures() const = 0;

        /** @brief Implementation for @ref isOpened() */
        virtual bool doIsOpened() const = 0;

        /** @brief Implementation for @ref openData() */
        virtual void doOpenData(Containers::ArrayView<const char> data);

        /** @brief Implementation for @ref close() */
        virtual void doClose() = 0;

        /**
         * @brief Implementation for @ref defaultScene()
         *
         * Default implementation returns `-1`.
         */
        virtual Int doDefaultScene();

        /**
         * @brief Implementation for @ref sceneCount()
         *
         * Default implementation returns `0`.
         */
        virtual UnsignedInt doSceneCount() const;

        /**
         * @brief Implementation for @ref sceneForName()
         *
         * Default implementation returns `-1`.
         */
        virtual Int doSceneForName(const std::string& name);

        /**
         * @brief Implementation for @ref sceneName()
         *
         * Default implementation returns empty string.
         */
        virtual std::string doSceneName(UnsignedInt id);

        /** @brief Implementation for @ref scene() */
        virtual std::optional<SceneData> doScene(UnsignedInt id);

        /**
         * @brief Implementation for @ref lightCount()
         *
         * Default implementation returns `0`.
         */
        virtual UnsignedInt doLightCount() const;

        /**
         * @brief Implementation for @ref lightForName()
         *
         * Default implementation returns `-1`.
         */
        virtual Int doLightForName(const std::string& name);

        /**
         * @brief Implementation for @ref lightName()
         *
         * Default implementation returns empty string.
         */
        virtual std::string doLightName(UnsignedInt id);

        /** @brief Implementation for @ref light() */
        virtual std::optional<LightData> doLight(UnsignedInt id);

        /**
         * @brief Implementation for @ref cameraCount()
         *
         * Default implementation returns `0`.
         */
        virtual UnsignedInt doCameraCount() const;

        /**
         * @brief Implementation for @ref cameraForName()
         *
         * Default implementation returns `-1`.
         */
        virtual Int doCameraForName(const std::string& name);

        /**
         * @brief Implementation for @ref cameraName()
         *
         * Default implementation returns empty string.
         */
        virtual std::string doCameraName(UnsignedInt id);

        /** @brief Implementation for @ref camera() */
        virtual std::optional<CameraData> doCamera(UnsignedInt id);

        /**
         * @brief Implementation for @ref object2DCount()
         *
         * Default implementation returns `0`.
         */
        virtual UnsignedInt doObject2DCount() const;

        /**
         * @brief Implementation for @ref object2DForName()
         *
         * Default implementation returns `-1`.
         */
        virtual Int doObject2DForName(const std::string& name);

        /**
         * @brief Implementation for @ref object2DName()
         *
         * Default implementation returns empty string.
         */
        virtual std::string doObject2DName(UnsignedInt id);

        /** @brief Implementation for @ref object2D() */
        virtual std::unique_ptr<ObjectData2D> doObject2D(UnsignedInt id);

        /**
         * @brief Implementation for @ref object3DCount()
         *
         * Default implementation returns `0`.
         */
        virtual UnsignedInt doObject3DCount() const;

        /**
         * @brief Implementation for @ref object3DForName()
         *
         * Default implementation returns `-1`.
         */
        virtual Int doObject3DForName(const std::string& name);

        /**
         * @brief Implementation for @ref object3DName()
         *
         * Default implementation returns empty string.
         */
        virtual std::string doObject3DName(UnsignedInt id);

        /** @brief Implementation for @ref object3D() */
        virtual std::unique_ptr<ObjectData3D> doObject3D(UnsignedInt id);

        /**
         * @brief Implementation for @ref mesh2DCount()
         *
         * Default implementation returns `0`.
         */
        virtual UnsignedInt doMesh2DCount() const;

        /**
         * @brief Implementation for @ref mesh2DForName()
         *
         * Default implementation returns `-1`.
         */
        virtual Int doMesh2DForName(const std::string& name);

        /**
         * @brief Implementation for @ref mesh2DName()
         *
         * Default implementation returns empty string.
         */
        virtual std::string doMesh2DName(UnsignedInt id);

        /** @brief Implementation for @ref mesh2D() */
        virtual std::optional<MeshData2D> doMesh2D(UnsignedInt id);

        /**
         * @brief Implementation for @ref mesh3DCount()
         *
         * Default implementation returns `0`.
         */
        virtual UnsignedInt doMesh3DCount() const;

        /**
         * @brief Implementation for @ref mesh3DForName()
         *
         * Default implementation returns `-1`.
         */
        virtual Int doMesh3DForName(const std::string& name);

        /**
         * @brief Implementation for @ref mesh3DName()
         *
         * Default implementation returns empty string.
         */
        virtual std::string doMesh3DName(UnsignedInt id);

        /** @brief Implementation for @ref mesh3D() */
        virtual std::optional<MeshData3D> doMesh3D(UnsignedInt id);

        /**
         * @brief Implementation for @ref materialCount()
         *
         * Default implementation returns `0`.
         */
        virtual UnsignedInt doMaterialCount() const;

        /**
         * @brief Implementation for @ref materialForName()
         *
         * Default implementation returns `-1`.
         */
        virtual Int doMaterialForName(const std::string& name);

        /**
         * @brief Implementation for @ref materialName()
         *
         * Default implementation returns empty string.
         */
        virtual std::string doMaterialName(UnsignedInt id);

        /** @brief Implementation for @ref material() */
        virtual std::unique_ptr<AbstractMaterialData> doMaterial(UnsignedInt id);

        /**
         * @brief Implementation for @ref textureCount()
         *
         * Default implementation returns `0`.
         */
        virtual UnsignedInt doTextureCount() const;

        /**
         * @brief Implementation for @ref textureForName()
         *
         * Default implementation returns `-1`.
         */
        virtual Int doTextureForName(const std::string& name);

        /**
         * @brief Implementation for @ref textureName()
         *
         * Default implementation returns empty string.
         */
        virtual std::string doTextureName(UnsignedInt id);

        /** @brief Implementation for @ref texture() */
        virtual std::optional<TextureData> doTexture(UnsignedInt id);

        /**
         * @brief Implementation for @ref image1DCount()
         *
         * Default implementation returns `0`.
         */
        virtual UnsignedInt doImage1DCount() const;

        /**
         * @brief Implementation for @ref image1DForName()
         *
         * Default implementation returns `-1`.
         */
        virtual Int doImage1DForName(const std::string& name);

        /**
         * @brief Implementation for @ref image1DName()
         *
         * Default implementation returns empty string.
         */
        virtual std::string doImage1DName(UnsignedInt id);

        /** @brief Implementation for @ref image1D() */
        virtual std::optional<ImageData1D> doImage1D(UnsignedInt id);

        /**
         * @brief Implementation for @ref image2DCount()
         *
         * Default implementation returns `0`.
         */
        virtual UnsignedInt doImage2DCount() const;

        /**
         * @brief Implementation for @ref image2DForName()
         *
         * Default implementation returns `-1`.
         */
        virtual Int doImage2DForName(const std::string& name);

        /**
         * @brief Implementation for @ref image2DName()
         *
         * Default implementation returns empty string.
         */
        virtual std::string doImage2DName(UnsignedInt id);

        /** @brief Implementation for @ref image2D() */
        virtual std::optional<ImageData2D> doImage2D(UnsignedInt id);

        /**
         * @brief Implementation for @ref image3DCount()
         *
         * Default implementation returns `0`.
         */
        virtual UnsignedInt doImage3DCount() const;

        /**
         * @brief Implementation for @ref image3DForName()
         *
         * Default implementation returns `-1`.
         */
        virtual Int doImage3DForName(const std::string& name);

        /**
         * @brief Implementation for @ref image3DName()
         *
         * Default implementation returns empty string.
         */
        virtual std::string doImage3DName(UnsignedInt id);

        /** @brief Implementation for @ref image3D() */
        virtual std::optional<ImageData3D> doImage3D(UnsignedInt id);

        /** @brief Implementation for @ref importerState() */
        virtual const void* doImporterState() const;
};

CORRADE_ENUMSET_OPERATORS(AbstractImporter::Features)

}}

#endif
