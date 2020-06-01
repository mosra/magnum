#ifndef Magnum_GL_DebugOutput_h
#define Magnum_GL_DebugOutput_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Class @ref Magnum::GL::DebugOutput, @ref Magnum::GL::DebugMessage, @ref Magnum::GL::DebugGroup
 */
#endif

#include <string>
#include <Corrade/Containers/ArrayView.h>

#include "Magnum/Magnum.h"
#include "Magnum/GL/OpenGL.h"
#include "Magnum/GL/visibility.h"

#ifndef MAGNUM_TARGET_WEBGL
namespace Magnum { namespace GL {

namespace Implementation { struct DebugState; }

/**
@brief Debug output

Manages OpenGL debug output. The debug messages are emitted either from driver
(such as GL error descriptions and various performance and optimization hints)
or from third party software and the application itself using @ref DebugMessage
and @ref DebugGroup, which can be also used to mark various portions of command
stream in various graphics debuggers, such as ApiTrace or gDEBugger.

@section GL-Magnum-DebugOutput-usage Basic usage

Support for debug output is provided by OpenGL 4.3 / OpenGL ES 3.2 or
@gl_extension{KHR,debug} (desktop/ES extension, covered also by
@gl_extension{ANDROID,extension_pack_es31a}). Subset of the functionality is
provided also by @gl_extension{EXT,debug_marker} (desktop/ES extensions) or
@gl_extension{GREMEDY,string_marker} (desktop only extension).

With OpenGL 4.3 / OpenGL ES 3.2 or @gl_extension{KHR,debug} desktop/ES extension,
the debug output needs to be enabled first. It can be enabled globally using
@ref Platform::Sdl2Application::GLConfiguration::Flag::Debug "Platform::*Application::GLConfiguration::Flag::Debug"
when creating context or only for some portions of the code using
@ref Renderer::Feature::DebugOutput. If enabled globally, some OpenGL drivers
may provide additional debugging information. In addition to that you can
control the output at even finer granularity using @ref setEnabled().

You can gather the messages either through graphics debugger or in the
application itself by setting up message callback using @ref setCallback() or
@ref setDefaultCallback(). You might also want to enable
@ref Renderer::Feature::DebugOutputSynchronous. Example usage, completely with
@ref DebugGroup and @ref DebugMessage is below.

@m_class{m-block m-success}

@par Enabling debug output from the command line / environment
    Apart from setting up the debug output callbacks manually, it's also
    possible to enable it conveniently using the `--magnum-gpu-validation`
    @ref GL-Context-command-line "command-line or environment option" --- ideal
    for quick debugging of rendering issues. If you are using application
    classes from the @ref Platform namespace, this option also ensures that
    @ref Platform::Sdl2Application::GLConfiguration::Flag::Debug "GLConfiguration::Flag::Debug"
    is passed for context creation, both with windowed and windowless
    application implementations.

@snippet MagnumGL.cpp DebugOutput-usage

With default callback the group entering/leaving and the inserted message (and
possibly also other messages) will be printed on standard output:

@code{.shell-session}
Debug output: application debug group enter (42): Scene rendering
Debug output: application marker (1337): Rendering transparent mesh
...
Debug output: application debug group leave (42): Scene rendering
@endcode

If only @gl_extension{EXT,debug_marker} or @gl_extension{GREMEDY,string_marker} are
supported, only user-inserted messages and debug groups are supported and they
can be seen only through graphics debugger.

If OpenGL 4.3 is not supported and neither @gl_extension{KHR,debug} nor
@gl_extension{EXT,debug_marker} nor @gl_extension{GREMEDY,string_marker} are
available, all the functions are essentially a no-op.

Besides inserting messages into GL command stream you can also annotate OpenGL
objects with labels. See @ref AbstractQuery::setLabel(),
@ref AbstractShaderProgram::setLabel(), @ref AbstractTexture::setLabel(),
@ref Buffer::setLabel(), @ref Framebuffer::setLabel(), @ref Mesh::setLabel(),
@ref Renderbuffer::setLabel(), @ref Shader::setLabel() and
@ref TransformFeedback::setLabel() for more information.

@requires_gles Debug output is not available in WebGL.
*/
class MAGNUM_GL_EXPORT DebugOutput {
    friend Implementation::DebugState;

