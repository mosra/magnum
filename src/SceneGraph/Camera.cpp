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

#include "Camera2D.hpp"
#include "Camera3D.hpp"

namespace Magnum { namespace SceneGraph {

#ifndef DOXYGEN_GENERATING_OUTPUT
template class MAGNUM_SCENEGRAPH_EXPORT AbstractCamera<2, GLfloat>;
template class MAGNUM_SCENEGRAPH_EXPORT AbstractCamera<3, GLfloat>;
template class MAGNUM_SCENEGRAPH_EXPORT Camera2D<GLfloat>;
template class MAGNUM_SCENEGRAPH_EXPORT Camera3D<GLfloat>;
#endif

}}
