#ifndef Magnum_Vk_Implementation_structureHelpers_h
#define Magnum_Vk_Implementation_structureHelpers_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021 Vladimír Vondruš <mosra@centrum.cz>

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
#include <Corrade/Utility/TypeTraits.h>

#include "Magnum/Magnum.h"
#include "Magnum/Vk/Vulkan.h"

namespace Magnum { namespace Vk { namespace Implementation {

/* All those helpers are designed in a way that only allows them to work with a
   "whitelisted" set of structures to avoid modifying external data by
   accident. Thus no "give me the first structure of this type" or "remove any
   structure of this type from the chain". */

/* Connecting one structure to a pNext chain. Anything that was connected to
   the `next` pointer before is reconnected to `structure.pNext` */
template<class T> inline void structureConnectOne(void*& next, T& structure, VkStructureType type) {
    void* const previousNext = next;
    next = &structure;
    structure.sType = type;
    structure.pNext = previousNext;
}

template<class T> inline void structureConnectOne(const void*& next, T& structure, VkStructureType type) {
    /* There's no better way as the pNext are either const void* or void*
       and it's a mess. For example VkDeviceCreateInfo has const void* but it
       can point to VkPhysicalDeviceFeatures2 which then has void* as it's
       primarily an output structure. So we'll just drop all const-correctness
       and operate on void*. */
    structureConnectOne(const_cast<void*&>(next), structure, type);
}

/* Meant to be used for connecting a longer chain of structures. Anything
   that was connected to the `next` pointer before is reconnected to
   `structure.pNext`; the `next` reference is rebound to the `structure.pNext`
   field connected so it can be passed to another structureConnect() again. */
template<class T> inline void structureConnect(Containers::Reference<void*>& next, T& structure, VkStructureType type) {
    structureConnectOne(*next, structure, type);
    next = structure.pNext;
}

template<class T> inline void structureConnect(Containers::Reference<const void*>& next, T& structure, VkStructureType type) {
    /* Same reasoning as in structureConnectOne(), we just drop all
       const-correctness and operate on void*. */
    structureConnect(reinterpret_cast<Containers::Reference<void*>&>(next), structure, type);
}

CORRADE_HAS_TYPE(IsVulkanStructure, decltype(T::pNext));

/* Returns a pointer to the pNext field that has a value of `pointer` or
   nullptr if no such structure if found. It can also return the `next`
   parameter, if its value is already the pointer. */
template<class T> inline void** structureFind(void*& next, const T& structure) {
    static_assert(IsVulkanStructure<T>::value, "passed type is not a Vulkan structure");

    /* We take a reference in order to make it possible to mutate the found
       result, however we need to rebind that reference in the loop, and
       assigning to `next` would mean we corrupt the original structure. */
    Containers::Reference<void*> nextRebindable = next;
    while(nextRebindable) {
        auto& found = *reinterpret_cast<VkBaseOutStructure*>(*nextRebindable);
        if(&found == static_cast<const void*>(&structure)) return &*nextRebindable;
        nextRebindable = reinterpret_cast<void*&>(found.pNext);
    }

    return nullptr;
}

template<class T> inline const void** structureFind(const void*& next, const T& structure) {
    /* Ugly. Yes. Same reasoning as in structureConnect(). */
    return const_cast<const void**>(structureFind(const_cast<void*&>(next), structure));
}

class AnyStructure {
    public:
        template<class T, class = typename std::enable_if<IsVulkanStructure<T>::value>::type> /*implicit*/ AnyStructure(const T& structure): _structure{reinterpret_cast<const VkBaseOutStructure*>(&structure)} {}

        /*implicit*/ operator const VkBaseOutStructure&() const { return *_structure; }

    private:
        const VkBaseOutStructure* _structure;
};

/* Given a chain of structures that are always connected in the same order (but
   not necessarily all of them), in which `next` points to the first structure
   of the chain, the function disconnects the chain from `next`, replacing the
   pointer with the first `pNext` value that points outside of the chain. The
   `structures` themselves are not touched in any way.

    Given the following, where 0-9 are the `structures` and 1, 3, 4, 5, 7, 8
    form a chain in which the last `pNext` points to `out`:

    next   ___         ___      out
        \ /   \       /   \    /
      +  +  +  +--+--+  +  +--+  +
      0  1  2  3  4  5  6  7  8  9

    The result is the following --- the structures are left intact, and `next`
    now points directly to `out`. After that, the structures can be cleared and
    repurposed or discarded without a risk of losing access to the chain end.

    next ----------------------- out
           ___         ___
          /   \       /   \    /
      +  +  +  +--+--+  +  +--+  +
      0  1  2  3  4  5  6  7  8  9s

*/
inline void structureDisconnectChain(const void*& next, std::initializer_list<AnyStructure> structures) {
    /* We take a reference in order to make it possible to mutate the found
       result, however we need to rebind that reference in the loop, and
       assigning to `next` would mean we corrupt the original structure. */
    Containers::Reference<const void* const> nextRebindable = next;

    for(const VkBaseOutStructure& structure: structures) {
        if(nextRebindable == &structure)
            nextRebindable = reinterpret_cast<const void* const&>(structure.pNext);
    }

    /* For safety we expect `next` actually pointed to something inside the
       chain */
    CORRADE_INTERNAL_ASSERT(&*nextRebindable != next);
    next = *nextRebindable;
}

inline void structureDisconnectChain(void*& next, std::initializer_list<AnyStructure> structures) {
    return structureDisconnectChain(const_cast<const void*&>(next), structures);
}

}}}

#endif
