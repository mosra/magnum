#ifndef Magnum_ResourceManager_h
#define Magnum_ResourceManager_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

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

/** @file /ResourceManager.h
 * @brief Class Magnum::ResourceManager, enum Magnum::ResourceDataState, Magnum::ResourcePolicy
 */

#include <unordered_map>

#include "Resource.h"

namespace Magnum {

/** @relates ResourceManager
 * @brief %Resource data state
 *
 * @see ResourceManager::set(), ResourceState
 */
enum class ResourceDataState: UnsignedByte {
    /**
     * The resource is currently loading. Parameter @p data in ResourceManager::set()
     * should be set to `null`.
     */
    Loading = UnsignedByte(ResourceState::Loading),

    /**
     * The resource was not found. Parameter @p data in ResourceManager::set()
     * should be set to `null`.
     */
    NotFound = UnsignedByte(ResourceState::NotFound),

    /**
     * The resource can be changed by the manager in the future. This is
     * slower, as Resource needs to ask the manager for new version every time
     * the data are accessed, but allows changing the data for e.g. debugging
     * purposes.
     */
    Mutable = UnsignedByte(ResourceState::Mutable),

    /**
     * The resource cannot be changed by the manager in the future. This is
     * faster, as Resource instances will ask for the data only one time, thus
     * suitable for production code.
     */
    Final = UnsignedByte(ResourceState::Final)
};

/** @relates ResourceManager
@brief %Resource policy

@see ResourceManager::set(), ResourceManager::free()
 */
enum class ResourcePolicy: UnsignedByte {
    /** The resource will stay resident for whole lifetime of resource manager. */
    Resident,

    /**
     * The resource will be unloaded when manually calling
     * ResourceManager::free() if nothing references it.
     */
    Manual,

