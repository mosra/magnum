#ifndef Magnum_Test_ResourceManagerTest_h
#define Magnum_Test_ResourceManagerTest_h
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

#include <TestSuite/Tester.h>

namespace Magnum {

template<class...> class ResourceManager;

namespace Test {

class Data {
    public:
        static std::size_t count;

        inline Data() { ++count; }
        inline ~Data() { --count; }
};

typedef Magnum::ResourceManager<int32_t, Data> ResourceManager;

class ResourceManagerTest: public Corrade::TestSuite::Tester<ResourceManagerTest> {
    public:
        ResourceManagerTest();

        void state();
        void referenceCountedPolicy();
        void manualPolicy();
        void destroy();

    private:
        ResourceManager* rm;
};

}}

#endif
