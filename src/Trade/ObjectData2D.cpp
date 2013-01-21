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

#include "ObjectData2D.h"

namespace Magnum { namespace Trade {

#ifndef DOXYGEN_GENERATING_OUTPUT
Debug operator<<(Debug debug, ObjectData2D::InstanceType value) {
    switch(value) {
        #define _c(value) case ObjectData2D::InstanceType::value: return debug << "Trade::ObjectData2D::InstanceType::" #value;
        _c(Camera)
        _c(Mesh)
        _c(Empty)
        #undef _c
    }

    return debug << "ObjectData2D::InstanceType::(invalid)";
}
#endif

}}
