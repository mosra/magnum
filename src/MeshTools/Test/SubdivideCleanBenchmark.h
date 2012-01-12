#ifndef Magnum_MeshTools_Test_SubdivideCleanBenchmark_h
#define Magnum_MeshTools_Test_SubdivideCleanBenchmark_h
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

#include <QtCore/QObject>

#include "Magnum.h"

namespace Magnum { namespace MeshTools { namespace Test {

class SubdivideCleanBenchmark: public QObject {
    Q_OBJECT

    private slots:
        void subdivide();
        void subdivideAndCleanMeshAfter();
        void subdivideAndCleanMeshBetween();

    private:
        static inline Magnum::Vector4 interpolator(const Magnum::Vector4& a, const Magnum::Vector4& b) {
            return (a+b).xyz().normalized();
        }
};

}}}

#endif
