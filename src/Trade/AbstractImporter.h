#ifndef Magnum_Trade_AbstractImporter_h
#define Magnum_Trade_AbstractImporter_h
/*
    Copyright © 2010, 2011, 2012 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

/** @file
 * @brief Class Magnum::Trade::AbstractImporter
 */

#include <Containers/EnumSet.h>
#include <PluginManager/Plugin.h>

#include "Trade/Trade.h"

#include "magnumVisibility.h"

namespace Magnum { namespace Trade {

/**
@brief Base for importer plugins

Importer is used for importing data like scenes, lights, objects, images,
textures etc.

@section AbstractImporter-subclassing Subclassing
Plugin implements function features(), one or more open() functions,
function close() and one or more pairs of data access functions, based on
which features are supported in given format.

For multi-data formats file opening shouldn't take long, all parsing should
be done in data parsing functions, because the user might want to import only
some data. This is obviously not the case for single-data formats like images,
as the file contains all data user wants to import.
*/
class MAGNUM_EXPORT AbstractImporter: public Corrade::PluginManager::Plugin {
    PLUGIN_INTERFACE("cz.mosra.magnum.Trade.AbstractImporter/0.2")

    public:
        /**
         * @brief Features supported by this importer
         *
         * @see Features, features()
         */
        enum class Feature {
            OpenFile = 0x01,    /**< Can open files specified by filename */
            OpenStream = 0x02   /**< Can open files from input streams */
        };

        /** @brief Set of features supported by this importer */
        typedef Corrade::Containers::EnumSet<Feature, int> Features;

        /** @brief Constructor */
        /* GCC 4.6 can't handle {} as default parameter */
        inline explicit AbstractImporter(Corrade::PluginManager::AbstractPluginManager* manager = nullptr, const std::string& plugin = std::string()): Plugin(manager, plugin) {}

        /** @brief Features supported by this importer */
        virtual Features features() const = 0;

        /**
         * @brief Open file
         * @param filename  Filename
         * @return Whether the file was successfully opened
         *
         * Closes previous file, if it was opened, and tries to open given
         * file. See also @ref Feature "Feature::OpenFile". Default
         * implementation prints message to error output and returns false.
         */
        virtual bool open(const std::string& filename);

        /**
         * @brief Open stream
         * @param in        Input stream
         * @return Whether the file was successfully opened
         *
         * See also open(const std::string&), @ref Feature
         * "Feature::OpenStream". Default implementation prints message to
         * error output and returns false.
         */
        virtual bool open(std::istream& in);

        /** @brief Close file */
        virtual void close() = 0;

        /** @{ @name Data accessors
         * Each function pair provides access to the data.
         */

        /**
         * @brief Default scene
         *
         * When there is more than one scene, returns ID of the default one.
         * If there is no default scene, returns -1.
         *
         * @note The function is not const, because the value will probably
         *      be lazy-populated.
         */
        virtual inline Int defaultScene() { return -1; }

        /** @brief %Scene count */
        virtual inline UnsignedInt sceneCount() const { return 0; }

        /**
         * @brief %Scene ID for given name
         *
         * If no scene for given name exists, returns -1.
         * @see sceneName()
         */
        virtual Int sceneForName(const std::string& name);

        /**
         * @brief %Scene name
         * @param id        %Scene ID, from range [0, sceneCount()).
         *
         * @see sceneForName()
         */
        virtual std::string sceneName(UnsignedInt id);

        /**
         * @brief %Scene
         * @param id        %Scene ID, from range [0, sceneCount()).
         *
         * Returns pointer to given scene or nullptr, if no such scene exists.
         */
        virtual SceneData* scene(UnsignedInt id);

        /** @brief %Light count */
        virtual inline UnsignedInt lightCount() const { return 0; }

        /**
         * @brief %Light ID for given name
         *
         * If no light for given name exists, returns -1.
         * @see lightName()
         */
        virtual Int lightForName(const std::string& name);

        /**
         * @brief %Light name
         * @param id        %Light ID, from range [0, lightCount()).
         *
         * @see lightForName()
         */
        virtual std::string lightName(UnsignedInt id);

        /**
         * @brief %Light
         * @param id        %Light ID, from range [0, lightCount()).
         *
         * Returns pointer to given light or nullptr, if no such light exists.
         */
        virtual LightData* light(UnsignedInt id);

        /** @brief Camera count */
        virtual inline UnsignedInt cameraCount() const { return 0; }

        /**
         * @brief Camera ID for given name
         *
         * If no camera for given name exists, returns -1.
         * @see cameraName()
         */
        virtual Int cameraForName(const std::string& name);

        /**
         * @brief Camera name
         * @param id        Camera ID, from range [0, cameraCount()).
         *
         * @see cameraForName()
         */
        virtual std::string cameraName(UnsignedInt id);

        /**
         * @brief Camera
         * @param id        Camera ID, from range [0, cameraCount()).
         *
         * Returns pointer to given camera or nullptr, if no such camera
         * exists.
         */
        virtual CameraData* camera(UnsignedInt id);

