#ifndef Magnum_DebugOutput_h
#define Magnum_DebugOutput_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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

#ifndef MAGNUM_TARGET_WEBGL
/** @file
 * @brief Class @ref Magnum::DebugOutput, @ref Magnum::DebugMessage, @ref Magnum::DebugGroup
 */
#endif

#include <string>
#include <Corrade/Containers/ArrayView.h>

#include "Magnum/OpenGL.h"
#include "Magnum/Magnum.h"
#include "Magnum/visibility.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <Corrade/Utility/Macros.h>
#endif

#ifndef MAGNUM_TARGET_WEBGL
namespace Magnum {

namespace Implementation { struct DebugState; }

/**
@brief Debug output

Manages OpenGL debug output. The debug messages are emitted either from driver
(such as GL error descriptions and various performance and optimization hints)
or from third party software and the application itself using @ref DebugMessage
and @ref DebugGroup, which can be also used to mark various portions of command
stream in various graphics debuggers, such as Apitrace or gDEBugger.

## Basic usage

Support for debug output is provided by OpenGL 4.3 or @extension{KHR,debug}
(desktop/ES extension, covered also by @extension{ANDROID,extension_pack_es31a}).
Subset of the functionality is provided also by @extension{EXT,debug_marker}
(desktop/ES extensions) or @extension{GREMEDY,string_marker} (desktop only
extension).

With OpenGL 4.3 or @extension{KHR,debug} desktop/ES extension, the debug output
needs to be enabled first. It can be enabled globally using
@ref Platform::Sdl2Application::Configuration::Flag::Debug "Platform::*Application::Configuration::Flag::Debug"
when creating context or only for some portions of the code using
@ref Renderer::Feature::DebugOutput. If enabled globally, some OpenGL drivers
may provide additional debugging information. In addition to that you can
control the output at even finer granularity using @ref setEnabled().

You can gather the messages either through graphics debugger or in the
application itself by setting up message callback using @ref setCallback() or
@ref setDefaultCallback(). You might also want to enable
@ref Renderer::Feature::DebugOutputSynchronous. Example usage, completely with
@ref DebugGroup and @link DebugMessage @endlink:

@code
Renderer::enable(Renderer::Feature::DebugOutput);
Renderer::enable(Renderer::Feature::DebugOutputSynchronous);
DebugOutput::setDefaultCallback();

// Disable rather spammy "Buffer detailed info" debug messages on NVidia drivers
DebugOutput::setEnabled(DebugOutput::Source::Api, DebugOutput::Type::Other, {131185}, false);

{
    DebugGroup group{DebugGroup::Source::Application, 42, "Scene rendering"};

    DebugMessage::insert(DebugMessage::Source::Application, DebugMessage::Type::Marker,
        1337, DebugOutput::Severity::Notification, "Rendering transparent mesh");

    Renderer::enable(Renderer::Feature::Blending);
    mesh.draw(shader);
    Renderer::disable(Renderer::Feature::Blending);

    // ...
}
@endcode

With default callback the group entering/leaving and the inserted message (and
possibly also other messages) will be printed on standard output:

> Debug output: application debug group enter (42): Scene rendering\n
> Debug output: application marker (1337): Rendering transparent mesh\n
> ...\n
> Debug output: application debug group leave (42): Scene rendering

If only @extension{EXT,debug_marker} or @extension{GREMEDY,string_marker} are
supported, only user-inserted messages and debug groups are supported and they
can be seen only through graphics debugger.

If OpenGL 4.3 is not supported and neither @extension{KHR,debug} nor
@extension{EXT,debug_marker} nor @extension{GREMEDY,string_marker} are
available, all the functions are essentially a no-op.

Besides inserting messages into GL command stream you can also annotate OpenGL
objects with labels. See @ref AbstractQuery::setLabel(),
@ref AbstractShaderProgram::setLabel(), @ref AbstractTexture::setLabel(),
@ref Buffer::setLabel(), @ref Framebuffer::setLabel(), @ref Mesh::setLabel(),
@ref Renderbuffer::setLabel(), @ref Shader::setLabel() and
@ref TransformFeedback::setLabel() for more information.

@requires_gles Debug output is not available in WebGL.
*/
class MAGNUM_EXPORT DebugOutput {
    friend Implementation::DebugState;

    public:
        /**
         * @brief Message source
         *
         * @see @ref setEnabled()
         */
        enum class Source: GLenum {
            /** OpenGL */
            #ifndef MAGNUM_TARGET_GLES
            Api = GL_DEBUG_SOURCE_API,
            #else
            Api = GL_DEBUG_SOURCE_API_KHR,
            #endif

