#ifndef Magnum_Physics_Implementation_AbstractDebugRenderer_h
#define Magnum_Physics_Implementation_AbstractDebugRenderer_h
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

#include "Color.h"
#include "ResourceManager.h"
#include "SceneGraph/Camera.h"

namespace Magnum {

class AbstractShaderProgram;
class Mesh;

namespace Shaders {
    template<std::uint8_t> class FlatShader;
}

namespace Physics { namespace Implementation {

struct Options;

template<std::uint8_t dimensions> class AbstractDebugRenderer: public SceneGraph::AbstractObject<dimensions>::ObjectType {
    public:
        AbstractDebugRenderer(ResourceKey shader, ResourceKey mesh, ResourceKey options, typename SceneGraph::AbstractObject<dimensions>::ObjectType* parent);

    protected:
        Resource<AbstractShaderProgram, Shaders::FlatShader<dimensions>> shader;
        Resource<Mesh> mesh;
        Resource<Options> options;
};

extern template class AbstractDebugRenderer<2>;
extern template class AbstractDebugRenderer<3>;

}}}

#endif
