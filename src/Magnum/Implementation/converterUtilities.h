#ifndef Magnum_Implementation_converterUtilities_h
#define Magnum_Implementation_converterUtilities_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022 Vladimír Vondruš <mosra@centrum.cz>

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

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/StaticArray.h>
#include <Corrade/PluginManager/AbstractPlugin.h>
#include <Corrade/Utility/ConfigurationGroup.h>

#include "Magnum/Magnum.h"

namespace Magnum { namespace Implementation {

/* Used only in executables where we don't want it to be exported */
namespace {

void setOptions(PluginManager::AbstractPlugin& plugin, const Containers::StringView anyPluginName, const Containers::StringView options) {
    for(const Containers::StringView option: options.splitWithoutEmptyParts(',')) {
        auto keyValue = option.partition('=');
        keyValue[0] = keyValue[0].trimmed();
        keyValue[2] = keyValue[2].trimmed();

        const Containers::Array<Containers::StringView> keyParts = keyValue[0].split('/');
        CORRADE_INTERNAL_ASSERT(!keyParts.isEmpty());
        Utility::ConfigurationGroup* group = &plugin.configuration();
        bool groupNotRecognized = false;
        for(std::size_t i = 0; i != keyParts.size() - 1; ++i) {
            Utility::ConfigurationGroup* subgroup = group->group(keyParts[i]);
            if(!subgroup) {
                groupNotRecognized = true;
                subgroup = group->addGroup(keyParts[i]);
            }
            group = subgroup;
        }

        /* Provide a warning message in case the plugin doesn't define given
           option in its default config. The plugin is not *required* to have
           those tho (could be backward compatibility entries, for example), so
           not an error.

           If it's an Any* plugin, then this check is provided by it directly,
           and since the Any* plugin obviously don't expose the options of the concrete plugins, this warning would fire for them always, which
           wouldn't help anything. */
        if((groupNotRecognized || !group->hasValue(keyParts.back())) && plugin.plugin() != anyPluginName) {
            Warning{} << "Option" << keyValue[0] << "not recognized by" << plugin.plugin();
        }

        /* If the option doesn't have an =, treat it as a boolean flag that's
           set to true. While there's no similar way to do an inverse, it's
           still nicer than causing a fatal error with those. */
        if(keyValue[1])
            group->setValue(keyParts.back(), keyValue[2]);
        else
            group->setValue(keyParts.back(), true);
    }
}

}

}}

#endif
