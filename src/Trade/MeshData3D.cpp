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

#include "MeshData3D.h"

#include "Math/Vector3.h"

namespace Magnum { namespace Trade {

MeshData3D::MeshData3D(Mesh::Primitive primitive, std::vector<UnsignedInt>* indices, std::vector<std::vector<Vector3>*> positions, std::vector<std::vector<Vector3>*> normals, std::vector<std::vector<Vector2>*> textureCoords2D): _primitive(primitive), _indices(indices), _positions(std::move(positions)), _normals(std::move(normals)), _textureCoords2D(std::move(textureCoords2D)) {}

MeshData3D::MeshData3D(MeshData3D&& other): _primitive(other._primitive), _indices(other._indices), _positions(std::move(other._positions)), _normals(std::move(other._normals)), _textureCoords2D(std::move(other._textureCoords2D)) {
    other._indices = nullptr;
}

MeshData3D& MeshData3D::operator=(MeshData3D&& other) {
    _primitive = other._primitive;
    std::swap(_indices, other._indices);
    std::swap(_positions, other._positions);
    std::swap(_normals, other._normals);
    std::swap(_textureCoords2D, other._textureCoords2D);
    return *this;
}

MeshData3D::~MeshData3D() {
    delete _indices;
    for(auto it = _positions.begin(); it != _positions.end(); ++it) delete *it;
    for(auto it = _normals.begin(); it != _normals.end(); ++it) delete *it;
    for(auto it = _textureCoords2D.begin(); it != _textureCoords2D.end(); ++it) delete *it;
}

}}
