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

#include "ResourceManagerTest.h"

#include <sstream>

#include "AbstractResourceLoader.h"
#include "ResourceManager.h"

#include "corradeCompatibility.h"

using namespace Corrade::Utility;

CORRADE_TEST_MAIN(Magnum::Test::ResourceManagerTest)

namespace Magnum { namespace Test {

class Data {
    public:
        static std::size_t count;

        inline Data() { ++count; }
        inline ~Data() { --count; }
};

typedef Magnum::ResourceManager<std::int32_t, Data> ResourceManager;

class IntResourceLoader: public AbstractResourceLoader<std::int32_t> {
    public:
        void load(ResourceKey key) override {
            AbstractResourceLoader::load(key);
        }

        void load() {
            set("hello", new std::int32_t(773), ResourceDataState::Final, ResourcePolicy::Resident);
            setNotFound("world");
        }
};

size_t Data::count = 0;

ResourceManagerTest::ResourceManagerTest() {
    addTests(&ResourceManagerTest::state,
             &ResourceManagerTest::stateFallback,
             &ResourceManagerTest::stateDisallowed,
             &ResourceManagerTest::basic,
             &ResourceManagerTest::residentPolicy,
             &ResourceManagerTest::referenceCountedPolicy,
             &ResourceManagerTest::manualPolicy,
             &ResourceManagerTest::loader);
}

void ResourceManagerTest::state() {
    ResourceManager rm;

    Resource<Data> data = rm.get<Data>("data");
    CORRADE_VERIFY(!data);
    CORRADE_COMPARE(data.state(), ResourceState::NotLoaded);
    CORRADE_COMPARE(rm.state<Data>("data"), ResourceState::NotLoaded);

    rm.set<Data>("data", nullptr, ResourceDataState::Loading, ResourcePolicy::Resident);
    CORRADE_VERIFY(!data);
    CORRADE_COMPARE(data.state(), ResourceState::Loading);
    CORRADE_COMPARE(rm.state<Data>("data"), ResourceState::Loading);

    rm.set<Data>("data", nullptr, ResourceDataState::NotFound, ResourcePolicy::Resident);
    CORRADE_VERIFY(!data);
    CORRADE_COMPARE(data.state(), ResourceState::NotFound);
    CORRADE_COMPARE(rm.state<Data>("data"), ResourceState::NotFound);

    /* Nothing happened at all */
    CORRADE_COMPARE(Data::count, 0);
}

void ResourceManagerTest::stateFallback() {
    {
        ResourceManager rm;
        rm.setFallback<Data>(new Data);

        Resource<Data> data = rm.get<Data>("data");
        CORRADE_VERIFY(data);
        CORRADE_COMPARE(data.state(), ResourceState::NotLoadedFallback);
        CORRADE_COMPARE(rm.state<Data>("data"), ResourceState::NotLoadedFallback);

        rm.set<Data>("data", nullptr, ResourceDataState::Loading, ResourcePolicy::Resident);
        CORRADE_VERIFY(data);
        CORRADE_COMPARE(data.state(), ResourceState::LoadingFallback);
        CORRADE_COMPARE(rm.state<Data>("data"), ResourceState::LoadingFallback);

        rm.set<Data>("data", nullptr, ResourceDataState::NotFound, ResourcePolicy::Resident);
        CORRADE_VERIFY(data);
        CORRADE_COMPARE(data.state(), ResourceState::NotFoundFallback);
        CORRADE_COMPARE(rm.state<Data>("data"), ResourceState::NotFoundFallback);

        /* Only fallback is here */
        CORRADE_COMPARE(Data::count, 1);
    }

    /* Fallback gets destroyed */
    CORRADE_COMPARE(Data::count, 0);
}

void ResourceManagerTest::stateDisallowed() {
    ResourceManager rm;

    std::ostringstream out;
    Error::setOutput(&out);

    Data d;
    rm.set("data", &d, ResourceDataState::Loading, ResourcePolicy::Resident);
    CORRADE_COMPARE(out.str(), "ResourceManager::set(): data should be null if and only if state is NotFound or Loading\n");

    out.str("");
    rm.set<Data>("data", nullptr, ResourceDataState::Final, ResourcePolicy::Resident);
    CORRADE_COMPARE(out.str(), "ResourceManager::set(): data should be null if and only if state is NotFound or Loading\n");
}

void ResourceManagerTest::basic() {
    ResourceManager rm;

    /* One mutable, one final */
    ResourceKey questionKey("the-question");
    ResourceKey answerKey("the-answer");
    rm.set(questionKey, new std::int32_t(10), ResourceDataState::Mutable, ResourcePolicy::Resident);
    rm.set(answerKey, new std::int32_t(42), ResourceDataState::Final, ResourcePolicy::Resident);
    Resource<std::int32_t> theQuestion = rm.get<std::int32_t>(questionKey);
    Resource<std::int32_t> theAnswer = rm.get<std::int32_t>(answerKey);

    /* Check basic functionality */
    CORRADE_COMPARE(theQuestion.state(), ResourceState::Mutable);
    CORRADE_COMPARE(theAnswer.state(), ResourceState::Final);
    CORRADE_COMPARE(*theQuestion, 10);
    CORRADE_COMPARE(*theAnswer, 42);
    CORRADE_COMPARE(rm.count<std::int32_t>(), 2);

    /* Cannot change already final resource */
    std::ostringstream out;
    Error::setOutput(&out);
    rm.set(answerKey, new std::int32_t(43), ResourceDataState::Mutable, ResourcePolicy::Resident);
    CORRADE_COMPARE(*theAnswer, 42);
    CORRADE_COMPARE(out.str(), "ResourceManager::set(): cannot change already final resource\n");

    /* But non-final can be changed */
    rm.set(questionKey, new std::int32_t(20), ResourceDataState::Final, ResourcePolicy::Resident);
    CORRADE_COMPARE(theQuestion.state(), ResourceState::Final);
    CORRADE_COMPARE(*theQuestion, 20);
}

void ResourceManagerTest::residentPolicy() {
    ResourceManager* rm = new ResourceManager;

    rm->set("blah", new Data(), ResourceDataState::Mutable, ResourcePolicy::Resident);
    CORRADE_COMPARE(Data::count, 1);

    rm->free();
    CORRADE_COMPARE(Data::count, 1);

    delete rm;
    CORRADE_COMPARE(Data::count, 0);
}

void ResourceManagerTest::referenceCountedPolicy() {
    ResourceManager rm;

    ResourceKey dataRefCountKey("dataRefCount");

    /* Reference counted resources must be requested first */
    {
        rm.set(dataRefCountKey, new Data(), ResourceDataState::Final, ResourcePolicy::ReferenceCounted);
        CORRADE_COMPARE(rm.count<Data>(), 0);
        Resource<Data> data = rm.get<Data>(dataRefCountKey);
        CORRADE_COMPARE(data.state(), ResourceState::NotLoaded);
        CORRADE_COMPARE(Data::count, 0);
    } {
        Resource<Data> data = rm.get<Data>(dataRefCountKey);
        CORRADE_COMPARE(rm.count<Data>(), 1);
        CORRADE_COMPARE(data.state(), ResourceState::NotLoaded);
        rm.set(dataRefCountKey, new Data(), ResourceDataState::Final, ResourcePolicy::ReferenceCounted);
        CORRADE_COMPARE(data.state(), ResourceState::Final);
        CORRADE_COMPARE(Data::count, 1);
    }

    CORRADE_COMPARE(rm.count<Data>(), 0);
    CORRADE_COMPARE(Data::count, 0);
}

void ResourceManagerTest::manualPolicy() {
    ResourceManager rm;

    ResourceKey dataKey("data");

    /* Manual free */
    {
        rm.set(dataKey, new Data(), ResourceDataState::Mutable, ResourcePolicy::Manual);
        Resource<Data> data = rm.get<Data>(dataKey);
        rm.free();
    }

    CORRADE_COMPARE(rm.count<Data>(), 1);
    CORRADE_COMPARE(Data::count, 1);
    rm.free();
    CORRADE_COMPARE(rm.count<Data>(), 0);
    CORRADE_COMPARE(Data::count, 0);

    rm.set(dataKey, new Data(), ResourceDataState::Mutable, ResourcePolicy::Manual);
    CORRADE_COMPARE(rm.count<Data>(), 1);
    CORRADE_COMPARE(Data::count, 1);
}

void ResourceManagerTest::loader() {
    ResourceManager rm;
    IntResourceLoader loader;
    rm.setLoader(&loader);

    Resource<Data> data = rm.get<Data>("data");
    Resource<std::int32_t> hello = rm.get<std::int32_t>("hello");
    Resource<std::int32_t> world = rm.get<std::int32_t>("world");
    CORRADE_COMPARE(data.state(), ResourceState::NotLoaded);
    CORRADE_COMPARE(hello.state(), ResourceState::Loading);
    CORRADE_COMPARE(world.state(), ResourceState::Loading);

    loader.load();
    CORRADE_COMPARE(hello.state(), ResourceState::Final);
    CORRADE_COMPARE(*hello, 773);
    CORRADE_COMPARE(world.state(), ResourceState::NotFound);
}

}}
