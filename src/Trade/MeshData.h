#ifndef Magnum_Trade_MeshData_h
#define Magnum_Trade_MeshData_h
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
 * @brief Class Magnum::Trade::MeshData
 */

#include "Mesh.h"

namespace Magnum { namespace Trade {

/**
@brief %Mesh data

Provides access to mesh data and additional information, such as primitive
type.
*/
class MAGNUM_EXPORT MeshData {
    MeshData(const MeshData& other) = delete;
    MeshData(MeshData&& other) = delete;
    MeshData& operator=(const MeshData& other) = delete;
    MeshData& operator=(MeshData&& other) = delete;

    public:
        /**
         * @brief Constructor
         * @param name              %Mesh name
         * @param primitive         Primitive
         * @param indices           Array with indices or 0, if this is not
         *      indexed mesh
         * @param vertices          Array with vertex arrays. At least one
         *      vertex array should be present.
         * @param normals           Array with normal arrays or empty array
         * @param textureCoords2D   Array with two-dimensional texture
         *      coordinate arrays or empty array
         */
        inline MeshData(const std::string& name, Mesh::Primitive primitive, std::vector<unsigned int>* indices, std::vector<std::vector<Vector4>*> vertices, std::vector<std::vector<Vector3>*> normals, std::vector<std::vector<Vector2>*> textureCoords2D): _name(name), _primitive(primitive), _indices(indices), _vertices(vertices), _normals(normals), _textureCoords2D(textureCoords2D) {}

        /** @brief Destructor */
        ~MeshData();

        /** @brief %Mesh name */
        inline std::string name() const { return _name; }

        /** @brief Primitive */
        inline Mesh::Primitive primitive() const { return _primitive; }

        /**
         * @brief Indices
         * @return Indices or nullptr if the mesh is not indexed.
         */
        inline std::vector<unsigned int>* indices() { return _indices; }
        inline const std::vector<unsigned int>* indices() const { return _indices; } /**< @overload */

        /** @brief Count of vertex arrays */
        inline unsigned int vertexArrayCount() const { return _vertices.size(); }

        /**
         * @brief Vertices
         * @param id    ID of vertex data array
         * @return Vertices or nullptr if there is no vertex array with given
         *      ID.
         */
        inline std::vector<Vector4>* vertices(unsigned int id) { return _vertices[id]; }
        inline const std::vector<Vector4>* vertices(unsigned int id) const { return _vertices[id]; } /**< @overload */

        /** @brief Count of normal arrays */
        inline unsigned int normalArrayCount() const { return _normals.size(); }

        /**
         * @brief Normals
         * @param id    ID of normal data array
         * @return Vertices or nullptr if there is no normal array with given
         *      ID.
         */
        inline std::vector<Vector3>* normals(unsigned int id) { return _normals[id]; }
        inline const std::vector<Vector3>* normals(unsigned int id) const { return _normals[id]; } /**< @overload */

        /** @brief Count of 2D texture coordinate arrays */
        inline unsigned int textureCoords2DArrayCount() const { return _textureCoords2D.size(); }

        /**
         * @brief 2D texture coordinates
         * @param id    ID of texture coordinates array
         * @return %Texture coordinates or nullptr if there is no texture
         *      coordinates array with given ID.
         */
        inline std::vector<Vector2>* textureCoords2D(unsigned int id) { return _textureCoords2D[id]; }
        inline const std::vector<Vector2>* textureCoords2D(unsigned int id) const { return _textureCoords2D[id]; } /**< @overload */

    private:
        std::string _name;
        Mesh::Primitive _primitive;
        std::vector<unsigned int>* _indices;
        std::vector<std::vector<Vector4>*> _vertices;
        std::vector<std::vector<Vector3>*> _normals;
        std::vector<std::vector<Vector2>*> _textureCoords2D;
};

}}

#endif
