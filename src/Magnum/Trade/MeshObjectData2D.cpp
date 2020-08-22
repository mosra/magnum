/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

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

#include "MeshObjectData2D.h"

namespace Magnum { namespace Trade {

MeshObjectData2D::MeshObjectData2D(std::vector<UnsignedInt> children, const Matrix3& transformation, const UnsignedInt instance, const Int material, const Int skin, const void* const importerState): ObjectData2D{std::move(children), transformation, ObjectInstanceType2D::Mesh, instance, importerState}, _material{material}, _skin{skin} {}

MeshObjectData2D::MeshObjectData2D(std::vector<UnsignedInt> children, const Vector2& translation, const Complex& rotation, const Vector2& scaling, const UnsignedInt instance, const Int material, const Int skin, const void* const importerState): ObjectData2D{std::move(children), translation, rotation, scaling, ObjectInstanceType2D::Mesh, instance, importerState}, _material{material}, _skin{skin} {}

}}