            /** Window system (GLX, WGL) */
            #ifndef MAGNUM_TARGET_GLES
            WindowSystem = GL_DEBUG_SOURCE_WINDOW_SYSTEM,
            #else
            WindowSystem = GL_DEBUG_SOURCE_WINDOW_SYSTEM_KHR,
            #endif

            /** Shader compiler */
            #ifndef MAGNUM_TARGET_GLES
            ShaderCompiler = GL_DEBUG_SOURCE_SHADER_COMPILER,
            #else
            ShaderCompiler = GL_DEBUG_SOURCE_SHADER_COMPILER_KHR,
            #endif

            /** External debugger or third-party middleware */
            #ifndef MAGNUM_TARGET_GLES
            ThirdParty = GL_DEBUG_SOURCE_THIRD_PARTY,
            #else
            ThirdParty = GL_DEBUG_SOURCE_THIRD_PARTY_KHR,
            #endif

            /** The application */
            #ifndef MAGNUM_TARGET_GLES
            Application = GL_DEBUG_SOURCE_APPLICATION,
            #else
            Application = GL_DEBUG_SOURCE_APPLICATION_KHR,
            #endif

            /** Any other source */
            #ifndef MAGNUM_TARGET_GLES
            Other = GL_DEBUG_SOURCE_OTHER
            #else
            Other = GL_DEBUG_SOURCE_OTHER_KHR
            #endif
        };

        /**
         * @brief Message type
         *
         * @see @ref setEnabled()
         */
        enum class Type: GLenum {
            /** OpenGL error */
            #ifndef MAGNUM_TARGET_GLES
            Error = GL_DEBUG_TYPE_ERROR,
            #else
            Error = GL_DEBUG_TYPE_ERROR_KHR,
            #endif

            /** Behavior that has been marked for deprecation */
            #ifndef MAGNUM_TARGET_GLES
            DeprecatedBehavior = GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR,
            #else
            DeprecatedBehavior = GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_KHR,
            #endif

            /** Behavior that is undefined according to the specification */
            #ifndef MAGNUM_TARGET_GLES
            UndefinedBehavior = GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR,
            #else
            UndefinedBehavior = GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_KHR,
            #endif

            /** Non-portable usage of extensions or shaders */
            #ifndef MAGNUM_TARGET_GLES
            Portability = GL_DEBUG_TYPE_PORTABILITY,
            #else
            Portability = GL_DEBUG_TYPE_PORTABILITY_KHR,
            #endif

            /** Implementation-dependent performance warning */
            #ifndef MAGNUM_TARGET_GLES
            Performance = GL_DEBUG_TYPE_PERFORMANCE,
            #else
            Performance = GL_DEBUG_TYPE_PERFORMANCE_KHR,
            #endif

            /** Annotation of the command stream */
            #ifndef MAGNUM_TARGET_GLES
            Marker = GL_DEBUG_TYPE_MARKER,
            #else
            Marker = GL_DEBUG_TYPE_MARKER_KHR,
            #endif

            /** Entering a debug group */
            #ifndef MAGNUM_TARGET_GLES
            PushGroup = GL_DEBUG_TYPE_PUSH_GROUP,
            #else
            PushGroup = GL_DEBUG_TYPE_PUSH_GROUP_KHR,
            #endif

            /** Leaving a debug group */
            #ifndef MAGNUM_TARGET_GLES
            PopGroup = GL_DEBUG_TYPE_POP_GROUP,
            #else
            PopGroup = GL_DEBUG_TYPE_POP_GROUP_KHR,
            #endif

            /** Any other type */
            #ifndef MAGNUM_TARGET_GLES
            Other = GL_DEBUG_TYPE_OTHER,
            #else
            Other = GL_DEBUG_TYPE_OTHER_KHR,
            #endif
        };

        /**
         * @brief Message severity
         *
         * @see @ref setEnabled()
         */
        enum class Severity: GLenum {
            /**
             * Any OpenGL error, dangerous undefined behavior, shader
             * compilation errors.
             */
            #ifndef MAGNUM_TARGET_GLES
            High = GL_DEBUG_SEVERITY_HIGH,
            #else
            High = GL_DEBUG_SEVERITY_HIGH_KHR,
            #endif

            /**
             * Severe performance warnings, shader compilation warnings, use of
             * deprecated behavior.
             */
            #ifndef MAGNUM_TARGET_GLES
            Medium = GL_DEBUG_SEVERITY_MEDIUM,
            #else
            Medium = GL_DEBUG_SEVERITY_MEDIUM_KHR,
            #endif

