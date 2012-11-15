#ifndef Magnum_Physics_Implementation_DebugRenderer_h
#define Magnum_Physics_Implementation_DebugRenderer_h
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

#include "ResourceManager.h"
#include "SceneGraph/Drawable.h"
#include "AbstractDebugRenderer.h"

namespace Magnum {

class AbstractShaderProgram;
class Mesh;

namespace Shaders {
    template<std::uint8_t> class FlatShader;
}

namespace Physics { namespace Implementation {

struct Options;

template<std::uint8_t dimensions> class DebugRenderer: public SceneGraph::Drawable<dimensions> {
    public:
        DebugRenderer(SceneGraph::AbstractObject<dimensions>* object, Resource<Options>&& options): SceneGraph::Drawable<dimensions>(object), options(options) {}

        inline ~DebugRenderer() {
            for(auto it = renderers.begin(); it != renderers.end(); ++it)
                delete *it;
        }

        inline void addRenderer(AbstractDebugRenderer<dimensions>* renderer) {
            renderers.push_back(renderer);
        }

        inline void draw(const typename DimensionTraits<dimensions>::MatrixType& transformationMatrix, SceneGraph::AbstractCamera<dimensions, GLfloat>* camera) override {
            for(auto it = renderers.begin(); it != renderers.end(); ++it)
                (*it)->draw(options, transformationMatrix, camera);
        }

    private:
        Resource<Options> options;
        std::vector<AbstractDebugRenderer<dimensions>*> renderers;
};

}}}

#endif
