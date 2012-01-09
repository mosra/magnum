#ifndef Magnum_AbstractImporter_h
#define Magnum_AbstractImporter_h
/*
    Copyright © 2010, 2011 Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Class Magnum::AbstractImporter
 */

#include <memory>

#include "PluginManager/Plugin.h"
#include "Image.h"

namespace Magnum {

class AbstractShaderProgram;
class AbstractTexture;
class AbstractMaterial;
class Camera;
class Light;
class Mesh;
class Object;
class Scene;

/**
@brief Base class for importer plugins

Importer is used for importing data like scenes, lights, objects, images,
textures etc.

@section AbstractImporterSubclassing Subclassing
<p>Plugin implements functions open(), close() and one or more pairs of data
access functions, based on which features are supported in given format.</p>
<p>For multi-data formats file opening shouldn't take long, all parsing should
be done in data parsing functions, because the user might want to import only
some data. This is obviously not the case for single-data formats like images,
as the file contains all data user wants to import.</p>

@subsection AbstractImporterMemoryManagement Memory management
<p>Every data access function returns std::shared_ptr, thus deletion of
underlying data is done automatically when last shared pointer instance is
destroyed. This allows for data reusing, e.g. one material can be used for many
meshes without the need for complex memory management.</p>
<p>Except for objects, the class should store its own copies of shared pointers
for all requested data until the file is closed, so when user requests the data
and then destroys his copy of shared pointer, the data are not deleted (and next
request will not require parsing them again).</p>
<p>As objects have their own hierarchy which doesn't involve shared pointers,
having copies of shared pointers for them will lead to dangling pointers when
any object deletes its child objects. Thus the class should store only one
shared pointer to root of each object tree.</p>
*/
class AbstractImporter: public Corrade::PluginManager::Plugin {
    PLUGIN_INTERFACE("cz.mosra.magnum.AbstractImporter/0.1")

    public:
        /** @brief Constructor */
        AbstractImporter(Corrade::PluginManager::AbstractPluginManager* manager = nullptr, const std::string& plugin = ""): Plugin(manager, plugin) {}

        /**
         * @brief Open file
         * @param in        Input stream
         * @return Whether the file was successfully opened
         *
         * Closes previous file, if it was opened, and tries to open given file.
         */
        virtual bool open(std::istream& in) = 0;

        /** @brief Close file */
        virtual void close() = 0;

        /** @{ @name Data accessors
         * Each function pair provides access to the data. The data are usually
         * hierarchic, so in most cases scene will contain all objects, every
         * object will have one of the materials, every material will have
         * some shader for rendering and possibly even some textures, which are
         * finally composed from images.
         */

        /** @brief Scene count */
        virtual inline size_t sceneCount() const { return 0; }

        /**
         * @brief Scene
         * @param id        Scene ID, from range [0, sceneCount()).
         *
         * Returns (shared) pointer to given scene or nullptr, if no such scene
         * exists.
         */
        virtual inline std::shared_ptr<Scene> scene(size_t id) { return nullptr; }

        /** @brief Light count */
        virtual inline size_t lightCount() const { return 0; }

        /**
         * @brief Light
         * @param id        Light ID, from range [0, lightCount()).
         *
         * Returns (shared) pointer to given light or nullptr, if no such
         * light exists.
         */
        virtual inline std::shared_ptr<Light> light(size_t id) { return nullptr; }

        /** @brief Camera count */
        virtual inline size_t cameraCount() const { return 0; }

        /**
         * @brief Camera
         * @param id        Camera ID, from range [0, cameraCount()).
         *
         * Returns (shared) pointer to given camera or nullptr, if no such
         * camera exists.
         */
        virtual inline std::shared_ptr<Camera> camera(size_t id) { return nullptr; }

        /** @brief Object count (without lights and cameras) */
        virtual inline size_t objectCount() const { return 0; }

        /**
         * @brief Object
         * @param id        Object ID, from range [0, objectCount()).
         *
         * Returns (shared) pointer to given object or nullptr, if no such
         * object exists.
         */
        virtual inline std::shared_ptr<Object> object(size_t id) { return nullptr; }

        /** @brief Mesh count */
        virtual inline size_t meshCount() const { return 0; }

        /**
         * @brief Mesh
         * @param id        Mesh ID, from range [0, meshCount()).
         *
         * Returns (shared) pointer to given mesh or nullptr, if no such
         * mesh exists.
         */
        virtual inline std::shared_ptr<Mesh> mesh(size_t id) { return nullptr; }

        /** @brief Material count */
        virtual inline size_t materialCount() const { return 0; }

        /**
         * @brief Material
         * @param id        Material ID, from range [0, materialCount()).
         *
         * Returns (shared) pointer to given material or nullptr, if no such
         * material exists.
         */
        virtual inline std::shared_ptr<AbstractMaterial> material(size_t id) { return nullptr; }

        /** @brief Shader count */
        virtual inline size_t shaderCount() const { return 0; }

        /**
         * @brief Shader
         * @param id        Shader ID, from range [0, shaderCount()).
         *
         * Returns (shared) pointer to given shader or nullptr, if no such
         * shader exists.
         */
        virtual inline std::shared_ptr<AbstractShaderProgram> shader(size_t id) { return nullptr; }

        /** @brief Texture count */
        virtual inline size_t textureCount() const { return 0; }

        /**
         * @brief Texture
         * @param id        Texture ID, from range [0, textureCount()).
         *
         * Returns (shared) pointer to given texture or nullptr, if no such
         * texture exists.
         */
        virtual inline std::shared_ptr<AbstractTexture> texture(size_t id) { return nullptr; }

        /** @brief One-dimensional image count */
        virtual inline size_t image1DCount() const { return 0; }

        /**
         * @brief One-dimensional image
         * @param id        Image ID, from range [0, image1DCount()).
         *
         * Returns (shared) pointer to given image or nullptr, if no such image
         * exists.
         */
        virtual inline std::shared_ptr<Image1D> image1D(size_t id) { return nullptr; }

        /** @brief Two-dimensional image count */
        virtual inline size_t image2DCount() const { return 0; }

        /**
         * @brief Two-dimensional image
         * @param id        Image ID, from range [0, image2DCount()).
         *
         * Returns (shared) pointer to given image or nullptr, if no such image
         * exists.
         */
        virtual inline std::shared_ptr<Image2D> image2D(size_t id) { return nullptr; }

        /** @brief Three-dimensional image count */
        virtual inline size_t image3DCount() const { return 0; }

        /**
         * @brief Three-dimensional image
         * @param id        Image ID, from range [0, image3DCount()).
         *
         * Returns (shared) pointer to given image or nullptr, if no such image
         * exists.
         */
        virtual inline std::shared_ptr<Image3D> image3D(size_t id) { return nullptr; }

        /*@}*/
};

}

#endif
