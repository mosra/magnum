#ifndef Magnum_Trade_AbstractImporter_h
#define Magnum_Trade_AbstractImporter_h
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

/** @file
 * @brief Class Magnum::Trade::AbstractImporter
 */

#include <Containers/EnumSet.h>
#include <PluginManager/AbstractPlugin.h>

#include "Magnum.h"
#include "magnumVisibility.h"
#include "Trade/Trade.h"

namespace Magnum { namespace Trade {

/**
@brief Base for importer plugins

Importer is used for importing data like scenes, lights, objects, images,
textures etc.

@section AbstractImporter-subclassing Subclassing

Plugin implements function doFeatures(), doIsOpened(), one of or both
doOpenData() and doOpenFile() functions, function doClose() and one or more
tuples of data access functions, based on which features are supported in given
format.

For multi-data formats file opening shouldn't take long, all parsing should
be done in data parsing functions, because the user might want to import only
some data. This is obviously not the case for single-data formats like images,
as the file contains all data user wants to import.

You don't need to do most of the redundant sanity checks, these things are
checked by the implementation:

-   Functions doOpenData() and doOpenFile() are called after the previous file
    was closed, function doClose() is called only if there is any file opened.
-   Function doOpenData() is called only if @ref Feature "Feature::OpenData"
    is supported.
-   All `do*()` implementations working on opened file are called only if
    there is any file opened.
-   All `do*()` implementations taking data ID as parameter are called only if
    the ID is from valid range.
*/
class MAGNUM_EXPORT AbstractImporter: public PluginManager::AbstractPlugin {
    CORRADE_PLUGIN_INTERFACE("cz.mosra.magnum.Trade.AbstractImporter/0.3")

    public:
        /**
         * @brief Features supported by this importer
         *
         * @see Features, features()
         */
        enum class Feature: UnsignedByte {
            /** Opening files from raw data using openData() */
            OpenData = 1 << 0
        };

        /** @brief Set of features supported by this importer */
        typedef Containers::EnumSet<Feature, UnsignedByte> Features;

        /** @brief Default constructor */
        explicit AbstractImporter();

        /** @brief Plugin manager constructor */
        explicit AbstractImporter(PluginManager::AbstractManager* manager, std::string plugin);

        /** @brief Features supported by this importer */
        Features features() const { return doFeatures(); }

        /** @brief Whether any file is opened */
        bool isOpened() const { return doIsOpened(); }

        /**
         * @brief Open raw data
         * @param data      Data
         * @param size      Data size
         *
         * Closes previous file, if it was opened, and tries to open given
         * file. Available only if @ref Feature "Feature::OpenData" is
         * supported. Returns `true` on success, `false` otherwise.
         * @see features(), openFile()
         */
        bool openData(Containers::ArrayReference<const unsigned char> data);

        /**
         * @brief Open file
         * @param filename  Filename
         *
         * Closes previous file, if it was opened, and tries to open given
         * file. Returns `true` on success, `false` otherwise.
         * @see features(), openData()
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

        /** @brief %Scene count */
        UnsignedInt sceneCount() const;

        /**
         * @brief %Scene ID for given name
         *
         * If no scene for given name exists, returns `-1`.
         * @see sceneName()
         */
        Int sceneForName(const std::string& name);

        /**
         * @brief %Scene name
         * @param id        %Scene ID, from range [0, sceneCount()).
         *
         * @see sceneForName()
         */
        std::string sceneName(UnsignedInt id);

        /**
         * @brief %Scene
         * @param id        %Scene ID, from range [0, sceneCount()).
         *
         * Returns given scene or `nullptr` if import failed. Deleting the data
         * is user responsibility.
         */
        SceneData* scene(UnsignedInt id);

        /** @brief %Light count */
        UnsignedInt lightCount() const;

        /**
         * @brief %Light ID for given name
         *
         * If no light for given name exists, returns `-1`.
         * @see lightName()
         */
        Int lightForName(const std::string& name);

        /**
         * @brief %Light name
         * @param id        %Light ID, from range [0, lightCount()).
         *
         * @see lightForName()
         */
        std::string lightName(UnsignedInt id);

        /**
         * @brief %Light
         * @param id        %Light ID, from range [0, lightCount()).
         *
         * Returns given light or `nullptr` if importing failed. Deleting the
         * data is user responsibility.
         */
        LightData* light(UnsignedInt id);

        /** @brief Camera count */
        UnsignedInt cameraCount() const;

