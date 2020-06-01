/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2019 Daniel Guzman <daniel.guzman85@gmail.com>

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
#include <Corrade/Utility/FormatStl.h>

#include "Magnum/AbstractResourceLoader.h"
#include "Magnum/ResourceManager.h"

namespace Magnum { namespace Test { namespace {

struct ResourceManagerTest: TestSuite::Tester {
    explicit ResourceManagerTest();

    void constructResource();
    void constructResourceEmpty();
    void constructResourceCopy();
    void constructResourceMove();

    void compare();
    void state();
    void stateFallback();
    void stateDisallowed();
    void basic();
    void changeFinalResource();
    void residentPolicy();
    void referenceCountedPolicy();
    void manualPolicy();
    void defaults();
    void clear();
    void clearWhileReferenced();

    void loader();
    void loaderSetNullptr();

    void debugResourceState();
    void debugResourceKey();
};

struct Data {
    static std::size_t count;

    explicit Data() { ++count; }
    ~Data() { --count; }
};

typedef Magnum::ResourceManager<Int, Data> ResourceManager;

size_t Data::count = 0;

ResourceManagerTest::ResourceManagerTest() {
    addTests({&ResourceManagerTest::constructResource,
              &ResourceManagerTest::constructResourceEmpty,
              &ResourceManagerTest::constructResourceCopy,
              &ResourceManagerTest::constructResourceMove,

              &ResourceManagerTest::compare,
              &ResourceManagerTest::state,
              &ResourceManagerTest::stateFallback,
              &ResourceManagerTest::stateDisallowed,
              &ResourceManagerTest::basic,
              &ResourceManagerTest::changeFinalResource,
              &ResourceManagerTest::residentPolicy,
              &ResourceManagerTest::referenceCountedPolicy,
              &ResourceManagerTest::manualPolicy,
              &ResourceManagerTest::defaults,
              &ResourceManagerTest::clear,
              &ResourceManagerTest::clearWhileReferenced,

              &ResourceManagerTest::loader,
              &ResourceManagerTest::loaderSetNullptr,

              &ResourceManagerTest::debugResourceState,
              &ResourceManagerTest::debugResourceKey});
}

void ResourceManagerTest::constructResource() {
    ResourceManager rm;
    rm.set("thing", 6432);

    Resource<Int> a = rm.get<Int>("thing");
    CORRADE_COMPARE(a.key(), ResourceKey("thing"));
    CORRADE_COMPARE(a.state(), ResourceState::Final);
    CORRADE_COMPARE(*a, 6432);
    CORRADE_COMPARE(rm.referenceCount<Int>("thing"), 1);
}

void ResourceManagerTest::constructResourceEmpty() {
    Resource<Int> a;
    CORRADE_COMPARE(a.key(), ResourceKey{});
    CORRADE_COMPARE(a.state(), ResourceState::Final);
    CORRADE_VERIFY(!a);
}

void ResourceManagerTest::constructResourceCopy() {
    ResourceManager rm;
    rm.set("thing", 6432);

    Resource<Int> a = rm.get<Int>("thing");
    CORRADE_COMPARE(rm.referenceCount<Int>("thing"), 1);

    Resource<Int> b = a;
    CORRADE_COMPARE(a.key(), ResourceKey("thing"));
    CORRADE_COMPARE(b.key(), ResourceKey("thing"));
    CORRADE_COMPARE(a.state(), ResourceState::Final);
    CORRADE_COMPARE(b.state(), ResourceState::Final);
    CORRADE_COMPARE(*a, 6432);
    CORRADE_COMPARE(*b, 6432);
    CORRADE_COMPARE(rm.referenceCount<Int>("thing"), 2);

    Resource<Int> c;
    c = b;
    CORRADE_COMPARE(b.key(), ResourceKey("thing"));
    CORRADE_COMPARE(c.key(), ResourceKey("thing"));
    CORRADE_COMPARE(b.state(), ResourceState::Final);
    CORRADE_COMPARE(c.state(), ResourceState::Final);
    CORRADE_COMPARE(*b, 6432);
    CORRADE_COMPARE(*c, 6432);
    CORRADE_COMPARE(rm.referenceCount<Int>("thing"), 3);
}

void ResourceManagerTest::constructResourceMove() {
    ResourceManager rm;
    rm.set("thing", 6432);

    Resource<Int> a = rm.get<Int>("thing");
    CORRADE_COMPARE(rm.referenceCount<Int>("thing"), 1);

    Resource<Int> b = std::move(a);
    CORRADE_COMPARE(b.key(), ResourceKey("thing"));
    CORRADE_COMPARE(b.state(), ResourceState::Final);
    CORRADE_COMPARE(*b, 6432);
    CORRADE_COMPARE(rm.referenceCount<Int>("thing"), 1);

    Resource<Int> c;
    c = std::move(b);
    CORRADE_COMPARE(c.key(), ResourceKey("thing"));
    CORRADE_COMPARE(c.state(), ResourceState::Final);
    CORRADE_COMPARE(*c, 6432);
    CORRADE_COMPARE(rm.referenceCount<Int>("thing"), 1);

    CORRADE_VERIFY(std::is_nothrow_move_constructible<Resource<Int>>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<Resource<Int>>::value);
}

void ResourceManagerTest::compare() {
    ResourceManager rm1;

    ResourceKey resKeyA("keyA");
    ResourceKey resKeyB("keyB");
    rm1.set(resKeyA, 1);
    rm1.set(resKeyB, 0);

    Resource<Int> resA1 = rm1.get<Int>(resKeyA);
    Resource<Int> resA2 = rm1.get<Int>(resKeyA);
    Resource<Int> resB = rm1.get<Int>(resKeyB);

    CORRADE_VERIFY(resA1 == resA1);
    CORRADE_VERIFY(resA1 == resA2);
    CORRADE_VERIFY(resA1 != resB);

    Magnum::ResourceManager<Int, Float> rm2;
    rm2.set(resKeyA, 1);
    rm2.set(resKeyA, 1.0f);

    Resource<Int> resAOther = rm2.get<Int>(resKeyA);
    Resource<Float> resADifferentType = rm2.get<Float>(resKeyA);
    /* Verify it checks for manager equality as well */
    CORRADE_VERIFY(resA1 != resAOther);
    /* If the comparison operator wouldn't be deleted, the implicit conversion
       would kick in and then 1.0f == 1.0f, which is wrong. With the deleted
       operator this doesn't compile. */
    //CORRADE_VERIFY(resA1 != resADifferentType);
    //CORRADE_VERIFY(!(resA1 == resADifferentType));
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

        /* Fetching a resource that's not loaded first */
        Resource<Data> data = rm.get<Data>("data");
        CORRADE_VERIFY(!data);
        CORRADE_COMPARE(data.state(), ResourceState::NotLoaded);
        CORRADE_COMPARE(rm.state<Data>("data"), ResourceState::NotLoaded);

        /* Setting a fallback should make the resource fetch it */
        rm.setFallback(Containers::pointer<Data>());
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

        /* Unsetting a fallback should make the resource go back to empty */
        rm.setFallback<Data>(nullptr);
        CORRADE_VERIFY(!data);
        CORRADE_COMPARE(data.state(), ResourceState::NotFound);
        CORRADE_COMPARE(rm.state<Data>("data"), ResourceState::NotFound);
    }