            /** Minor performance warnings, trivial undefined behavior. */
            #ifndef MAGNUM_TARGET_GLES
            Low = GL_DEBUG_SEVERITY_LOW,
            #else
            Low = GL_DEBUG_SEVERITY_LOW_KHR,
            #endif

            /** Any message other than error or performance warning. */
            #ifndef MAGNUM_TARGET_GLES
            Notification = GL_DEBUG_SEVERITY_NOTIFICATION
            #else
            Notification = GL_DEBUG_SEVERITY_NOTIFICATION_KHR
            #endif
        };

        /**
         * @brief Debug callback
         *
         * @see @ref setCallback()
         */
        typedef void(*Callback)(Source, Type, UnsignedInt, Severity, const std::string&, const void*);

        /**
         * @brief Max count of debug messages in log
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If OpenGL 4.3 is not supported and @extension{KHR,debug}
         * desktop or ES extension (covered also by @extension{ANDROID,extension_pack_es31a})
         * is not available, returns `0`.
         * @see @fn_gl{Get} with @def_gl{MAX_DEBUG_LOGGED_MESSAGES}
         */
        static Int maxLoggedMessages();

        /**
         * @brief Max debug message length
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If OpenGL 4.3 is not supported and @extension{KHR,debug}
         * desktop or ES extension (covered also by @extension{ANDROID,extension_pack_es31a})
         * is not available, returns `0`.
         * @see @fn_gl{Get} with @def_gl{MAX_DEBUG_MESSAGE_LENGTH}
         */
        static Int maxMessageLength();

        /**
         * @brief Enable or disable particular output type
         *
         * @attention If any @ref DebugGroup is active when making this call,
         *      the setting will be remembered only for the time in which the
         *      group is active and leaving it will revert the setting to state
         *      set in parent debug group. See @ref DebugGroup documentation
         *      for more information.
         *
         * If OpenGL 4.3 is not supported and @extension{KHR,debug} desktop or
         * ES extension (covered also by @extension{ANDROID,extension_pack_es31a})
         * is not available, this function does nothing.
         * @see @ref Renderer::Feature::DebugOutput, @fn_gl{DebugMessageControl}
         */
        static void setEnabled(Source source, Type type, std::initializer_list<UnsignedInt> ids, bool enabled) {
            setEnabledInternal(GLenum(source), GLenum(type), GL_DONT_CARE, ids, enabled);
        }

        /** @overload */
        static void setEnabled(Source source, Type type, Severity severity, bool enabled) {
            setEnabledInternal(GLenum(source), GLenum(type), GLenum(severity), {}, enabled);
        }

        /** @overload */
        static void setEnabled(Source source, Type type, bool enabled) {
            setEnabledInternal(GLenum(source), GLenum(type), GL_DONT_CARE, {}, enabled);
        }

        /** @overload */
        static void setEnabled(Source source, Severity severity, bool enabled) {
            setEnabledInternal(GLenum(source), GL_DONT_CARE, GLenum(severity), {}, enabled);
        }

        /** @overload */
        static void setEnabled(Source source, bool enabled) {
            setEnabledInternal(GLenum(source), GL_DONT_CARE, GL_DONT_CARE, {}, enabled);
        }

        /** @overload */
        static void setEnabled(Type type, Severity severity, bool enabled) {
            setEnabledInternal(GL_DONT_CARE, GLenum(type), GLenum(severity), {}, enabled);
        }

        /** @overload */
        static void setEnabled(Type type, bool enabled) {
            setEnabledInternal(GL_DONT_CARE, GLenum(type), GL_DONT_CARE, {}, enabled);
        }

        /** @overload */
        static void setEnabled(Severity severity, bool enabled) {
            setEnabledInternal(GL_DONT_CARE, GL_DONT_CARE, GLenum(severity), {}, enabled);
        }

        /** @overload */
        static void setEnabled(bool enabled) {
            setEnabledInternal(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, {}, enabled);
        }

        /**
         * @brief Set debug message callback
         *
         * The messages are sent to the callback only if
         * @ref Renderer::Feature::DebugOutput is enabled. If OpenGL 4.3 is not
         * supported and @extension{KHR,debug} desktop or ES extension (covered
         * also by @extension{ANDROID,extension_pack_es31a}) is not
         * available, this function does nothing.
         * @see @ref setDefaultCallback(),
         *      @ref Renderer::Feature::DebugOutputSynchronous,
         *      @fn_gl{DebugMessageCallback}
         */
        static void setCallback(Callback callback, const void* userParam = nullptr);

