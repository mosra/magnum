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

#include "DebugOutput.h"

#ifndef MAGNUM_TARGET_WEBGL
#include <Corrade/Containers/StringView.h>
#include <Corrade/Utility/Assert.h>
#include <Corrade/Utility/Debug.h>

#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/Implementation/State.h"
#include "Magnum/GL/Implementation/DebugState.h"
#include "Magnum/GL/Implementation/defaultDebugCallback.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <string>
#endif

namespace Magnum { namespace GL {

namespace Implementation {

void defaultDebugCallback(const DebugOutput::Source source, const DebugOutput::Type type, const UnsignedInt id, const DebugOutput::Severity severity, const Containers::StringView string, std::ostream* out) {
    Debug output{out};
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

    output << "(" << Debug::nospace << id << Debug::nospace << "):" << string;
}

}

namespace {

void
#ifdef CORRADE_TARGET_WINDOWS
APIENTRY
#endif
callbackWrapper(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
    const auto& callback = *static_cast<const Implementation::DebugState::MessageCallback*>(userParam);
    callback.callback(DebugOutput::Source(source), DebugOutput::Type(type), id, DebugOutput::Severity(severity), {message, std::size_t(length)}, callback.userParam);
}

}

Int DebugOutput::maxLoggedMessages() {
    if(!Context::current().isExtensionSupported<Extensions::KHR::debug>())
        return 0;

    GLint& value = Context::current().state().debug.maxLoggedMessages;

    if(value == 0) {
        #ifndef MAGNUM_TARGET_GLES2
        glGetIntegerv(GL_MAX_DEBUG_LOGGED_MESSAGES, &value);
        #else
        glGetIntegerv(GL_MAX_DEBUG_LOGGED_MESSAGES_KHR, &value);
        #endif
    }

    return value;
}

Int DebugOutput::maxMessageLength() {
    if(!Context::current().isExtensionSupported<Extensions::KHR::debug>())
        return 0;

    GLint& value = Context::current().state().debug.maxMessageLength;

    if(value == 0) {
        #ifndef MAGNUM_TARGET_GLES2
        glGetIntegerv(GL_MAX_DEBUG_MESSAGE_LENGTH, &value);
        #else
        glGetIntegerv(GL_MAX_DEBUG_MESSAGE_LENGTH_KHR, &value);
        #endif
    }

    return value;
}

void DebugOutput::setCallback(const Callback callback, const void* userParam) {
    Context::current().state().debug.messageCallback.userParam = userParam;
    Context::current().state().debug.callbackImplementation(callback);
}

#ifdef MAGNUM_BUILD_DEPRECATED
void DebugOutput::setCallback(void(*const callback)(Source, Type, UnsignedInt, Severity, const std::string&, const void*), const void* userParam) {
    /* This is a second delegation step after the callbackWrapper() which
       converts from raw GL types. Alternatively there could be a deprecated
       version of all callbackImplementation*() variants, but this is less
       code in total -- just two extra members in the MessageCallback
       struct. */
    Context::current().state().debug.messageCallback.userParam = &Context::current().state().debug.messageCallback;
    Context::current().state().debug.messageCallback.callbackStlString = callback;
    Context::current().state().debug.messageCallback.userParamStlString = userParam;
    Context::current().state().debug.callbackImplementation([](DebugOutput::Source source, DebugOutput::Type type, UnsignedInt id, DebugOutput::Severity severity, const Containers::StringView string, const void* userParam) {
        const auto& messageCallback = *static_cast<const Implementation::DebugState::MessageCallback*>(userParam);
        messageCallback.callbackStlString(source, type, id, severity, string, messageCallback.userParamStlString);
    });
}
#endif

void DebugOutput::setDefaultCallback() {
    setCallback([](DebugOutput::Source source, DebugOutput::Type type, UnsignedInt id, DebugOutput::Severity severity, const Containers::StringView string, const void*) {
        Implementation::defaultDebugCallback(source, type, id, severity, string, Debug::output());
    });
}

void DebugOutput::setEnabledInternal(const GLenum source, const GLenum type, const GLenum severity, const std::initializer_list<UnsignedInt> ids, const bool enabled) {
    Context::current().state().debug.controlImplementation(source, type, severity, ids.size(), ids.begin(), enabled);
}

void DebugOutput::controlImplementationNoOp(GLenum, GLenum, GLenum, GLsizei, const GLuint*, GLboolean) {}

void DebugOutput::callbackImplementationNoOp(Callback) {}

#ifndef MAGNUM_TARGET_GLES2
void DebugOutput::callbackImplementationKhrDesktopES32(const Callback callback) {
    /* Replace the callback */
    const bool setPreviously = !!Context::current().state().debug.messageCallback.callback;

    Context::current().state().debug.messageCallback.callback = callback;

    /* Adding callback */
    if(!setPreviously && callback)
        glDebugMessageCallback(callbackWrapper,  &Context::current().state().debug.messageCallback);

    /* Deleting callback */
    else if(setPreviously && !callback)
        glDebugMessageCallback(nullptr, nullptr);
}
#endif

#ifdef MAGNUM_TARGET_GLES
void DebugOutput::callbackImplementationKhrES(const Callback callback) {
    /* Replace the callback */
    const bool setPreviously = !!Context::current().state().debug.messageCallback.callback;

    Context::current().state().debug.messageCallback.callback = callback;

    /* Adding callback */
    if(!setPreviously && callback)
        glDebugMessageCallbackKHR(callbackWrapper, &Context::current().state().debug.messageCallback);

    /* Deleting callback */
    else if(setPreviously && !callback)
        glDebugMessageCallbackKHR(nullptr, nullptr);
}
#endif

Debug& operator<<(Debug& debug, const DebugOutput::Source value) {
    debug << "GL::DebugOutput::Source" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case DebugOutput::Source::value: return debug << "::" #value;
        _c(Api)
        _c(WindowSystem)
        _c(ShaderCompiler)
        _c(ThirdParty)
        _c(Application)
        _c(Other)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << Debug::hex << GLenum(value) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const DebugOutput::Type value) {
    debug << "GL::DebugOutput::Type" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case DebugOutput::Type::value: return debug << "::" #value;
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
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << Debug::hex << GLenum(value) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const DebugOutput::Severity value) {
    debug << "GL::DebugOutput::Severity" << Debug::nospace;

    switch(value) {
        #define _c(value) case DebugOutput::Severity::value: return debug << "::" #value;
        _c(High)
        _c(Medium)
        _c(Low)
        _c(Notification)
        #undef _c
    }

    return debug << "(" << Debug::nospace << Debug::hex << GLenum(value) << Debug::nospace << ")";
}

void DebugMessage::insert(const Source source, const Type type, const UnsignedInt id, const DebugOutput::Severity severity, const Containers::StringView string) {
    Context::current().state().debug.messageInsertImplementation(GLenum(source), GLenum(type), id, GLenum(severity), string.size(), string.data());
}

void DebugMessage::insertImplementationNoOp(GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar*) {}

void DebugMessage::insertImplementationExt(GLenum, GLenum, GLuint, GLenum, const GLsizei length, const GLchar* const message) {
    glInsertEventMarkerEXT(length, message);
}

#ifndef MAGNUM_TARGET_GLES
void DebugMessage::insertImplementationGremedy(GLenum, GLenum, GLuint, GLenum, const GLsizei length, const GLchar* const message) {
    glStringMarkerGREMEDY(length, message);
}
#endif

Debug& operator<<(Debug& debug, const DebugMessage::Source value) {
    debug << "GL::DebugMessage::Source" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case DebugMessage::Source::value: return debug << "::" #value;
        _c(ThirdParty)
        _c(Application)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << Debug::hex << GLenum(value) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const DebugMessage::Type value) {
    debug << "GL::DebugMessage::Type" << Debug::nospace;

    switch(value) {
        #define _c(value) case DebugMessage::Type::value: return debug << "::" #value;
        _c(Error)
        _c(DeprecatedBehavior)
        _c(UndefinedBehavior)
        _c(Portability)
        _c(Performance)
        _c(Other)
        _c(Marker)
        #undef _c
    }

    return debug << "(" << Debug::nospace << Debug::hex << GLenum(value) << Debug::nospace << ")";
}

Int DebugGroup::maxStackDepth() {
    if(!Context::current().isExtensionSupported<Extensions::KHR::debug>())
        return 0;

    GLint& value = Context::current().state().debug.maxStackDepth;

    if(value == 0) {
        #ifndef MAGNUM_TARGET_GLES2
        glGetIntegerv(GL_MAX_DEBUG_GROUP_STACK_DEPTH, &value);
        #else
        glGetIntegerv(GL_MAX_DEBUG_GROUP_STACK_DEPTH_KHR, &value);
        #endif
    }

    return value;
}

DebugGroup::DebugGroup(const Source source, const UnsignedInt id, const Containers::StringView message): DebugGroup{} {
    push(source, id, message);
}

void DebugGroup::push(const Source source, const UnsignedInt id, const Containers::StringView message) {
    CORRADE_ASSERT(!_active, "GL::DebugGroup::push(): group is already active", );
    Context::current().state().debug.pushGroupImplementation(GLenum(source), id, message.size(), message.data());
    _active = true;
}

void DebugGroup::pop() {
    CORRADE_ASSERT(_active, "GL::DebugGroup::pop(): group is not active", );
    Context::current().state().debug.popGroupImplementation();
    _active = false;
}

void DebugGroup::pushImplementationNoOp(GLenum, GLuint, GLsizei, const GLchar*) {}

void DebugGroup::pushImplementationExt(GLenum, GLuint, const GLsizei length, const GLchar* const message) {
    glPushGroupMarkerEXT(length, message);
}

void DebugGroup::popImplementationNoOp() {}

Debug& operator<<(Debug& debug, const DebugGroup::Source value) {
    debug << "GL::DebugGroup::Source" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case DebugGroup::Source::value: return debug << "::" #value;
        _c(ThirdParty)
        _c(Application)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << Debug::hex << GLenum(value) << Debug::nospace << ")";
}

}}
#endif
