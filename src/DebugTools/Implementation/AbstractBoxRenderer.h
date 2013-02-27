#ifndef Magnum_DebugTools_Implementation_AbstractBoxRenderer_h
#define Magnum_DebugTools_Implementation_AbstractBoxRenderer_h
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

#include "AbstractShapeRenderer.h"

#include "Resource.h"
#include "Shaders/Shaders.h"

#include "corradeCompatibility.h"

namespace Magnum { namespace DebugTools { namespace Implementation {

template<UnsignedInt dimensions> class AbstractBoxRenderer;

template<> class AbstractBoxRenderer<2>: public AbstractShapeRenderer<2> {
    public:
        AbstractBoxRenderer();
};

template<> class AbstractBoxRenderer<3>: public AbstractShapeRenderer<3> {
    public:
        AbstractBoxRenderer();
};

}}}

#endif