        /**
         * @brief Camera ID for given name
         *
         * If no camera for given name exists, returns `-1`.
         * @see cameraName()
         */
        Int cameraForName(const std::string& name);

        /**
         * @brief Camera name
         * @param id        Camera ID, from range [0, cameraCount()).
         *
         * @see cameraForName()
         */
        std::string cameraName(UnsignedInt id);

        /**
         * @brief Camera
         * @param id        Camera ID, from range [0, cameraCount()).
         *
         * Returns given camera or `nullptr` if importing failed. Deleting the
         * data is user responsibility.
         */
        CameraData* camera(UnsignedInt id);

        /** @brief Two-dimensional object count */
        UnsignedInt object2DCount() const;

        /**
         * @brief Two-dimensional object ID for given name
         *
         * If no scene for given name exists, returns `-1`.
         * @see object2DName()
         */
        Int object2DForName(const std::string& name);

        /**
         * @brief Two-dimensional object name
         * @param id        Object ID, from range [0, object2DCount()).
         *
         * @see object2DForName()
         */
        std::string object2DName(UnsignedInt id);

        /**
         * @brief Two-dimensional object
         * @param id        Object ID, from range [0, object2DCount()).
         *
         * Returns given object or `nullptr` if importing failed. Deleting the
         * data is user responsibility.
         */
        ObjectData2D* object2D(UnsignedInt id);

        /** @brief Three-dimensional object count */
        UnsignedInt object3DCount() const;

        /**
         * @brief Three-dimensional object ID for given name
         *
         * If no scene for given name exists, returns `-1`.
         * @see object3DName()
         */
        Int object3DForName(const std::string& name);

        /**
         * @brief Three-dimensional object name
         * @param id        Object ID, from range [0, object3DCount()).
         *
         * @see object3DForName()
         */
        std::string object3DName(UnsignedInt id);

        /**
         * @brief Three-dimensional object
         * @param id        Object ID, from range [0, object3DCount()).
         *
         * Returns given object or `nullptr` if importing failed. Deleting the
         * data is user responsibility.
         */
        ObjectData3D* object3D(UnsignedInt id);

        /** @brief Two-dimensional mesh count */
        UnsignedInt mesh2DCount() const;

        /**
         * @brief Two-dimensional mesh ID for given name
         *
         * If no mesh for given name exists, returns `-1`.
         * @see mesh2DName()
         */
        Int mesh2DForName(const std::string& name);

        /**
         * @brief Two-dimensional mesh name
         * @param id        %Mesh ID, from range [0, mesh2DCount()).
         *
         * @see mesh2DForName()
         */
        std::string mesh2DName(UnsignedInt id);

        /**
         * @brief Two-dimensional mesh
         * @param id        %Mesh ID, from range [0, mesh2DCount()).
         *
         * Returns given mesh or `nullptr` if importing failed. Deleting the
         * data is user responsibility.
         */
        MeshData2D* mesh2D(UnsignedInt id);

        /** @brief Three-dimensional mesh count */
        UnsignedInt mesh3DCount() const;

        /**
         * @brief Three-dimensional mesh ID for given name
         *
         * If no mesh for given name exists, returns `-1`.
         * @see mesh3DName()
         */
        Int mesh3DForName(const std::string& name);

        /**
         * @brief Three-dimensional mesh name
         * @param id        %Mesh ID, from range [0, mesh3DCount()).
         *
         * @see mesh3DForName()
         */
        std::string mesh3DName(UnsignedInt id);

        /**
         * @brief Three-dimensional mesh
         * @param id        %Mesh ID, from range [0, mesh3DCount()).
         *
         * Returns given mesh or `nullptr` if importing failed. Deleting the
         * data is user responsibility.
         */
        MeshData3D* mesh3D(UnsignedInt id);

        /** @brief Material count */
        UnsignedInt materialCount() const;

        /**
         * @brief Material ID for given name
         *
         * If no material for given name exists, returns `-1`.
         * @see materialName()
         */
        Int materialForName(const std::string& name);

        /**
         * @brief Material name
         * @param id        Material ID, from range [0, materialCount()).
         *
         * @see materialForName()
         */
        std::string materialName(UnsignedInt id);

        /**
         * @brief Material
         * @param id        Material ID, from range [0, materialCount()).
         *
         * Returns given material or `nullptr` if importing failed. Deleting
         * the data is user responsibility.
         */
        AbstractMaterialData* material(UnsignedInt id);