    /** The resource will be unloaded when last reference to it is gone. */
    ReferenceCounted
};

template<class> class AbstractResourceLoader;

namespace Implementation {

template<class T> class ResourceManagerData {
    template<class, class> friend class Magnum::Resource;
    friend class AbstractResourceLoader<T>;

    ResourceManagerData(const ResourceManagerData<T>&) = delete;
    ResourceManagerData(ResourceManagerData<T>&&) = delete;
    ResourceManagerData<T>& operator=(const ResourceManagerData<T>&) = delete;
    ResourceManagerData<T>& operator=(ResourceManagerData<T>&&) = delete;

    public:
        virtual ~ResourceManagerData();

        std::size_t lastChange() const { return _lastChange; }

        std::size_t count() const { return _data.size(); }

        std::size_t referenceCount(ResourceKey key) const;

        ResourceState state(ResourceKey key) const;

        template<class U> Resource<T, U> get(ResourceKey key);

        void set(ResourceKey key, T* data, ResourceDataState state, ResourcePolicy policy);

        T* fallback() { return _fallback; }
        const T* fallback() const { return _fallback; }

        void setFallback(T* data);

        void free();

        AbstractResourceLoader<T>* loader() { return _loader; }
        const AbstractResourceLoader<T>* loader() const { return _loader; }

        void setLoader(AbstractResourceLoader<T>* loader);

    protected:
        ResourceManagerData(): _fallback(nullptr), _loader(nullptr), _lastChange(0) {}

    private:
        struct Data;

        const Data& data(ResourceKey key) { return _data[key]; }

        void incrementReferenceCount(ResourceKey key) {
            ++_data[key].referenceCount;
        }

        void decrementReferenceCount(ResourceKey key);

        std::unordered_map<ResourceKey, Data> _data;
        T* _fallback;
        AbstractResourceLoader<T>* _loader;
        std::size_t _lastChange;
};

}

/**
@brief %Resource manager

Provides storage for arbitrary set of types, accessible globally using
instance().

@section ResourceManager-usage Usage

Each resource is referenced from Resource class. For optimizing performance,
each resource can be set as mutable or final. Mutable resources can be
modified by the manager and thus each %Resource instance asks the manager for
modifications on each access. On the other hand, final resources cannot be
modified by the manager, so %Resource instances don't have to ask the manager
every time, which is faster.

It's possible to provide fallback for resources which are not available using
setFallback(). Accessing data of such resources will access the fallback
instead of failing on null pointer dereference. Availability and state of each
resource can be queried through function state() on the manager or
Resource::state() on each resource.

The resources can be managed in three ways - resident resources, which stay in
memory for whole lifetime of the manager, manually managed resources, which
can be deleted by calling free() if nothing references them anymore, and
reference counted resources, which are deleted as soon as the last reference
to them is removed.

%Resource state and policy is configured when setting the resource data in
set() and can be changed each time the data are updated, although already
final resources cannot obviously be set as mutable again.

Basic usage is:
- Typedef'ing manager of desired types, creating its instance.
@code
typedef ResourceManager<Mesh, Texture2D, AbstractShaderProgram> MyResourceManager;
MyResourceManager manager;
@endcode
- Filling the manager with resource data and acquiring the resources. Note
  that a resource can be acquired with get() even before the manager contains
  the data for it, as long as the resource data are not accessed (or fallback
  is provided).
@code
MyResourceManager* manager = MyResourceManager::instance();
Resource<Texture2D> texture(manager->get<Texture2D>("texture"));
Resource<AbstractShaderProgram, MyShader> shader(manager->get<AbstractShaderProgram, MyShader>("shader"));
Resource<Mesh> cube(manager->get<Mesh>("cube"));

// The manager doesn't have data for the cube yet, add them
if(!cube) {
    Mesh* mesh = new Mesh;
    // ...
    manager->set(cube.key(), mesh, ResourceDataState::Final, ResourcePolicy::Resident);
}
@endcode
- Using the resource data.
@code
shader->use();
texture->bind(layer);
cube->draw();
@endcode
- Destroying resource references and deleting manager instance when nothing
  references the resources anymore.

@see AbstractResourceLoader
*/
/* Due to too much work involved with explicit template instantiation (all
   Resource combinations, all ResourceManagerData...), this class doesn't have
   template implementation file. */
template<class... Types> class ResourceManager: private Implementation::ResourceManagerData<Types>... {
    public:
        /** @brief Global instance */
        static ResourceManager<Types...>* instance();

        /**
         * @brief Constructor
         *
         * Sets global instance pointer to itself.
         * @attention Only one instance of given ResourceManager type can be
         *      created.
         * @see instance()
         */
        explicit ResourceManager();

        /**
         * @brief Destructor
         *
         * Sets global instance pointer to `nullptr`.
         * @see instance()
         */
        ~ResourceManager();

        /** @brief Count of resources of given type */
        template<class T> std::size_t count() {
            return this->Implementation::ResourceManagerData<T>::count();
        }

        /**
         * @brief Get resource reference
         *
         * In some cases it's desirable to store various different types under
         * one base type for memory efficiency reasons. To avoid putting the
         * responsibility of proper casting on the user, the acquired resource
         * can be defined to cast the type automatically when accessing the
         * data. This is commonly used for shaders, e.g.:
         * @code
         * Resource<AbstractShaderProgram, MyShader> shader = manager->get<AbstractShaderProgram, MyShader>("shader");
         * @endcode
         */
        template<class T, class U = T> Resource<T, U> get(ResourceKey key) {
            return this->Implementation::ResourceManagerData<T>::template get<U>(key);
        }

        /**
         * @brief Reference count of given resource
         *
         * @see set()
         */
        template<class T> std::size_t referenceCount(ResourceKey key) const {
            return this->Implementation::ResourceManagerData<T>::referenceCount(key);
        }

        /**
         * @brief %Resource state
         *
         * @see set(), Resource::state()
         */
        template<class T> ResourceState state(ResourceKey key) const {
            return this->Implementation::ResourceManagerData<T>::state(key);
        }

        /**
         * @brief Set resource data
         * @return Pointer to self (for method chaining)
         *
         * If @p policy is set to `ResourcePolicy::ReferenceCounted`, there
         * must be already at least one reference to given resource, otherwise
         * the data will be deleted immediately and no resource will be
         * added. To avoid spending unnecessary loading time, add
         * reference-counted resources only if they are already referenced:
         * @code
         * if(manager.referenceCount<T>("myresource")) {
         *     // load data...
         *     manager.set("myresource", data, state, ResourcePolicy::ReferenceCounted);
         * }
         * @endcode
         * @attention If resource state is already `ResourceState::Final`,
         *      subsequent updates are not possible.
         * @see referenceCount(), state()
         */
        template<class T> ResourceManager<Types...>* set(ResourceKey key, T* data, ResourceDataState state, ResourcePolicy policy) {
            this->Implementation::ResourceManagerData<T>::set(key, data, state, policy);
            return this;
        }

        /**
         * @brief Set resource data
         * @return Pointer to self (for method chaining)
         *
         * Same as above function with state set to @ref ResourceDataState "ResourceDataState::Final"
         * and policy to @ref ResourcePolicy "ResourcePolicy::Resident".
         */
        template<class T> ResourceManager<Types...>* set(ResourceKey key, T* data) {
            this->Implementation::ResourceManagerData<T>::set(key, data, ResourceDataState::Final, ResourcePolicy::Resident);
            return this;
        }

        /** @brief Fallback for not found resources */
        template<class T> T* fallback() {
            return this->Implementation::ResourceManagerData<T>::fallback();
        }

        /** @overload */
        template<class T> const T* fallback() const {
            return this->Implementation::ResourceManagerData<T>::fallback();
        }

        /**
         * @brief Set fallback for not found resources
         * @return Pointer to self (for method chaining)
         */
        template<class T> ResourceManager<Types...>* setFallback(T* data) {
            this->Implementation::ResourceManagerData<T>::setFallback(data);
            return this;
        }

        /**
         * @brief Free all resources of given type which are not referenced
         * @return Pointer to self (for method chaining)
         */
        template<class T> ResourceManager<Types...>* free() {
            this->Implementation::ResourceManagerData<T>::free();
            return this;
        }

        /**
         * @brief Free all resources which are not referenced
         * @return Pointer to self (for method chaining)
         */
        ResourceManager<Types...>* free() {
            freeInternal<Types...>();
            return this;
        }

        /** @brief Loader for given type of resources */
        template<class T> AbstractResourceLoader<T>* loader() {
            return this->Implementation::ResourceManagerData<T>::loader();
        }

        /** @overload */
        template<class T> const AbstractResourceLoader<T>* loader() const {
            return this->Implementation::ResourceManagerData<T>::loader();
        }

        /**
         * @brief Set loader for given type of resources
         * @return Pointer to self (for method chaining)
         *
         * See AbstractResourceLoader documentation for more information.
         */
        template<class T> ResourceManager<Types...>* setLoader(AbstractResourceLoader<T>* loader) {
            this->Implementation::ResourceManagerData<T>::setLoader(loader);
            return this;
        }

    private:
        template<class FirstType, class ...NextTypes> void freeInternal() {
            free<FirstType>();
            freeInternal<NextTypes...>();
        }
        template<class...> void freeInternal() const {}

        static ResourceManager<Types...>*& internalInstance();
};

#ifndef MAGNUM_RESOURCEMANAGER_DONT_DEFINE_INTERNALINSTANCE
template<class ...Types> ResourceManager<Types...>*& ResourceManager<Types...>::internalInstance() {
    static ResourceManager<Types...>* _instance(nullptr);
    return _instance;
}
#endif

namespace Implementation {

template<class T> ResourceManagerData<T>::~ResourceManagerData() {
    delete _fallback;

    if(_loader) {
        _loader->manager = nullptr;
        delete _loader;
    }
}

template<class T> std::size_t ResourceManagerData<T>::referenceCount(const ResourceKey key) const {
    auto it = _data.find(key);
    if(it == _data.end()) return 0;
    return it->second.referenceCount;
}

template<class T> ResourceState ResourceManagerData<T>::state(const ResourceKey key) const {
    const auto it = _data.find(key);

    /* Resource not loaded */
    if(it == _data.end() || !it->second.data) {
        /* Fallback found, add *Fallback to state */
        if(_fallback) {
            if(it != _data.end() && it->second.state == ResourceDataState::Loading)
                return ResourceState::LoadingFallback;
            else if(it != _data.end() && it->second.state == ResourceDataState::NotFound)
                return ResourceState::NotFoundFallback;
            else return ResourceState::NotLoadedFallback;
        }

        /* Fallback not found, loading didn't start yet */
        if(it == _data.end() || (it->second.state != ResourceDataState::Loading && it->second.state != ResourceDataState::NotFound))
            return ResourceState::NotLoaded;
    }

    /* Loading / NotFound without fallback, Mutable / Final */
    return static_cast<ResourceState>(it->second.state);
}

template<class T> template<class U> Resource<T, U> ResourceManagerData<T>::get(ResourceKey key) {
    /* Ask loader for the data, if they aren't there yet */
    if(_loader && _data.find(key) == _data.end())
        _loader->load(key);

    return Resource<T, U>(this, key);
}

template<class T> void ResourceManagerData<T>::set(const ResourceKey key, T* const data, const ResourceDataState state, const ResourcePolicy policy) {
    auto it = _data.find(key);

    /* NotFound / Loading state shouldn't have any data */
    CORRADE_ASSERT((data == nullptr) == (state == ResourceDataState::NotFound || state == ResourceDataState::Loading),
        "ResourceManager::set(): data should be null if and only if state is NotFound or Loading", );

    /* Cannot change resource with already final state */
    CORRADE_ASSERT(it == _data.end() || it->second.state != ResourceDataState::Final,
        "ResourceManager::set(): cannot change already final resource" << key, );

    /* If nothing is referencing reference-counted resource, we're done */
    if(policy == ResourcePolicy::ReferenceCounted && (it == _data.end() || it->second.referenceCount == 0)) {
        Warning() << "ResourceManager: Reference-counted resource with key" << key << "isn't referenced from anywhere, deleting it immediately";
        delete data;

        /* Delete also already present resource (it could be here
            because previous policy could be other than
            ReferenceCounted) */
        if(it != _data.end()) _data.erase(it);

        return;

    /* Insert it, if not already here */
    } else if(it == _data.end())
        #ifndef CORRADE_GCC46_COMPATIBILITY
        it = _data.emplace(key, Data()).first;
        #else
        it = _data.insert({key, Data()}).first;
        #endif

    /* Replace previous data */
    delete it->second.data;
    it->second.data = data;
    it->second.state = state;
    it->second.policy = policy;
    ++_lastChange;
}

template<class T> void ResourceManagerData<T>::setFallback(T* const data) {
    delete _fallback;
    _fallback = data;
}

template<class T> void ResourceManagerData<T>::free() {
    /* Delete all non-referenced non-resident resources */
    for(auto it = _data.begin(); it != _data.end(); ) {
        if(it->second.policy != ResourcePolicy::Resident && !it->second.referenceCount)
            it = _data.erase(it);
        else ++it;
    }
}

template<class T> void ResourceManagerData<T>::setLoader(AbstractResourceLoader<T>* const loader) {
    /* Delete previous loader */
    delete _loader;

    /* Add new loader */
    if((_loader = loader)) _loader->manager = this;
}

template<class T> void ResourceManagerData<T>::decrementReferenceCount(ResourceKey key) {
    auto it = _data.find(key);

    /* Free the resource if it is reference counted */
    if(--it->second.referenceCount == 0 && it->second.policy == ResourcePolicy::ReferenceCounted)
        _data.erase(it);
}

template<class T> struct ResourceManagerData<T>::Data {
    Data(const Data&) = delete;
    Data& operator=(const Data&) = delete;
    Data& operator=(Data&&) = delete;

    Data(): data(nullptr), state(ResourceDataState::Mutable), policy(ResourcePolicy::Manual), referenceCount(0) {}

    Data(Data&& other): data(other.data), state(other.state), policy(other.policy), referenceCount(other.referenceCount) {
        other.data = nullptr;
        other.referenceCount = 0;
    }

    ~Data();

    T* data;
    ResourceDataState state;
    ResourcePolicy policy;
    std::size_t referenceCount;
};

template<class T> inline ResourceManagerData<T>::Data::~Data() {
    CORRADE_ASSERT(referenceCount == 0,
        "ResourceManager::~ResourceManager(): destroyed while data are still referenced", );
    delete data;
}

}

template<class ...Types> ResourceManager<Types...>* ResourceManager<Types...>::instance() {
    CORRADE_ASSERT(internalInstance(), "ResourceManager::instance(): no instance exists", nullptr);
    return internalInstance();
}

template<class ...Types> ResourceManager<Types...>::ResourceManager() {
    CORRADE_ASSERT(!internalInstance(), "ResourceManager::ResourceManager(): another instance is already created", );
    internalInstance() = this;
}

template<class ...Types> ResourceManager<Types...>::~ResourceManager() {
    CORRADE_INTERNAL_ASSERT(internalInstance() == this);
    internalInstance() = nullptr;
}

}

/* Make the definition complete */
#include "AbstractResourceLoader.h"

#endif