        /**
         * @brief Set default debug message callback
         *
         * See @ref setCallback() for more information. The message is printed
         * to @ref Corrade::Utility::Debug "Debug" output in the following
         * format:
         * @code
         * DebugMessage::insert(DebugMessage::Source::Application,
         *      DebugMessage::Type::Marker, 1337, DebugOutput::Severity::Notification, "Hello from OpenGL command stream!");
         * @endcode
         *
         * > Debug output: application marker (1337): Hello from OpenGL command stream!
         */
        static void setDefaultCallback();

        /** @brief There's no point in having an instance of this class */
        DebugOutput() = delete;

    private:
        static void setEnabledInternal(GLenum source, GLenum type, GLenum severity, std::initializer_list<UnsignedInt> ids, bool enabled);
        static MAGNUM_LOCAL void controlImplementationNoOp(GLenum, GLenum, GLenum, std::initializer_list<UnsignedInt>, bool);
        static MAGNUM_LOCAL void controlImplementationKhr(GLenum source, GLenum type, GLenum severity, std::initializer_list<UnsignedInt> ids, bool enabled);

        static MAGNUM_LOCAL void callbackImplementationNoOp(Callback, const void*);
        static MAGNUM_LOCAL void callbackImplementationKhr(Callback callback, const void* userParam);
};

/** @debugoperatorclassenum{Magnum::DebugOutput,Magnum::DebugOutput::Source} */
MAGNUM_EXPORT Debug& operator<<(Debug& debug, DebugOutput::Source value);

/** @debugoperatorclassenum{Magnum::DebugOutput,Magnum::DebugOutput::Type} */
MAGNUM_EXPORT Debug& operator<<(Debug& debug, DebugOutput::Type value);

/** @debugoperatorclassenum{Magnum::DebugOutput,Magnum::DebugOutput::Severity} */
MAGNUM_EXPORT Debug& operator<<(Debug& debug, DebugOutput::Severity value);

/**
@brief Debug message

Allows inserting messages GL command stream with labels, useful for example
with conjunction with various graphics debuggers, such as Apitrace or
gDEBugger.

## Basic usage

See @ref DebugOutput for introduction.

If OpenGL 4.3 is supported or @extension{KHR,debug} desktop or ES extension
(covered also by @extension{ANDROID,extension_pack_es31a}) is available and
default debug output callback is enabled for given kind of messages, the
inserted message will be printed on standard output in the following form:

@code
DebugMessage::insert(DebugMessage::Source::Application, DebugMessage::Type::Marker,
    1337, DebugOutput::Severity::Notification, "Hello from OpenGL command stream!");
@endcode

> Debug output: application marker (1337): Hello from OpenGL command stream!

If only @extension{EXT,debug_marker} or @extension{GREMEDY,string_marker} are
available, the message can be seen only through graphics debugger.

If OpenGL 4.3 is not supported and neither @extension{KHR,debug} nor
@extension{EXT,debug_marker} nor @extension{GREMEDY,string_marker} are
available, the function is essentially a no-op.

## Performance notes

If you ensure that you always use the `const char` overload of @ref insert()
and the debug output is either not supported or turned off, the calls will not
result in any allocations and thus won't have any negative performance effects.

@see @ref DebugGroup

@requires_gles Debug output is not available in WebGL.
*/
class MAGNUM_EXPORT DebugMessage {
    friend Implementation::DebugState;

    public:
        /**
         * @brief Message source
         *
         * @see @ref insert()
         */
        enum class Source: GLenum {
            #ifdef MAGNUM_BUILD_DEPRECATED
            /** @copydoc DebugOutput::Source::Api
             * @deprecated Use @ref DebugOutput::Source::Api instead.
             */
            Api CORRADE_DEPRECATED_ENUM("use DebugOutput::Source::Api instead") = GLenum(DebugOutput::Source::Api),

            /** @copydoc DebugOutput::Source::WindowSystem
             * @deprecated Use @ref DebugOutput::Source::WindowSystem instead.
             */
            WindowSystem CORRADE_DEPRECATED_ENUM("use DebugOutput::Source::WindowSystem instead") = GLenum(DebugOutput::Source::WindowSystem),