    public:
        /**
         * @brief Message source
         *
         * @see @ref setEnabled()
         * @m_enum_values_as_keywords
         */
        enum class Source: GLenum {
            /** OpenGL */
            #ifndef MAGNUM_TARGET_GLES2
            Api = GL_DEBUG_SOURCE_API,
            #else
            Api = GL_DEBUG_SOURCE_API_KHR,
            #endif

            /** Window system (GLX, WGL) */
            #ifndef MAGNUM_TARGET_GLES2
            WindowSystem = GL_DEBUG_SOURCE_WINDOW_SYSTEM,
            #else
            WindowSystem = GL_DEBUG_SOURCE_WINDOW_SYSTEM_KHR,
            #endif

            /** Shader compiler */
            #ifndef MAGNUM_TARGET_GLES2
            ShaderCompiler = GL_DEBUG_SOURCE_SHADER_COMPILER,
            #else
            ShaderCompiler = GL_DEBUG_SOURCE_SHADER_COMPILER_KHR,
            #endif

            /** External debugger or third-party middleware */
            #ifndef MAGNUM_TARGET_GLES2
            ThirdParty = GL_DEBUG_SOURCE_THIRD_PARTY,
            #else
            ThirdParty = GL_DEBUG_SOURCE_THIRD_PARTY_KHR,
            #endif

            /** The application */
            #ifndef MAGNUM_TARGET_GLES2
            Application = GL_DEBUG_SOURCE_APPLICATION,
            #else
            Application = GL_DEBUG_SOURCE_APPLICATION_KHR,
            #endif

            /** Any other source */
            #ifndef MAGNUM_TARGET_GLES2
            Other = GL_DEBUG_SOURCE_OTHER
            #else
            Other = GL_DEBUG_SOURCE_OTHER_KHR
            #endif
        };

        /**
         * @brief Message type
         *
         * @see @ref setEnabled()
         * @m_enum_values_as_keywords
         */
        enum class Type: GLenum {
            /** OpenGL error */
            #ifndef MAGNUM_TARGET_GLES2
            Error = GL_DEBUG_TYPE_ERROR,
            #else
            Error = GL_DEBUG_TYPE_ERROR_KHR,
            #endif

            /** Behavior that has been marked for deprecation */
            #ifndef MAGNUM_TARGET_GLES2
            DeprecatedBehavior = GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR,
            #else
            DeprecatedBehavior = GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_KHR,
            #endif

            /** Behavior that is undefined according to the specification */
            #ifndef MAGNUM_TARGET_GLES2
            UndefinedBehavior = GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR,
            #else
            UndefinedBehavior = GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_KHR,
            #endif

            /** Non-portable usage of extensions or shaders */
            #ifndef MAGNUM_TARGET_GLES2
            Portability = GL_DEBUG_TYPE_PORTABILITY,
            #else
            Portability = GL_DEBUG_TYPE_PORTABILITY_KHR,
            #endif

            /** Implementation-dependent performance warning */
            #ifndef MAGNUM_TARGET_GLES2
            Performance = GL_DEBUG_TYPE_PERFORMANCE,
            #else
            Performance = GL_DEBUG_TYPE_PERFORMANCE_KHR,
            #endif

            /** Annotation of the command stream */
            #ifndef MAGNUM_TARGET_GLES2
            Marker = GL_DEBUG_TYPE_MARKER,
            #else
            Marker = GL_DEBUG_TYPE_MARKER_KHR,
            #endif

            /** Entering a debug group */
            #ifndef MAGNUM_TARGET_GLES2
            PushGroup = GL_DEBUG_TYPE_PUSH_GROUP,
            #else
            PushGroup = GL_DEBUG_TYPE_PUSH_GROUP_KHR,
            #endif

