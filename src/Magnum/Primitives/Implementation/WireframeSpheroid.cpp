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

#include "WireframeSpheroid.h"

#include "Magnum/Math/Functions.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Mesh.h"
#include "Magnum/Trade/ArrayAllocator.h"
#include "Magnum/Trade/MeshData.h"

namespace Magnum { namespace Primitives { namespace Implementation {

WireframeSpheroid::WireframeSpheroid(const UnsignedInt segments): _segments(segments) {}

void WireframeSpheroid::bottomHemisphere(const Float endY, const UnsignedInt rings) {
    CORRADE_INTERNAL_ASSERT(_vertexData.empty());

    /* Initial vertex */
    Containers::arrayAppend<Trade::ArrayAllocator>(_vertexData,
        Vector3::yAxis(endY - 1.0f));

    /* Connect initial vertex to first ring */
    for(UnsignedInt i = 0; i != 4; ++i)
        Containers::arrayAppend<Trade::ArrayAllocator>(_indexData, {0u, i+1});

    /* Hemisphere vertices and indices */
    const Rad ringAngleIncrement(Constants::piHalf()/rings);
    for(UnsignedInt j = 0; j != rings-1; ++j) {
        const Rad angle = Float(j+1)*ringAngleIncrement;
        const std::pair<Float, Float> sincos = Math::sincos(angle);

        Containers::arrayAppend<Trade::ArrayAllocator>(_vertexData, {
            {0.0f, endY - sincos.second, sincos.first},
            {sincos.first, endY - sincos.second, 0.0f},
            {0.0f, endY - sincos.second, -sincos.first},
            {-sincos.first, endY - sincos.second, 0.0f}
        });

        /* Connect vertices to next ring */
        for(UnsignedInt i = 0; i != 4; ++i) {
            Containers::arrayAppend<Trade::ArrayAllocator>(_indexData,
                {UnsignedInt(_vertexData.size()) - 4 + i, UnsignedInt(_vertexData.size()) + i});
        }
    }
}

void WireframeSpheroid::topHemisphere(const Float startY, const UnsignedInt rings) {
    /* Connect previous ring to following vertices (if any) */
    if(rings > 1) for(UnsignedInt i = 0; i != 4; ++i) {
        Containers::arrayAppend<Trade::ArrayAllocator>(_indexData,
            {UnsignedInt(_vertexData.size()) - 4*_segments + i, UnsignedInt(_vertexData.size()) + i});
    }

    /* Hemisphere vertices and indices */
    const Rad ringAngleIncrement(Constants::piHalf()/rings);
    for(UnsignedInt j = 0; j != rings-1; ++j) {
        const Rad angle = Float(j+1)*ringAngleIncrement;
        const std::pair<Float, Float> sincos = Math::sincos(angle);

        /* Connect previous hemisphere ring to current vertices */
        if(j != 0) for(UnsignedInt i = 0; i != 4; ++i) {
            Containers::arrayAppend<Trade::ArrayAllocator>(_indexData,
                {UnsignedInt(_vertexData.size()) - 4 + i, UnsignedInt(_vertexData.size()) + i});
        }

        Containers::arrayAppend<Trade::ArrayAllocator>(_vertexData, {
            {0.0f, startY + sincos.first, sincos.second},
            {sincos.second, startY + sincos.first, 0.0f},
            {0.0f, startY + sincos.first, -sincos.second},
            {-sincos.second, startY + sincos.first, 0.0f}
        });
    }

    /* Final vertex */
    Containers::arrayAppend<Trade::ArrayAllocator>(_vertexData,
        Vector3::yAxis(startY + 1.0f));

    /* Connect last ring to final vertex */
    if(rings > 1) for(UnsignedInt i = 0; i != 4; ++i)
        Containers::arrayAppend<Trade::ArrayAllocator>(_indexData,
            {UnsignedInt(_vertexData.size()) - 5 + i, UnsignedInt(_vertexData.size()) - 1});
    else for(UnsignedInt i = 0; i != 4; ++i)
        Containers::arrayAppend<Trade::ArrayAllocator>(_indexData,
            {UnsignedInt(_vertexData.size()) - 4*_segments + i - 1, UnsignedInt(_vertexData.size()) - 1});
}

void WireframeSpheroid::ring(const Float y) {
    /* Ring vertices and indices */
    const Rad segmentAngleIncrement(Constants::piHalf()/_segments);
    for(UnsignedInt j = 0; j != _segments; ++j) {
        for(UnsignedInt i = 0; i != 4; ++i) {
            const Rad segmentAngle = Rad(Float(i)*Constants::piHalf()) + Float(j)*segmentAngleIncrement;
            const std::pair<Float, Float> sincos = Math::sincos(segmentAngle);
            if(j != 0) Containers::arrayAppend<Trade::ArrayAllocator>(_indexData,
                {UnsignedInt(_vertexData.size() - 4), UnsignedInt(_vertexData.size())});
            Containers::arrayAppend<Trade::ArrayAllocator>(_vertexData,
                {sincos.first, y, sincos.second});
        }
    }

    /* Close the ring */
    for(UnsignedInt i = 0; i != 4; ++i)
        Containers::arrayAppend<Trade::ArrayAllocator>(_indexData,
            {UnsignedInt(_vertexData.size()) - 4 + i, UnsignedInt(_vertexData.size()) - 4*_segments + (i + 1)%4});
}

void WireframeSpheroid::cylinder() {
    /* Connect four vertex pairs of previous and next ring */
    for(UnsignedInt i = 0; i != 4; ++i)
        Containers::arrayAppend<Trade::ArrayAllocator>(_indexData,
            {UnsignedInt(_vertexData.size()) - 4*_segments + i, UnsignedInt(_vertexData.size()) + i});
}

namespace {

constexpr Trade::MeshAttributeData AttributeData[]{
    Trade::MeshAttributeData{Trade::MeshAttribute::Position, VertexFormat::Vector3,
        0, 0, sizeof(Vector3)}
};

}

Trade::MeshData WireframeSpheroid::finalize() {
    Trade::MeshIndexData indices{_indexData};
    Trade::MeshAttributeData positions{Trade::MeshAttribute::Position, Containers::arrayView(_vertexData)};
    const UnsignedInt vertexCount = _vertexData.size();
    return Trade::MeshData{MeshPrimitive::Lines,
        Containers::arrayAllocatorCast<char, Trade::ArrayAllocator>(std::move(_indexData)), indices,
        Containers::arrayAllocatorCast<char, Trade::ArrayAllocator>(std::move(_vertexData)),
        Trade::meshAttributeDataNonOwningArray(AttributeData), vertexCount};
}

}}}