            /** @copydoc DebugOutput::Source::ShaderCompiler
             * @deprecated Use @ref DebugOutput::Source::ShaderCompiler instead.
             */
            ShaderCompiler CORRADE_DEPRECATED_ENUM("use DebugOutput::Source::ShaderCompiler instead") = GLenum(DebugOutput::Source::ShaderCompiler),
            #endif

            /** External debugger or third-party middleware */
            #ifndef MAGNUM_TARGET_GLES
            ThirdParty = GL_DEBUG_SOURCE_THIRD_PARTY,
            #else
            ThirdParty = GL_DEBUG_SOURCE_THIRD_PARTY_KHR,
            #endif

            /** The application */
            #ifndef MAGNUM_TARGET_GLES
            Application = GL_DEBUG_SOURCE_APPLICATION,
            #else
            Application = GL_DEBUG_SOURCE_APPLICATION_KHR,
            #endif

            #ifdef MAGNUM_BUILD_DEPRECATED
            /** @copydoc DebugOutput::Source::Other
             * @deprecated Use @ref DebugOutput::Source::Other instead.
             */
            Other CORRADE_DEPRECATED_ENUM("use DebugOutput::Source::Other instead") = GLenum(DebugOutput::Source::Other)
            #endif
        };

        /**
         * @brief Message type
         *
         * @see @ref insert()
         */
        enum class Type: GLenum {
            /** OpenGL error */
            #ifndef MAGNUM_TARGET_GLES
            Error = GL_DEBUG_TYPE_ERROR,
            #else
            Error = GL_DEBUG_TYPE_ERROR_KHR,
            #endif

            /** Behavior that has been marked for deprecation */
            #ifndef MAGNUM_TARGET_GLES
            DeprecatedBehavior = GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR,
            #else
            DeprecatedBehavior = GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_KHR,
            #endif

            /** Behavior that is undefined according to the specification */
            #ifndef MAGNUM_TARGET_GLES
            UndefinedBehavior = GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR,
            #else
            UndefinedBehavior = GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_KHR,
            #endif

            /** Non-portable usage of extensions or shaders */
            #ifndef MAGNUM_TARGET_GLES
            Portability = GL_DEBUG_TYPE_PORTABILITY,
            #else
            Portability = GL_DEBUG_TYPE_PORTABILITY_KHR,
            #endif

            /** Implementation-dependent performance warning */
            #ifndef MAGNUM_TARGET_GLES
            Performance = GL_DEBUG_TYPE_PERFORMANCE,
            #else
            Performance = GL_DEBUG_TYPE_PERFORMANCE_KHR,
            #endif

            /** Annotation of the command stream */
            #ifndef MAGNUM_TARGET_GLES
            Marker = GL_DEBUG_TYPE_MARKER,
            #else
            Marker = GL_DEBUG_TYPE_MARKER_KHR,
            #endif

            /** Any other type */
            #ifndef MAGNUM_TARGET_GLES
            Other = GL_DEBUG_TYPE_OTHER
            #else
            Other = GL_DEBUG_TYPE_OTHER_KHR
            #endif
        };

        #ifdef MAGNUM_BUILD_DEPRECATED
        /** @copybrief DebugOutput::Severity
         * @deprecated Use @ref DebugOutput::Severity instead.
         */
        CORRADE_DEPRECATED("use DebugOutput::Severity instead") typedef DebugOutput::Severity Severity;

        /** @copybrief DebugOutput::Callback
         * @deprecated Use @ref DebugOutput::Callback instead.
         */
        /* Can't mark this as deprecated because compiler then complains when I use it as a parameter in setCallback() */
        typedef CORRADE_DEPRECATED("use DebugOutput::Callback instead") void(*Callback)(DebugMessage::Source, DebugMessage::Type, UnsignedInt, DebugOutput::Severity, const std::string&, const void*);

        /** @copybrief DebugOutput::maxLoggedMessages()
         * @deprecated Use @ref DebugOutput::maxLoggedMessages() instead.
         */
        CORRADE_DEPRECATED("use DebugOutput::maxLoggedMessages() instead") static Int maxLoggedMessages() {
            return DebugOutput::maxLoggedMessages();
        }

        /** @copybrief DebugOutput::maxMessageLength()
         * @deprecated Use @ref DebugOutput::maxMessageLength() instead.
         */
        CORRADE_DEPRECATED("use DebugOutput::maxMessageLength() instead") static Int maxMessageLength() {
            return DebugOutput::maxMessageLength();
        }

