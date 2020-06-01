#ifndef Magnum_Resource_h
#define Magnum_Resource_h
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

/** @file
 * @brief Class @ref Magnum::ResourceKey, @ref Magnum::Resource, enum @ref Magnum::ResourceState
 */

#include <Corrade/Utility/Assert.h>
#include <Corrade/Utility/MurmurHash2.h>

/* This is needed because ResourceKey is implicitly convertible from a
   std::string -- and if DebugStl.h wouldn't be included, an attempt to print a
   std::string would choose the ResourceKey printer instead, producing unwanted
   results. */
/** @todo remove when we have a StringView and ResourceKey is ported to it */
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/Magnum.h"
#include "Magnum/visibility.h"

namespace Magnum {

/**
@brief Resource state

@see @ref Resource::state(), @ref ResourceManager::state()
*/
enum class ResourceState: UnsignedByte {
    /** The resource is not yet loaded (and no fallback is available). */
    NotLoaded,

    /** The resource is not yet loaded and fallback resource is used instead. */
    NotLoadedFallback,

    /** The resource is currently loading (and no fallback is available). */
    Loading,

    /** The resource is currently loading and fallback resource is used instead. */
    LoadingFallback,

    /** The resource was not found (and no fallback is available). */
    NotFound,

    /** The resource was not found and fallback resource is used instead. */
    NotFoundFallback,

    /** The resource is loaded, but can be changed by the manager at any time. */
    Mutable,

    /** The resource is loaded and won't be changed by the manager anymore. */
    Final
};

/** @debugoperatorenum{ResourceState} */
MAGNUM_EXPORT Debug& operator<<(Debug& debug, ResourceState value);

/**
@brief Key for accessing resource

See @ref ResourceManager for more information.
*/
class ResourceKey: public Utility::MurmurHash2::Digest {
    public:
        /**
         * @brief Default constructor
         *
         * Creates zero key. Note that it is not the same as calling other
         * constructors with empty string.
         */
        constexpr /*implicit*/ ResourceKey() {}

        /** @brief Construct resource key directly from hashed value */
        explicit ResourceKey(std::size_t key): Utility::MurmurHash2::Digest{Utility::MurmurHash2::Digest::fromByteArray(reinterpret_cast<const char*>(&key))} {}

        /** @brief Constructor */
        /*implicit*/ ResourceKey(const std::string& key): Utility::MurmurHash2::Digest(Utility::MurmurHash2()(key)) {}

        /**
         * @brief Constructor
         * @todo constexpr
         */
        template<std::size_t size> constexpr ResourceKey(const char(&key)[size]): Utility::MurmurHash2::Digest(Utility::MurmurHash2()(key)) {}
};

/** @debugoperator{ResourceKey} */
MAGNUM_EXPORT Debug& operator<<(Debug& debug, const ResourceKey& value);

namespace Implementation {
    template<class> class ResourceManagerData;
}

/**
@brief Resource reference

See @ref ResourceManager for more information.
*/
#ifdef DOXYGEN_GENERATING_OUTPUT
template<class T, class U = T>
#else
template<class T, class U>
#endif
class Resource {
    public:
        /**
         * @brief Default constructor
         *
         * Creates empty resource. Resources are acquired from the manager by
         * calling @ref ResourceManager::get().
         */
        explicit Resource(): _manager{nullptr}, _lastCheck{0}, _state{ResourceState::Final}, _data{nullptr} {}

        /** @brief Copy constructor */
        Resource(const Resource<T, U>& other): _manager{other._manager}, _key{other._key}, _lastCheck{other._lastCheck}, _state{other._state}, _data{other._data} {
            if(_manager) _manager->incrementReferenceCount(_key);
        }

        /** @brief Move constructor */
        Resource(Resource<T, U>&& other) noexcept;

        /** @brief Destructor */
        ~Resource() {
            if(_manager) _manager->decrementReferenceCount(_key);
        }

        /** @brief Copy assignment */
        Resource<T, U>& operator=(const Resource<T, U>& other);

        /** @brief Move assignment */
        Resource<T, U>& operator=(Resource<T, U>&& other) noexcept;

        /** @brief Equality comparison */
        bool operator==(const Resource<T, U>& other) const {
            return _manager == other._manager && _key == other._key;
        }

        /** @brief Equality comparison with other types is explicitly disallowed */
        template<class V, class W> bool operator==(const Resource<V, W>&) const = delete;

        /** @brief Non-equality comparison */
        bool operator!=(const Resource<T, U>& other) const {
            return !operator==(other);
        }

        /** @brief Non-equality comparison with other types is explicitly disallowed */
        template<class V, class W> bool operator!=(const Resource<V, W>&) const = delete;

        /** @brief Resource key */
        ResourceKey key() const { return _key; }

