#ifndef Magnum_DebugMessage_h
#define Magnum_DebugMessage_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014
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

/** @file
 * @brief Class @ref Magnum::DebugMessage
 */

#include <string>
#include <Corrade/Containers/Array.h>

#include "Magnum/Magnum.h"
#include "Magnum/OpenGL.h"
#include "Magnum/visibility.h"

namespace Magnum {

namespace Implementation { struct DebugState; }

/**
@brief Debug message

Allows retrieving and inserting debug messages from and to OpenGL command
stream, for example with conjunction with various debuggers, such as Apitrace
or gDEBugger.

## Basic usage

To retrieve debug messages from either the GL or your application you need to
have OpenGL 4.3 or @extension{KHR,debug} desktop/ES extension. You need to
enable @ref Renderer::Feature::DebugOutput and possibly also
@ref Renderer::Feature::DebugOutputSynchronous. Then set up message callback
using @ref setCallback() or use the default one provided in
@ref setDefaultCallback():

@code
Renderer::enable(Renderer::Feature::DebugOutput);
Renderer::enable(Renderer::Feature::DebugOutputSynchronous);

DebugMessage::setDefaultCallback();
DebugMessage::insert(DebugMessage::Source::Application, DebugMessage::Type::Marker,
    1337, DebugMessage::Severity::Notification, "Hello from OpenGL command stream!");
@endcode

With default callback the messages will be printed on standard output:

    DebugMessage::Source::Application DebugMessage::Type::Marker -1 DebugMessage::Severity::Notification
        Hello from OpenGL command stream!
*/
class MAGNUM_EXPORT DebugMessage {
    friend struct Implementation::DebugState;

    public:
        /**
         * @brief Message source
         *
         * @see @ref insert(), @ref setCallback()
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

            /** %Shader compiler */
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
         * @see @ref insert(), @ref setCallback()
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

            /** Any other type */
            #ifndef MAGNUM_TARGET_GLES
            Other = GL_DEBUG_TYPE_OTHER,
            #else
            Other = GL_DEBUG_TYPE_OTHER_KHR,
            #endif

            /** Annotation of the command stream */
            #ifndef MAGNUM_TARGET_GLES
            Marker = GL_DEBUG_TYPE_MARKER
            #else
            Marker = GL_DEBUG_TYPE_MARKER_KHR
            #endif
        };

        /**
         * @brief Message severity
         *
         * @see @ref insert(), @ref setCallback()
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
         * desktop or ES extension is not available, returns `0`.
         * @see @fn_gl{Get} with @def_gl{MAX_DEBUG_LOGGED_MESSAGES}
         */
        static Int maxLoggedMessages();

        /**
         * @brief Max debug message length
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If OpenGL 4.3 is not supported and @extension{KHR,debug}
         * desktop or ES extension is not available, returns `0`.
         * @see @fn_gl{Get} with @def_gl{MAX_DEBUG_MESSAGE_LENGTH}
         */
        static Int maxMessageLength();

        /**
         * @brief Insert message
         * @param source    Message source. Allowed values are
         *      @ref Source::ThirdParty  or @ref Source::Application.
         * @param type      Message type
         * @param id        Message-specific ID
         * @param severity  Message severity
         * @param string    The actual message
         *
         * If OpenGL 4.3 is not supported and none of @extension{KHR,debug},
         * @extension2{EXT,debug_marker} or @extension{GREMEDY,string_marker}
         * is available, this function does nothing.
         *
         * If @extension{KHR,debug} is not available and only @extension2{EXT,debug_marker}
         * or @extension{GREMEDY,string_marker} are available, only @p string
         * is used and all other parameters are ignored. The call is then
         * equivalent to the following:
         * @code
         * DebugMessage::insert(DebugMessage::Source::Application,
         *      DebugMessage::Type::Marker, 0, DebugMessage::Severity::Notification, string);
         * @endcode
         *
         * @see @ref maxMessageLength(), @fn_gl{DebugMessageInsert},
         *      @fn_gl_extension2{InsertEventMarker,EXT,debug_marker} or
         *      @fn_gl_extension{StringMarker,GREMEDY,string_marker}
         */
        static void insert(Source source, Type type, UnsignedInt id, Severity severity, const std::string& string) {
            insertInternal(source, type, id, severity, {string.data(), string.size()});
        }

        /** @overload */
        template<std::size_t size> static void insert(Source source, Type type, UnsignedInt id, Severity severity, const char(&string)[size]) {
            insertInternal(source, type, id, severity, {string, size - 1});
        }

        /**
         * @brief Set debug message callback
         *
         * The messages are sent to the callback only if
         * @ref Renderer::Feature::DebugOutput is enabled. If OpenGL 4.3 is not
         * supported and @extension{KHR,debug} is not available, this function
         * does nothing.
         * @see @ref setDefaultCallback(),
         *      @ref Renderer::Feature::DebugOutputSynchronous,
         *      @fn_gl{DebugMessageCallback}
         */
        static void setCallback(Callback callback, const void* userParam = nullptr);

        /**
         * @brief Set default debug message callback
         *
         * See @ref setCallback() for more information. The message is printed
         * to either @ref Corrade::Utility::Error "Error", @ref Corrade::Utility::Warning "Warning"
         * or @ref Corrade::Utility::Debug "Debug" in the following format:
         * @code
         * DebugMessage::insert(DebugMessage::Source::Application,
         *      DebugMessage::Type::Marker, 1337, DebugMessage::Severity::Notification, "Hello from OpenGL command stream!");
         * @endcode
         *
         * > %DebugMessage::Source::Application %DebugMessage::Type::Marker -1 %DebugMessage::Severity::Notification\n
         * > &nbsp;&nbsp;&nbsp;&nbsp;Hello from OpenGL command stream!
         */
        static void setDefaultCallback();

        DebugMessage() = delete;

    private:
        static void insertInternal(Source source, Type type, UnsignedInt id, Severity severity, Containers::ArrayReference<const char> string);
        static MAGNUM_LOCAL void insertImplementationNoOp(Source, Type, UnsignedInt, Severity, Containers::ArrayReference<const char>);
        static MAGNUM_LOCAL void insertImplementationKhr(Source source, Type type, UnsignedInt id, Severity severity, Containers::ArrayReference<const char> string);
        static MAGNUM_LOCAL void insertImplementationExt(Source, Type, UnsignedInt, Severity, Containers::ArrayReference<const char> string);
        #ifndef MAGNUM_TARGET_GLES
        static MAGNUM_LOCAL void insertImplementationGremedy(Source, Type, UnsignedInt, Severity, Containers::ArrayReference<const char> string);
        #endif

        static MAGNUM_LOCAL void callbackImplementationNoOp(Callback, const void*);
        static MAGNUM_LOCAL void callbackImplementationKhr(Callback callback, const void* userParam);
};

/** @debugoperatorclassenum{Magnum::DebugMessage,Magnum::DebugMessage::Source} */
Debug MAGNUM_EXPORT operator<<(Debug debug, DebugMessage::Source value);

/** @debugoperatorclassenum{Magnum::DebugMessage,Magnum::DebugMessage::Type} */
Debug MAGNUM_EXPORT operator<<(Debug debug, DebugMessage::Type value);

/** @debugoperatorclassenum{Magnum::DebugMessage,Magnum::DebugMessage::Severity} */
Debug MAGNUM_EXPORT operator<<(Debug debug, DebugMessage::Severity value);

}

#endif