    /* Fallback gets destroyed */
    CORRADE_COMPARE(Data::count, 0);
}

void ResourceManagerTest::stateDisallowed() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

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

    /* Non-final can be changed */
    rm.set(questionKey, 20, ResourceDataState::Final, ResourcePolicy::Resident);
    CORRADE_COMPARE(theQuestion.state(), ResourceState::Final);
    CORRADE_COMPARE(*theQuestion, 20);
}

void ResourceManagerTest::changeFinalResource() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

    ResourceManager rm;

    ResourceKey answerKey("the-answer");
    rm.set(answerKey, 42, ResourceDataState::Final, ResourcePolicy::Resident);
    Resource<Int> theAnswer = rm.get<Int>(answerKey);
    CORRADE_COMPARE(theAnswer.state(), ResourceState::Final);
    CORRADE_COMPARE(*theAnswer, 42);
    CORRADE_COMPARE(rm.count<Int>(), 1);

    /* Final can not be changed */
    std::ostringstream out;
    Error redirectError{&out};
    int a = 43; /* Done this way to prevent a memory leak on assert (yes, the code is bad) */
    rm.set(answerKey, &a, ResourceDataState::Mutable, ResourcePolicy::Resident);
    CORRADE_COMPARE(*theAnswer, 42);
    CORRADE_COMPARE(out.str(), Utility::formatString("ResourceManager::set(): cannot change already final resource ResourceKey(0x{})\n", answerKey.hexString()));
}

