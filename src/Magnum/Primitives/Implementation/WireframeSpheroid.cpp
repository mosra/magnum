/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
              Vladimír Vondruš <mosra@centrum.cz>

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
#include "Magnum/Trade/MeshData3D.h"

namespace Magnum { namespace Primitives { namespace Implementation {

WireframeSpheroid::WireframeSpheroid(const UnsignedInt segments): _segments(segments) {}

void WireframeSpheroid::bottomHemisphere(const Float endY, const UnsignedInt rings) {
    CORRADE_INTERNAL_ASSERT(_positions.empty());

    /* Initial vertex */
    _positions.push_back(Vector3::yAxis(endY - 1.0f));

    /* Connect initial vertex to first ring */
    for(UnsignedInt i = 0; i != 4; ++i)
        _indices.insert(_indices.end(), {0, i+1});

    /* Hemisphere vertices and indices */
    const Rad ringAngleIncrement(Constants::piHalf()/rings);
    for(UnsignedInt j = 0; j != rings-1; ++j) {
        const Rad angle = Float(j+1)*ringAngleIncrement;

        _positions.emplace_back(0.0f, endY - Math::cos(angle), Math::sin(angle));
        _positions.emplace_back(Math::sin(angle), endY - Math::cos(angle), 0.0f);
        _positions.emplace_back(0.0f, endY - Math::cos(angle), -Math::sin(angle));
        _positions.emplace_back(-Math::sin(angle), endY - Math::cos(angle), 0.0f);

        /* Connect vertices to next ring */
        for(UnsignedInt i = 0; i != 4; ++i)
            _indices.insert(_indices.end(), {UnsignedInt(_positions.size())-4+i, UnsignedInt(_positions.size())+i});
    }
}

void WireframeSpheroid::topHemisphere(const Float startY, const UnsignedInt rings) {
    /* Connect previous ring to following vertices */
    for(UnsignedInt i = 0; i != 4; ++i)
        _indices.insert(_indices.end(), {UnsignedInt(_positions.size())-4*_segments+i, UnsignedInt(_positions.size())+i});

    /* Hemisphere vertices and indices */
    const Rad ringAngleIncrement(Constants::piHalf()/rings);
    for(UnsignedInt j = 0; j != rings-1; ++j) {
        const Rad angle = Float(j+1)*ringAngleIncrement;

        /* Connect previous hemisphere ring to current vertices */
        if(j != 0) for(UnsignedInt i = 0; i != 4; ++i)
            _indices.insert(_indices.end(), {UnsignedInt(_positions.size())-4+i, UnsignedInt(_positions.size())+i});

        _positions.emplace_back(0.0f, startY + Math::sin(angle), Math::cos(angle));
        _positions.emplace_back(Math::cos(angle), startY + Math::sin(angle), 0.0f);
        _positions.emplace_back(0.0f, startY + Math::sin(angle), -Math::cos(angle));
        _positions.emplace_back(-Math::cos(angle), startY + Math::sin(angle), 0.0f);
    }

    /* Final vertex */
    _positions.push_back(Vector3::yAxis(startY + 1.0f));

    /* Connect last ring to final vertex */
    for(UnsignedInt i = 0; i != 4; ++i)
        _indices.insert(_indices.end(), {UnsignedInt(_positions.size())-5+i, UnsignedInt(_positions.size())-1});
}

void WireframeSpheroid::ring(const Float y) {
    /* Ring vertices and indices */
    const Rad segmentAngleIncrement(Constants::piHalf()/_segments);
    for(UnsignedInt j = 0; j != _segments; ++j) {
        for(UnsignedInt i = 0; i != 4; ++i) {
            const Rad segmentAngle = Rad(Float(i)*Constants::piHalf()) + Float(j)*segmentAngleIncrement;
            if(j != 0) _indices.insert(_indices.end(), {UnsignedInt(_positions.size()-4), UnsignedInt(_positions.size())});
            _positions.emplace_back(Math::sin(segmentAngle), y, Math::cos(segmentAngle));
        }
    }

    /* Close the ring */
    for(UnsignedInt i = 0; i != 4; ++i)
        _indices.insert(_indices.end(), {UnsignedInt(_positions.size())-4+i, UnsignedInt(_positions.size())-4*_segments+(i+1)%4});
}

void WireframeSpheroid::cylinder() {
    /* Connect four vertex pairs of previous and next ring */
    for(UnsignedInt i = 0; i != 4; ++i)
        _indices.insert(_indices.end(), {UnsignedInt(_positions.size())-4*_segments+i, UnsignedInt(_positions.size())+i});
}

Trade::MeshData3D WireframeSpheroid::finalize() {
    return Trade::MeshData3D{MeshPrimitive::Lines, std::move(_indices), {std::move(_positions)}, {}, {}, {}, nullptr};
}

}}}
