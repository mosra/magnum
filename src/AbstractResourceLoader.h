#ifndef Magnum_AbstractResourceLoader_h
#define Magnum_AbstractResourceLoader_h
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

/** @file
 * @brief Class Magnum::AbstractResourceLoader
 */

#include <string>

#include "ResourceManager.h"

namespace Magnum {

/**
@brief Base for resource loaders

Provides (a)synchronous resource loading for ResourceManager.

@section AbstractResourceLoader-usage Usage and subclassing

Usage is done by subclassing. Subclass instances can be added to
ResourceManager using ResourceManager::setLoader(). After adding the loader,
each call to ResourceManager::get() will call load() implementation unless the
resource is already loaded (or loading is in progress). Note that resources
requested before the loader was added are not be affected by the loader.

Subclassing is done by implementing at least load() function. The loading can
be done synchronously or asynchronously (i.e., in another thread). The base
implementation provides interface to ResourceManager and manages loading
progress (which is then available through functions requestedCount(),
loadedCount() and notFoundCount()). You shouldn't access the ResourceManager
directly when loading the data.

Your load() implementation must call the base implementation at the beginning
so ResourceManager is informed about loading state. Then, after your resources
are loaded, call set() to pass them to ResourceManager or call setNotFound()
to indicate that the resource was not found.

You can also implement name() to provide meaningful names for resource keys.

Example implementation for synchronous mesh loader:
@code
class MeshResourceLoader: public AbstractResourceLoader<Mesh> {
    public:
        void load(ResourceKey key) {
            // Indicate that loading has begun
            AbstractResourceLoader<Mesh>::load(key);

            // Load the mesh...

            // Not found
            if(!found) {
                setNotFound(key);
                return;
            }

            // Found, pass it to resource manager
            set(key, mesh, state, policy);
        }
};
@endcode

You can then add it to resource manager instance like this:
@code
MyResourceManager manager;
MeshResourceLoader loader;

manager->setLoader(loader);

// This will now automatically request the mesh from loader by calling load()
Resource<Mesh> myMesh = manager->get<Mesh>("my-mesh");
@endcode
*/
template<class T> class AbstractResourceLoader {
    friend class Implementation::ResourceManagerData<T>;

    public:
        inline explicit AbstractResourceLoader(): manager(nullptr), _requestedCount(0), _loadedCount(0), _notFoundCount(0) {}

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
         * requested features is incremented. Depending on implementation the
         * resource might be loaded synchronously or asynchronously.
         *
         * See class documentation for reimplementation guide.
         *
         * @see ResourceManager::state(), requestedCount(), notFoundCount(),
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