void ResourceManagerTest::residentPolicy() {
    {
        ResourceManager rm;

        rm.set("blah", Containers::pointer<Data>(), ResourceDataState::Mutable, ResourcePolicy::Resident);
        CORRADE_COMPARE(Data::count, 1);

        rm.free();
        CORRADE_COMPARE(Data::count, 1);
    }
    CORRADE_COMPARE(Data::count, 0);
}

void ResourceManagerTest::referenceCountedPolicy() {
    ResourceManager rm;

    ResourceKey dataRefCountKey("dataRefCount");

    /* Resource is deleted after all references are removed */
    rm.set(dataRefCountKey, Containers::pointer<Data>(), ResourceDataState::Final, ResourcePolicy::ReferenceCounted);
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
    rm.set(dataRefCountKey, Containers::pointer<Data>(), ResourceDataState::Final, ResourcePolicy::ReferenceCounted);
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
        rm.set(dataKey, Containers::pointer<Data>(), ResourceDataState::Mutable, ResourcePolicy::Manual);
        Resource<Data> data = rm.get<Data>(dataKey);
        rm.free();
    }

    CORRADE_COMPARE(rm.count<Data>(), 1);
    CORRADE_COMPARE(Data::count, 1);
    rm.free();
    CORRADE_COMPARE(rm.count<Data>(), 0);
    CORRADE_COMPARE(Data::count, 0);

    rm.set(dataKey, Containers::pointer<Data>(), ResourceDataState::Mutable, ResourcePolicy::Manual);
    CORRADE_COMPARE(rm.count<Data>(), 1);
    CORRADE_COMPARE(Data::count, 1);
}

void ResourceManagerTest::defaults() {
    ResourceManager rm;
    rm.set("data", Containers::pointer<Data>());
    CORRADE_COMPARE(rm.state<Data>("data"), ResourceState::Final);
}

void ResourceManagerTest::clear() {
    ResourceManager rm;

    rm.set("blah", Containers::pointer<Data>());
    CORRADE_COMPARE(Data::count, 1);

    rm.free();
    CORRADE_COMPARE(Data::count, 1);

    rm.clear();
    CORRADE_COMPARE(Data::count, 0);
}