            /** Leaving a debug group */
            #ifndef MAGNUM_TARGET_GLES2
            PopGroup = GL_DEBUG_TYPE_POP_GROUP,
            #else
            PopGroup = GL_DEBUG_TYPE_POP_GROUP_KHR,
            #endif

            /** Any other type */
            #ifndef MAGNUM_TARGET_GLES2
            Other = GL_DEBUG_TYPE_OTHER,
            #else
            Other = GL_DEBUG_TYPE_OTHER_KHR,
            #endif
        };

        /**
         * @brief Message severity
         *
         * @see @ref setEnabled()
         * @m_enum_values_as_keywords
         */
        enum class Severity: GLenum {
            /**
             * Any OpenGL error, dangerous undefined behavior, shader
             * compilation errors.
             */
            #ifndef MAGNUM_TARGET_GLES2
            High = GL_DEBUG_SEVERITY_HIGH,
            #else
            High = GL_DEBUG_SEVERITY_HIGH_KHR,
            #endif

            /**
             * Severe performance warnings, shader compilation warnings, use of
             * deprecated behavior.
             */
            #ifndef MAGNUM_TARGET_GLES2
            Medium = GL_DEBUG_SEVERITY_MEDIUM,
            #else
            Medium = GL_DEBUG_SEVERITY_MEDIUM_KHR,
            #endif

            /** Minor performance warnings, trivial undefined behavior. */
            #ifndef MAGNUM_TARGET_GLES2
            Low = GL_DEBUG_SEVERITY_LOW,
            #else
            Low = GL_DEBUG_SEVERITY_LOW_KHR,
            #endif

            /** Any message other than error or performance warning. */
            #ifndef MAGNUM_TARGET_GLES2
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
         * OpenGL calls. If OpenGL 4.3 / OpenGL ES 3.2 is not supported and
         * @gl_extension{KHR,debug} desktop or ES extension (covered also by
         * @gl_extension{ANDROID,extension_pack_es31a}) is not available, returns
         * @cpp 0 @ce.
         * @see @fn_gl{Get} with @def_gl_keyword{MAX_DEBUG_LOGGED_MESSAGES}
         */
        static Int maxLoggedMessages();

        /**
         * @brief Max debug message length
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If OpenGL 4.3 / OpenGL ES 3.2 is not supported and
         * @gl_extension{KHR,debug} desktop or ES extension (covered also by
         * @gl_extension{ANDROID,extension_pack_es31a}) is not available, returns
         * @cpp 0 @ce.
         * @see @fn_gl{Get} with @def_gl_keyword{MAX_DEBUG_MESSAGE_LENGTH}
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
         * If OpenGL 4.3 / OpenGL ES 3.2 is not supported and @gl_extension{KHR,debug}
         * desktop or ES extension (covered also by
         * @gl_extension{ANDROID,extension_pack_es31a}) is not available, this
         * function does nothing.
         * @see @ref Renderer::Feature::DebugOutput,
         *      @fn_gl_keyword{DebugMessageControl}
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
         * @ref Renderer::Feature::DebugOutput is enabled. If OpenGL 4.3 /
         * OpenGL ES 3.2 is not supported and @gl_extension{KHR,debug} desktop or
         * ES extension (covered also by @gl_extension{ANDROID,extension_pack_es31a})
         * is not available, this function does nothing.
         *
         * @attention The function is not necessarily called from the same
         *      thread as the one that caused the message to appear --- in
         *      particular, you can't assume the @ref GL::Context will be
         *      present in the callback context. It might work on some drivers,
         *      but not on others.
         *
         * @see @ref setDefaultCallback(),
         *      @ref Renderer::Feature::DebugOutputSynchronous,
         *      @fn_gl_keyword{DebugMessageCallback}
         */
        static void setCallback(Callback callback, const void* userParam = nullptr);