        /** @copybrief DebugOutput::setEnabled()
         * @deprecated Use @ref DebugOutput::setEnabled() instead.
         */
        #ifdef DOXYGEN_GENERATING_OUTPUT
        template<class ...T> static void setEnabled(T... args);
        #else
        CORRADE_DEPRECATED("use DebugOutput::setEnabled() instead") static void setEnabled(Source source, Type type, std::initializer_list<UnsignedInt> ids, bool enabled) {
            DebugOutput::setEnabled(DebugOutput::Source(source), DebugOutput::Type(type), ids, enabled);
        }
        CORRADE_DEPRECATED("use DebugOutput::setEnabled() instead") static void setEnabled(Source source, Type type, DebugOutput::Severity severity, bool enabled) {
            DebugOutput::setEnabled(DebugOutput::Source(source), DebugOutput::Type(type), severity, enabled);
        }
        CORRADE_DEPRECATED("use DebugOutput::setEnabled() instead") static void setEnabled(Source source, Type type, bool enabled) {
            DebugOutput::setEnabled(DebugOutput::Source(source), DebugOutput::Type(type), enabled);
        }
        CORRADE_DEPRECATED("use DebugOutput::setEnabled() instead") static void setEnabled(Source source, DebugOutput::Severity severity, bool enabled) {
            DebugOutput::setEnabled(DebugOutput::Source(source), severity, enabled);
        }
        CORRADE_DEPRECATED("use DebugOutput::setEnabled() instead") static void setEnabled(Source source, bool enabled) {
            DebugOutput::setEnabled(DebugOutput::Source(source), enabled);
        }
        CORRADE_DEPRECATED("use DebugOutput::setEnabled() instead") static void setEnabled(Type type, DebugOutput::Severity severity, bool enabled) {
            DebugOutput::setEnabled(DebugOutput::Type(type), severity, enabled);
        }
        CORRADE_DEPRECATED("use DebugOutput::setEnabled() instead") static void setEnabled(Type type, bool enabled) {
            DebugOutput::setEnabled(DebugOutput::Type(type), enabled);
        }
        CORRADE_DEPRECATED("use DebugOutput::setEnabled() instead") static void setEnabled(DebugOutput::Severity severity, bool enabled) {
            DebugOutput::setEnabled(severity, enabled);
        }
        CORRADE_DEPRECATED("use DebugOutput::setEnabled() instead") static void setEnabled(bool enabled) {
            DebugOutput::setEnabled(enabled);
        }
        #endif

        /** @copybrief DebugOutput::setCallback()
         * @deprecated Use @ref DebugOutput::setCallback() instead.
         */
        CORRADE_DEPRECATED("use DebugOutput::setCallback() instead") static void setCallback(DebugOutput::Callback callback, const void* userParam = nullptr) {
            DebugOutput::setCallback(reinterpret_cast<DebugOutput::Callback>(callback), userParam);
        }

        /** @copybrief DebugOutput::setDefaultCallback()
         * @deprecated Use @ref DebugOutput::setDefaultCallback() instead.
         */
        CORRADE_DEPRECATED("use DebugOutput::setDefaultCallback() instead") static void setDefaultCallback() {
            DebugOutput::setDefaultCallback();
        }
        #endif

        /**
         * @brief Insert message
         * @param source    Message source
         * @param type      Message type
         * @param id        Message-specific ID
         * @param severity  Message severity
         * @param string    The actual message
         *
         * If OpenGL 4.3 is not supported and neither @extension{KHR,debug}
         * (covered also by @extension{ANDROID,extension_pack_es31a}) nor
         * @extension{EXT,debug_marker} (desktop or ES extensions) nor
         * @extension{GREMEDY,string_marker} (desktop only extension) are
         * available, this function does nothing.
         *
         * If @extension{KHR,debug} is not available and only @extension{EXT,debug_marker}
         * or @extension{GREMEDY,string_marker} are available, only @p string
         * is used and all other parameters are ignored.
         * @see @ref DebugOutput::maxMessageLength(), @fn_gl{DebugMessageInsert},
         *      @fn_gl_extension{InsertEventMarker,EXT,debug_marker} or
         *      @fn_gl_extension{StringMarker,GREMEDY,string_marker}
         */
        static void insert(Source source, Type type, UnsignedInt id, DebugOutput::Severity severity, const std::string& string) {
            insertInternal(source, type, id, severity, {string.data(), string.size()});
        }

        /** @overload */
        template<std::size_t size> static void insert(Source source, Type type, UnsignedInt id, DebugOutput::Severity severity, const char(&string)[size]) {
            insertInternal(source, type, id, severity, {string, size - 1});
        }

