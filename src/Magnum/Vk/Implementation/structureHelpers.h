#ifndef Magnum_Vk_Implementation_structureHelpers_h
#define Magnum_Vk_Implementation_structureHelpers_h
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

#include <Corrade/Containers/Reference.h>
#include <Corrade/Utility/Assert.h>

#include "Magnum/Magnum.h"
#include "Magnum/Vk/Vulkan.h"

namespace Magnum { namespace Vk { namespace Implementation {

/* Meant to be used for connecting a longer chain of structures. Anything
   that was connected to the `next` pointer before is reconnected to
   `structure.pNext`; the `next` reference is rebound to the `structure.pNext`
   field connected so it can be passed to another structureConnect() again. */
template<class T> inline void structureConnect(Containers::Reference<void*>& next, T& structure, VkStructureType type) {
    void* const previousNext = next;
    *next = &structure;
    structure.sType = type;
    structure.pNext = previousNext;
    next = structure.pNext;
}

template<class T> inline void structureConnect(Containers::Reference<const void*>& next, T& structure, VkStructureType type) {
    /* There's no better way as the pNext are either const void* or void*
       and it's a mess. For example VkDeviceCreateInfo has const void* but it
       can point to VkPhysicalDeviceFeatures2 which then has void* as it's
       primarily an output structure. So we'll just drop all const-correctness
       and operate on void*. */
    structureConnect(reinterpret_cast<Containers::Reference<void*>&>(next), structure, type);
}

}}}

#endif
