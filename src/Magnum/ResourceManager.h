#ifndef Magnum_ResourceManager_h
#define Magnum_ResourceManager_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

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

/** @file
 * @brief Class @ref Magnum::ResourceManager, @ref Magnum::ResourceDataState, @ref Magnum::ResourcePolicy
 */

#include <unordered_map>
#include <Corrade/Containers/Pointer.h>

#include "Magnum/Resource.h"

namespace Magnum {

/**
@brief Resource data state

@see @ref ResourceManager::set(), @ref ResourceState
*/
enum class ResourceDataState: UnsignedByte {
    /**
     * The resource is currently loading. Parameter @p data in
     * @ref ResourceManager::set() should be set to @cpp nullptr @ce.
     */
    Loading = UnsignedByte(ResourceState::Loading),

    /**
     * The resource was not found. Parameter @p data in
     * @ref ResourceManager::set() should be set to @cpp nullptr @ce.
     */
    NotFound = UnsignedByte(ResourceState::NotFound),

    /**
     * The resource can be changed by the manager in the future. This is
     * slower, as @ref Resource needs to ask the manager for new version every
     * time the data are accessed, but allows changing the data for e.g.
     * debugging purposes.
     */
    Mutable = UnsignedByte(ResourceState::Mutable),

    /**
     * The resource cannot be changed by the manager in the future. This is
     * faster, as @ref Resource instances will ask for the data only one time,
     * thus suitable for production code.
     */
    Final = UnsignedByte(ResourceState::Final)
};

/**
@brief Resource policy

@see @ref ResourceManager::set(), @ref ResourceManager::free()
 */
enum class ResourcePolicy: UnsignedByte {
    /** The resource will stay resident for whole lifetime of resource manager. */
    Resident,

    /**
     * The resource will be unloaded when manually calling
     * @ref ResourceManager::free() if nothing references it.
     */
    Manual,