        /** @brief There's no point in having an instance of this class */
        DebugMessage() = delete;

    private:
        static void insertInternal(Source source, Type type, UnsignedInt id, DebugOutput::Severity severity, Containers::ArrayView<const char> string);
        static MAGNUM_LOCAL void insertImplementationNoOp(Source, Type, UnsignedInt, DebugOutput::Severity, Containers::ArrayView<const char>);
        static MAGNUM_LOCAL void insertImplementationKhr(Source source, Type type, UnsignedInt id, DebugOutput::Severity severity, Containers::ArrayView<const char> string);
        static MAGNUM_LOCAL void insertImplementationExt(Source, Type, UnsignedInt, DebugOutput::Severity, Containers::ArrayView<const char> string);
        #ifndef MAGNUM_TARGET_GLES
        static MAGNUM_LOCAL void insertImplementationGremedy(Source, Type, UnsignedInt, DebugOutput::Severity, Containers::ArrayView<const char> string);
        #endif
};

/** @debugoperatorclassenum{Magnum::DebugMessage,Magnum::DebugMessage::Source} */
MAGNUM_EXPORT Debug& operator<<(Debug& debug, DebugMessage::Source value);

/** @debugoperatorclassenum{Magnum::DebugMessage,Magnum::DebugMessage::Type} */
MAGNUM_EXPORT Debug& operator<<(Debug& debug, DebugMessage::Type value);

/**
@brief Debug group

Allows marking portions of GL command stream with labels, useful for example
with conjunction with various graphics debuggers, such as Apitrace or gDEBugger.

## Basic usage

See @ref DebugOutput for introduction.

Easiest way is to push debug group by creating instance and pop it
automatically at the end of scope:
@code
{
    // Push debug group
    DebugGroup group{DebugGroup::Source::Application, 42, "Scene rendering"};

    Renderer::enable(Renderer::Feature::Blending);
    mesh.draw(shader);
    Renderer::disable(Renderer::Feature::Blending);

    // The debug group is popped automatically at the end of the scope
}
@endcode

If, for some reason, you need to pop in different scope, you can call @ref push()
and @ref pop() manually:
@code
DebugGroup group;

group.push(DebugGroup::Source::Application, 42, "Scene rendering");

Renderer::enable(Renderer::Feature::Blending);
mesh.draw(shader);
Renderer::disable(Renderer::Feature::Blending);

group.pop();
@endcode

If OpenGL 4.3 is supported or @extension{KHR,debug} desktop or ES extension
(covered also by @extension{ANDROID,extension_pack_es31a}) is available and
the default debug output callback is enabled for these kinds of messages, the
group entering and leaving will be printed on standard output in the following
form:

> Debug output: application debug group enter (42): Scene rendering\n
> Debug output: application debug group leave (42): Scene rendering

If only @extension{EXT,debug_marker} is available, the group can be seen only
through graphics debugger.

If OpenGL 4.3 is not supported and neither @extension{KHR,debug} nor
@extension{EXT,debug_marker} are available, the functions are essentially a
no-op.

@attention To avoid accidental debug group stack overflow/underflow, you cannot
    call @ref push() again when the group is already pushed onto the stack,
    similarly for @ref pop(). So if you want to have nested debug groups, you
    need to create one instance for each level.

## Interaction with debug output volume control

Besides putting hierarchical messages in debug output, the group also affects
settings done by @ref DebugOutput::setEnabled(). Entering debug group inherits
the settings from previously active debug group, call to
@ref DebugOutput::setEnabled() will be remembered only for
the time in which given group is active and leaving it will revert the setting
to state set in parent debug group. No state is preserved, thus calling
@ref push() after previous @ref pop() will not restore settings done when the
group was active previously.

## Performance notes

If you ensure that you always use the `const char` overload of @ref push()
and the debug output is either not supported or turned off, the calls will not
result in any allocations and thus won't have any negative performance effects.

@see @ref DebugMessage

@requires_gles Debug output is not available in WebGL.
*/
class MAGNUM_EXPORT DebugGroup {
    friend Implementation::DebugState;

    public:
        /**
         * @brief Message source
         *
         * @see @ref DebugGroup(), @ref push()
         */
        enum class Source: GLenum {
            /** External debugger or third-party middleware */
            #ifndef MAGNUM_TARGET_GLES
            ThirdParty = GL_DEBUG_SOURCE_THIRD_PARTY,
            #else
            ThirdParty = GL_DEBUG_SOURCE_THIRD_PARTY_KHR,
            #endif

