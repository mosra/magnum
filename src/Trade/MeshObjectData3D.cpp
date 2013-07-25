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

#include "MeshObjectData3D.h"

namespace Magnum { namespace Trade {

MeshObjectData3D::MeshObjectData3D(std::vector<UnsignedInt> children, const Matrix4& transformation, UnsignedInt instance, UnsignedInt material, std::vector<std::pair<UnsignedInt, UnsignedInt>> textureCoordinate2DBindings): ObjectData3D(std::move(children), transformation, InstanceType::Mesh, instance), _material(material), _textureCoordinate2DBindings(std::move(textureCoordinate2DBindings)) {}

MeshObjectData3D::MeshObjectData3D(MeshObjectData3D&&) = default;

MeshObjectData3D::~MeshObjectData3D() = default;

MeshObjectData3D& MeshObjectData3D::operator=(MeshObjectData3D&&) = default;

}}
