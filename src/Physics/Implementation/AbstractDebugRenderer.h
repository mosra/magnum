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

#include "DimensionTraits.h"
#include "ResourceManager.h"
#include "SceneGraph/SceneGraph.h"

namespace Magnum {

class AbstractShaderProgram;
class Mesh;

namespace Shaders {
    template<std::uint8_t> class FlatShader;
}

namespace Physics { namespace Implementation {

struct Options;

template<std::uint8_t dimensions> class AbstractDebugRenderer {
    public:
        AbstractDebugRenderer(ResourceKey shader, ResourceKey mesh);

        virtual ~AbstractDebugRenderer();

        virtual void draw(Resource<Options>& options, const typename DimensionTraits<dimensions>::MatrixType& transformationMatrix, SceneGraph::AbstractCamera<dimensions, GLfloat>* camera) = 0;

    protected:
        Resource<AbstractShaderProgram, Shaders::FlatShader<dimensions>> shader;
        Resource<Mesh> mesh;
};

}}}

#endif