        /**
         * @brief Set default debug message callback
         *
         * See @ref setCallback() for more information. The message is printed
         * to @ref Corrade::Utility::Debug "Debug" output in the following
         * format:
         *
         * @snippet MagnumGL.cpp DebugOutput-setDefaultCallback
         *
         * @code{.shell-session}
         * Debug output: application marker (1337): Hello from OpenGL command stream!
         * @endcode
         */
        static void setDefaultCallback();

        /** @brief There's no point in having an instance of this class */
        DebugOutput() = delete;

    private:
        static void setEnabledInternal(GLenum source, GLenum type, GLenum severity, std::initializer_list<UnsignedInt> ids, bool enabled);
        static MAGNUM_GL_LOCAL void controlImplementationNoOp(GLenum, GLenum, GLenum, std::initializer_list<UnsignedInt>, bool);
        #ifndef MAGNUM_TARGET_GLES2
        static MAGNUM_GL_LOCAL void controlImplementationKhrDesktopES32(GLenum source, GLenum type, GLenum severity, std::initializer_list<UnsignedInt> ids, bool enabled);
        #endif
        #ifdef MAGNUM_TARGET_GLES
        static MAGNUM_GL_LOCAL void controlImplementationKhrES(GLenum source, GLenum type, GLenum severity, std::initializer_list<UnsignedInt> ids, bool enabled);
        #endif

        static MAGNUM_GL_LOCAL void callbackImplementationNoOp(Callback, const void*);
        #ifndef MAGNUM_TARGET_GLES2
        static MAGNUM_GL_LOCAL void callbackImplementationKhrDesktopES32(Callback callback, const void* userParam);
        #endif
        #ifdef MAGNUM_TARGET_GLES
        static MAGNUM_GL_LOCAL void callbackImplementationKhrES(Callback callback, const void* userParam);
        #endif
};

/** @debugoperatorclassenum{DebugOutput,DebugOutput::Source} */
MAGNUM_GL_EXPORT Debug& operator<<(Debug& debug, DebugOutput::Source value);

/** @debugoperatorclassenum{DebugOutput,DebugOutput::Type} */
MAGNUM_GL_EXPORT Debug& operator<<(Debug& debug, DebugOutput::Type value);

/** @debugoperatorclassenum{DebugOutput,DebugOutput::Severity} */
MAGNUM_GL_EXPORT Debug& operator<<(Debug& debug, DebugOutput::Severity value);

/**
@brief Debug message

Allows inserting messages GL command stream with labels, useful for example
with conjunction with various graphics debuggers, such as ApiTrace or
gDEBugger.

@section GL-DebugMessage-usage Basic usage

See @ref DebugOutput for introduction.

If OpenGL 4.3 / OpenGL ES 3.2 is supported or @gl_extension{KHR,debug} desktop or
ES extension (covered also by @gl_extension{ANDROID,extension_pack_es31a}) is
available and default debug output callback is enabled for given kind of
messages, the inserted message will be printed on standard output in the
following form:

@snippet MagnumGL.cpp DebugMessage-usage

<p>
@code{.shell-session}
Debug output: application marker (1337): Hello from OpenGL command stream!
@endcode
</p>

If only @gl_extension{EXT,debug_marker} or @gl_extension{GREMEDY,string_marker} are
available, the message can be seen only through graphics debugger.

If OpenGL 4.3 is not supported and neither @gl_extension{KHR,debug} nor
@gl_extension{EXT,debug_marker} nor @gl_extension{GREMEDY,string_marker} are
available, the function is essentially a no-op.

@section GL-DebugMessage-performance-notes Performance notes

If you ensure that you always use the @cpp const char @ce overload of
@ref insert() and the debug output is either not supported or turned off, the
calls will not result in any allocations and thus won't have any negative
performance effects.

@see @ref DebugGroup
@requires_gles Debug output is not available in WebGL.
*/
class MAGNUM_GL_EXPORT DebugMessage {
    friend Implementation::DebugState;