        /** @brief %Texture count */
        UnsignedInt textureCount() const;

        /**
         * @brief %Texture ID for given name
         *
         * If no texture for given name exists, returns `-1`.
         * @see textureName()
         */
        Int textureForName(const std::string& name);

        /**
         * @brief %Texture name
         * @param id        %Texture ID, from range [0, textureCount()).
         *
         * @see textureForName()
         */
        std::string textureName(UnsignedInt id);

        /**
         * @brief %Texture
         * @param id        %Texture ID, from range [0, textureCount()).
         *
         * Returns given texture or `nullptr` if importing failed. Deleting the
         * data is user responsibility.
         */
        TextureData* texture(UnsignedInt id);

        /** @brief One-dimensional image count */
        UnsignedInt image1DCount() const;

        /**
         * @brief One-dimensional image ID for given name
         *
         * If no image for given name exists, returns `-1`.
         * @see image1Dname()
         */
        Int image1DForName(const std::string& name);

        /**
         * @brief One-dimensional image name
         * @param id        %Image ID, from range [0, image1DCount()).
         *
         * @see image1DForName()
         */
        std::string image1DName(UnsignedInt id);

        /**
         * @brief One-dimensional image
         * @param id        %Image ID, from range [0, image1DCount()).
         *
         * Returns given image or `nullptr` if importing failed. Deleting the
         * data is user responsibility.
         */
        ImageData1D* image1D(UnsignedInt id);

        /** @brief Two-dimensional image count */
        UnsignedInt image2DCount() const;

        /**
         * @brief Two-dimensional image ID for given name
         *
         * If no image for given name exists, returns `-1`.
         * @see image2DName()
         */
        Int image2DForName(const std::string& name);

        /**
         * @brief Two-dimensional image name
         * @param id        %Image ID, from range [0, image2DCount()).
         *
         * @see image2DForName()
         */
        std::string image2DName(UnsignedInt id);

        /**
         * @brief Two-dimensional image
         * @param id        %Image ID, from range [0, image2DCount()).
         *
         * Returns given image or `nullptr` if importing failed. Deleting the
         * data is user responsibility.
         */
        ImageData2D* image2D(UnsignedInt id);

        /** @brief Three-dimensional image count */
        UnsignedInt image3DCount() const;

        /**
         * @brief Three-dimensional image ID for given name
         *
         * If no image for given name exists, returns `-1`.
         * @see image3DName()
         */
        Int image3DForName(const std::string& name);

        /**
         * @brief Three-dimensional image name
         * @param id        %Image ID, from range [0, image3DCount()).
         *
         * @see image3DForName()
         */
        std::string image3DName(UnsignedInt id);

        /**
         * @brief Three-dimensional image
         * @param id        %Image ID, from range [0, image3DCount()).
         *
         * Returns given image or `nullptr` if importing failed. Deleting the
         * data is user responsibility.
         */
        ImageData3D* image3D(UnsignedInt id);

        /*@}*/

    #ifndef DOXYGEN_GENERATING_OUTPUT
    private:
    #else
    protected:
    #endif
        /** @brief Implementation for features() */
        virtual Features doFeatures() const = 0;

        /** @brief Implementation for isOpened() */
        virtual bool doIsOpened() const = 0;

        /** @brief Implementation for openData() */
        virtual void doOpenData(Containers::ArrayReference<const unsigned char> data);

        /**
         * @brief Implementation for openFile()
         *
         * If @ref Feature "Feature::OpenData" is supported, default
         * implementation opens the file and calls doOpenData() with its
         * contents.
         */
        virtual void doOpenFile(const std::string& filename);

        /** @brief Implementation for close() */
        virtual void doClose() = 0;

        /** @brief Implementation for defaultScene() */
        virtual Int doDefaultScene();

        /** @brief Implementation for sceneCount() */
        virtual UnsignedInt doSceneCount() const;

        /** @brief Implementation for sceneForName() */
        virtual Int doSceneForName(const std::string& name);

        /** @brief Implementation for sceneName() */
        virtual std::string doSceneName(UnsignedInt id);

        /** @brief Implementation for scene() */
        virtual SceneData* doScene(UnsignedInt id);

        /** @brief Implementation for lightCount() */
        virtual UnsignedInt doLightCount() const;

        /** @brief Implementation for lightForName() */
        virtual Int doLightForName(const std::string& name);

        /** @brief Implementation for lightName() */
        virtual std::string doLightName(UnsignedInt id);

