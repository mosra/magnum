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

#include <sstream>
#include <TestSuite/Tester.h>

#include "AbstractImage.h"

namespace Magnum { namespace Test {

class AbstractImageTest: public Corrade::TestSuite::Tester {
    public:
        explicit AbstractImageTest();

        void debugFormat();
        void debugType();
};

AbstractImageTest::AbstractImageTest() {
    addTests(&AbstractImageTest::debugFormat,
             &AbstractImageTest::debugType);
}

void AbstractImageTest::debugFormat() {
    std::ostringstream o;
    Debug(&o) << AbstractImage::Format::RGBA;
    CORRADE_COMPARE(o.str(), "AbstractImage::Format::RGBA\n");
}

void AbstractImageTest::debugType() {
    std::ostringstream o;
    Debug(&o) << AbstractImage::Type::UnsignedShort5551;
    CORRADE_COMPARE(o.str(), "AbstractImage::Type::UnsignedShort5551\n");
}

}}

CORRADE_TEST_MAIN(Magnum::Test::AbstractImageTest)
