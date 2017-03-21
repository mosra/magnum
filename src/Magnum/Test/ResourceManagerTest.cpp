/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
              Vladimír Vondruš <mosra@centrum.cz>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

#include <sstream>
#include <Corrade/TestSuite/Tester.h>

#include "Magnum/AbstractResourceLoader.h"
#include "Magnum/ResourceManager.h"

namespace Magnum { namespace Test {

struct ResourceManagerTest: TestSuite::Tester {
    explicit ResourceManagerTest();

    void state();
    void stateFallback();
    void stateDisallowed();
    void basic();
    void residentPolicy();
    void referenceCountedPolicy();
    void manualPolicy();
    void defaults();
    void clear();
    void clearWhileReferenced();
    void loader();

    void debugResourceState();
};

struct Data {
    static std::size_t count;

    explicit Data() { ++count; }
    ~Data() { --count; }
};

typedef Magnum::ResourceManager<Int, Data> ResourceManager;

size_t Data::count = 0;

ResourceManagerTest::ResourceManagerTest() {
    addTests({&ResourceManagerTest::state,
              &ResourceManagerTest::stateFallback,
              &ResourceManagerTest::stateDisallowed,
              &ResourceManagerTest::basic,
              &ResourceManagerTest::residentPolicy,
              &ResourceManagerTest::referenceCountedPolicy,
              &ResourceManagerTest::manualPolicy,
              &ResourceManagerTest::defaults,
              &ResourceManagerTest::clear,
              &ResourceManagerTest::clearWhileReferenced,
              &ResourceManagerTest::loader,

              &ResourceManagerTest::debugResourceState});
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
        rm.setFallback(new Data);

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
    Error redirectError{&out};

    Data d; /* Done this way to prevent memory leak on assertion (yes, the code is bad) */
    rm.set("data", &d, ResourceDataState::Loading, ResourcePolicy::Resident);
    CORRADE_COMPARE(out.str(), "ResourceManager::set(): data should be null if and only if state is NotFound or Loading\n");

    out.str({});
    rm.set<Data>("data", nullptr, ResourceDataState::Final, ResourcePolicy::Resident);
    CORRADE_COMPARE(out.str(), "ResourceManager::set(): data should be null if and only if state is NotFound or Loading\n");
}

void ResourceManagerTest::basic() {
    ResourceManager rm;

    /* One mutable, one final */
    ResourceKey questionKey("the-question");
    ResourceKey answerKey("the-answer");
    rm.set(questionKey, 10, ResourceDataState::Mutable, ResourcePolicy::Resident);
    rm.set(answerKey, 42, ResourceDataState::Final, ResourcePolicy::Resident);
    Resource<Int> theQuestion = rm.get<Int>(questionKey);
    Resource<Int> theAnswer = rm.get<Int>(answerKey);

    /* Check basic functionality */
    CORRADE_COMPARE(theQuestion.state(), ResourceState::Mutable);
    CORRADE_COMPARE(theAnswer.state(), ResourceState::Final);
    CORRADE_COMPARE(*theQuestion, 10);
    CORRADE_COMPARE(*theAnswer, 42);
    CORRADE_COMPARE(rm.count<Int>(), 2);

    /* Cannot change already final resource */
    std::ostringstream out;
    Error redirectError{&out};
    int a = 43; /* Done this way to prevent a memory leak on assert (yes, the code is bad) */
    rm.set(answerKey, &a, ResourceDataState::Mutable, ResourcePolicy::Resident);
    CORRADE_COMPARE(*theAnswer, 42);
    CORRADE_COMPARE(out.str(), "ResourceManager::set(): cannot change already final resource " + answerKey.hexString() + '\n');

    /* But non-final can be changed */
    rm.set(questionKey, 20, ResourceDataState::Final, ResourcePolicy::Resident);
    CORRADE_COMPARE(theQuestion.state(), ResourceState::Final);
    CORRADE_COMPARE(*theQuestion, 20);
}

void ResourceManagerTest::residentPolicy() {
    ResourceManager* rm = new ResourceManager;

    rm->set("blah", new Data, ResourceDataState::Mutable, ResourcePolicy::Resident);
    CORRADE_COMPARE(Data::count, 1);

    rm->free();
    CORRADE_COMPARE(Data::count, 1);

    delete rm;
    CORRADE_COMPARE(Data::count, 0);
}

void ResourceManagerTest::referenceCountedPolicy() {
    ResourceManager rm;

    ResourceKey dataRefCountKey("dataRefCount");

    /* Resource is deleted after all references are removed */
    rm.set(dataRefCountKey, new Data, ResourceDataState::Final, ResourcePolicy::ReferenceCounted);
    CORRADE_COMPARE(rm.count<Data>(), 1);
    {
        Resource<Data> data = rm.get<Data>(dataRefCountKey);
        CORRADE_COMPARE(data.state(), ResourceState::Final);
        CORRADE_COMPARE(Data::count, 1);
    }

    CORRADE_COMPARE(rm.count<Data>(), 0);
    CORRADE_COMPARE(Data::count, 0);

    /* Reference counted resources which were not used once will stay loaded
       until free() is called */
    rm.set(dataRefCountKey, new Data, ResourceDataState::Final, ResourcePolicy::ReferenceCounted);
    CORRADE_COMPARE(rm.count<Data>(), 1);
    CORRADE_COMPARE(rm.state<Data>(dataRefCountKey), ResourceState::Final);
    CORRADE_COMPARE(rm.referenceCount<Data>(dataRefCountKey), 0);

    rm.free<Data>();
    CORRADE_COMPARE(rm.count<Data>(), 0);
    CORRADE_COMPARE(rm.state<Data>(dataRefCountKey), ResourceState::NotLoaded);
    CORRADE_COMPARE(rm.referenceCount<Data>(dataRefCountKey), 0);
}

void ResourceManagerTest::manualPolicy() {
    ResourceManager rm;

    ResourceKey dataKey("data");

    /* Manual free */
    {
        rm.set(dataKey, new Data, ResourceDataState::Mutable, ResourcePolicy::Manual);
        Resource<Data> data = rm.get<Data>(dataKey);
        rm.free();
    }

    CORRADE_COMPARE(rm.count<Data>(), 1);
    CORRADE_COMPARE(Data::count, 1);
    rm.free();
    CORRADE_COMPARE(rm.count<Data>(), 0);
    CORRADE_COMPARE(Data::count, 0);

    rm.set(dataKey, new Data, ResourceDataState::Mutable, ResourcePolicy::Manual);
    CORRADE_COMPARE(rm.count<Data>(), 1);
    CORRADE_COMPARE(Data::count, 1);
}

void ResourceManagerTest::defaults() {
    ResourceManager rm;
    rm.set("data", new Data);
    CORRADE_COMPARE(rm.state<Data>("data"), ResourceState::Final);
}

void ResourceManagerTest::clear() {
    ResourceManager rm;

    rm.set("blah", new Data);
    CORRADE_COMPARE(Data::count, 1);

    rm.free();
    CORRADE_COMPARE(Data::count, 1);

    rm.clear();
    CORRADE_COMPARE(Data::count, 0);
}

void ResourceManagerTest::clearWhileReferenced() {
    /* Should cover also the destruction case */

    std::ostringstream out;
    Error redirectError{&out};

    ResourceManager rm;
    int a{}; /* Done this way to prevent leak on assertion (yes, the code is bad) */
    rm.set("blah", &a);
    /** @todo this will leak, is there any better solution without hitting assertion in decrementReferenceCount()? */
    /** @todo remove the suppression from package/ci/leaksanitizer.conf then */
    new Resource<Int>(rm.get<Int>("blah"));

    rm.clear();
    CORRADE_COMPARE(out.str(), "ResourceManager: cleared/destroyed while data are still referenced\n");
}

void ResourceManagerTest::loader() {
    class IntResourceLoader: public AbstractResourceLoader<Int> {
        public:
            IntResourceLoader(): resource(ResourceManager::instance().get<Data>("data")) {}

            void load() {
                set("hello", 773, ResourceDataState::Final, ResourcePolicy::Resident);
                setNotFound("world");
            }

        private:
            void doLoad(ResourceKey) override {}

            std::string doName(ResourceKey key) const override {
                if(key == ResourceKey("hello")) return "hello";
                return "";
            }

            /* To verify that the loader is destroyed before unloading
               _all types of_ resources */
            Resource<Data> resource;
    };

    auto rm = new ResourceManager;
    auto loader = new IntResourceLoader;
    rm->setLoader(loader);

    {
        Resource<Data> data = rm->get<Data>("data");
        Resource<Int> hello = rm->get<Int>("hello");
        Resource<Int> world = rm->get<Int>("world");
        CORRADE_COMPARE(data.state(), ResourceState::NotLoaded);
        CORRADE_COMPARE(hello.state(), ResourceState::Loading);
        CORRADE_COMPARE(world.state(), ResourceState::Loading);

        CORRADE_COMPARE(loader->requestedCount(), 2);
        CORRADE_COMPARE(loader->loadedCount(), 0);
        CORRADE_COMPARE(loader->notFoundCount(), 0);
        CORRADE_COMPARE(loader->name(ResourceKey("hello")), "hello");

        loader->load();
        CORRADE_COMPARE(hello.state(), ResourceState::Final);
        CORRADE_COMPARE(*hello, 773);
        CORRADE_COMPARE(world.state(), ResourceState::NotFound);

        CORRADE_COMPARE(loader->requestedCount(), 2);
        CORRADE_COMPARE(loader->loadedCount(), 1);
        CORRADE_COMPARE(loader->notFoundCount(), 1);

        /* Verify that the loader is deleted at proper time */
        rm->set("data", new Data);
        CORRADE_COMPARE(Data::count, 1);
    }

    delete rm;
    CORRADE_COMPARE(Data::count, 0);
}

void ResourceManagerTest::debugResourceState() {
    std::ostringstream out;
    Debug{&out} << ResourceState::Loading << ResourceState(0xbe);
    CORRADE_COMPARE(out.str(), "ResourceState::Loading ResourceState(0xbe)\n");
}

}}

CORRADE_TEST_MAIN(Magnum::Test::ResourceManagerTest)
