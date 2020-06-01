#ifndef Magnum_GL_Renderbuffer_h
#define Magnum_GL_Renderbuffer_h
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

/** @file
 * @brief Class @ref Magnum::GL::Renderbuffer
 */

#include <Corrade/Containers/ArrayView.h>

#include "Magnum/Tags.h"
#include "Magnum/GL/AbstractObject.h"
#include "Magnum/GL/GL.h"

namespace Magnum { namespace GL {

namespace Implementation { struct FramebufferState; }

/**
@brief Renderbuffer

Attachable to a framebuffer as render target, see the @ref Framebuffer class
for a detailed usage example.

@section GL-Renderbuffer-performance-optimizations Performance optimizations

The engine tracks currently bound renderbuffer to avoid unnecessary calls to
@fn_gl{BindRenderbuffer} in @ref setStorage(). Renderbuffer limits and
implementation-defined values (such as @ref maxSize()) are cached, so repeated
queries don't result in repeated @fn_gl{Get} calls.

If @gl_extension{ARB,direct_state_access} (part of OpenGL 4.5) is available,
functions @ref setStorage() and @ref setStorageMultisample() use DSA to avoid
unnecessary calls to @fn_gl{BindRenderbuffer}. See their respective
documentation for more information.

@requires_gl30 Extension @gl_extension{ARB,framebuffer_object}
*/
class MAGNUM_GL_EXPORT Renderbuffer: public AbstractObject {
    friend Implementation::FramebufferState;

    public:
        /**
         * @brief Max supported renderbuffer size
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls.
         * @see @ref setStorage(), @ref setStorageMultisample(), @fn_gl{Get}
         *      with @def_gl_keyword{MAX_RENDERBUFFER_SIZE}
         */
        static Int maxSize();

        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        /**
         * @brief Max supported sample count
         *
         * The result is cached, repeated queries don't result in repeated
         * OpenGL calls. If neither OpenGL ES 3.0 nor ES extension
         * @gl_extension{ANGLE,framebuffer_multisample} /
         * @gl_extension{NV,framebuffer_multisample} is available, returns
         * @cpp 0 @ce.
         * @see @ref setStorageMultisample(), @fn_gl{Get} with
         *      @def_gl_keyword{MAX_SAMPLES}
         * @requires_webgl20 Multisample framebuffers are not available in
         *      WebGL 1.0.
         */
        static Int maxSamples();
        #endif

        /**
         * @brief Wrap existing OpenGL renderbuffer object
         * @param id            OpenGL renderbuffer ID
         * @param flags         Object creation flags
         *
         * The @p id is expected to be of an existing OpenGL renderbuffer
         * object. Unlike renderbuffer created using constructor, the OpenGL
         * object is by default not deleted on destruction, use @p flags for
         * different behavior.
         * @see @ref release()
         */
        static Renderbuffer wrap(GLuint id, ObjectFlags flags = {}) {
            return Renderbuffer{id, flags};
        }

        /**
         * @brief Constructor
         *
         * Generates new OpenGL renderbuffer object. If @gl_extension{ARB,direct_state_access}
         * (part of OpenGL 4.5) is not available, the renderbuffer is created
         * on first use.
         * @see @ref Renderbuffer(NoCreateT), @ref wrap(),
         *      @fn_gl_keyword{CreateRenderbuffers}, eventually
         *      @fn_gl_keyword{GenRenderbuffers}
         */
        explicit Renderbuffer();

        /**
         * @brief Construct without creating the underlying OpenGL object
         *
         * The constructed instance is equivalent to moved-from state. Useful
         * in cases where you will overwrite the instance later anyway. Move
         * another object over it to make it useful.
         *
         * This function can be safely used for constructing (and later
         * destructing) objects even without any OpenGL context being active.
         * However note that this is a low-level and a potentially dangerous
         * API, see the documentation of @ref NoCreate for alternatives.
         * @see @ref Renderbuffer(), @ref wrap()
         */
        explicit Renderbuffer(NoCreateT) noexcept: _id{0}, _flags{ObjectFlag::DeleteOnDestruction} {}

        /** @brief Copying is not allowed */
        Renderbuffer(const Renderbuffer&) = delete;

        /** @brief Move constructor */
        /* MinGW complains loudly if the declaration doesn't also have inline */
        inline Renderbuffer(Renderbuffer&& other) noexcept;

        /**
         * @brief Destructor
         *
         * Deletes associated OpenGL renderbuffer object.
         * @see @ref wrap(), @ref release(), @fn_gl_keyword{DeleteRenderbuffers}
         */
        ~Renderbuffer();

        /** @brief Copying is not allowed */
        Renderbuffer& operator=(const Renderbuffer&) = delete;

        /** @brief Move assignment */
        /* MinGW complains loudly if the declaration doesn't also have inline */
        inline Renderbuffer& operator=(Renderbuffer&& other) noexcept;

        /** @brief OpenGL renderbuffer ID */
        GLuint id() const { return _id; }

        /**
         * @brief Release OpenGL object
         *
         * Releases ownership of OpenGL renderbuffer object and returns its ID
         * so it is not deleted on destruction. The internal state is then
         * equivalent to moved-from state.
         * @see @ref wrap()
         */
        /* MinGW complains loudly if the declaration doesn't also have inline */
        inline GLuint release();