    /** The resource will be unloaded when last reference to it is gone. */
    ReferenceCounted
};

template<class> class AbstractResourceLoader;

namespace Implementation {

/** @todo Print either resource key or name string based on loader capabilities */

template<class T> class ResourceManagerData {
    template<class, class> friend class Magnum::Resource;
    friend AbstractResourceLoader<T>;

    public:
        ResourceManagerData(const ResourceManagerData<T>&) = delete;
        ResourceManagerData(ResourceManagerData<T>&&) = delete;

        virtual ~ResourceManagerData();

        ResourceManagerData<T>& operator=(const ResourceManagerData<T>&) = delete;
        ResourceManagerData<T>& operator=(ResourceManagerData<T>&&) = delete;

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

        void clear() { _data.clear(); }

        AbstractResourceLoader<T>* loader() { return _loader; }
        const AbstractResourceLoader<T>* loader() const { return _loader; }

        void freeLoader();

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

/* Helper class for defining which real types are in the type pack */
template<class...> struct ResourceTypePack {};

}

/**
@brief Resource manager

Provides storage for arbitrary set of types.

@section ResourceMananger-usage Usage

Each resource is referenced from @ref Resource class. For optimizing
performance, each resource can be set as mutable or final. Mutable resources
can be modified by the manager and thus each @ref Resource instance asks the
manager for modifications on each access. On the other hand, final resources
cannot be modified by the manager, so @ref Resource instances don't have to ask
the manager every time, which is faster.

It's possible to provide fallback for resources which are not available using
@ref setFallback(). Accessing data of such resources will access the fallback
instead of failing on null pointer dereference. Availability and state of each
resource can be queried through function @ref state() on the manager or
@ref Resource::state() on each resource.

The resources can be managed in three ways - resident resources, which stay in
memory for whole lifetime of the manager, manually managed resources, which
can be deleted by calling @ref free() if nothing references them anymore, and
reference counted resources, which are deleted as soon as the last reference
to them is removed.

Resource state and policy is configured when setting the resource data in
@ref set() and can be changed each time the data are updated, although already
final resources cannot obviously be set as mutable again.

Basic usage is:

<ul>
<li>
    Typedef'ing manager of desired types, creating its instance.

    @snippet Magnum.cpp ResourceManager-typedef
</li>
<li>
    Filling the manager with resource data and acquiring the resources. Note
    that a resource can be acquired with @ref get() even before the manager
    contains the data for it, as long as the resource data are not accessed (or
    fallback is provided).

    @snippet Magnum.cpp ResourceManager-fill
</li>
<li>
    Using the resource data.

    @snippet Magnum.cpp ResourceManager-use
</li>
<li>
    Destroying resource references and deleting manager instance when nothing
    references the resources anymore.
</li>
</ul>

@see @ref AbstractResourceLoader
*/
/* Due to too much work involved with explicit template instantiation (all
   Resource combinations, all ResourceManagerData...), this class doesn't have
   template implementation file. */
template<class... Types> class ResourceManager: private Implementation::ResourceManagerData<Types>... {
    public:
        /** @brief Constructor */
        explicit ResourceManager();

        /**
         * @brief Destructor
         *
         * Expects that all resources are not referenced anymore at the point
         * of destruction.
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
         *
         * @snippet Magnum.cpp ResourceManager-get-derived
         */
        template<class T, class U = T> Resource<T, U> get(ResourceKey key) {
            return this->Implementation::ResourceManagerData<T>::template get<U>(key);
        }

        /**
         * @brief Reference count of given resource
         *
         * @see @ref set()
         */
        template<class T> std::size_t referenceCount(ResourceKey key) const {
            return this->Implementation::ResourceManagerData<T>::referenceCount(key);
        }

        /**
         * @brief Resource state
         *
         * @see @ref set(), @ref Resource::state()
         */
        template<class T> ResourceState state(ResourceKey key) const {
            return this->Implementation::ResourceManagerData<T>::state(key);
        }

        /**
         * @brief Set resource data
         * @return Reference to self (for method chaining)
         *
         * Resources with @ref ResourcePolicy::ReferenceCounted are added with
         * zero reference count. It means that all reference counted resources
         * which were only loaded but not used will stay loaded and you need to
         * explicitly call @ref free() to delete them.
         * @attention Subsequent updates are not possible if resource state is
         *      already @ref ResourceState::Final.
         * @see @ref referenceCount(), @ref state()
         */
        template<class T> ResourceManager<Types...>& set(ResourceKey key, T* data, ResourceDataState state, ResourcePolicy policy) {
            this->Implementation::ResourceManagerData<T>::set(key, data, state, policy);
            return *this;
        }

        /**
         * @overload
         * @m_since{2019,10}
         */
        template<class T> ResourceManager<Types...>& set(ResourceKey key, Containers::Pointer<T>&& data, ResourceDataState state, ResourcePolicy policy) {
            set(key, data.release(), state, policy);
            return *this;
        }

        /** @overload */
        template<class U> ResourceManager<Types...>& set(ResourceKey key, U&& data, ResourceDataState state, ResourcePolicy policy) {
            return set(key, new typename std::decay<U>::type(std::forward<U>(data)), state, policy);
        }

        /**
         * @brief Set resource data
         * @return Reference to self (for method chaining)
         *
         * Same as above with state set to @ref ResourceDataState::Final and
         * policy to @ref ResourcePolicy::Resident.
         */
        template<class T> ResourceManager<Types...>& set(ResourceKey key, T* data) {
            return set(key, data, ResourceDataState::Final, ResourcePolicy::Resident);
        }

        /** @overload */
        template<class T> ResourceManager<Types...>& set(ResourceKey key, Containers::Pointer<T>&& data) {
            return set(key, data.release());
        }

        /** @overload */
        template<class U> ResourceManager<Types...>& set(ResourceKey key, U&& data) {
            return set(key, new typename std::decay<U>::type(std::forward<U>(data)));
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
         * @return Reference to self (for method chaining)
         */
        template<class T> ResourceManager<Types...>& setFallback(T* data) {
            this->Implementation::ResourceManagerData<T>::setFallback(data);
            return *this;
        }

        /**
         * @overload
         * @m_since{2019,10}
         */
        template<class T> ResourceManager<Types...>& setFallback(Containers::Pointer<T>&& data) {
            setFallback(data.release());
            return *this;
        }

        /** @overload */
        template<class U> ResourceManager<Types...>& setFallback(U&& data) {
            return setFallback(new typename std::decay<U>::type(std::forward<U>(data)));
        }

        /**
         * @brief Free all resources of given type which are not referenced
         * @return Reference to self (for method chaining)
         */
        template<class T> ResourceManager<Types...>& free() {
            this->Implementation::ResourceManagerData<T>::free();
            return *this;
        }

        /**
         * @brief Free all resources which are not referenced
         * @return Reference to self (for method chaining)
         */
        ResourceManager<Types...>& free() {
            freeInternal(Implementation::ResourceTypePack<Types...>{});
            return *this;
        }

        /**
         * @brief Clear all resources of given type
         * @return Reference to self (for method chaining)
         *
         * Unlike @ref free() this function assumes that no resource is
         * referenced.
         */
        template<class T> ResourceManager<Types...>& clear() {
            this->Implementation::ResourceManagerData<T>::clear();
            return *this;
        }

        /**
         * @brief Clear all resources
         * @return Reference to self (for method chaining)
         *
         * Unlike @ref free() this function assumes that no resource is
         * referenced.
         */
        ResourceManager<Types...>& clear() {
            clearInternal(Implementation::ResourceTypePack<Types...>{});
            return *this;
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
         * @param loader    Loader or @cpp nullptr @ce if unsetting previous
         *      loader.
         * @return Reference to self (for method chaining)
         *
         * See @ref AbstractResourceLoader documentation for more information.
         * @attention The loader is deleted on destruction before unloading
         *      all resources.
         */
        template<class T> ResourceManager<Types...>& setLoader(AbstractResourceLoader<T>* loader) {
            this->Implementation::ResourceManagerData<T>::setLoader(loader);
            return *this;
        }

        /** @overload */
        template<class T> ResourceManager<Types...>& setLoader(Containers::Pointer<AbstractResourceLoader<T>>&& loader) {
            return setLoader(loader.release());
        }

    private:
        template<class FirstType, class ...NextTypes> void freeInternal(Implementation::ResourceTypePack<FirstType, NextTypes...>) {
            free<FirstType>();
            freeInternal(Implementation::ResourceTypePack<NextTypes...>{});
        }
        void freeInternal(Implementation::ResourceTypePack<>) const {}

        template<class FirstType, class ...NextTypes> void clearInternal(Implementation::ResourceTypePack<FirstType, NextTypes...>) {
            clear<FirstType>();
            clearInternal(Implementation::ResourceTypePack<NextTypes...>{});
        }
        void clearInternal(Implementation::ResourceTypePack<>) const {}

        template<class FirstType, class ...NextTypes> void freeLoaders(Implementation::ResourceTypePack<FirstType, NextTypes...>) {
            Implementation::ResourceManagerData<FirstType>::freeLoader();
            freeLoaders(Implementation::ResourceTypePack<NextTypes...>{});
        }
        void freeLoaders(Implementation::ResourceTypePack<>) const {}
};

namespace Implementation {

template<class T> void safeDelete(T* data) {
    static_assert(sizeof(T) > 0, "Cannot delete pointer to incomplete type");
    delete data;
}

template<class T> ResourceManagerData<T>::~ResourceManagerData() {
    /* Loaders are already deleted via freeLoaders() from ResourceManager */
    safeDelete(_fallback);
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

    /* Insert the resource, if not already there */
    if(it == _data.end())
        it = _data.emplace(key, Data()).first;

    /* Otherwise delete previous data */
    else safeDelete(it->second.data);

    it->second.data = data;
    it->second.state = state;
    it->second.policy = policy;
    ++_lastChange;
}

template<class T> void ResourceManagerData<T>::setFallback(T* const data) {
    safeDelete(_fallback);
    _fallback = data;
    /* Notify resources also in this case, as some of them could go from empty
       to a fallback (or from a fallback to empty) */
    ++_lastChange;
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

template<class T> void ResourceManagerData<T>::freeLoader() {
    if(!_loader) return;

    _loader->manager = nullptr;
    delete _loader;
}

template<class T> void ResourceManagerData<T>::decrementReferenceCount(ResourceKey key) {
    auto it = _data.find(key);
    CORRADE_INTERNAL_ASSERT(it != _data.end());

    /* Free the resource if it is reference counted */
    if(--it->second.referenceCount == 0 && it->second.policy == ResourcePolicy::ReferenceCounted)
        _data.erase(it);
}

template<class T> struct ResourceManagerData<T>::Data {
    Data(): data(nullptr), state(ResourceDataState::Mutable), policy(ResourcePolicy::Manual), referenceCount(0) {}

    Data(const Data&) = delete;

    Data(Data&& other): data(other.data), state(other.state), policy(other.policy), referenceCount(other.referenceCount) {
        other.data = nullptr;
        other.referenceCount = 0;
    }

    ~Data();

    Data& operator=(const Data&) = delete;
    Data& operator=(Data&&) = delete;

    T* data;
    ResourceDataState state;
    ResourcePolicy policy;
    std::size_t referenceCount;
};

template<class T> inline ResourceManagerData<T>::Data::~Data() {
    CORRADE_ASSERT(referenceCount == 0,
        "ResourceManager: cleared/destroyed while data are still referenced", );
    safeDelete(data);
}

}

template<class ...Types> ResourceManager<Types...>::ResourceManager() = default;

template<class ...Types> ResourceManager<Types...>::~ResourceManager() {
    freeLoaders(typename Implementation::ResourceTypePack<Types...>{});
}

}

/* Make the definition complete */
#include "AbstractResourceLoader.h"

#endif
