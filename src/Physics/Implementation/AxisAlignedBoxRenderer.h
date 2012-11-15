#ifndef Magnum_Physics_Implementation_AxisAlignedBoxRenderer_h
#define Magnum_Physics_Implementation_AxisAlignedBoxRenderer_h
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

#include "Physics/AxisAlignedBox.h"

#include "magnumCompatibility.h"

namespace Magnum { namespace Physics { namespace Implementation {

template<std::uint8_t dimensions> class AxisAlignedBoxRenderer: public AbstractBoxRenderer<dimensions> {
    public:
        inline AxisAlignedBoxRenderer(AxisAlignedBox<dimensions>& axisAlignedBox): axisAlignedBox(axisAlignedBox) {}

        void draw(Resource<Options>& options, const typename DimensionTraits<dimensions>::MatrixType& transformation, typename SceneGraph::AbstractCamera<dimensions>* camera) override;

    private:
        AxisAlignedBox<dimensions>& axisAlignedBox;
};

}}}

#endif
