/*
    Copyright © 2010 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

#include "ObjectTest.h"

#include <QtTest/QTest>

QTEST_APPLESS_MAIN(Magnum::Test::ObjectTest)

namespace Magnum { namespace Test {

void ObjectTest::parenting() {
    Object root;

    Object* childOne = new Object(&root);
    Object* childTwo = new Object(&root);

    QVERIFY(childOne->parent() == &root);
    QVERIFY(root.children().size() == 2);

    /* Reparent to another */
    childTwo->setParent(childOne);
    QVERIFY(root.children().size() == 1 && *root.children().begin() == childOne);
    QVERIFY(childOne->children().size() == 1 && *childOne->children().begin() == childTwo);

    /* Delete child */
    delete childTwo;
    QVERIFY(childOne->children().size() == 0);
}

}}
