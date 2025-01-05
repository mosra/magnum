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

#include <sstream> /** @todo remove once Configuration is stream-free */
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/String.h>
#include <Corrade/Utility/Configuration.h>
#include <Corrade/Utility/DebugStl.h> /** @todo remove once Configuration is stream-free */

#include "Magnum/Implementation/converterUtilities.h"

namespace Magnum { namespace Test { namespace {

struct ConverterUtilitiesTest: TestSuite::Tester {
    explicit ConverterUtilitiesTest();

    void setOptions();
};

const struct {
    TestSuite::TestCaseDescriptionSourceLocation name;
    const char* config;
    const char* options;
    const char* anyPluginName;
    const char* expectedConfig;
    const char* expectedWarning;
} SetOptionsData[]{
    {"", R"([configuration]
option=
another=
)",
        "option=value", "AnyPlugin", R"([configuration]
option=value
another=
)", nullptr},
    {"two options", R"([configuration]
option=
some=
another=
)",
        "option=value,another=yes", "AnyPlugin", R"([configuration]
option=value
some=
another=yes
)", nullptr},
    {"implicit true", R"([configuration]
option=
another=
)",
        "option=value,another", "AnyPlugin", R"([configuration]
option=value
another=true
)", nullptr},
    {"group", R"([configuration]
option=
[configuration/group]
option=
another=
)",
        "group/option=value", "AnyPlugin", R"([configuration]
option=
[configuration/group]
option=value
another=
)", nullptr},
    {"nested group + root option after", R"([configuration]
option=
another=
[configuration/group]
option=
[configuration/group/nested]
option=
another=
)",
        "group/nested/option=value,another=yes", "AnyPlugin", R"([configuration]
option=
another=yes
[configuration/group]
option=
[configuration/group/nested]
option=value
another=
)", nullptr},
    {"unrecognized option", R"([configuration]
option=
[configuration/group]
)",
        "notFound=value", "AnyPlugin", R"([configuration]
option=
notFound=value
[configuration/group]
)",
        /* The trailing space is there because the plugin name is empty */
        "Option notFound not recognized by \n"},
    {"unrecognized option in Any plugin", R"([configuration]
option=
[configuration/group]
)",
        "notFound=value", "", R"([configuration]
option=
notFound=value
[configuration/group]
)", nullptr},
    {"unrecognized group", R"([configuration]
option=
[configuration/group]
option=
)",
        "notFound/option=value", "AnyPlugin", R"([configuration]
option=
[configuration/group]
option=
[configuration/notFound]
option=value
)",
        /* The trailing space is there because the plugin name is empty */
        "Option notFound/option not recognized by \n"},
    {"unrecognized nested group", R"([configuration]
option=
[configuration/group]
option=
)",
        "group/notFound/nested/option=value", "AnyPlugin", R"([configuration]
option=
[configuration/group]
option=
[configuration/group/notFound/nested]
option=value
)",
        /* The trailing space is there because the plugin name is empty */
        "Option group/notFound/nested/option not recognized by \n"},
    /* This should not warn for emptyGroup, since it's a common use case (for
       example GltfImporter's customSceneFields). It should also remember that
       the group was initially empty to not warn again when more options are
       subsequently added. OTOH, for subgroups added to an empty group it
       still warns. */
    {"unrecognized option in empty config subgroup", R"([configuration]
option=
[configuration/emptyGroup]
# No values originally here
[configuration/nonEmptyGroup]
option=
)",
        "emptyGroup/notFound=value,nonEmptyGroup/notFound=value,emptyGroup/another,emptyGroup/subgroup/notFound=value", "AnyPlugin", R"([configuration]
option=
[configuration/emptyGroup]
# No values originally here
notFound=value
another=true
[configuration/emptyGroup/subgroup]
notFound=value
[configuration/nonEmptyGroup]
option=
notFound=value
)",
        /* The trailing space is there because the plugin name is empty */
        "Option nonEmptyGroup/notFound not recognized by \n"
        "Option emptyGroup/subgroup/notFound not recognized by \n"},
    /* OTOH this should warn, as it's an option in the root configuration */
    {"unrecognized option in empty root config", R"([configuration]
)",
        "notFound=value", "AnyPlugin", R"([configuration]
notFound=value
)",
        /* The trailing space is there because the plugin name is empty */
        "Option notFound not recognized by \n"},

    /* Adding new unrecognized options to existing groups doesn't warn */
    {"add an unrecognized option that doesn't exist yet", R"([configuration]
option=
another=
)",
        "+yetanother=value", "AnyPlugin", R"([configuration]
option=
another=
yetanother=value
)", nullptr},
    {"add an option that exists already", R"([configuration]
option=
another=
)",
        "+option=value", "AnyPlugin", R"([configuration]
option=
another=
option=value
)", nullptr},
    {"two options, add second with implicit true", R"([configuration]
option=
another=
)",
        "option=value,+yetanother", "AnyPlugin", R"([configuration]
option=value
another=
yetanother=true
)", nullptr},
    {"add an option to an existing subgroup", R"([configuration]
option=
[configuration/group]
option=
another=
)",
        "+group/option=value", "AnyPlugin", R"([configuration]
option=
[configuration/group]
option=
another=
option=value
)", nullptr},
    {"add an option to an unrecognized subgroup", R"([configuration]
option=
[configuration/group]
option=
another=
)",
        "+notFound/option=value", "AnyPlugin", R"([configuration]
option=
[configuration/group]
option=
another=
[configuration/notFound]
option=value
)",
    /* The trailing space is there because the plugin name is empty */
    "Option notFound/option not recognized by \n"},
};

ConverterUtilitiesTest::ConverterUtilitiesTest() {
    addInstancedTests({&ConverterUtilitiesTest::setOptions},
        Containers::arraySize(SetOptionsData));
}

void ConverterUtilitiesTest::setOptions() {
    auto&& data = SetOptionsData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    struct: PluginManager::AbstractPlugin {} plugin;

    /** @todo UGH, fix the insane Configuration API already */
    std::stringstream in;
    in << data.config;
    plugin.configuration() = Utility::ConfigurationGroup{*Utility::Configuration{in}.group("configuration")};

    {
        Containers::String out;
        Warning redirectWarning{&out};
        Implementation::setOptions(plugin, data.anyPluginName, data.options);
        if(data.expectedWarning)
            CORRADE_COMPARE(out, data.expectedWarning);
        else
            CORRADE_COMPARE(out, "");
    }

    Utility::Configuration conf;
    /** @todo ugh, is there no better way to serialize a ConfigurationGroup? */
    conf.addGroup("configuration", new Utility::ConfigurationGroup{plugin.configuration()});
    CORRADE_COMPARE(conf.group("configuration")->configuration(), &conf);
    std::stringstream out;
    conf.save(out);
    CORRADE_COMPARE_AS(out.str(),
        data.expectedConfig,
        TestSuite::Compare::String);
}

}}}

CORRADE_TEST_MAIN(Magnum::Test::ConverterUtilitiesTest)