        /** @brief Implementation for light() */
        virtual LightData* doLight(UnsignedInt id);

        /** @brief Implementation for cameraCount() */
        virtual UnsignedInt doCameraCount() const;

        /** @brief Implementation for cameraForName() */
        virtual Int doCameraForName(const std::string& name);

        /** @brief Implementation for cameraName() */
        virtual std::string doCameraName(UnsignedInt id);

        /** @brief Implementation for camera() */
        virtual CameraData* doCamera(UnsignedInt id);

        /** @brief Implementation for object2DCount() */
        virtual UnsignedInt doObject2DCount() const;

        /** @brief Implementation for object2DForName() */
        virtual Int doObject2DForName(const std::string& name);

        /** @brief Implementation for object2DName() */
        virtual std::string doObject2DName(UnsignedInt id);

        /** @brief Implementation for object2D() */
        virtual ObjectData2D* doObject2D(UnsignedInt id);

        /** @brief Implementation for object3DCount() */
        virtual UnsignedInt doObject3DCount() const;

        /** @brief Implementation for object3DForName() */
        virtual Int doObject3DForName(const std::string& name);

        /** @brief Implementation for object3DName() */
        virtual std::string doObject3DName(UnsignedInt id);

        /** @brief Implementation for object3D() */
        virtual ObjectData3D* doObject3D(UnsignedInt id);

        /** @brief Implementation for mesh2DCount() */
        virtual UnsignedInt doMesh2DCount() const;

        /** @brief Implementation for mesh2DForName() */
        virtual Int doMesh2DForName(const std::string& name);

        /** @brief Implementation for mesh2DName() */
        virtual std::string doMesh2DName(UnsignedInt id);

        /** @brief Implementation for mesh2D() */
        virtual MeshData2D* doMesh2D(UnsignedInt id);

        /** @brief Implementation for mesh3DCount() */
        virtual UnsignedInt doMesh3DCount() const;

        /** @brief Implementation for mesh3DForName() */
        virtual Int doMesh3DForName(const std::string& name);

        /** @brief Implementation for mesh3DName() */
        virtual std::string doMesh3DName(UnsignedInt id);

        /** @brief Implementation for mesh3D() */
        virtual MeshData3D* doMesh3D(UnsignedInt id);

        /** @brief Implementation for materialCount() */
        virtual UnsignedInt doMaterialCount() const;

        /** @brief Implementation for materialForName() */
        virtual Int doMaterialForName(const std::string& name);

        /** @brief Implementation for materialName() */
        virtual std::string doMaterialName(UnsignedInt id);

        /** @brief Implementation for material() */
        virtual AbstractMaterialData* doMaterial(UnsignedInt id);

        /** @brief Implementation for textureCount() */
        virtual UnsignedInt doTextureCount() const;

        /** @brief Implementation for textureForName() */
        virtual Int doTextureForName(const std::string& name);

        /** @brief Implementation for textureName() */
        virtual std::string doTextureName(UnsignedInt id);

        /** @brief Implementation for texture() */
        virtual TextureData* doTexture(UnsignedInt id);

        /** @brief Implementation for image1DCount() */
        virtual UnsignedInt doImage1DCount() const;

        /** @brief Implementation for image1DForName() */
        virtual Int doImage1DForName(const std::string& name);

        /** @brief Implementation for image1DName() */
        virtual std::string doImage1DName(UnsignedInt id);

        /** @brief Implementation for image1D() */
        virtual ImageData1D* doImage1D(UnsignedInt id);

        /** @brief Implementation for image2DCount() */
        virtual UnsignedInt doImage2DCount() const;

        /** @brief Implementation for image2DForName() */
        virtual Int doImage2DForName(const std::string& name);

        /** @brief Implementation for image2DName() */
        virtual std::string doImage2DName(UnsignedInt id);

        /** @brief Implementation for image2D() */
        virtual ImageData2D* doImage2D(UnsignedInt id);

        /** @brief Implementation for image3DCount() */
        virtual UnsignedInt doImage3DCount() const;

        /** @brief Implementation for image3DForName() */
        virtual Int doImage3DForName(const std::string& name);

        /** @brief Implementation for image3DName() */
        virtual std::string doImage3DName(UnsignedInt id);

        /** @brief Implementation for image3D() */
        virtual ImageData3D* doImage3D(UnsignedInt id);
};

CORRADE_ENUMSET_OPERATORS(AbstractImporter::Features)

}}

#endif
