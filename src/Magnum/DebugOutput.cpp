/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015
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

#include "DebugOutput.h"

#ifndef MAGNUM_TARGET_WEBGL
#include <Corrade/Utility/Assert.h>
#include <Corrade/Utility/Debug.h>

#include "Magnum/Context.h"
#include "Magnum/Extensions.h"
#include "Magnum/Implementation/State.h"
#include "Magnum/Implementation/DebugState.h"

#if defined(CORRADE_TARGET_NACL_NEWLIB) || defined(CORRADE_TARGET_ANDROID)
#include <sstream>
#endif

namespace Magnum {

namespace {

#ifndef CORRADE_TARGET_NACL
void
#ifdef CORRADE_TARGET_WINDOWS
APIENTRY
#endif
callbackWrapper(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
    Context::current().state().debug->messageCallback(DebugOutput::Source(source), DebugOutput::Type(type), id, DebugOutput::Severity(severity), std::string{message, std::size_t(length)}, userParam);
}
#endif

void defaultCallback(const DebugOutput::Source source, const DebugOutput::Type type, const UnsignedInt id, const DebugOutput::Severity severity, const std::string& string, const void*) {
    Debug output;
    output << "Debug output:";

    switch(severity) {
        case DebugOutput::Severity::High:
            output << "high severity"; break;

        case DebugOutput::Severity::Medium:
            output << "medium severity"; break;

        case DebugOutput::Severity::Low:
            output << "low severity"; break;

        case DebugOutput::Severity::Notification: ;
    }

    switch(source) {
        case DebugOutput::Source::Api:
            output << "API"; break;
        case DebugOutput::Source::WindowSystem:
            output << "window system"; break;
        case DebugOutput::Source::ShaderCompiler:
            output << "shader compiler"; break;
        case DebugOutput::Source::ThirdParty:
            output << "third party"; break;
        case DebugOutput::Source::Application:
            output << "application"; break;

        case DebugOutput::Source::Other: ;
    }

    switch(type) {
        case DebugOutput::Type::Error:
            output << "error"; break;
        case DebugOutput::Type::DeprecatedBehavior:
            output << "deprecated behavior note"; break;
        case DebugOutput::Type::UndefinedBehavior:
            output << "undefined behavior note"; break;
        case DebugOutput::Type::Portability:
            output << "portability note"; break;
        case DebugOutput::Type::Performance:
            output << "performance note"; break;
        case DebugOutput::Type::Marker:
            output << "marker"; break;
        case DebugOutput::Type::PushGroup:
            output << "debug group enter"; break;
        case DebugOutput::Type::PopGroup:
            output << "debug group leave"; break;

        case DebugOutput::Type::Other: ;
    }

    /** @todo Remove when this is fixed everywhere (also the include above) */
    #if defined(CORRADE_TARGET_NACL_NEWLIB) || defined(CORRADE_TARGET_ANDROID)
    std::ostringstream converter;
    converter << id;
    #endif

    output << '(' +
        #if !defined(CORRADE_TARGET_NACL_NEWLIB) && !defined(CORRADE_TARGET_ANDROID)
        std::to_string(id) +
        #else
        converter.str() +
        #endif
        "):" << string;
}

}

Int DebugOutput::maxLoggedMessages() {
    if(!Context::current().isExtensionSupported<Extensions::GL::KHR::debug>())
        return 0;

    GLint& value = Context::current().state().debug->maxLoggedMessages;

    if(value == 0) {
        #ifndef MAGNUM_TARGET_GLES
        glGetIntegerv(GL_MAX_DEBUG_LOGGED_MESSAGES, &value);
        #else
        glGetIntegerv(GL_MAX_DEBUG_LOGGED_MESSAGES_KHR, &value);
        #endif
    }

    return value;
}

Int DebugOutput::maxMessageLength() {
    if(!Context::current().isExtensionSupported<Extensions::GL::KHR::debug>())
        return 0;

    GLint& value = Context::current().state().debug->maxMessageLength;

    if(value == 0) {
        #ifndef MAGNUM_TARGET_GLES
        glGetIntegerv(GL_MAX_DEBUG_MESSAGE_LENGTH, &value);
        #else
        glGetIntegerv(GL_MAX_DEBUG_MESSAGE_LENGTH_KHR, &value);
        #endif
    }

    return value;
}

void DebugOutput::setCallback(const Callback callback, const void* userParam) {
    Context::current().state().debug->callbackImplementation(callback, userParam);
}

void DebugOutput::setDefaultCallback() {
    setCallback(defaultCallback, nullptr);
}

void DebugOutput::setEnabledInternal(const GLenum source, const GLenum type, const GLenum severity, const std::initializer_list<UnsignedInt> ids, const bool enabled) {
    Context::current().state().debug->controlImplementation(source, type, severity, ids, enabled);
}

void DebugOutput::controlImplementationNoOp(GLenum, GLenum, GLenum, std::initializer_list<UnsignedInt>, bool) {}

void DebugOutput::controlImplementationKhr(const GLenum source, const GLenum type, const GLenum severity, const std::initializer_list<UnsignedInt> ids, const bool enabled) {
    #ifndef CORRADE_TARGET_NACL
    #ifndef MAGNUM_TARGET_GLES
    glDebugMessageControl
    #else
    glDebugMessageControlKHR
    #endif
        (source, type, severity, ids.size(), ids.begin(), enabled);
    #else
    static_cast<void>(source);
    static_cast<void>(type);
    static_cast<void>(severity);
    static_cast<void>(ids);
    static_cast<void>(enabled);
    CORRADE_ASSERT_UNREACHABLE();
    #endif
}

void DebugOutput::callbackImplementationNoOp(Callback, const void*) {}

void DebugOutput::callbackImplementationKhr(const Callback callback, const void* userParam) {
    /* Replace the callback */
    const Callback original = Context::current().state().debug->messageCallback;
    Context::current().state().debug->messageCallback = callback;

    /* Adding callback */
    if(!original && callback) {
        #ifndef CORRADE_TARGET_NACL
        #ifndef MAGNUM_TARGET_GLES
        glDebugMessageCallback
        #else
        glDebugMessageCallbackKHR
        #endif
            (callbackWrapper, userParam);
        #else
        static_cast<void>(userParam);
        CORRADE_ASSERT_UNREACHABLE();
        #endif

    /* Deleting callback */
    } else if(original && !callback) {
        #ifndef CORRADE_TARGET_NACL
        #ifndef MAGNUM_TARGET_GLES
        glDebugMessageCallback
        #else
        glDebugMessageCallbackKHR
        #endif
            (nullptr, nullptr);
        #else
        CORRADE_ASSERT_UNREACHABLE();
        #endif
    }
}

#ifndef DOXYGEN_GENERATING_OUTPUT
Debug& operator<<(Debug& debug, const DebugOutput::Source value) {
    switch(value) {
        #define _c(value) case DebugOutput::Source::value: return debug << "DebugOutput::Source::" #value;
        _c(Api)
        _c(WindowSystem)
        _c(ShaderCompiler)
        _c(ThirdParty)
        _c(Application)
        _c(Other)
        #undef _c
    }

    return debug << "DebugOutput::Source::(invalid)";
}

Debug& operator<<(Debug& debug, const DebugOutput::Type value) {
    switch(value) {
        #define _c(value) case DebugOutput::Type::value: return debug << "DebugOutput::Type::" #value;
        _c(Error)
        _c(DeprecatedBehavior)
        _c(UndefinedBehavior)
        _c(Portability)
        _c(Performance)
        _c(Marker)
        _c(PushGroup)
        _c(PopGroup)
        _c(Other)
        #undef _c
    }

    return debug << "DebugOutput::Type::(invalid)";
}

Debug& operator<<(Debug& debug, const DebugOutput::Severity value) {
    switch(value) {
        #define _c(value) case DebugOutput::Severity::value: return debug << "DebugOutput::Severity::" #value;
        _c(High)
        _c(Medium)
        _c(Low)
        _c(Notification)
        #undef _c
    }

    return debug << "DebugOutput::Severity::(invalid)";
}
#endif

void DebugMessage::insertInternal(const Source source, const Type type, const UnsignedInt id, const DebugOutput::Severity severity, const Containers::ArrayView<const char> string) {
    Context::current().state().debug->messageInsertImplementation(source, type, id, severity, string);
}

void DebugMessage::insertImplementationNoOp(Source, Type, UnsignedInt, DebugOutput::Severity, const Containers::ArrayView<const char>) {}

void DebugMessage::insertImplementationKhr(const Source source, const Type type, const UnsignedInt id, const DebugOutput::Severity severity, const Containers::ArrayView<const char> string) {
    #ifndef CORRADE_TARGET_NACL
    #ifndef MAGNUM_TARGET_GLES
    glDebugMessageInsert
    #else
    glDebugMessageInsertKHR
    #endif
        (GLenum(source), GLenum(type), id, GLenum(severity), string.size(), string.data());
    #else
    static_cast<void>(source);
    static_cast<void>(type);
    static_cast<void>(id);
    static_cast<void>(severity);
    static_cast<void>(string);
    CORRADE_ASSERT_UNREACHABLE();
    #endif
}

void DebugMessage::insertImplementationExt(Source, Type, UnsignedInt, DebugOutput::Severity, const Containers::ArrayView<const char> string) {
    #ifndef CORRADE_TARGET_NACL
    glInsertEventMarkerEXT(string.size(), string.data());
    #else
    static_cast<void>(string);
    CORRADE_ASSERT_UNREACHABLE();
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void DebugMessage::insertImplementationGremedy(Source, Type, UnsignedInt, DebugOutput::Severity, const Containers::ArrayView<const char> string) {
    glStringMarkerGREMEDY(string.size(), string.data());
}
#endif

#ifndef DOXYGEN_GENERATING_OUTPUT
Debug& operator<<(Debug& debug, const DebugMessage::Source value) {
    switch(value) {
        #define _c(value) case DebugMessage::Source::value: return debug << "DebugMessage::Source::" #value;
        _c(ThirdParty)
        _c(Application)
        #undef _c
        #ifdef MAGNUM_BUILD_DEPRECATED
        case DebugMessage::Source::Api:
        case DebugMessage::Source::WindowSystem:
        case DebugMessage::Source::ShaderCompiler:
        case DebugMessage::Source::Other:
            return debug << DebugOutput::Source(value);
        #endif
    }

    return debug << "DebugMessage::Source::(invalid)";
}

Debug& operator<<(Debug& debug, const DebugMessage::Type value) {
    switch(value) {
        #define _c(value) case DebugMessage::Type::value: return debug << "DebugMessage::Type::" #value;
        _c(Error)
        _c(DeprecatedBehavior)
        _c(UndefinedBehavior)
        _c(Portability)
        _c(Performance)
        _c(Other)
        _c(Marker)
        #undef _c
    }

    return debug << "DebugMessage::Type::(invalid)";
}
#endif

Int DebugGroup::maxStackDepth() {
    if(!Context::current().isExtensionSupported<Extensions::GL::KHR::debug>())
        return 0;

    GLint& value = Context::current().state().debug->maxStackDepth;

    if(value == 0) {
        #ifndef MAGNUM_TARGET_GLES
        glGetIntegerv(GL_MAX_DEBUG_GROUP_STACK_DEPTH, &value);
        #else
        glGetIntegerv(GL_MAX_DEBUG_GROUP_STACK_DEPTH_KHR, &value);
        #endif
    }

    return value;
}

void DebugGroup::pushInternal(const Source source, const UnsignedInt id, const Containers::ArrayView<const char> message) {
    CORRADE_ASSERT(!_active, "DebugGroup::push(): group is already active", );
    Context::current().state().debug->pushGroupImplementation(source, id, message);
    _active = true;
}

void DebugGroup::pop() {
    CORRADE_ASSERT(_active, "DebugGroup::pop(): group is not active", );
    Context::current().state().debug->popGroupImplementation();
    _active = false;
}

void DebugGroup::pushImplementationNoOp(Source, UnsignedInt, Containers::ArrayView<const char>) {}

void DebugGroup::pushImplementationKhr(const Source source, const UnsignedInt id, const Containers::ArrayView<const char> message) {
    #ifndef CORRADE_TARGET_NACL
    #ifndef MAGNUM_TARGET_GLES
    glPushDebugGroup
    #else
    glPushDebugGroupKHR
    #endif
        (GLenum(source), id, message.size(), message.data());
    #else
    static_cast<void>(source);
    static_cast<void>(id);
    static_cast<void>(message);
    CORRADE_ASSERT_UNREACHABLE();
    #endif
}

void DebugGroup::pushImplementationExt(Source, UnsignedInt, const Containers::ArrayView<const char> message) {
    #ifndef CORRADE_TARGET_NACL
    glPushGroupMarkerEXT(message.size(), message.data());
    #else
    static_cast<void>(message);
    CORRADE_ASSERT_UNREACHABLE();
    #endif
}

void DebugGroup::popImplementationNoOp() {}

void DebugGroup::popImplementationKhr() {
    #ifndef MAGNUM_TARGET_GLES
    glPopDebugGroup();
    #elif !defined(CORRADE_TARGET_NACL)
    glPopDebugGroupKHR();
    #else
    CORRADE_ASSERT_UNREACHABLE();
    #endif
}

void DebugGroup::popImplementationExt() {
    #ifndef CORRADE_TARGET_NACL
    glPopGroupMarkerEXT();
    #else
    CORRADE_ASSERT_UNREACHABLE();
    #endif
}

#ifndef DOXYGEN_GENERATING_OUTPUT
Debug& operator<<(Debug& debug, const DebugGroup::Source value) {
    switch(value) {
        #define _c(value) case DebugGroup::Source::value: return debug << "DebugGroup::Source::" #value;
        _c(ThirdParty)
        _c(Application)
        #undef _c
    }

    return debug << "DebugGroup::Source::(invalid)";
}
#endif

}
#endif
