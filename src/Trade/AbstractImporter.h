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

#include "PluginManager/Plugin.h"
#include "ImageData.h"

namespace Magnum {

/**
@brief Data format exchange

Contains plugin interfaces for importing data of various formats and classes
for direct access to the data.
*/
namespace Trade {

class AbstractMaterialData;
class CameraData;
class LightData;
class MeshData;
class ObjectData;
class SceneData;
class TextureData;

/**
@brief Base class for importer plugins

Importer is used for importing data like scenes, lights, objects, images,
textures etc.

@section AbstractImporterSubclassing Subclassing
<p>Plugin implements function features(), one or more open() functions,
function close() and one or more pairs of data access functions, based on
which features are supported in given format.</p>
<p>For multi-data formats file opening shouldn't take long, all parsing should
be done in data parsing functions, because the user might want to import only
some data. This is obviously not the case for single-data formats like images,
as the file contains all data user wants to import.</p>
*/
class MAGNUM_EXPORT AbstractImporter: public Corrade::PluginManager::Plugin {
    PLUGIN_INTERFACE("cz.mosra.magnum.Trade.AbstractImporter/0.1")

    public:
        /** @brief Features supported by this importer */
        enum Feature {
            OpenFile = 0x01,    /**< Can open files specified by filename */
            OpenStream = 0x02   /**< Can open files from input streams */
        };

        /** @brief Constructor */
        AbstractImporter(Corrade::PluginManager::AbstractPluginManager* manager = nullptr, const std::string& plugin = ""): Plugin(manager, plugin) {}

        /**
         * @brief Features supported by this importer
         * @return OR-ed combination of values from Feature enum.
         */
        virtual int features() const = 0;

        /**
         * @brief Open file
         * @param filename  Filename
         * @return Whether the file was successfully opened
         *
         * Closes previous file, if it was opened, and tries to open given
         * file. See also Feature::OpenFile. Default implementation prints
         * message to error output and returns false.
         */
        virtual bool open(const std::string& filename);

        /**
         * @brief Open stream
         * @param in        Input stream
         * @return Whether the file was successfully opened
         *
         * See also open(const std::string&), Feature::OpenStream. Default
         * implementation prints message to error output and returns false.
         */
        virtual bool open(std::istream& in);

        /** @brief Close file */
        virtual void close() = 0;

        /** @{ @name Data accessors
         * Each function pair provides access to the data.
         */

        /** @brief Scene count */
        virtual inline size_t sceneCount() const { return 0; }

        /**
         * @brief Scene
         * @param id        Scene ID, from range [0, sceneCount()).
         *
         * Returns pointer to given scene or nullptr, if no such scene exists.
         */
        virtual inline SceneData* scene(size_t id) { return nullptr; }

        /** @brief Light count */
        virtual inline size_t lightCount() const { return 0; }

        /**
         * @brief Light
         * @param id        Light ID, from range [0, lightCount()).
         *
         * Returns pointer to given light or nullptr, if no such light exists.
         */
        virtual inline LightData* light(size_t id) { return nullptr; }

        /** @brief Camera count */
        virtual inline size_t cameraCount() const { return 0; }

        /**
         * @brief Camera
         * @param id        Camera ID, from range [0, cameraCount()).
         *
         * Returns pointer to given camera or nullptr, if no such camera
         * exists.
         */
        virtual inline CameraData* camera(size_t id) { return nullptr; }

        /** @brief Object count (without lights and cameras) */
        virtual inline size_t objectCount() const { return 0; }

        /**
         * @brief Object
         * @param id        Object ID, from range [0, objectCount()).
         *
         * Returns pointer to given object or nullptr, if no such object
         * exists.
         */
        virtual inline ObjectData* object(size_t id) { return nullptr; }

        /** @brief Mesh count */
        virtual inline size_t meshCount() const { return 0; }

        /**
         * @brief Mesh
         * @param id        Mesh ID, from range [0, meshCount()).
         *
         * Returns pointer to given mesh or nullptr, if no such mesh exists.
         */
        virtual inline MeshData* mesh(size_t id) { return nullptr; }

        /** @brief Material count */
        virtual inline size_t materialCount() const { return 0; }

        /**
         * @brief Material
         * @param id        Material ID, from range [0, materialCount()).
         *
         * Returns pointer to given material or nullptr, if no such material
         * exists.
         */
        virtual inline AbstractMaterialData* material(size_t id) { return nullptr; }

        /** @brief Texture count */
        virtual inline size_t textureCount() const { return 0; }

        /**
         * @brief Texture
         * @param id        Texture ID, from range [0, textureCount()).
         *
         * Returns pointer to given texture or nullptr, if no such texture
         * exists.
         */
        virtual inline TextureData* texture(size_t id) { return nullptr; }

        /** @brief One-dimensional image count */
        virtual inline size_t image1DCount() const { return 0; }

        /**
         * @brief One-dimensional image
         * @param id        Image ID, from range [0, image1DCount()).
         *
         * Returns pointer to given image or nullptr, if no such image exists.
         */
        virtual inline ImageData1D* image1D(size_t id) { return nullptr; }

        /** @brief Two-dimensional image count */
        virtual inline size_t image2DCount() const { return 0; }

        /**
         * @brief Two-dimensional image
         * @param id        Image ID, from range [0, image2DCount()).
         *
         * Returns pointer to given image or nullptr, if no such image exists.
         */
        virtual inline ImageData2D* image2D(size_t id) { return nullptr; }

        /** @brief Three-dimensional image count */
        virtual inline size_t image3DCount() const { return 0; }

        /**
         * @brief Three-dimensional image
         * @param id        Image ID, from range [0, image3DCount()).
         *
         * Returns pointer to given image or nullptr, if no such image exists.
         */
        virtual inline ImageData3D* image3D(size_t id) { return nullptr; }

        /*@}*/
};

}}

#endif