        /**
         * @brief Resource state
         *
         * @see @ref operator bool(), @ref ResourceManager::state()
         */
        ResourceState state() {
            acquire();
            return _state;
        }

        /**
         * @brief Whether the resource is available
         *
         * Returns @cpp false @ce when resource is not loaded and no fallback
         * is available (i.e. @ref state() is either
         * @ref ResourceState::NotLoaded, @ref ResourceState::Loading or
         * @ref ResourceState::NotFound), @cpp true @ce otherwise.
         */
        operator bool() {
            acquire();
            return _data;
        }

        /**
         * @brief Pointer to resource data
         *
         * Returns @cpp nullptr @ce if the resource is not loaded.
         */
        operator U*() {
            acquire();
            return static_cast<U*>(_data);
        }

        /**
         * @brief Reference to resource data
         *
         * The resource must be loaded before accessing it. Use boolean
         * conversion operator or @ref state() for testing whether it is
         * loaded.
         */
        U& operator*() {
            acquire();
            CORRADE_ASSERT(_data, "Resource: accessing not loaded data with key" << _key, *static_cast<U*>(_data));
            return *static_cast<U*>(_data);
        }

        /**
         * @brief Access to resource data
         *
         * The resource must be loaded before accessing it. Use boolean
         * conversion operator or @ref state() for testing whether it is
         * loaded.
         */
        U* operator->() {
            acquire();
            CORRADE_ASSERT(_data, "Resource: accessing not loaded data with key" << _key, nullptr);
            return static_cast<U*>(_data);
        }

    private:
        #ifndef DOXYGEN_GENERATING_OUTPUT /* https://bugzilla.gnome.org/show_bug.cgi?id=776986 */
        friend Implementation::ResourceManagerData<T>;
        #endif

        Resource(Implementation::ResourceManagerData<T>* manager, ResourceKey key): _manager{manager}, _key{key}, _lastCheck{0}, _state{ResourceState::NotLoaded}, _data{nullptr} {
            manager->incrementReferenceCount(key);
        }

        void acquire();

        Implementation::ResourceManagerData<T>* _manager;
        ResourceKey _key;
        std::size_t _lastCheck;
        ResourceState _state;
        T* _data;
};

template<class T, class U> Resource<T, U>& Resource<T, U>::operator=(const Resource<T, U>& other) {
    if(_manager) _manager->decrementReferenceCount(_key);

    _manager = other._manager;
    _key = other._key;
    _lastCheck = other._lastCheck;
    _state = other._state;
    _data = other._data;

    if(_manager) _manager->incrementReferenceCount(_key);
    return *this;
}

template<class T, class U> Resource<T, U>::Resource(Resource<T, U>&& other) noexcept: _manager(other._manager), _key(other._key), _lastCheck(other._lastCheck), _state(other._state), _data(other._data) {
    other._manager = nullptr;
    other._key = {};
    other._lastCheck = 0;
    other._state = ResourceState::Final;
    other._data = nullptr;
}

template<class T, class U> Resource<T, U>& Resource<T, U>::operator=(Resource<T, U>&& other) noexcept {
    using std::swap;
    swap(_manager, other._manager);
    swap(_key, other._key);
    swap(_lastCheck, other._lastCheck);
    swap(_state, other._state);
    swap(_data, other._data);
    return *this;
}

template<class T, class U> void Resource<T, U>::acquire() {
    /* The data are already final, nothing to do */
    if(_state == ResourceState::Final) return;

    /* Nothing changed since last check */
    if(_manager->lastChange() <= _lastCheck) return;

    /* Acquire new data and save last check time */
    const typename Implementation::ResourceManagerData<T>::Data& d = _manager->data(_key);
    _lastCheck = _manager->lastChange();

    /* Try to get the data */
    _data = d.data;
    _state = static_cast<ResourceState>(d.state);

    /* Data are not available */
    if(!_data) {
        /* Fallback found, add *Fallback to state */
        if((_data = _manager->fallback())) {
            if(_state == ResourceState::Loading)
                _state = ResourceState::LoadingFallback;
            else if(_state == ResourceState::NotFound)
                _state = ResourceState::NotFoundFallback;
            else _state = ResourceState::NotLoadedFallback;

        /* Fallback not found and loading didn't start yet */
        } else if(_state != ResourceState::Loading && _state != ResourceState::NotFound)
            _state = ResourceState::NotLoaded;
    }
}

}

namespace std {
    /** @brief `std::hash` specialization for @ref Magnum::ResourceKey */
    template<> struct hash<Magnum::ResourceKey> {
        #ifndef DOXYGEN_GENERATING_OUTPUT
        std::size_t operator()(Magnum::ResourceKey key) const {
            return *reinterpret_cast<const std::size_t*>(key.byteArray());
        }
        #endif
    };
}

/* Make the definition complete */
#include "ResourceManager.h"

#endif
