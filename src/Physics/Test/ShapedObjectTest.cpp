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

#include "ShapedObjectTest.h"

#include "Physics/ShapedObjectGroup.h"
#include "Physics/ShapedObject.h"

CORRADE_TEST_MAIN(Magnum::Physics::Test::ShapedObjectTest)

namespace Magnum { namespace Physics { namespace Test {

ShapedObjectTest::ShapedObjectTest() {
    addTests(&ShapedObjectTest::clean);
}

void ShapedObjectTest::clean() {
    ShapedObjectGroup3D group;

    ShapedObject3D a(&group), b(&group);

    /* Everything is dirty at the beginning */
    CORRADE_VERIFY(group.isDirty());
    CORRADE_VERIFY(a.isDirty());
    CORRADE_VERIFY(b.isDirty());

    /* Cleaning object will not clean anything other */
    a.setClean();
    CORRADE_VERIFY(group.isDirty());
    CORRADE_VERIFY(!a.isDirty());
    CORRADE_VERIFY(b.isDirty());

    /* Setting group clean will clean whole group */
    a.setDirty();
    group.setClean();
    CORRADE_VERIFY(!group.isDirty());
    CORRADE_VERIFY(!a.isDirty());
    CORRADE_VERIFY(!b.isDirty());

    /* Setting object dirty will set also the group, but not other objects */
    b.setDirty();
    CORRADE_VERIFY(group.isDirty());
    CORRADE_VERIFY(!a.isDirty());
    CORRADE_VERIFY(b.isDirty());
}

}}}
