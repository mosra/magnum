#ifndef Magnum_DebugTools_Implementation_AbstractShapeRenderer_h
#define Magnum_DebugTools_Implementation_AbstractShapeRenderer_h
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
#include "Resource.h"
#include "DebugTools/DebugTools.h"
#include "SceneGraph/SceneGraph.h"
#include "Shaders/Shaders.h"
#include "Trade/Trade.h"

namespace Magnum { namespace DebugTools { namespace Implementation {

template<std::uint8_t dimensions> struct MeshData;

template<> struct MeshData<2> { typedef Trade::MeshData2D Type; };
template<> struct MeshData<3> { typedef Trade::MeshData3D Type; };

template<std::uint8_t dimensions> class AbstractShapeRenderer {
    public:
        AbstractShapeRenderer(ResourceKey mesh, ResourceKey vertexBuffer, ResourceKey indexBuffer);
        virtual ~AbstractShapeRenderer();

        virtual void draw(Resource<ShapeRendererOptions>& options, const typename DimensionTraits<dimensions>::MatrixType& transformationMatrix, SceneGraph::AbstractCamera<dimensions>* camera) = 0;

    protected:
        /* Call only if the mesh resource isn't already present */
        void createResources(typename MeshData<dimensions>::Type data);

        Resource<AbstractShaderProgram, Shaders::FlatShader<dimensions>> shader;
        Resource<Mesh> mesh;

    private:
        Resource<Buffer> indexBuffer, vertexBuffer;
};

}}}

#endif
