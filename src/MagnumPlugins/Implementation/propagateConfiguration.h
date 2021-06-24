#ifndef Magnum_Implementation_propagateConfiguration_h
#define Magnum_Implementation_propagateConfiguration_h
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

#include <Corrade/Containers/Pair.h>
#include <Corrade/Containers/Reference.h>
#include <Corrade/Utility/ConfigurationGroup.h>

#include "Magnum/Magnum.h"

/* Used by Any* plugins to propagate configuration to the concrete
   implementation. Assumes that the Any* plugin itself doesn't have any
   configuration options and so propagates all groups and values that were
   set, emitting a warning if the target doesn't have such option in its
   default configuration. */

namespace Magnum { namespace Implementation {

/* Used only in plugins where we don't want it to be exported */
namespace {

void propagateConfiguration(const char* warningPrefix, const Containers::String& groupPrefix, const Containers::StringView plugin, const Utility::ConfigurationGroup& src, Utility::ConfigurationGroup& dst) {
    using namespace Containers::Literals;

    /* Propagate values */
    for(Containers::Pair<Containers::StringView, Containers::StringView> value: src.values()) {
        if(!dst.hasValue(value.first())) {
            Warning{} << warningPrefix << "option" << "/"_s.joinWithoutEmptyParts({groupPrefix, value.first()}) << "not recognized by" << plugin;
        }

        dst.setValue(value.first(), value.second());
    }

    /* Recursively propagate groups */
    for(Containers::Pair<Containers::StringView, Containers::Reference<const Utility::ConfigurationGroup>> group: src.groups()) {
        Utility::ConfigurationGroup* dstGroup = dst.group(group.first());
        if(!dstGroup) dstGroup = dst.addGroup(group.first());
        propagateConfiguration(warningPrefix, "/"_s.joinWithoutEmptyParts({groupPrefix, group.first()}), plugin, group.second(), *dstGroup);
    }
}

}

}}

#endif