        /** @brief Two-dimensional object count */
        virtual inline UnsignedInt object2DCount() const { return 0; }

        /**
         * @brief Two-dimensional object ID for given name
         *
         * If no scene for given name exists, returns -1.
         * @see object2DName()
         */
        virtual Int object2DForName(const std::string& name);

        /**
         * @brief Two-dimensional object name
         * @param id        Object ID, from range [0, object2DCount()).
         *
         * @see object2DForName()
         */
        virtual std::string object2DName(UnsignedInt id);

        /**
         * @brief Two-dimensional object
         * @param id        Object ID, from range [0, object2DCount()).
         *
         * Returns pointer to given object or nullptr, if no such object
         * exists.
         */
        virtual ObjectData2D* object2D(UnsignedInt id);

        /** @brief Three-dimensional object count */
        virtual inline UnsignedInt object3DCount() const { return 0; }

        /**
         * @brief Three-dimensional object ID for given name
         *
         * If no scene for given name exists, returns -1.
         * @see object3DName()
         */
        virtual Int object3DForName(const std::string& name);

        /**
         * @brief Three-dimensional object name
         * @param id        Object ID, from range [0, object3DCount()).
         *
         * @see object3DForName()
         */
        virtual std::string object3DName(UnsignedInt id);

        /**
         * @brief Three-dimensional object
         * @param id        Object ID, from range [0, object3DCount()).
         *
         * Returns pointer to given object or nullptr, if no such object
         * exists.
         */
        virtual ObjectData3D* object3D(UnsignedInt id);

        /** @brief Two-dimensional mesh count */
        virtual inline UnsignedInt mesh2DCount() const { return 0; }

        /**
         * @brief Two-dimensional mesh ID for given name
         *
         * If no mesh for given name exists, returns -1.
         * @see mesh2DName()
         */
        virtual Int mesh2DForName(const std::string& name);

        /**
         * @brief Two-dimensional mesh name
         * @param id        %Mesh ID, from range [0, mesh2DCount()).
         *
         * @see mesh2DForName()
         */
        virtual std::string mesh2DName(UnsignedInt id);

        /**
         * @brief Two-dimensional mesh
         * @param id        %Mesh ID, from range [0, mesh2DCount()).
         *
         * Returns pointer to given mesh or nullptr, if no such mesh exists.
         */
        virtual MeshData2D* mesh2D(UnsignedInt id);

        /** @brief Three-dimensional mesh count */
        virtual inline UnsignedInt mesh3DCount() const { return 0; }

        /**
         * @brief Three-dimensional mesh ID for given name
         *
         * If no mesh for given name exists, returns -1.
         * @see mesh3DName()
         */
        virtual Int mesh3DForName(const std::string& name);

        /**
         * @brief Three-dimensional mesh name
         * @param id        %Mesh ID, from range [0, mesh3DCount()).
         *
         * @see mesh3DForName()
         */
        virtual std::string mesh3DName(UnsignedInt id);

        /**
         * @brief Three-dimensional mesh
         * @param id        %Mesh ID, from range [0, mesh3DCount()).
         *
         * Returns pointer to given mesh or nullptr, if no such mesh exists.
         */
        virtual MeshData3D* mesh3D(UnsignedInt id);

        /** @brief Material count */
        virtual inline UnsignedInt materialCount() const { return 0; }

        /**
         * @brief Material ID for given name
         *
         * If no material for given name exists, returns -1.
         * @see materialName()
         */
        virtual Int materialForName(const std::string& name);

        /**
         * @brief Material name
         * @param id        Material ID, from range [0, materialCount()).
         *
         * @see materialForName()
         */
        virtual std::string materialName(UnsignedInt id);

        /**
         * @brief Material
         * @param id        Material ID, from range [0, materialCount()).
         *
         * Returns pointer to given material or nullptr, if no such material
         * exists.
         */
        virtual AbstractMaterialData* material(UnsignedInt id);

        /** @brief %Texture count */
        virtual inline UnsignedInt textureCount() const { return 0; }

        /**
         * @brief %Texture ID for given name
         *
         * If no texture for given name exists, returns -1.
         * @see textureName()
         */
        virtual Int textureForName(const std::string& name);

        /**
         * @brief %Texture name
         * @param id        %Texture ID, from range [0, textureCount()).
         *
         * @see textureForName()
         */
        virtual std::string textureName(UnsignedInt id);

        /**
         * @brief %Texture
         * @param id        %Texture ID, from range [0, textureCount()).
         *
         * Returns pointer to given texture or nullptr, if no such texture
         * exists.
         */
        virtual TextureData* texture(UnsignedInt id);

        /** @brief One-dimensional image count */
        virtual inline UnsignedInt image1DCount() const { return 0; }

        /**
         * @brief One-dimensional image ID for given name
         *
         * If no image for given name exists, returns -1.
         * @see image1Dname()
         */
        virtual Int image1DForName(const std::string& name);

        /**
         * @brief One-dimensional image name
         * @param id        %Image ID, from range [0, image1DCount()).
         *
         * @see image1DForName()
         */
        virtual std::string image1DName(UnsignedInt id);

