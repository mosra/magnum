#ifndef Magnum_ResourceManager_h
#define Magnum_ResourceManager_h
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

/** @file
 * @brief Class Magnum::ResourceManager, enum Magnum::ResourceDataState, Magnum::ResourcePolicy
 */

#include <unordered_map>

#include "Resource.h"

namespace Magnum {

/** @relates ResourceManager
 * @brief %Resource data state
 *
 * @see ResourceManager::set()
 */
enum class ResourceDataState: std::uint8_t {
    /**
     * The resource can be changed by the manager in the future. This is
     * slower, as Resource needs to ask the manager for new version every time
     * the data are accessed, but allows changing the data for e.g. debugging
     * purposes.
     */
    Mutable = int(ResourceState::Mutable),

    /**
     * The resource cannot be changed by the manager in the future. This is
     * faster, as Resource instances will ask for the data only one time, thus
     * suitable for production code.
     */
    Final = int(ResourceState::Final)
};

/** @relates ResourceManager
@brief %Resource policy

@see ResourceManager::set(), ResourceManager::free()
 */
enum class ResourcePolicy: std::uint8_t {
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

#ifndef DOXYGEN_GENERATING_OUTPUT
namespace Implementation {
    struct ResourceKeyHash {
        inline std::size_t operator()(ResourceKey key) const {
            return *reinterpret_cast<const std::size_t*>(key.byteArray());
        }
    };