            /** The application */
            #ifndef MAGNUM_TARGET_GLES
            Application = GL_DEBUG_SOURCE_APPLICATION
            #else
            Application = GL_DEBUG_SOURCE_APPLICATION_KHR
            #endif
        };

        /**
         * @brief Max debug group stack depth
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If OpenGL 4.3 is not supported and @extension{KHR,debug}
         * desktop or ES extension (covered also by @extension{ANDROID,extension_pack_es31a})
         * is not available, returns `0`.
         * @see @fn_gl{Get} with @def_gl{MAX_DEBUG_GROUP_STACK_DEPTH}
         */
        static Int maxStackDepth();

        /**
         * @brief Default constructor
         *
         * Doesn't do anything. Call @ref push() to enter debug group.
         */
        explicit DebugGroup(): _active{false} {}

        /**
         * @brief Constructor
         *
         * Calls @ref push().
         * @see @link ~DebugGroup() @endlink, @ref pop()
         */
        explicit DebugGroup(Source source, UnsignedInt id, const std::string& message): DebugGroup{} {
            push(source, id, message);
        }

        /** @overload */
        template<std::size_t size> explicit DebugGroup(Source source, UnsignedInt id, const char(&message)[size]): DebugGroup{} {
            push(source, id, message);
        }

        /**
         * @brief Destructor
         *
         * If the group is active, calls @ref pop().
         */
        ~DebugGroup() { if(_active) pop(); }

        /**
         * @brief Push debug group onto the stack
         *
         * Expects that the group isn't already pushed on the stack. The group
         * entering message is put into debug output with
         * @ref DebugOutput::Type::PushGroup and
         * @ref DebugOutput::Severity::Notification.
         *
         * If OpenGL 4.3 is not supported and neither @extension{KHR,debug}
         * (covered also by @extension{ANDROID,extension_pack_es31a}) nor
         * @extension{EXT,debug_marker} is available, this function does
         * nothing. If @extension{KHR,debug} is not available and only
         * @extension{EXT,debug_marker} is available, only @p message is used
         * and all other parameters are ignored.
         * @see @ref pop(), @ref maxStackDepth(), @ref DebugOutput::maxMessageLength(),
         *      @ref Renderer::Error::StackOverflow, @fn_gl{PushDebugGroup} or
         *      @fn_gl_extension{PushGroupMarker,EXT,debug_marker}
         */
        void push(Source source, UnsignedInt id, const std::string& message) {
            pushInternal(source, id, {message.data(), message.size()});
        }

        /** @overload */
        template<std::size_t size> void push(Source source, UnsignedInt id, const char(&message)[size]) {
            pushInternal(source, id, {message, size - 1});
        }

        /**
         * @brief Pop debug group from the stack
         *
         * Expects that the group is currently pushed on the stack. Leaving the
         * group will also revert all @ref DebugOutput::setEnabled() settings
         * done when the group was active. See class documentation for more
         * information. The group leaving message is put into debug output with
         * @ref DebugOutput::Type::PopGroup and
         * @ref DebugOutput::Severity::Notification.
         *
         * If OpenGL 4.3 is not supported and neither @extension{KHR,debug}
         * (covered also by @extension{ANDROID,extension_pack_es31a}) nor
         * @extension{EXT,debug_marker} is available, this function does
         * nothing.
         * @see @ref push(), @ref Renderer::Error::StackUnderflow,
         *      @fn_gl{PopDebugGroup} or
         *      @fn_gl_extension{PopGroupMarker,EXT,debug_marker}
         */
        void pop();

    private:
        void pushInternal(Source source, UnsignedInt id, Containers::ArrayView<const char> message);

        static MAGNUM_LOCAL void pushImplementationNoOp(Source source, UnsignedInt id, Containers::ArrayView<const char> message);
        static MAGNUM_LOCAL void pushImplementationKhr(Source source, UnsignedInt id, Containers::ArrayView<const char> message);
        static MAGNUM_LOCAL void pushImplementationExt(Source source, UnsignedInt id, Containers::ArrayView<const char> message);

        static MAGNUM_LOCAL void popImplementationNoOp();
        static MAGNUM_LOCAL void popImplementationKhr();
        static MAGNUM_LOCAL void popImplementationExt();

        bool _active;
};

/** @debugoperatorclassenum{Magnum::DebugGroup,Magnum::DebugGroup::Source} */
MAGNUM_EXPORT Debug& operator<<(Debug& debug, DebugGroup::Source value);

}
#else
#error this header is not available in WebGL build
#endif

#endif
