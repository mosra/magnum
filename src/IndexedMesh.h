#ifndef Magnum_IndexedMesh_h
#define Magnum_IndexedMesh_h
/*
    Copyright © 2010 Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Class Magnum::IndexedMesh
 */

#include "Mesh.h"
#include "Buffer.h"

namespace Magnum {

/**
 * @brief Indexed mesh
 */
class IndexedMesh: public Mesh {
    DISABLE_COPY(IndexedMesh)

    public:
        /**
         * @brief Constructor
         * @param _primitive    Primitive type
         * @param _vertexCount  Count of unique vertices
         * @param _indexCount   Count of indices
         * @param _indexType    Type of indices (GL_UNSIGNED_BYTE,
         *      GL_UNSIGNED_SHORT or GL_UNSIGNED_INT)
         */
        inline IndexedMesh(Primitive _primitive, GLsizei _vertexCount, GLsizei _indexCount, GLenum _indexType = GL_UNSIGNED_SHORT): Mesh(_primitive, _vertexCount), _indexBuffer(Buffer::ElementArrayBuffer), indexCount(_indexCount), indexType(_indexType) {}

        /**
         * @brief Index buffer
         *
         * Returns pointer to index buffer, which should be then filled with
         * indices (of type specified in constructor).
         */
        inline Buffer* indexBuffer() { return &_indexBuffer; }

        /**
         * @brief Draw the mesh
         *
         * Binds attributes to buffers, bind index buffer and draws the mesh.
         * Expects an active shader with all uniforms set.
         */
        virtual void draw();

    private:
        Buffer _indexBuffer;
        GLsizei indexCount;
        GLenum indexType;
};

}

#endif
