#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
#             Vladimír Vondruš <mosra@centrum.cz>
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

corrade_add_test(ShapesShapeImplementationTest ShapeImplementationTest.cpp LIBRARIES MagnumShapes)
corrade_add_test(ShapesAxisAlignedBoxTest AxisAlignedBoxTest.cpp LIBRARIES MagnumShapes)
corrade_add_test(ShapesBoxTest BoxTest.cpp LIBRARIES MagnumShapes)
corrade_add_test(ShapesCapsuleTest CapsuleTest.cpp LIBRARIES MagnumShapes)
corrade_add_test(ShapesCollisionTest CollisionTest.cpp LIBRARIES MagnumShapes)
corrade_add_test(ShapesCylinderTest CylinderTest.cpp LIBRARIES MagnumShapes)
corrade_add_test(ShapesLineTest LineTest.cpp LIBRARIES MagnumShapes)
corrade_add_test(ShapesPlaneTest PlaneTest.cpp LIBRARIES MagnumShapes)
corrade_add_test(ShapesPointTest PointTest.cpp LIBRARIES MagnumShapes)
corrade_add_test(ShapesCompositionTest CompositionTest.cpp LIBRARIES MagnumShapes)
corrade_add_test(ShapesSphereTest SphereTest.cpp LIBRARIES MagnumShapes)

corrade_add_test(ShapesShapeTest ShapeTest.cpp LIBRARIES MagnumShapes)

set_target_properties(
    ShapesShapeImplementationTest
    ShapesAxisAlignedBoxTest
    ShapesBoxTest
    ShapesCapsuleTest
    ShapesCollisionTest
    ShapesCylinderTest
    ShapesLineTest
    ShapesPlaneTest
    ShapesPointTest
    ShapesCompositionTest
    ShapesSphereTest
    ShapesShapeTest
    PROPERTIES FOLDER "Magnum/Shapes/Test")