        /**
         * @brief One-dimensional image
         * @param id        %Image ID, from range [0, image1DCount()).
         *
         * Returns pointer to given image or nullptr, if no such image exists.
         */
        virtual ImageData1D* image1D(UnsignedInt id);

        /** @brief Two-dimensional image count */
        virtual inline UnsignedInt image2DCount() const { return 0; }

        /**
         * @brief Two-dimensional image ID for given name
         *
         * If no image for given name exists, returns -1.
         * @see image2DName()
         */
        virtual Int image2DForName(const std::string& name);

        /**
         * @brief Two-dimensional image name
         * @param id        %Image ID, from range [0, image2DCount()).
         *
         * @see image2DForName()
         */
        virtual std::string image2DName(UnsignedInt id);

        /**
         * @brief Two-dimensional image
         * @param id        %Image ID, from range [0, image2DCount()).
         *
         * Returns pointer to given image or nullptr, if no such image exists.
         */
        virtual ImageData2D* image2D(UnsignedInt id);

        /** @brief Three-dimensional image count */
        virtual inline UnsignedInt image3DCount() const { return 0; }

        /**
         * @brief Three-dimensional image ID for given name
         *
         * If no image for given name exists, returns -1.
         * @see image3DName()
         */
        virtual Int image3DForName(const std::string& name);

        /**
         * @brief Three-dimensional image name
         * @param id        %Image ID, from range [0, image3DCount()).
         *
         * @see image3DForName()
         */
        virtual std::string image3DName(UnsignedInt id);

        /**
         * @brief Three-dimensional image
         * @param id        %Image ID, from range [0, image3DCount()).
         *
         * Returns pointer to given image or nullptr, if no such image exists.
         */
        virtual ImageData3D* image3D(UnsignedInt id);

        /*@}*/
};

CORRADE_ENUMSET_OPERATORS(AbstractImporter::Features)

/* Implementations for inline functions with unused parameters */
inline Int AbstractImporter::sceneForName(const std::string&) { return -1; }
inline std::string AbstractImporter::sceneName(UnsignedInt) { return {}; }
inline SceneData* AbstractImporter::scene(UnsignedInt) { return nullptr; }
inline Int AbstractImporter::lightForName(const std::string&) { return -1; }
inline std::string AbstractImporter::lightName(UnsignedInt) { return {}; }
inline LightData* AbstractImporter::light(UnsignedInt) { return nullptr; }
inline Int AbstractImporter::cameraForName(const std::string&) { return -1; }
inline std::string AbstractImporter::cameraName(UnsignedInt) { return {}; }
inline CameraData* AbstractImporter::camera(UnsignedInt) { return nullptr; }
inline Int AbstractImporter::object2DForName(const std::string&) { return -1; }
inline std::string AbstractImporter::object2DName(UnsignedInt) { return {}; }
inline ObjectData2D* AbstractImporter::object2D(UnsignedInt) { return nullptr; }
inline Int AbstractImporter::object3DForName(const std::string&) { return -1; }
inline std::string AbstractImporter::object3DName(UnsignedInt) { return {}; }
inline ObjectData3D* AbstractImporter::object3D(UnsignedInt) { return nullptr; }
inline Int AbstractImporter::mesh2DForName(const std::string&) { return -1; }
inline std::string AbstractImporter::mesh2DName(UnsignedInt) { return {}; }
inline MeshData2D* AbstractImporter::mesh2D(UnsignedInt) { return nullptr; }
inline Int AbstractImporter::mesh3DForName(const std::string&) { return -1; }
inline std::string AbstractImporter::mesh3DName(UnsignedInt) { return {}; }
inline MeshData3D* AbstractImporter::mesh3D(UnsignedInt) { return nullptr; }
inline Int AbstractImporter::materialForName(const std::string&) { return -1; }
inline std::string AbstractImporter::materialName(UnsignedInt) { return {}; }
inline AbstractMaterialData* AbstractImporter::material(UnsignedInt) { return nullptr; }
inline Int AbstractImporter::textureForName(const std::string&) { return -1; }
inline std::string AbstractImporter::textureName(UnsignedInt) { return {}; }
inline TextureData* AbstractImporter::texture(UnsignedInt) { return nullptr; }
inline Int AbstractImporter::image1DForName(const std::string&) { return -1; }
inline std::string AbstractImporter::image1DName(UnsignedInt) { return {}; }
inline ImageData1D* AbstractImporter::image1D(UnsignedInt) { return nullptr; }
inline Int AbstractImporter::image2DForName(const std::string&) { return -1; }
inline std::string AbstractImporter::image2DName(UnsignedInt) { return {}; }
inline ImageData2D* AbstractImporter::image2D(UnsignedInt) { return nullptr; }
inline Int AbstractImporter::image3DForName(const std::string&) { return -1; }
inline std::string AbstractImporter::image3DName(UnsignedInt) { return {}; }
inline ImageData3D* AbstractImporter::image3D(UnsignedInt) { return nullptr; }

}}

#endif
