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

#include "MeshBuilderBenchmark.h"

#include <QtTest/QTest>

#include "MeshBuilder.h"
#include "Magnum.h"
#include "Primitives/Icosphere.h"

QTEST_APPLESS_MAIN(Magnum::Test::MeshBuilderBenchmark)

namespace Magnum { namespace Test {

void MeshBuilderBenchmark::subdivide() {
    QBENCHMARK {
        MeshBuilder<Vector4> builder;
        builder.setData(Primitives::Icosahedron::vertices, Primitives::Icosahedron::indices, 12, 60);

        /* Subdivide 5 times */
        builder.subdivide(interpolator);
        builder.subdivide(interpolator);
        builder.subdivide(interpolator);
        builder.subdivide(interpolator);
        builder.subdivide(interpolator);
    }
}

void MeshBuilderBenchmark::subdivideAndCleanMeshAfter() {
    QBENCHMARK {
        MeshBuilder<Vector4> builder;
        builder.setData(Primitives::Icosahedron::vertices, Primitives::Icosahedron::indices, 12, 60);

        /* Subdivide 5 times */
        builder.subdivide(interpolator);
        builder.subdivide(interpolator);
        builder.subdivide(interpolator);
        builder.subdivide(interpolator);
        builder.subdivide(interpolator);

        builder.cleanMesh();
    }
}

void MeshBuilderBenchmark::subdivideAndCleanMeshBetween() {
    QBENCHMARK {
        MeshBuilder<Vector4> builder;
        builder.setData(Primitives::Icosahedron::vertices, Primitives::Icosahedron::indices, 12, 60);

        /* Subdivide 5 times */
        builder.subdivide(interpolator);
        builder.cleanMesh();
        builder.subdivide(interpolator);
        builder.cleanMesh();
        builder.subdivide(interpolator);
        builder.cleanMesh();
        builder.subdivide(interpolator);
        builder.cleanMesh();
        builder.subdivide(interpolator);
        builder.cleanMesh();
    }
}

}}
