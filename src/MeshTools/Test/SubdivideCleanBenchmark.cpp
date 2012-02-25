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

#include "SubdivideCleanBenchmark.h"

#include <QtTest/QTest>

#include "Primitives/Icosphere.h"
#include "MeshTools/Clean.h"
#include "MeshTools/Subdivide.h"

QTEST_APPLESS_MAIN(Magnum::MeshTools::Test::SubdivideCleanBenchmark)

namespace Magnum { namespace MeshTools { namespace Test {

void SubdivideCleanBenchmark::subdivide() {
    QBENCHMARK {
        Primitives::Icosphere<0> icosphere;

        /* Subdivide 5 times */
        MeshTools::subdivide(*icosphere.indices(), *icosphere.vertices(0), interpolator);
        MeshTools::subdivide(*icosphere.indices(), *icosphere.vertices(0), interpolator);
        MeshTools::subdivide(*icosphere.indices(), *icosphere.vertices(0), interpolator);
        MeshTools::subdivide(*icosphere.indices(), *icosphere.vertices(0), interpolator);
        MeshTools::subdivide(*icosphere.indices(), *icosphere.vertices(0), interpolator);
    }
}

void SubdivideCleanBenchmark::subdivideAndCleanMeshAfter() {
    QBENCHMARK {
        Primitives::Icosphere<0> icosphere;

        /* Subdivide 5 times */
        MeshTools::subdivide(*icosphere.indices(), *icosphere.vertices(0), interpolator);
        MeshTools::subdivide(*icosphere.indices(), *icosphere.vertices(0), interpolator);
        MeshTools::subdivide(*icosphere.indices(), *icosphere.vertices(0), interpolator);
        MeshTools::subdivide(*icosphere.indices(), *icosphere.vertices(0), interpolator);
        MeshTools::subdivide(*icosphere.indices(), *icosphere.vertices(0), interpolator);

        MeshTools::clean(*icosphere.indices(), *icosphere.vertices(0));
    }
}

void SubdivideCleanBenchmark::subdivideAndCleanMeshBetween() {
    QBENCHMARK {
        Primitives::Icosphere<0> icosphere;

        /* Subdivide 5 times */
        MeshTools::subdivide(*icosphere.indices(), *icosphere.vertices(0), interpolator);
        MeshTools::clean(*icosphere.indices(), *icosphere.vertices(0));
        MeshTools::subdivide(*icosphere.indices(), *icosphere.vertices(0), interpolator);
        MeshTools::clean(*icosphere.indices(), *icosphere.vertices(0));
        MeshTools::subdivide(*icosphere.indices(), *icosphere.vertices(0), interpolator);
        MeshTools::clean(*icosphere.indices(), *icosphere.vertices(0));
        MeshTools::subdivide(*icosphere.indices(), *icosphere.vertices(0), interpolator);
        MeshTools::clean(*icosphere.indices(), *icosphere.vertices(0));
        MeshTools::subdivide(*icosphere.indices(), *icosphere.vertices(0), interpolator);
        MeshTools::clean(*icosphere.indices(), *icosphere.vertices(0));
    }
}

}}}
