/*
    Copyright © 2010, 2011, 2012 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

#include "AbstractBoxRenderer.h"

#include "Primitives/Cube.h"
#include "Primitives/Square.h"
#include "Trade/MeshData2D.h"
#include "Trade/MeshData3D.h"

namespace Magnum { namespace DebugTools { namespace Implementation {

AbstractBoxRenderer<2>::AbstractBoxRenderer(): AbstractShapeRenderer<2>("box2d", "box2d-vertices", {}) {
    if(!mesh) this->createResources(Primitives::Square::wireframe());
}

AbstractBoxRenderer<3>::AbstractBoxRenderer(): AbstractShapeRenderer<3>("box3d", "box3d-vertices", "box3d-indices") {
    if(!mesh) this->createResources(Primitives::Cube::wireframe());
}

template class AbstractBoxRenderer<2>;
template class AbstractBoxRenderer<3>;

}}}