    template<class T> class ResourceManagerData {
        template<class, class> friend class Resource;

        ResourceManagerData(const ResourceManagerData<T>&) = delete;
        ResourceManagerData(ResourceManagerData<T>&&) = delete;
        ResourceManagerData<T>& operator=(const ResourceManagerData<T>&) = delete;
        ResourceManagerData<T>& operator=(ResourceManagerData<T>&&) = delete;

        public:
            inline virtual ~ResourceManagerData() {
                delete _fallback;
            }

            inline std::size_t lastChange() const { return _lastChange; }

            inline std::size_t count() const { return _data.size(); }

            std::size_t referenceCount(ResourceKey key) const {
                auto it = _data.find(key);
                if(it == _data.end()) return 0;
                return it->second.referenceCount;
            }

            ResourceState state(ResourceKey key) const {
                auto it = _data.find(key);
                if(it == _data.end() || !it->second.data)
                    return _fallback ? ResourceState::Fallback : ResourceState::NotLoaded;
                else
                    return static_cast<ResourceState>(it->second.state);
            }

            template<class U> inline Resource<T, U> get(ResourceKey key) {
                return Resource<T, U>(this, key);
            }

            void set(ResourceKey key, T* data, ResourceDataState state, ResourcePolicy policy) {
                auto it = _data.find(key);

                /* Cannot change resource with already final state */
                CORRADE_ASSERT(it == _data.end() || it->second.state != ResourceDataState::Final, "ResourceManager: cannot change already final resource", );

                /* If nothing is referencing reference-counted resource, we're done */
                if(policy == ResourcePolicy::ReferenceCounted && (it == _data.end() || it->second.referenceCount == 0)) {
                    Corrade::Utility::Warning() << "ResourceManager: Reference-counted resource with key" << key << "isn't referenced from anywhere, deleting it immediately";
                    delete data;

                    /* Delete also already present resource (it could be here
                       because previous policy could be other than
                       ReferenceCounted) */
                    if(it != _data.end()) _data.erase(it);

                    return;

                /* Insert it, if not already here */
                } else if(it == _data.end())
                    it = _data.insert(std::make_pair(key, Data())).first;

                /* Replace previous data */
                delete it->second.data;
                it->second.data = data;
                it->second.state = state;
                it->second.policy = policy;
                ++_lastChange;
            }

            inline T* fallback() { return _fallback; }
            inline const T* fallback() const { return _fallback; }

            inline void setFallback(T* data) {
                delete _fallback;
                _fallback = data;
            }

            void free() {
                /* Delete all non-referenced non-resident resources */
                for(auto it = _data.begin(); it != _data.end(); ) {
                    if(it->second.policy != ResourcePolicy::Resident && !it->second.referenceCount)
                        it = _data.erase(it);
                    else ++it;
                }
            }

        protected:
            inline ResourceManagerData(): _fallback(nullptr), _lastChange(0) {}

        private:
            struct Data {
                Data(const Data&) = delete;
                Data& operator=(const Data&) = delete;
                Data& operator=(Data&&) = delete;

                inline Data(): data(nullptr), state(ResourceDataState::Mutable), policy(ResourcePolicy::Manual), referenceCount(0) {}

                inline Data(Data&& other): data(other.data), state(other.state), policy(other.policy), referenceCount(other.referenceCount) {
                    other.data = nullptr;
                    other.referenceCount = 0;
                }

                inline ~Data() {
                    CORRADE_ASSERT(referenceCount == 0, "ResourceManager: cannot destruct it while data are still referenced", );
                    delete data;
                }

                T* data;
                ResourceDataState state;
                ResourcePolicy policy;
                std::size_t referenceCount;
            };

            inline const Data& data(ResourceKey key) {
                return _data[key];
            }

            inline void incrementReferenceCount(ResourceKey key) {
                ++_data[key].referenceCount;
            }

            inline void decrementReferenceCount(ResourceKey key) {
                auto it = _data.find(key);

                /* Free the resource if it is reference counted */
                if(--it->second.referenceCount == 0 && it->second.policy == ResourcePolicy::ReferenceCounted)
                    _data.erase(it);
            }

            std::unordered_map<ResourceKey, Data, ResourceKeyHash> _data;
            T* _fallback;
            std::size_t _lastChange;
    };
}
#endif

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
*/
/* Due to too much work involved with explicit template instantiation (all
   Resource combinations, all ResourceManagerData...), this class doesn't have
   template implementation file. */
template<class... Types> class ResourceManager: private Implementation::ResourceManagerData<Types>... {
    public:
        /** @brief Global instance */
        inline static ResourceManager<Types...>* instance() {
            CORRADE_ASSERT(internalInstance(), "ResourceManager::instance(): no instance exists", nullptr);
            return internalInstance();
        }

        /**
         * @brief Constructor
         *
         * Sets global instance pointer to itself.
         * @attention Only one instance of given ResourceManager type can be
         *      created.
         * @see instance()
         */
        inline ResourceManager() {
            CORRADE_ASSERT(!internalInstance(), "ResourceManager::ResourceManager(): another instance is already created", );
            internalInstance() = this;
        }

        /**
         * @brief Destructor
         *
         * Sets global instance pointer to `nullptr`.
         * @see instance()
         */
        inline ~ResourceManager() {
            CORRADE_INTERNAL_ASSERT(internalInstance() == this);
            internalInstance() = nullptr;
        }

        /** @brief Count of resources of given type */
        template<class T> inline std::size_t count() {
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
        template<class T, class U = T> inline Resource<T, U> get(ResourceKey key) {
            return this->Implementation::ResourceManagerData<T>::template get<U>(key);
        }

        /**
         * @brief Reference count of given resource
         *
         * @see set()
         */
        template<class T> inline std::size_t referenceCount(ResourceKey key) const {
            return this->Implementation::ResourceManagerData<T>::referenceCount(key);
        }

        /**
         * @brief %Resource state
         *
         * @see set(), Resource::state()
         */
        template<class T> inline ResourceState state(ResourceKey key) const {
            return this->Implementation::ResourceManagerData<T>::state(key);
        }

        /**
         * @brief Set resource data
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
         * subsequent updates are not possible.
         * @see referenceCount(), state()
         */
        template<class T> inline void set(ResourceKey key, T* data, ResourceDataState state, ResourcePolicy policy) {
            this->Implementation::ResourceManagerData<T>::set(key, data, state, policy);
        }

        /** @brief Fallback for not found resources */
        template<class T> inline T* fallback() {
            return this->Implementation::ResourceManagerData<T>::fallback();
        }

        /** @overload */
        template<class T> inline const T* fallback() const {
            return this->Implementation::ResourceManagerData<T>::fallback();
        }

        /** @brief Set fallback for not found resources */
        template<class T> inline void setFallback(T* data) {
            return this->Implementation::ResourceManagerData<T>::setFallback(data);
        }

        /** @brief Free all resources of given type which are not referenced */
        template<class T> inline void free() {
            return this->Implementation::ResourceManagerData<T>::free();
        }

        /** @brief Free all resources which are not referenced */
        inline void free() {
            freeInternal(std::common_type<Types>()...);
        }

    private:
        template<class FirstType, class ...NextTypes> inline void freeInternal(std::common_type<FirstType>, std::common_type<NextTypes>... t) {
            free<FirstType>();
            freeInternal(t...);
        }
        inline void freeInternal() const {}

        static ResourceManager<Types...>*& internalInstance();
};

#ifndef MAGNUM_RESOURCEMANAGER_DONT_DEFINE_INTERNALINSTANCE
template<class ...Types> ResourceManager<Types...>*& ResourceManager<Types...>::internalInstance() {
    static ResourceManager<Types...>* _instance(nullptr);
    return _instance;
}
#endif

}


#endif
