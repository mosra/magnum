#ifndef Magnum_Implementation_converterUtilities_h
#define Magnum_Implementation_converterUtilities_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
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

#include <unordered_set>
#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/StaticArray.h>
#include <Corrade/PluginManager/AbstractPlugin.h>
#include <Corrade/Utility/ConfigurationGroup.h>

#include "Magnum/Magnum.h"

namespace Magnum { namespace Implementation {

/* Used only in executables where we don't want it to be exported */
namespace {

void setOptions(const Containers::StringView pluginName, Utility::ConfigurationGroup& configuration, const Containers::StringView anyPluginName, const Containers::StringView options) {
    std::unordered_set<const Utility::ConfigurationGroup*> emptySubgroups;
    for(const Containers::StringView option: options.splitWithoutEmptyParts(',')) {
        auto keyValue = option.partition('=');
        keyValue[0] = keyValue[0].trimmed();
        keyValue[2] = keyValue[2].trimmed();

        bool addValue = false;
        if(keyValue[0].hasPrefix('+')) {
            keyValue[0] = keyValue[0].exceptPrefix(1);
            addValue = true;
        }

        const Containers::Array<Containers::StringView> keyParts = keyValue[0].split('/');
        CORRADE_INTERNAL_ASSERT(!keyParts.isEmpty());
        Utility::ConfigurationGroup* group = &configuration;
        bool groupNotRecognized = false;
        for(std::size_t i = 0; i != keyParts.size() - 1; ++i) {
            Utility::ConfigurationGroup* subgroup = group->group(keyParts[i]);
            if(!subgroup) {
                groupNotRecognized = true;
                subgroup = group->addGroup(keyParts[i]);
            /* For existing subgroups (i.e., not the root configuration)
               remember if the group was initially empty (no subgroups, no
               values; comments can be there). For those we won't warn about
               unrecognized options below as it's a common use case (for
               example GltfImporter's customSceneFieldTypes). Has to be done
               upfront in case more than one option is added to the same group
               -- otherwise adding the second would warn again, as the group
               is no longer empty at that point. */
            } else if(!subgroup->hasGroups() && !subgroup->hasValues()) {
                emptySubgroups.insert(subgroup);
            }
            group = subgroup;
        }

        /* Provide a warning message in case the plugin doesn't define given
           option in its default config. The plugin is not *required* to have
           those tho (could be backward compatibility entries, for example), so
           not an error.

           If it's an Any* plugin, then this check is provided by it directly,
           and since the Any* plugin obviously don't expose the options of the
           concrete plugins, this warning would fire for them always, which
           wouldn't help anything. */
        if((groupNotRecognized || (!group->hasValue(keyParts.back()) &&
                /* The warning isn't printed in case a value is added into an
                   empty subgroup, see above */
                emptySubgroups.find(group) == emptySubgroups.end() &&
                /* The warning also isn't printed in case a new value is added
                   with `+` instead of modifying an existing one -- e.g. a
                   plugin can support 0 to n values of a certain key, which
                   means by default there won't be any */
                !addValue
            )) && pluginName != anyPluginName)
        {
            Warning{} << "Option" << keyValue[0] << "not recognized by" << pluginName;
        }

        /* If the option doesn't have an =, treat it as a boolean flag that's
           set to true. While there's no similar way to do an inverse, it's
           still nicer than causing a fatal error with those. */
        if(keyValue[1]) {
            if(addValue)
                group->addValue(keyParts.back(), keyValue[2]);
            else
                group->setValue(keyParts.back(), keyValue[2]);
        } else {
            if(addValue)
                group->addValue(keyParts.back(), true);
            else
                group->setValue(keyParts.back(), true);
        }
    }
}

void setOptions(PluginManager::AbstractPlugin& plugin, const Containers::StringView anyPluginName, const Containers::StringView options) {
    setOptions(plugin.plugin(), plugin.configuration(), anyPluginName, options);
}

}

}}

#endif
