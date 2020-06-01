#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
#               2020 Vladimír Vondruš <mosra@centrum.cz>
#
#   Permission is hereby granted, free of charge, to any person obtaining a
#   copy of this software and associated documentation files (the "Software"),
#   to deal in the Software without restriction, including without limitation
#   the rights to use, copy, modify, merge, publish, distribute, sublicense,
#   and/or sell copies of the Software, and to permit persons to whom the
#   Software is furnished to do so, subject to the following conditions:
#
#   The above copyright notice and this permission notice shall be included
#   in all copies or substantial portions of the Software.
#
#   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
#   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
#   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
#   DEALINGS IN THE SOFTWARE.
#

corrade_add_test(PrimitivesAxisTest AxisTest.cpp LIBRARIES MagnumPrimitives)
corrade_add_test(PrimitivesCapsuleTest CapsuleTest.cpp LIBRARIES MagnumPrimitives)
corrade_add_test(PrimitivesCircleTest CircleTest.cpp LIBRARIES MagnumPrimitives)
corrade_add_test(PrimitivesCrosshairTest CrosshairTest.cpp LIBRARIES MagnumPrimitives)
corrade_add_test(PrimitivesCubeTest CubeTest.cpp LIBRARIES MagnumPrimitives)
corrade_add_test(PrimitivesConeTest ConeTest.cpp LIBRARIES MagnumPrimitives)
corrade_add_test(PrimitivesCylinderTest CylinderTest.cpp LIBRARIES MagnumPrimitives)
corrade_add_test(PrimitivesGradientTest GradientTest.cpp LIBRARIES MagnumPrimitives)
corrade_add_test(PrimitivesGridTest GridTest.cpp LIBRARIES MagnumPrimitives)
corrade_add_test(PrimitivesIcosphereTest IcosphereTest.cpp LIBRARIES MagnumPrimitives)
corrade_add_test(PrimitivesLineTest LineTest.cpp LIBRARIES MagnumPrimitives)
corrade_add_test(PrimitivesPlaneTest PlaneTest.cpp LIBRARIES MagnumPrimitives)
corrade_add_test(PrimitivesSquareTest SquareTest.cpp LIBRARIES MagnumPrimitives)
corrade_add_test(PrimitivesUVSphereTest UVSphereTest.cpp LIBRARIES MagnumPrimitives)

set_target_properties(
    PrimitivesAxisTest
    PrimitivesCapsuleTest
    PrimitivesCircleTest
    PrimitivesCrosshairTest
    PrimitivesCubeTest
    PrimitivesConeTest
    PrimitivesCylinderTest
    PrimitivesGradientTest
    PrimitivesGridTest
    PrimitivesIcosphereTest
    PrimitivesLineTest
    PrimitivesPlaneTest
    PrimitivesSquareTest
    PrimitivesUVSphereTest
    PROPERTIES FOLDER "Magnum/Primitives/Test")
