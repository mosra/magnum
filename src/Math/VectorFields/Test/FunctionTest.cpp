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

#include "FunctionTest.h"

#include "Math/VectorFields/Function.h"

CORRADE_TEST_MAIN(Magnum::Math::VectorFields::Test::FunctionTest)

namespace Magnum { namespace Math { namespace VectorFields { namespace Test {

FunctionTest::FunctionTest() {
    addTests(&FunctionTest::extract);
}

void FunctionTest::extract() {
    VariableValue<'x', float> x(1.0f);
    VariableValue<'y', float> y(-1.0f);
    VariableValue<'z', float> z(10.0f);

    CORRADE_COMPARE((Implementation::extract<'y', float>(x, y, z)), -1.0f);
}

}}}}
