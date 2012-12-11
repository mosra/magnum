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

#include "TypeTraitsTest.h"

#include <sstream>
#include <Utility/Configuration.h>

#include "TypeTraits.h"

CORRADE_TEST_MAIN(Magnum::Test::TypeTraitsTest)

using namespace Corrade::Utility;

namespace Magnum { namespace Test {

TypeTraitsTest::TypeTraitsTest() {
    addTests(&TypeTraitsTest::debug,
             &TypeTraitsTest::configuration);
}

void TypeTraitsTest::debug() {
    std::ostringstream o;
    Debug(&o) << Type::UnsignedShort;
    CORRADE_COMPARE(o.str(), "Type::UnsignedShort\n");
}

void TypeTraitsTest::configuration() {
    Configuration c;

    c.setValue("type", Type::Byte);
    CORRADE_COMPARE(c.value<std::string>("type"), "Byte");
    CORRADE_COMPARE(c.value<Type>("type"), Type::Byte);
}

}}