        #ifndef MAGNUM_TARGET_WEBGL
        /**
         * @brief Renderbuffer label
         *
         * The result is *not* cached, repeated queries will result in repeated
         * OpenGL calls. If OpenGL 4.3 / OpenGL ES 3.2 is not supported and
         * neither @gl_extension{KHR,debug} (covered also by
         * @gl_extension{ANDROID,extension_pack_es31a}) nor @gl_extension{EXT,debug_label}
         * desktop or ES extension is available, this function returns empty
         * string.
         * @see @fn_gl_keyword{GetObjectLabel} or
         *      @fn_gl_extension_keyword{GetObjectLabel,EXT,debug_label} with
         *      @def_gl{RENDERBUFFER}
         * @requires_gles Debug output is not available in WebGL.
         */
        std::string label();

        /**
         * @brief Set renderbuffer label
         * @return Reference to self (for method chaining)
         *
         * Default is empty string. If OpenGL 4.3 / OpenGL ES 3.2 is not
         * supported and neither @gl_extension{KHR,debug} (covered also by
         * @gl_extension{ANDROID,extension_pack_es31a}) nor @gl_extension{EXT,debug_label}
         * desktop or ES extension is available, this function does nothing.
         * @see @ref maxLabelLength(), @fn_gl_keyword{ObjectLabel} or
         *      @fn_gl_extension_keyword{LabelObject,EXT,debug_label} with
         *      @def_gl{RENDERBUFFER}
         * @requires_gles Debug output is not available in WebGL.
         */
        Renderbuffer& setLabel(const std::string& label) {
            return setLabelInternal({label.data(), label.size()});
        }

        /** @overload */
        template<std::size_t size> Renderbuffer& setLabel(const char(&label)[size]) {
            return setLabelInternal({label, size - 1});
        }
        #endif

        /**
         * @brief Set renderbuffer storage
         * @param internalFormat    Internal format
         * @param size              Renderbuffer size
         *
         * If @gl_extension{ARB,direct_state_access} (part of OpenGL 4.5) is
         * available, the renderbuffer is bound before the operation (if not
         * already).
         * @see @ref maxSize(), @fn_gl2_keyword{NamedRenderbufferStorage,RenderbufferStorage},
         *      eventually @fn_gl{BindRenderbuffer} and
         *      @fn_gl_keyword{RenderbufferStorage}
         */
        void setStorage(RenderbufferFormat internalFormat, const Vector2i& size);

        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        /**
         * @brief Set multisample renderbuffer storage
         * @param samples           Sample count
         * @param internalFormat    Internal format
         * @param size              Renderbuffer size
         *
         * If @gl_extension{ARB,direct_state_access} (part of OpenGL 4.5) is
         * available, the renderbuffer is bound before the operation (if not
         * already).
         * @see @ref maxSize(), @ref maxSamples(),
         *      @fn_gl2_keyword{NamedRenderbufferStorageMultisample,RenderbufferStorageMultisample},
         *      eventually @fn_gl{BindRenderbuffer} and
         *      @fn_gl_keyword{RenderbufferStorageMultisample}
         * @requires_gles30 Extension @gl_extension{ANGLE,framebuffer_multisample}
         *      or @gl_extension{NV,framebuffer_multisample} in OpenGL ES 2.0.
         * @requires_webgl20 Multisample framebuffers are not available in
         *      WebGL 1.0.
         * @todo How about @gl_extension{APPLE,framebuffer_multisample}, @fn_gl_extension{RenderbufferStorageMultisample,EXT,multisampled_render_to_texture}?
         */
        void setStorageMultisample(Int samples, RenderbufferFormat internalFormat, const Vector2i& size);
        #endif

    private:
        explicit Renderbuffer(GLuint id, ObjectFlags flags) noexcept: _id{id}, _flags{flags} {}

        void MAGNUM_GL_LOCAL createImplementationDefault();
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_GL_LOCAL createImplementationDSA();
        #endif

        void MAGNUM_GL_LOCAL createIfNotAlready();

        #ifndef MAGNUM_TARGET_WEBGL
        Renderbuffer& setLabelInternal(Containers::ArrayView<const char> label);
        #endif

        void MAGNUM_GL_LOCAL storageImplementationDefault(RenderbufferFormat internalFormat, const Vector2i& size);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_GL_LOCAL storageImplementationDSA(RenderbufferFormat internalFormat, const Vector2i& size);
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        void MAGNUM_GL_LOCAL storageMultisampleImplementationDefault(GLsizei samples, RenderbufferFormat internalFormat, const Vector2i& size);
        #ifndef MAGNUM_TARGET_GLES
        void MAGNUM_GL_LOCAL storageMultisampleImplementationDSA(GLsizei samples, RenderbufferFormat internalFormat, const Vector2i& size);
        #endif
        #elif !defined(MAGNUM_TARGET_WEBGL)
        void MAGNUM_GL_LOCAL storageMultisampleImplementationANGLE(GLsizei samples, RenderbufferFormat internalFormat, const Vector2i& size);
        void MAGNUM_GL_LOCAL storageMultisampleImplementationNV(GLsizei samples, RenderbufferFormat internalFormat, const Vector2i& size);
        #endif

        void MAGNUM_GL_LOCAL bind();

        GLuint _id;
        ObjectFlags _flags;
};

inline Renderbuffer::Renderbuffer(Renderbuffer&& other) noexcept: _id{other._id}, _flags{other._flags} {
    other._id = 0;
}

inline Renderbuffer& Renderbuffer::operator=(Renderbuffer&& other) noexcept {
    using std::swap;
    swap(_id, other._id);
    swap(_flags, other._flags);
    return *this;
}

inline GLuint Renderbuffer::release() {
    const GLuint id = _id;
    _id = 0;
    return id;
}

}}

#endif
