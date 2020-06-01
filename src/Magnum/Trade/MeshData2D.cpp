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

#define _MAGNUM_NO_DEPRECATED_MESHDATA /* So it doesn't yell here */

#include "MeshData2D.h"

#include <Corrade/Containers/ArrayViewStl.h>

#include "Magnum/Math/Color.h"

namespace Magnum { namespace Trade {

MeshData2D::MeshData2D(const MeshPrimitive primitive, std::vector<UnsignedInt> indices, std::vector<std::vector<Vector2>> positions, std::vector<std::vector<Vector2>> textureCoords2D, std::vector<std::vector<Color4>> colors, const void* const importerState): _primitive{primitive}, _indices{std::move(indices)}, _positions{std::move(positions)}, _textureCoords2D{std::move(textureCoords2D)}, _colors{std::move(colors)}, _importerState{importerState} {
    CORRADE_ASSERT(!_positions.empty(), "Trade::MeshData2D: no position array specified", );
}

#ifdef MAGNUM_BUILD_DEPRECATED
MeshData2D::MeshData2D(const MeshData& other): _primitive{other.primitive()}, _importerState{other.importerState()} {
    /* Copy indices, if any */
    if(other.isIndexed()) {
        _indices.resize(other.indexCount());
        other.indicesInto(_indices);
    }

    /* Copy attributes */
    _positions.resize(other.attributeCount(MeshAttribute::Position));
    for(UnsignedInt i = 0; i != _positions.size(); ++i) {
        _positions[i].resize(other.vertexCount());
        other.positions2DInto(_positions[i], i);
    }
    _textureCoords2D.resize(other.attributeCount(MeshAttribute::TextureCoordinates));
    for(UnsignedInt i = 0; i != _textureCoords2D.size(); ++i) {
        _textureCoords2D[i].resize(other.vertexCount());
        other.textureCoordinates2DInto(_textureCoords2D[i], i);
    }
    _colors.resize(other.attributeCount(MeshAttribute::Color));
    for(UnsignedInt i = 0; i != _colors.size(); ++i) {
        _colors[i].resize(other.vertexCount());
        other.colorsInto(_colors[i], i);
    }

    CORRADE_ASSERT(!_positions.empty(), "Trade::MeshData3D: no position array specified in MeshData", );
}
#endif

CORRADE_IGNORE_DEPRECATED_PUSH /* MSVC warns here */
MeshData2D::MeshData2D(MeshData2D&&)
    #if !defined(__GNUC__) || __GNUC__*100 + __GNUC_MINOR__ != 409
    noexcept
    #endif
    = default;
CORRADE_IGNORE_DEPRECATED_POP

MeshData2D::~MeshData2D() = default;

CORRADE_IGNORE_DEPRECATED_PUSH /* GCC why you warn on return and not on param */
MeshData2D& MeshData2D::operator=(MeshData2D&&)
    #if !defined(__GNUC__) || __GNUC__*100 + __GNUC_MINOR__ != 409
    noexcept
    #endif
    = default;
CORRADE_IGNORE_DEPRECATED_POP

std::vector<UnsignedInt>& MeshData2D::indices() {
    CORRADE_ASSERT(isIndexed(), "Trade::MeshData2D::indices(): the mesh is not indexed", _indices);
    return _indices;
}

const std::vector<UnsignedInt>& MeshData2D::indices() const {
    CORRADE_ASSERT(isIndexed(), "Trade::MeshData2D::indices(): the mesh is not indexed", _indices);
    return _indices;
}

std::vector<Vector2>& MeshData2D::positions(const UnsignedInt id) {
    CORRADE_ASSERT(id < positionArrayCount(), "Trade::MeshData2D::positions(): index out of range", _positions[id]);
    return _positions[id];
}

const std::vector<Vector2>& MeshData2D::positions(const UnsignedInt id) const {
    CORRADE_ASSERT(id < positionArrayCount(), "Trade::MeshData2D::positions(): index out of range", _positions[id]);
    return _positions[id];
}

std::vector<Vector2>& MeshData2D::textureCoords2D(const UnsignedInt id) {
    CORRADE_ASSERT(id < textureCoords2DArrayCount(), "Trade::MeshData2D::textureCoords2D(): index out of range", _textureCoords2D[id]);
    return _textureCoords2D[id];
}

const std::vector<Vector2>& MeshData2D::textureCoords2D(const UnsignedInt id) const {
    CORRADE_ASSERT(id < textureCoords2DArrayCount(), "Trade::MeshData2D::textureCoords2D(): index out of range", _textureCoords2D[id]);
    return _textureCoords2D[id];
}

std::vector<Color4>& MeshData2D::colors(const UnsignedInt id) {
    CORRADE_ASSERT(id < colorArrayCount(), "Trade::MeshData3D::colors(): index out of range", _colors[id]);
    return _colors[id];
}

const std::vector<Color4>& MeshData2D::colors(const UnsignedInt id) const {
    CORRADE_ASSERT(id < colorArrayCount(), "Trade::MeshData3D::colors(): index out of range", _colors[id]);
    return _colors[id];
}

}}
