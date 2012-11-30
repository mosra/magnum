#ifndef Magnum_AbstractResourceLoader_h
#define Magnum_AbstractResourceLoader_h
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
 * @brief Class Magnum::AbstractResourceLoader
 */

#include "Magnum.h"

#include <string>

#include "ResourceManager.h"

namespace Magnum {

/**
@brief Base for resource loaders

Provides asynchronous resource loading for ResourceManager.
*/
template<class T> class AbstractResourceLoader {
    friend class Implementation::ResourceManagerData<T>;

    public:
        inline AbstractResourceLoader(): manager(nullptr), _requestedCount(0), _loadedCount(0), _notFoundCount(0) {}

        inline virtual ~AbstractResourceLoader() {
            if(manager) manager->_loader = nullptr;
        }

        /**
         * @brief Count of requested resources
         *
         * Count of resources requested by calling load().
         */
        inline std::size_t requestedCount() const { return _requestedCount; }

        /**
         * @brief Count of not found resources
         *
         * Count of resources requested by calling load(), but not found by
         * the loader.
         */
        inline std::size_t notFoundCount() const { return _notFoundCount; }

        /**
         * @brief Count of loaded resources
         *
         * Count of resources requested by calling load(), but not found by
         * the loader.
         */
        inline std::size_t loadedCount() const { return _loadedCount; }

        /**
         * @brief %Resource name corresponding to given key
         *
         * If no such resource exists or the resource name is not available,
         * returns empty string. Default implementation returns empty string.
         */
        virtual std::string name(ResourceKey key) const;

        /**
         * @brief Request resource to be loaded
         *
         * If the resource isn't yet loaded or loading, state of the resource
         * is set to @ref Resource::ResourceState "ResourceState::Loading" and count of
         * requested features is incremented.
         *
         * The resource might be loaded asynchronously and added to
         * ResourceManager when loading is done.
         * @see ResourceManager::state(), requestedCount(), notFountCount(),
         *      loadedCount()
         */
        virtual void load(ResourceKey key) = 0;

    protected:
        /**
         * @brief Set loaded resource to resource manager
         *
         * Also increments count of loaded resources. Parameter @p state
         * must be either @ref ResourceManager::ResourceDataState "ResourceDataState::Mutable"
         * or @ref ResourceManager::ResourceDataState "ResourceDataState::Final". See
         * ResourceManager::set() for more information.
         * @see loadedCount()
         */
        inline void set(ResourceKey key, T* data, ResourceDataState state, ResourcePolicy policy) {
            CORRADE_ASSERT(state == ResourceDataState::Mutable || state == ResourceDataState::Final,
                "AbstractResourceLoader::set(): state must be either Mutable or Final", );
            ++_loadedCount;
            manager->set(key, data, state, policy);
        }

        /**
         * @brief Mark resource as not found
         *
         * Also increments count of not found resources. See
         * ResourceManager::setNotFound() for more information.
         * @see notFountCount()
         */
        inline void setNotFound(ResourceKey key) {
            ++_notFoundCount;
            /** @todo What policy for notfound resources? */
            manager->set(key, nullptr, ResourceDataState::NotFound, ResourcePolicy::Resident);
        }

    private:
        Implementation::ResourceManagerData<T>* manager;
        std::size_t _requestedCount;
        std::size_t _loadedCount;
        std::size_t _notFoundCount;
};

template<class T> inline std::string AbstractResourceLoader<T>::name(ResourceKey) const { return {}; }

template<class T> inline void AbstractResourceLoader<T>::load(ResourceKey key) {
    ++_requestedCount;
    /** @todo What policy for loading resources? */
    manager->set(key, nullptr, ResourceDataState::Loading, ResourcePolicy::Resident);
}

}

#endif
