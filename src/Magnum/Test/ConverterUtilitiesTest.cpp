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

#include <sstream>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/Configuration.h>

#include "Magnum/Implementation/converterUtilities.h"

namespace Magnum { namespace Test { namespace {

struct ConverterUtilitiesTest: TestSuite::Tester {
    explicit ConverterUtilitiesTest();

    void setOptions();
};

const struct {
    const char* name;
    const char* options;
    const char* anyPluginName;
    const char* expectedConfig;
    const char* expectedWarning;
} SetOptionsData[]{
    {"", "option=value", "AnyPlugin", R"([configuration]
option=value
another=
[configuration/group]
option=
[configuration/group/nested]
option=
)", nullptr},
    {"two options", "option=value,another=yes", "AnyPlugin", R"([configuration]
option=value
another=yes
[configuration/group]
option=
[configuration/group/nested]
option=
)", nullptr},
    {"implicit true", "option=value,another", "AnyPlugin", R"([configuration]
option=value
another=true
[configuration/group]
option=
[configuration/group/nested]
option=
)", nullptr},
    {"group", "group/option=value", "AnyPlugin", R"([configuration]
option=
another=
[configuration/group]
option=value
[configuration/group/nested]
option=
)", nullptr},
    {"nested group + root option after", "group/nested/option=value,another=yes", "AnyPlugin", R"([configuration]
option=
another=yes
[configuration/group]
option=
[configuration/group/nested]
option=value
)", nullptr},
    {"unrecognized option", "notFound=value", "AnyPlugin", R"([configuration]
option=
another=
notFound=value
[configuration/group]
option=
[configuration/group/nested]
option=
)",
        /* The trailing space is there because the plugin name is empty */
        "Option notFound not recognized by \n"},
    {"unrecognized option in Any plugin", "notFound=value", "", R"([configuration]
option=
another=
notFound=value
[configuration/group]
option=
[configuration/group/nested]
option=
)", nullptr},
    {"unrecognized group", "notFound/option=value", "AnyPlugin", R"([configuration]
option=
another=
[configuration/group]
option=
[configuration/group/nested]
option=
[configuration/notFound]
option=value
)",
        /* The trailing space is there because the plugin name is empty */
        "Option notFound/option not recognized by \n"},
    {"unrecognized nested group", "group/notFound/nested/option=value", "AnyPlugin", R"([configuration]
option=
another=
[configuration/group]
option=
[configuration/group/nested]
option=
[configuration/group/notFound/nested]
option=value
)",
        /* The trailing space is there because the plugin name is empty */
        "Option group/notFound/nested/option not recognized by \n"}
};

ConverterUtilitiesTest::ConverterUtilitiesTest() {
    addInstancedTests({&ConverterUtilitiesTest::setOptions},
        Containers::arraySize(SetOptionsData));
}

void ConverterUtilitiesTest::setOptions() {
    auto&& data = SetOptionsData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    struct Plugin: PluginManager::AbstractPlugin {
        explicit Plugin() {
            /* Populate default config */
            configuration().setValue("option", "");
            configuration().setValue("another", "");
            Utility::ConfigurationGroup& group = *configuration().addGroup("group");
            group.setValue("option", "");
            group.addGroup("nested")->setValue("option", "");
        }
    } plugin;

    {
        std::ostringstream out;
        Warning redirectWarning{&out};
        Implementation::setOptions(plugin, data.anyPluginName, data.options);
        if(data.expectedWarning)
            CORRADE_COMPARE(out.str(), data.expectedWarning);
        else
            CORRADE_COMPARE(out.str(), "");
    }

    Utility::Configuration conf;
    /** @todo ugh, is there no better way to serialize a ConfigurationGroup? */
    conf.addGroup("configuration", new Utility::ConfigurationGroup{plugin.configuration()});
    CORRADE_COMPARE(conf.group("configuration")->configuration(), &conf);
    std::ostringstream out;
    conf.save(out);
    CORRADE_COMPARE(out.str(), data.expectedConfig);
}

}}}

CORRADE_TEST_MAIN(Magnum::Test::ConverterUtilitiesTest)
