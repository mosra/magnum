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

#include "FlipNormals.h"

#include <vector>
#include <Corrade/Containers/StridedArrayView.h>

#include "Magnum/Math/Vector3.h"

namespace Magnum { namespace MeshTools {

namespace {

template<class T> inline void flipFaceWindingInPlaceImplementation(const Containers::StridedArrayView1D<T>& indices) {
    CORRADE_ASSERT(!(indices.size()%3), "MeshTools::flipNormals(): index count is not divisible by 3!", );

    using std::swap;
    for(std::size_t i = 0; i != indices.size(); i += 3)
        swap(indices[i+1], indices[i+2]);
}

}

void flipFaceWindingInPlace(const Containers::StridedArrayView1D<UnsignedInt>& indices) {
    flipFaceWindingInPlaceImplementation(indices);
}

void flipFaceWindingInPlace(const Containers::StridedArrayView1D<UnsignedShort>& indices) {
    flipFaceWindingInPlaceImplementation(indices);
}

void flipFaceWindingInPlace(const Containers::StridedArrayView1D<UnsignedByte>& indices) {
    flipFaceWindingInPlaceImplementation(indices);
}

void flipFaceWindingInPlace(const Containers::StridedArrayView2D<char>& indices) {
    CORRADE_ASSERT(indices.isContiguous<1>(), "MeshTools::flipFaceWindingInPlace(): second index view dimension is not contiguous", );
    if(indices.size()[1] == 4)
        return flipFaceWindingInPlaceImplementation(Containers::arrayCast<1, UnsignedInt>(indices));
    else if(indices.size()[1] == 2)
        return flipFaceWindingInPlaceImplementation(Containers::arrayCast<1, UnsignedShort>(indices));
    else {
        CORRADE_ASSERT(indices.size()[1] == 1, "MeshTools::flipFaceWindingInPlace(): expected index type size 1, 2 or 4 but got" << indices.size()[1], );
        return flipFaceWindingInPlaceImplementation(Containers::arrayCast<1, UnsignedByte>(indices));
    }
}

void flipNormalsInPlace(const Containers::StridedArrayView1D<Vector3>& normals) {
    for(Vector3& normal: normals)
        normal = -normal;
}

#ifdef MAGNUM_BUILD_DEPRECATED
void flipNormals(std::vector<UnsignedInt>& indices, std::vector<Vector3>& normals) {
    flipNormalsInPlace(indices, normals);
}

void flipFaceWinding(std::vector<UnsignedInt>& indices) {
    flipFaceWindingInPlace(indices);
}

void flipNormals(std::vector<Vector3>& normals) {
    flipNormalsInPlace(normals);
}
#endif

}}
