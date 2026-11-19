/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025, 2026
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

#include <Corrade/Containers/String.h>
#include <Corrade/TestSuite/Tester.h>

#include "Magnum/Magnum.h"
#include "Magnum/Shaders/baseGL.h"

namespace Magnum { namespace Shaders { namespace Test { namespace {

struct BaseGL_Test: TestSuite::Tester {
    explicit BaseGL_Test();

    void configurationConstruct();
    void configurationConstructCopy();
    void configurationConstructMove();
    void configurationSetters();

    void shaderWrapperConstructNoCreate();
    void shaderWrapperConstructCopy();
};

using namespace Containers::Literals;

BaseGL_Test::BaseGL_Test() {
    addTests({&BaseGL_Test::configurationConstruct,
              &BaseGL_Test::configurationConstructCopy,
              &BaseGL_Test::configurationConstructMove,
              &BaseGL_Test::configurationSetters,

              &BaseGL_Test::shaderWrapperConstructNoCreate,
              &BaseGL_Test::shaderWrapperConstructCopy});
}

void BaseGL_Test::configurationConstruct() {
    Implementation::ConfigurationBaseGL configuration;
    CORRADE_COMPARE(configuration.extraSource(), "");
    CORRADE_COMPARE(configuration.extraSource().flags(), Containers::StringViewFlag::NullTerminated);
}

void BaseGL_Test::configurationConstructCopy() {
    Implementation::ConfigurationBaseGL a;
    a.setExtraSource("// hello\n"_s);
    CORRADE_COMPARE(a.extraSource(), "// hello\n");
    CORRADE_COMPARE(a.extraSource().flags(), Containers::StringViewFlag::NullTerminated|Containers::StringViewFlag::Global);

    Implementation::ConfigurationBaseGL b = a;
    CORRADE_COMPARE(b.extraSource(), "// hello\n");
    /* Copying invokes a copy constructor on the string, so the Global flag
       gets lost */
    CORRADE_COMPARE(b.extraSource().flags(), Containers::StringViewFlag::NullTerminated);

    Implementation::ConfigurationBaseGL c;
    c.setExtraSource("// hi\n"_s);
    c = a;
    CORRADE_COMPARE(c.extraSource(), "// hello\n");
    /* Same here */
    CORRADE_COMPARE(c.extraSource().flags(), Containers::StringViewFlag::NullTerminated);
}

void BaseGL_Test::configurationConstructMove() {
    Implementation::ConfigurationBaseGL a;
    a.setExtraSource("// hello\n"_s);

    Implementation::ConfigurationBaseGL b = Utility::move(a);
    CORRADE_COMPARE(b.extraSource(), "// hello\n");
    CORRADE_COMPARE(b.extraSource().flags(), Containers::StringViewFlag::NullTerminated|Containers::StringViewFlag::Global);

    Implementation::ConfigurationBaseGL c;
    c.setExtraSource("// hi\n"_s);
    c = Utility::move(b);
    CORRADE_COMPARE(c.extraSource(), "// hello\n");
    CORRADE_COMPARE(c.extraSource().flags(), Containers::StringViewFlag::NullTerminated|Containers::StringViewFlag::Global);
}

void BaseGL_Test::configurationSetters() {
    Implementation::ConfigurationBaseGL configuration;
    configuration.setExtraSource("// hello\n"_s);
    CORRADE_COMPARE(configuration.extraSource(), "// hello\n");
    CORRADE_COMPARE(configuration.extraSource().flags(), Containers::StringViewFlag::NullTerminated|Containers::StringViewFlag::Global);

    /* Setting a global non-null-terminated string makes a copy */
    {
        Containers::StringView source = "// non-null-terminated\n!"_s.exceptSuffix(1);
        configuration.setExtraSource(source);
        CORRADE_COMPARE(configuration.extraSource(), source);
        CORRADE_COMPARE(configuration.extraSource().flags(), Containers::StringViewFlag::NullTerminated);
        CORRADE_VERIFY(configuration.extraSource().data() != source.data());

    /* Setting a local null-terminated string makes a copy */
    } {
        Containers::StringView source = "// local, null-terminated\n";
        configuration.setExtraSource(source);
        CORRADE_COMPARE(configuration.extraSource(), source);
        CORRADE_COMPARE(configuration.extraSource().flags(), Containers::StringViewFlag::NullTerminated);
        CORRADE_VERIFY(configuration.extraSource().data() != source.data());

    /* Setting a local non-null-terminated string makes a copy */
    } {
        Containers::StringView source = Containers::StringView{"// non-null-terminated\n!"}.exceptSuffix(1);
        configuration.setExtraSource(source);
        CORRADE_COMPARE(configuration.extraSource(), source);
        CORRADE_COMPARE(configuration.extraSource().flags(), Containers::StringViewFlag::NullTerminated);
        CORRADE_VERIFY(configuration.extraSource().data() != source.data());

    /* Setting a r-value global string view keeps the Global flag */
    } {
        Containers::StringView source = "// r-value String\n"_s;
        configuration.setExtraSource(Containers::String::nullTerminatedGlobalView(source));
        CORRADE_COMPARE(configuration.extraSource(), source);
        CORRADE_COMPARE(configuration.extraSource().flags(), Containers::StringViewFlag::NullTerminated|Containers::StringViewFlag::Global);
        CORRADE_COMPARE(configuration.extraSource().data(), static_cast<const void*>(source.data()));

    /* Setting a r-value acquires ownership of the local string */
    } {
        const char* source = "// r-value String\n";
        configuration.setExtraSource(Containers::String{source, [](char*, std::size_t) {}});
        CORRADE_COMPARE(configuration.extraSource(), source);
        CORRADE_COMPARE(configuration.extraSource().flags(), Containers::StringViewFlag::NullTerminated);
        CORRADE_COMPARE(configuration.extraSource().data(), static_cast<const void*>(source));

    /* Setting an empty string resets back to default */
    } {
        configuration.setExtraSource(Containers::StringView{});
        CORRADE_COMPARE(configuration.extraSource(), "");
        CORRADE_COMPARE(configuration.extraSource().flags(), Containers::StringViewFlag::NullTerminated);
    }
}

void BaseGL_Test::shaderWrapperConstructNoCreate() {
    Implementation::GLShaderWrapper shader{NoCreate};
    CORRADE_COMPARE(shader.type, 0);
    CORRADE_COMPARE(shader.id, 0);
}

void BaseGL_Test::shaderWrapperConstructCopy() {
    CORRADE_VERIFY(!std::is_copy_constructible<Implementation::GLShaderWrapper>{});
    CORRADE_VERIFY(!std::is_copy_assignable<Implementation::GLShaderWrapper>{});
}

}}}}

CORRADE_TEST_MAIN(Magnum::Shaders::Test::BaseGL_Test)
