#ifndef Magnum_ResourceManager_hpp
#define Magnum_ResourceManager_hpp
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

#include "ResourceManager.h"

/*
    File-local definition of ResourceManager instance holder for use in cases
    where the class is used across library boundaries, in which case additional
    care must be done to ensure a single static instance.

    Usage: typedef the resource manager with Implementation::ResourceManagerLocalInstance
    as a first type and then include this file in a _single_ *.cpp file.

    This symbol is always exported.
*/

namespace Magnum { namespace Implementation {

template<class ...Types>
#ifndef _MSC_VER
CORRADE_VISIBILITY_EXPORT
#endif
ResourceManager<Types...>*& ResourceManagerLocalInstanceImplementation<Types...>::internalInstance() {
    static ResourceManager<Types...>* _instance(nullptr);
    return _instance;
}

}}

#endif
