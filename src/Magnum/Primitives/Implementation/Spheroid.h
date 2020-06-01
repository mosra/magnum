#ifndef Magnum_Primitives_Implementation_Spheroid_h
#define Magnum_Primitives_Implementation_Spheroid_h
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

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/EnumSet.h>
#include <Corrade/Containers/StridedArrayView.h>

#include "Magnum/Magnum.h"
#include "Magnum/Trade/Trade.h"

namespace Magnum { namespace Primitives { namespace Implementation {

class Spheroid {
    public:
        enum class Flag: UnsignedByte {
            TextureCoordinates = 1 << 0,
            Tangents = 1 << 1
        };

        typedef Containers::EnumSet<Flag> Flags;

        explicit Spheroid(UnsignedInt segments, Flags flags);

        void capVertex(Float y, Float normalY, Float textureCoordsV);
        void hemisphereVertexRings(UnsignedInt count, Float centerY, Rad startRingAngle, Rad ringAngleIncrement, Float startTextureCoordsV, Float textureCoordsVIncrement);
        void cylinderVertexRings(UnsignedInt count, Float startY, const Vector2& increment, Float startTextureCoordsV, Float textureCoordsVIncrement);
        void bottomFaceRing();
        void faceRings(UnsignedInt count, UnsignedInt offset = 1);
        void topFaceRing();
        void capVertexRing(Float y, Float textureCoordsV, const Vector3& normal);

        Trade::MeshData finalize();

    private:
        UnsignedInt _segments;
        Flags _flags;
        std::size_t _stride;
        std::size_t _textureCoordinateOffset,
            _tangentOffset;
        std::size_t _attributeCount;

        Containers::Array<UnsignedInt> _indexData;
        Containers::Array<char> _vertexData;

        void append(const Vector3& position, const Vector3& normal);
        Vector3 lastVertexPosition(std::size_t offsetFromEnd);
        Vector3 lastVertexNormal(std::size_t offsetFromEnd);
        Vector4& lastVertexTangent(std::size_t offsetFromEnd);
        Vector2& lastVertexTextureCoords(std::size_t offsetFromEnd);
};

CORRADE_ENUMSET_OPERATORS(Spheroid::Flags)

}}}

#endif