void ResourceManagerTest::clearWhileReferenced() {
    #ifdef CORRADE_NO_ASSERT
    CORRADE_SKIP("CORRADE_NO_ASSERT defined, can't test assertions");
    #endif

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
            IntResourceLoader(ResourceManager& instance): resource{instance.get<Data>("data")} {}

            void load() {
                set("hello", Containers::pointer<Int>(773), ResourceDataState::Final, ResourcePolicy::Resident);
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

    {
        ResourceManager rm;
        Containers::Pointer<IntResourceLoader> loaderPtr{Containers::InPlaceInit, rm};
        IntResourceLoader& loader = *loaderPtr;
        rm.setLoader<Int>(std::move(loaderPtr));

        Resource<Data> data = rm.get<Data>("data");
        Resource<Int> hello = rm.get<Int>("hello");
        Resource<Int> world = rm.get<Int>("world");
        CORRADE_COMPARE(data.state(), ResourceState::NotLoaded);
        CORRADE_COMPARE(hello.state(), ResourceState::Loading);
        CORRADE_COMPARE(world.state(), ResourceState::Loading);

        CORRADE_COMPARE(loader.requestedCount(), 2);
        CORRADE_COMPARE(loader.loadedCount(), 0);
        CORRADE_COMPARE(loader.notFoundCount(), 0);
        CORRADE_COMPARE(loader.name(ResourceKey("hello")), "hello");

        loader.load();
        CORRADE_COMPARE(hello.state(), ResourceState::Final);
        CORRADE_COMPARE(*hello, 773);
        CORRADE_COMPARE(world.state(), ResourceState::NotFound);

        CORRADE_COMPARE(loader.requestedCount(), 2);
        CORRADE_COMPARE(loader.loadedCount(), 1);
        CORRADE_COMPARE(loader.notFoundCount(), 1);

        /* Verify that the loader is deleted at proper time */
        rm.set("data", Containers::pointer<Data>());
        CORRADE_COMPARE(Data::count, 1);
    }

    CORRADE_COMPARE(Data::count, 0);
}

void ResourceManagerTest::loaderSetNullptr() {
    class IntResourceLoader: public AbstractResourceLoader<Int> {
        public:
            void load() {
                set("hello", 1337, ResourceDataState::Final, ResourcePolicy::Resident);
                set("world", 42, ResourceDataState::Final, ResourcePolicy::Resident);
            }

        private:
            void doLoad(ResourceKey key) override {
                /* Verify that calling load() with nullptr + Loading works */
                set(key, nullptr, ResourceDataState::Loading, ResourcePolicy::Resident);
                set("world", nullptr, ResourceDataState::Loading, ResourcePolicy::Resident);
            }
    };

    ResourceManager rm;
    Containers::Pointer<IntResourceLoader> loaderPtr{Containers::InPlaceInit};
    IntResourceLoader& loader = *loaderPtr;
    rm.setLoader<Int>(std::move(loaderPtr));

    CORRADE_COMPARE(rm.state<Int>("hello"), ResourceState::NotLoaded);
    CORRADE_COMPARE(rm.state<Int>("world"), ResourceState::NotLoaded);

    /* Loading "hello" triggers a load of "world" as well */
    Resource<Int> hello = rm.get<Int>("hello");
    CORRADE_COMPARE(hello.state(), ResourceState::Loading);
    CORRADE_COMPARE(rm.state<Int>("world"), ResourceState::Loading);
    CORRADE_COMPARE(loader.requestedCount(), 1);
    CORRADE_COMPARE(loader.loadedCount(), 0);
    CORRADE_COMPARE(loader.notFoundCount(), 0);

    /* Load the things */
    loader.load();
    CORRADE_COMPARE(hello.state(), ResourceState::Final);
    CORRADE_COMPARE(*hello, 1337);
    CORRADE_COMPARE(loader.requestedCount(), 1);
    CORRADE_COMPARE(loader.loadedCount(), 2);
    CORRADE_COMPARE(loader.notFoundCount(), 0);

    /* World is now loaded as well as a side-effect */
    Resource<Int> world = rm.get<Int>("world");
    CORRADE_COMPARE(world.state(), ResourceState::Final);
    CORRADE_COMPARE(*world, 42);
}

void ResourceManagerTest::debugResourceState() {
    std::ostringstream out;
    Debug{&out} << ResourceState::Loading << ResourceState(0xbe);
    CORRADE_COMPARE(out.str(), "ResourceState::Loading ResourceState(0xbe)\n");
}

void ResourceManagerTest::debugResourceKey() {
    std::ostringstream out;
    ResourceKey hello = "hello";
    Debug{&out} << hello;
    CORRADE_COMPARE(out.str(), Utility::formatString("ResourceKey(0x{})\n", hello.hexString()));
}

}}}

CORRADE_TEST_MAIN(Magnum::Test::ResourceManagerTest)
