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

#include "Buffer.h"
#include "Physics/DebugDrawResourceManager.h"
#include "Primitives/Cube.h"
#include "Primitives/Square.h"
#include "Shaders/FlatShader.h"

namespace Magnum { namespace Physics { namespace Implementation {

namespace {
    template<std::uint8_t> struct BoxMesh {};

    template<> struct BoxMesh<2> {
        static ResourceKey shader() { return {"shader2d"}; }
        static ResourceKey key() { return {"box2d"}; }

        static Mesh* mesh(Buffer* buffer) {
            Primitives::Square square;
            Mesh* mesh = new Mesh;
            buffer->setData(*square.positions(0), Buffer::Usage::StaticDraw);
            return mesh->setPrimitive(square.primitive())
                ->setVertexCount(square.positions(0)->size())
                ->addVertexBuffer(buffer, Shaders::FlatShader<2>::Position());
        }
    };

    template<> struct BoxMesh<3> {
        static ResourceKey shader() { return {"shader3d"}; }
        static ResourceKey key() { return {"box3d"}; }

        static Mesh* mesh(Buffer* buffer) {
            Primitives::Cube cube;
            Mesh* mesh = new Mesh;
            buffer->setData(*cube.positions(0), Buffer::Usage::StaticDraw);
            return mesh->setPrimitive(cube.primitive())
                ->setVertexCount(cube.positions(0)->size())
                ->addVertexBuffer(buffer, Shaders::FlatShader<2>::Position());
        }
    };
}

template<std::uint8_t dimensions> AbstractBoxRenderer<dimensions>::AbstractBoxRenderer(): AbstractDebugRenderer<dimensions>(BoxMesh<dimensions>::shader(), BoxMesh<dimensions>::key()), buffer(DebugDrawResourceManager::instance()->get<Buffer>(BoxMesh<dimensions>::key())) {
    if(!this->mesh) {
        DebugDrawResourceManager::instance()->set(this->buffer.key(), new Buffer, ResourceDataState::Final, ResourcePolicy::Manual);
        DebugDrawResourceManager::instance()->set<Mesh>(this->mesh.key(), BoxMesh<dimensions>::mesh(buffer), ResourceDataState::Final, ResourcePolicy::Manual);
    }
}

template<std::uint8_t dimensions> AbstractBoxRenderer<dimensions>::~AbstractBoxRenderer() {}

template class AbstractBoxRenderer<2>;
template class AbstractBoxRenderer<3>;

}}}