    public:
        /**
         * @brief Message source
         *
         * @see @ref insert()
         * @m_enum_values_as_keywords
         */
        enum class Source: GLenum {
            /** External debugger or third-party middleware */
            #ifndef MAGNUM_TARGET_GLES2
            ThirdParty = GL_DEBUG_SOURCE_THIRD_PARTY,
            #else
            ThirdParty = GL_DEBUG_SOURCE_THIRD_PARTY_KHR,
            #endif

            /** The application */
            #ifndef MAGNUM_TARGET_GLES2
            Application = GL_DEBUG_SOURCE_APPLICATION
            #else
            Application = GL_DEBUG_SOURCE_APPLICATION_KHR
            #endif
        };

        /**
         * @brief Message type
         *
         * @see @ref insert()
         * @m_enum_values_as_keywords
         */
        enum class Type: GLenum {
            /** OpenGL error */
            #ifndef MAGNUM_TARGET_GLES2
            Error = GL_DEBUG_TYPE_ERROR,
            #else
            Error = GL_DEBUG_TYPE_ERROR_KHR,
            #endif

            /** Behavior that has been marked for deprecation */
            #ifndef MAGNUM_TARGET_GLES2
            DeprecatedBehavior = GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR,
            #else
            DeprecatedBehavior = GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_KHR,
            #endif

            /** Behavior that is undefined according to the specification */
            #ifndef MAGNUM_TARGET_GLES2
            UndefinedBehavior = GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR,
            #else
            UndefinedBehavior = GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_KHR,
            #endif

            /** Non-portable usage of extensions or shaders */
            #ifndef MAGNUM_TARGET_GLES2
            Portability = GL_DEBUG_TYPE_PORTABILITY,
            #else
            Portability = GL_DEBUG_TYPE_PORTABILITY_KHR,
            #endif

            /** Implementation-dependent performance warning */
            #ifndef MAGNUM_TARGET_GLES2
            Performance = GL_DEBUG_TYPE_PERFORMANCE,
            #else
            Performance = GL_DEBUG_TYPE_PERFORMANCE_KHR,
            #endif

            /** Annotation of the command stream */
            #ifndef MAGNUM_TARGET_GLES2
            Marker = GL_DEBUG_TYPE_MARKER,
            #else
            Marker = GL_DEBUG_TYPE_MARKER_KHR,
            #endif

            /** Any other type */
            #ifndef MAGNUM_TARGET_GLES2
            Other = GL_DEBUG_TYPE_OTHER
            #else
            Other = GL_DEBUG_TYPE_OTHER_KHR
            #endif
        };

