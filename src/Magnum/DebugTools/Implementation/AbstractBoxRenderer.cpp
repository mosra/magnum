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

#include "AbstractBoxRenderer.h"

#include "Magnum/Mesh.h"
#include "Magnum/Primitives/Cube.h"
#include "Magnum/Primitives/Square.h"
#include "Magnum/Trade/MeshData2D.h"
#include "Magnum/Trade/MeshData3D.h"

namespace Magnum { namespace DebugTools { namespace Implementation {

AbstractBoxRenderer<2>::AbstractBoxRenderer(): AbstractShapeRenderer<2>("box2d", "box2d-vertices", {}) {
    if(!wireframeMesh) AbstractShapeRenderer<2>::createResources(Primitives::Square::wireframe());
}

AbstractBoxRenderer<3>::AbstractBoxRenderer(): AbstractShapeRenderer<3>("box3d", "box3d-vertices", "box3d-indices") {
    if(!wireframeMesh) AbstractShapeRenderer<3>::createResources(Primitives::Cube::wireframe());
}

template class AbstractBoxRenderer<2>;
template class AbstractBoxRenderer<3>;

}}}
