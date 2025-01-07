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

#include <sstream> /** @todo remove once the DebugOutput internals are stream-free */
#include <Corrade/Containers/StringView.h>
#include <Corrade/Containers/ScopeGuard.h>
#include <Corrade/Utility/DebugStl.h> /** @todo remove once the DebugOutput internals are stream-free */

#include "Magnum/GL/Context.h"
#include "Magnum/GL/DebugOutput.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/OpenGLTester.h"
#include "Magnum/GL/Implementation/defaultDebugCallback.h"

namespace Magnum { namespace GL { namespace Test { namespace {

struct DebugOutputGLTest: OpenGLTester {
    explicit DebugOutputGLTest();

    void setCallbackDefault();
    #ifdef MAGNUM_BUILD_DEPRECATED
    void setCallbackDeprecated();
    #endif

    void setup();
    void teardown();

    void setEnabled();

    void messageNoOp();
    void message();
    void messageFallback();

    void groupNoOp();
    void group();
    void groupFallback();

    std::ostringstream _out;
};

using namespace Containers::Literals;

const struct {
    const char* name;
    Containers::StringView message;
} MessageData[]{
    {"",
        "Hello from OpenGL command stream!"},
    {"non-null-terminated string",
        "Hello from OpenGL command stream!!"_s.exceptSuffix(1)},
};

const struct {
    const char* name;
    Containers::StringView automaticGroupName, manualGroupName;
} GroupData[]{
    {"",
        "Automatic debug group",
        "Manual debug group"},
    {"non-null-terminated string",
        "Automatic debug group!"_s.exceptSuffix(1),
        "Manual debug group!"_s.exceptSuffix(1)}
};

DebugOutputGLTest::DebugOutputGLTest() {
    addTests({&DebugOutputGLTest::setCallbackDefault,
              #ifdef MAGNUM_BUILD_DEPRECATED
              &DebugOutputGLTest::setCallbackDeprecated,
              #endif
              });

    addTests({&DebugOutputGLTest::setEnabled,

              &DebugOutputGLTest::messageNoOp},
        &DebugOutputGLTest::setup, &DebugOutputGLTest::teardown);

    addInstancedTests({&DebugOutputGLTest::message},
        Containers::arraySize(MessageData),
        &DebugOutputGLTest::setup, &DebugOutputGLTest::teardown);

    addTests({&DebugOutputGLTest::messageFallback,

              &DebugOutputGLTest::groupNoOp},
        &DebugOutputGLTest::setup, &DebugOutputGLTest::teardown);

    addInstancedTests({&DebugOutputGLTest::group},
        Containers::arraySize(GroupData),
        &DebugOutputGLTest::setup, &DebugOutputGLTest::teardown);

    addTests({&DebugOutputGLTest::groupFallback},
        &DebugOutputGLTest::setup, &DebugOutputGLTest::teardown);
}

void DebugOutputGLTest::setCallbackDefault() {
    if(!Context::current().isExtensionSupported<Extensions::KHR::debug>())
        CORRADE_SKIP(Extensions::KHR::debug::string() << "is not supported.");

    DebugOutput::setDefaultCallback();

    MAGNUM_VERIFY_NO_GL_ERROR();
}

#ifdef MAGNUM_BUILD_DEPRECATED
void DebugOutputGLTest::setCallbackDeprecated() {
    if(!Context::current().isExtensionSupported<Extensions::KHR::debug>())
        CORRADE_SKIP(Extensions::KHR::debug::string() << "is not supported.");

    Renderer::enable(Renderer::Feature::DebugOutput);
    Renderer::enable(Renderer::Feature::DebugOutputSynchronous);

    Containers::ScopeGuard guard{[]() {
        Renderer::disable(Renderer::Feature::DebugOutput);
        Renderer::disable(Renderer::Feature::DebugOutputSynchronous);
        DebugOutput::setDefaultCallback();
    }};

    std::ostringstream out;
    CORRADE_IGNORE_DEPRECATED_PUSH
    DebugOutput::setCallback([](DebugOutput::Source source, DebugOutput::Type type, UnsignedInt id, DebugOutput::Severity severity, const std::string& string, const void* userPtr) {
        Implementation::defaultDebugCallback(source, type, id, severity, string, static_cast<std::ostringstream*>(const_cast<void*>(userPtr)));
    }, &out);
    CORRADE_IGNORE_DEPRECATED_POP

    DebugMessage::insert(DebugMessage::Source::Application, DebugMessage::Type::Marker,
        1337, DebugOutput::Severity::High, "Hello from OpenGL command stream!");

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(out.str(),
        "Debug output: high severity application marker (1337): Hello from OpenGL command stream!\n");
}
#endif

void DebugOutputGLTest::setup() {
    _out.str({});

    if(!Context::current().isExtensionSupported<Extensions::KHR::debug>())
        return;

    Renderer::enable(Renderer::Feature::DebugOutput);
    Renderer::enable(Renderer::Feature::DebugOutputSynchronous);
    DebugOutput::setCallback([](DebugOutput::Source source, DebugOutput::Type type, UnsignedInt id, DebugOutput::Severity severity, Containers::StringView string, const void* userPtr) {
        Implementation::defaultDebugCallback(source, type, id, severity, string, static_cast<std::ostringstream*>(const_cast<void*>(userPtr)));
    }, &_out);
}

void DebugOutputGLTest::teardown() {
    if(!Context::current().isExtensionSupported<Extensions::KHR::debug>())
        return;

    Renderer::disable(Renderer::Feature::DebugOutput);
    Renderer::disable(Renderer::Feature::DebugOutputSynchronous);
    DebugOutput::setDefaultCallback();
}

void DebugOutputGLTest::setEnabled() {
    if(!Context::current().isExtensionSupported<Extensions::KHR::debug>())
        CORRADE_SKIP(Extensions::KHR::debug::string() << "is not supported.");

    /* Try at least some combinations. Calling a less-specific version will
       reset the more-specific setting from earlier. */
    DebugOutput::setEnabled(true);
    DebugOutput::setEnabled(DebugOutput::Source::Application, true);
    DebugOutput::setEnabled(DebugOutput::Source::Application, DebugOutput::Type::UndefinedBehavior, {3168, 35487, 234487}, false);

    MAGNUM_VERIFY_NO_GL_ERROR();

    DebugMessage::insert(DebugMessage::Source::Application, DebugMessage::Type::UndefinedBehavior,
        35487, DebugOutput::Severity::Notification, "This message should get ignored");

    /* Reset everything back */
    DebugOutput::setEnabled(true);

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(_out.str(), "");
}

void DebugOutputGLTest::messageNoOp() {
    if(Context::current().isExtensionSupported<Extensions::KHR::debug>() ||
       Context::current().isExtensionSupported<Extensions::EXT::debug_marker>()
       #ifndef MAGNUM_TARGET_GLES
       || Context::current().isExtensionSupported<Extensions::GREMEDY::string_marker>()
       #endif
    )
        CORRADE_SKIP("The extensions are supported, cannot test.");

    DebugMessage::insert(DebugMessage::Source::Application, DebugMessage::Type::Marker,
        1337, DebugOutput::Severity::Notification, "Hello from OpenGL command stream!");

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(_out.str(), "");
}

void DebugOutputGLTest::message() {
    auto&& data = MessageData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    if(!Context::current().isExtensionSupported<Extensions::KHR::debug>())
        CORRADE_SKIP(Extensions::KHR::debug::string() << "is not supported.");

    /* Need to be careful, because the test runner is using debug output too */
    DebugMessage::insert(DebugMessage::Source::Application, DebugMessage::Type::Marker,
        1337, DebugOutput::Severity::High, data.message);

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(_out.str(),
        "Debug output: high severity application marker (1337): Hello from OpenGL command stream!\n");
}

void DebugOutputGLTest::messageFallback() {
    if(Context::current().isExtensionSupported<Extensions::KHR::debug>() ||
     (!Context::current().isExtensionSupported<Extensions::EXT::debug_marker>()
   #ifndef MAGNUM_TARGET_GLES
   && !Context::current().isExtensionSupported<Extensions::GREMEDY::string_marker>()
   #endif
    ))
        CORRADE_SKIP("No proper extension is supported");

    DebugMessage::insert(DebugMessage::Source::Application, DebugMessage::Type::Marker,
        1337, DebugOutput::Severity::Notification, "Hello from OpenGL command stream!");

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(_out.str(), "");
}

void DebugOutputGLTest::groupNoOp() {
    if(Context::current().isExtensionSupported<Extensions::KHR::debug>() ||
       Context::current().isExtensionSupported<Extensions::EXT::debug_marker>())
        CORRADE_SKIP("The extensions are supported, cannot test.");

    {
        DebugGroup g{DebugGroup::Source::Application, 1337, "Debug group"};
    }

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(_out.str(), "");
}

void DebugOutputGLTest::group() {
    auto&& data = GroupData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    if(!Context::current().isExtensionSupported<Extensions::KHR::debug>())
        CORRADE_SKIP(Extensions::KHR::debug::string() << "is not supported.");

    /* Need to be careful, because the test runner is using debug output too */
    {
        DebugGroup g1{DebugGroup::Source::Application, 42, data.automaticGroupName};
        DebugGroup g2;
        g2.push(DebugGroup::Source::ThirdParty, 1337, data.manualGroupName);
        g2.pop();
    }

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(_out.str(),
        "Debug output: application debug group enter (42): Automatic debug group\n"
        "Debug output: third party debug group enter (1337): Manual debug group\n"
        "Debug output: third party debug group leave (1337): Manual debug group\n"
        "Debug output: application debug group leave (42): Automatic debug group\n");
}

void DebugOutputGLTest::groupFallback() {
    if(Context::current().isExtensionSupported<Extensions::KHR::debug>() ||
      !Context::current().isExtensionSupported<Extensions::EXT::debug_marker>())
        CORRADE_SKIP("No proper extension is supported");

    {
        DebugGroup g{DebugGroup::Source::Application, 1337, "Debug group"};
    }

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(_out.str(), "");
}

}}}}

CORRADE_TEST_MAIN(Magnum::GL::Test::DebugOutputGLTest)