        /**
         * @brief Insert message
         * @param source    Message source
         * @param type      Message type
         * @param id        Message-specific ID
         * @param severity  Message severity
         * @param string    The actual message
         *
         * If OpenGL 4.3 / OpenGL ES 3.2 is not supported and neither
         * @gl_extension{KHR,debug} (covered also by @gl_extension{ANDROID,extension_pack_es31a})
         * nor @gl_extension{EXT,debug_marker} (desktop or ES extensions) nor
         * @gl_extension{GREMEDY,string_marker} (desktop only extension) are
         * available, this function does nothing.
         *
         * If @gl_extension{KHR,debug} is not available and only @gl_extension{EXT,debug_marker}
         * or @gl_extension{GREMEDY,string_marker} are available, only @p string
         * is used and all other parameters are ignored.
         * @see @ref DebugOutput::maxMessageLength(),
         *      @fn_gl_keyword{DebugMessageInsert},
         *      @fn_gl_extension_keyword{InsertEventMarker,EXT,debug_marker} or
         *      @fn_gl_extension_keyword{StringMarker,GREMEDY,string_marker}
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
        static MAGNUM_GL_LOCAL void insertImplementationNoOp(Source, Type, UnsignedInt, DebugOutput::Severity, Containers::ArrayView<const char>);
        #ifndef MAGNUM_TARGET_GLES2
        static MAGNUM_GL_LOCAL void insertImplementationKhrDesktopES32(Source source, Type type, UnsignedInt id, DebugOutput::Severity severity, Containers::ArrayView<const char> string);
        #endif
        #ifdef MAGNUM_TARGET_GLES
        static MAGNUM_GL_LOCAL void insertImplementationKhrES(Source source, Type type, UnsignedInt id, DebugOutput::Severity severity, Containers::ArrayView<const char> string);
        #endif
        static MAGNUM_GL_LOCAL void insertImplementationExt(Source, Type, UnsignedInt, DebugOutput::Severity, Containers::ArrayView<const char> string);
        #ifndef MAGNUM_TARGET_GLES
        static MAGNUM_GL_LOCAL void insertImplementationGremedy(Source, Type, UnsignedInt, DebugOutput::Severity, Containers::ArrayView<const char> string);
        #endif
};

/** @debugoperatorclassenum{DebugMessage,DebugMessage::Source} */
MAGNUM_GL_EXPORT Debug& operator<<(Debug& debug, DebugMessage::Source value);

/** @debugoperatorclassenum{DebugMessage,DebugMessage::Type} */
MAGNUM_GL_EXPORT Debug& operator<<(Debug& debug, DebugMessage::Type value);

/**
@brief Debug group

Allows marking portions of GL command stream with labels, useful for example
with conjunction with various graphics debuggers, such as Apitrace or gDEBugger.

@section GL-DebugGroup-usage Basic usage

See @ref DebugOutput for introduction.

Easiest way is to push debug group by creating instance and pop it
automatically at the end of scope:

@snippet MagnumGL.cpp DebugGroup-usage1

If, for some reason, you need to pop in different scope, you can call @ref push()
and @ref pop() manually:

@snippet MagnumGL.cpp DebugGroup-usage2

If OpenGL 4.3 / OpenGL ES 3.2 is supported or @gl_extension{KHR,debug} desktop or
ES extension (covered also by @gl_extension{ANDROID,extension_pack_es31a}) is
available and the default debug output callback is enabled for these kinds of
messages, the group entering and leaving will be printed on standard output in
the following form:

@code{.shell-session}
Debug output: application debug group enter (42): Scene rendering
Debug output: application debug group leave (42): Scene rendering
@endcode

If only @gl_extension{EXT,debug_marker} is available, the group can be seen only
through graphics debugger.

If OpenGL 4.3 / OpenGL ES 3.2 is not supported and neither @gl_extension{KHR,debug}
nor @gl_extension{EXT,debug_marker} are available, the functions are essentially a
no-op.

@attention To avoid accidental debug group stack overflow/underflow, you cannot
    call @ref push() again when the group is already pushed onto the stack,
    similarly for @ref pop(). So if you want to have nested debug groups, you
    need to create one instance for each level.

@section GL-DebugGroup-volume-control Interaction with debug output volume control

Besides putting hierarchical messages in debug output, the group also affects
settings done by @ref DebugOutput::setEnabled(). Entering debug group inherits
the settings from previously active debug group, call to
@ref DebugOutput::setEnabled() will be remembered only for
the time in which given group is active and leaving it will revert the setting
to state set in parent debug group. No state is preserved, thus calling
@ref push() after previous @ref pop() will not restore settings done when the
group was active previously.

@section GL-DebugGroup-performance-notes Performance notes

If you ensure that you always use the @cpp const char @ce overload of
@ref push() and the debug output is either not supported or turned off, the
calls will not result in any allocations and thus won't have any negative
performance effects.

@see @ref DebugMessage
@requires_gles Debug output is not available in WebGL.
*/
class MAGNUM_GL_EXPORT DebugGroup {
    friend Implementation::DebugState;

    public:
        /**
         * @brief Message source
         *
         * @see @ref DebugGroup(), @ref push()
         */
        enum class Source: GLenum {
            /** External debugger or third-party middleware */
            #ifndef MAGNUM_TARGET_GLES2
            ThirdParty = GL_DEBUG_SOURCE_THIRD_PARTY,
            #else
            ThirdParty = GL_DEBUG_SOURCE_THIRD_PARTY_KHR,
            #endif

