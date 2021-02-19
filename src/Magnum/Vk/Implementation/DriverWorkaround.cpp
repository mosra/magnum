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

#include <Corrade/Containers/GrowableArray.h>
#include <Corrade/Containers/StringView.h>
#include <Corrade/Utility/Debug.h>

#include "Magnum/Magnum.h"
#include "Magnum/Vk/Implementation/DriverWorkaround.h"

namespace Magnum { namespace Vk { namespace Implementation {

namespace {

using namespace Containers::Literals;

/* Search the code for the following strings to see where they are implemented */
constexpr Containers::StringView KnownWorkarounds[]{
/* [workarounds] */
/* For layered image copies, SwiftShader (5.0? the version reporting is messy)
   expects the layer offsets/counts to be included as second/third dimension of
   the image offset/extent instead. Actually, having the Vulkan API contain
   just 3D offset and extent with no layer offset/count would make more sense
   to me as well -- the last dimension can be either in the offset/extent or
   layer offset/count, but never in both, so the extra fields feel redundant.
   Or maybe it's reserving space for layered 3D images? */
"swiftshader-image-copy-extent-instead-of-layers"_s,

/* Multi-entrypoint SPIR-V modules that use the same location indices for
   vertex outputs and fragment outputs (for example passing interpolated vertex
   color through location 0 and having fragment output at location 0 as well)
   will cause the fragment output to be always zero. Happens only when such a
   multi-entrypoint SPIR-V module is used for the vertex shader, doesn't happen
   with single-entrypoint modules. The fix is remapping the vertex/fragment
   interface to not use the same location IDs as the fragment output. That
   however causes SwiftShader to complain about zero format in the now-unused
   location 0 such as

    SwiftShader/src/Vulkan/VkFormat.cpp:1351 WARNING: UNSUPPORTED: Format: 0
    SwiftShader/src/Pipeline/VertexRoutine.cpp:494 WARNING: UNSUPPORTED: stream.format 0

   but apart from this noise everything works as expected. */
"swiftshader-spirv-multi-entrypoint-conflicting-locations"_s,
/* [workarounds] */
};

/* I could use std::find(), right? Well, it'd be a whole lot more typing and
   an #include <algorithm> *and* #include <iterator> or whatever as well,
   because apparently ONE CAN'T GET std::begin() / std::end() without including
   tens thousands lines of irrelevant shit, FFS.

   Also the comparison to array end to discover if it wasn't found is just a
   useless verbose crap shit as well, so we'll do better here and return a null
   view instead.

   Moreover, based on the experience with GL, I don't expect there being too
   many workarounds used heavily (10 at most, maybe?) so I won't bother with
   some binary search, which needs extra testing effort. */
Containers::StringView findWorkaround(Containers::StringView workaround) {
    for(Containers::StringView i: KnownWorkarounds)
        if(workaround == i) return i;
    return {};
}

}

void disableWorkaround(Containers::Array<std::pair<Containers::StringView, bool>>& encounteredWorkarounds, const Containers::StringView workaround) {
    /* Find the workaround. Note that we'll add the found view to the array
       and not the passed view, as the found view is guaranteed to stay in
       scope */
    Containers::StringView found = findWorkaround(workaround);

    /* Ignore unknown workarounds */
    /** @todo this will probably cause false positives when both GL and Vulkan
        is used together? */
    if(found.isEmpty()) {
        Warning{} << "Vk: unknown workaround" << workaround;
        return;
    }

    arrayAppend(encounteredWorkarounds, Containers::InPlaceInit, found, true);
}

Containers::Array<std::pair<Containers::StringView, bool>> disableAllWorkarounds() {
    Containers::Array<std::pair<Containers::StringView, bool>> encounteredWorkarounds;
    for(Containers::StringView i: KnownWorkarounds)
        arrayAppend(encounteredWorkarounds, Containers::InPlaceInit, i, true);
    return encounteredWorkarounds;
}

bool isDriverWorkaroundDisabled(Containers::Array<std::pair<Containers::StringView, bool>>& encounteredWorkarounds, const Containers::StringView workaround) {
    /* Find the workaround. Note that we'll add the found view to the array
       and not the passed view, as the found view is guaranteed to stay in
       scope */
    Containers::StringView found = findWorkaround(workaround);
    CORRADE_INTERNAL_ASSERT(!found.isEmpty());

    /* If the workaround was already asked for or disabled, return its state,
       otherwise add it to the list as used one. Here we again cheat a bit and
       compare just data pointers instead of the whole string as we store only
       the views in the KnownWorkarounds list. */
    for(const auto& i: encounteredWorkarounds)
        if(i.first.data() == found.data()) return i.second;
    arrayAppend(encounteredWorkarounds, Containers::InPlaceInit, found, false);
    return false;
}

}}}
