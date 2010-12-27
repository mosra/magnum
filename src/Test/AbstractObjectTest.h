#ifndef Magnum_Test_AbstractObjectTest_h
#define Magnum_Test_AbstractObjectTest_h
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

#include <QtCore/QObject>

#include "AbstractObject.h"

namespace Magnum { namespace Test {

class Object: public AbstractObject {
    public:
        inline Object(AbstractObject* parent = 0): AbstractObject(parent) {}
        inline virtual void draw(const Magnum::Matrix4& transformationMatrix, const Magnum::Matrix4& projectionMatrix) {}

        inline const std::set<AbstractObject*>& children() { return AbstractObject::children(); }
};

class AbstractObjectTest: public QObject {
    Q_OBJECT

    private slots:
        void parenting();
};

}}

#endif