            /** The application */
            #ifndef MAGNUM_TARGET_GLES2
            Application = GL_DEBUG_SOURCE_APPLICATION
            #else
            Application = GL_DEBUG_SOURCE_APPLICATION_KHR
            #endif
        };

        /**
         * @brief Max debug group stack depth
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If OpenGL 4.3 / OpenGL ES 3.2 is not supported and
         * @gl_extension{KHR,debug} desktop or ES extension (covered also by
         * @gl_extension{ANDROID,extension_pack_es31a}) is not available, returns
         * `0`.
         * @see @fn_gl{Get} with @def_gl_keyword{MAX_DEBUG_GROUP_STACK_DEPTH}
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
         * If OpenGL 4.3 / OpenGL ES 3.2 is not supported and neither
         * @gl_extension{KHR,debug} (covered also by @gl_extension{ANDROID,extension_pack_es31a})
         * nor @gl_extension{EXT,debug_marker} is available, this function does
         * nothing. If @gl_extension{KHR,debug} is not available and only
         * @gl_extension{EXT,debug_marker} is available, only @p message is used
         * and all other parameters are ignored.
         * @see @ref pop(), @ref maxStackDepth(), @ref DebugOutput::maxMessageLength(),
         *      @ref Renderer::Error::StackOverflow, @fn_gl_keyword{PushDebugGroup}
         *      or @fn_gl_extension_keyword{PushGroupMarker,EXT,debug_marker}
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
         * If OpenGL 4.3 / OpenGL ES 3.2 is not supported and neither
         * @gl_extension{KHR,debug} (covered also by @gl_extension{ANDROID,extension_pack_es31a})
         * nor @gl_extension{EXT,debug_marker} is available, this function does
         * nothing.
         * @see @ref push(), @ref Renderer::Error::StackUnderflow,
         *      @fn_gl_keyword{PopDebugGroup} or
         *      @fn_gl_extension_keyword{PopGroupMarker,EXT,debug_marker}
         */
        void pop();

    private:
        void pushInternal(Source source, UnsignedInt id, Containers::ArrayView<const char> message);

        static MAGNUM_GL_LOCAL void pushImplementationNoOp(Source source, UnsignedInt id, Containers::ArrayView<const char> message);
        #ifndef MAGNUM_TARGET_GLES2
        static MAGNUM_GL_LOCAL void pushImplementationKhrDesktopES32(Source source, UnsignedInt id, Containers::ArrayView<const char> message);
        #endif
        #ifdef MAGNUM_TARGET_GLES
        static MAGNUM_GL_LOCAL void pushImplementationKhrES(Source source, UnsignedInt id, Containers::ArrayView<const char> message);
        #endif
        static MAGNUM_GL_LOCAL void pushImplementationExt(Source source, UnsignedInt id, Containers::ArrayView<const char> message);

        static MAGNUM_GL_LOCAL void popImplementationNoOp();
        #ifndef MAGNUM_TARGET_GLES2
        static MAGNUM_GL_LOCAL void popImplementationKhrDesktopES32();
        #endif
        #ifdef MAGNUM_TARGET_GLES
        static MAGNUM_GL_LOCAL void popImplementationKhrES();
        #endif
        static MAGNUM_GL_LOCAL void popImplementationExt();

        bool _active;
};

/** @debugoperatorclassenum{DebugGroup,DebugGroup::Source} */
MAGNUM_GL_EXPORT Debug& operator<<(Debug& debug, DebugGroup::Source value);

/* Exposed for testing */
namespace Implementation {
    MAGNUM_GL_EXPORT void defaultDebugCallback(DebugOutput::Source source, DebugOutput::Type type, UnsignedInt id, DebugOutput::Severity severity, const std::string& string, std::ostream* output);
}

}}
#else
#error this header is not available in WebGL build
#endif

#endif
