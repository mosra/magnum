#ifndef Magnum_DefaultFramebuffer_h
#define Magnum_DefaultFramebuffer_h
/*
    Copyright © 2010, 2011, 2012 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

/** @file
 * @brief Class Magnum::DefaultFramebuffer
 */

#include "AbstractFramebuffer.h"

namespace Magnum {

/**
@brief Default framebuffer

@see @ref defaultFramebuffer, Framebuffer
*/
class MAGNUM_EXPORT DefaultFramebuffer: public AbstractFramebuffer {
    friend class Context;

    public:
        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Draw attachment
         *
         * @see mapForDraw()
         * @requires_gles30 Draw attachments for default framebuffer are
         *      available only in OpenGL ES 3.0.
         */
        enum class DrawAttachment: GLenum {
            /** Don't use the output. */
            None = GL_NONE,

            #ifndef MAGNUM_TARGET_GLES
            /**
             * Write output to back left framebuffer.
             * @requires_gl Stereo rendering is not available in OpenGL ES.
             */
            BackLeft = GL_BACK_LEFT,

            /**
             * Write output to back right framebuffer.
             * @requires_gl Stereo rendering is not available in OpenGL ES.
             */
            BackRight = GL_BACK_RIGHT,

            /**
             * Write output to front left framebuffer.
             * @requires_gl Stereo rendering is not available in OpenGL ES.
             */
            FrontLeft = GL_FRONT_LEFT,

            /**
             * Write output to front right framebuffer.
             * @requires_gl Stereo rendering is not available in OpenGL ES.
             */
            FrontRight = GL_FRONT_RIGHT,
            #endif

            /**
             * Write output to back framebuffer.
             *
             * On desktop OpenGL, this is equal to @ref DrawAttachment "DrawAttachment::BackLeft".
             */
            #ifdef MAGNUM_TARGET_GLES
            Back = GL_BACK,
            #else
            Back = GL_BACK_LEFT,
            #endif

            /**
             * Write output to front framebuffer.
             *
             * On desktop OpenGL, this is equal to @ref DrawAttachment "DrawAttachment::FrontLeft".
             */
            #ifdef MAGNUM_TARGET_GLES
            Front = GL_FRONT
            #else
            Front = GL_FRONT_LEFT
            #endif
        };
        #endif

        /**
         * @brief Read attachment
         *
         * @see mapForRead()
         * @requires_gles30 %Extension @es_extension2{NV,read_buffer,GL_NV_read_buffer}
         */
        enum class ReadAttachment: GLenum {
            /** Don't read from any framebuffer */
            None = GL_NONE,

            #ifndef MAGNUM_TARGET_GLES
            /**
             * Read from back left framebuffer.
             * @requires_gl Stereo rendering is not available in OpenGL ES.
             */
            BackLeft = GL_BACK_LEFT,

            /**
             * Read from back right framebuffer.
             * @requires_gl Stereo rendering is not available in OpenGL ES.
             */
            BackRight = GL_BACK_RIGHT,

            /**
             * Read from front left framebuffer.
             * @requires_gl Stereo rendering is not available in OpenGL ES.
             */
            FrontLeft = GL_FRONT_LEFT,

            /**
             * Read from front right framebuffer.
             * @requires_gl Stereo rendering is not available in OpenGL ES.
             */
            FrontRight = GL_FRONT_RIGHT,

            /**
             * Read from left framebuffer.
             * @requires_gl Stereo rendering is not available in OpenGL ES.
             */
            Left = GL_LEFT,

            /**
             * Read from right framebuffer.
             * @requires_gl Stereo rendering is not available in OpenGL ES.
             */
            Right = GL_RIGHT,
            #endif

            /** Read from back framebuffer. */
            Back = GL_BACK,

            /**
             * Read from front framebuffer.
             * @requires_es_extension %Extension @es_extension2{NV,read_buffer,GL_NV_read_buffer}
             */
            Front = GL_FRONT

            #ifndef MAGNUM_TARGET_GLES
            ,

            /**
             * Read from front and back framebuffer.
             * @requires_gl In OpenGL ES you must specify either
             *      @ref Magnum::DefaultFramebuffer::ReadAttachment "ReadAttachment::Front"
             *      or @ref Magnum::DefaultFramebuffer::ReadAttachment "ReadAttachment::Back".
             */
            FrontAndBack = GL_FRONT_AND_BACK
            #endif
        };

        explicit MAGNUM_LOCAL DefaultFramebuffer();

        #ifndef MAGNUM_TARGET_GLES2
        /**
         * @brief Map shader outputs to buffer attachment
         *
         * @p attachments is list of shader outputs mapped to framebuffer
         * buffer attachments. Shader outputs which are not listed are not
         * used, you can achieve the same by passing @ref DrawAttachment "DrawAttachment::None"
         * as attachment. Example usage:
         * @code
         * framebuffer.mapForDraw({{MyShader::ColorOutput, DefaultFramebuffer::DrawAttachment::BackLeft},
         *                         {MyShader::NormalOutput, DefaultFramebuffer::DrawAttachment::None}});
         * @endcode
         * @see mapForRead(), @fn_gl{BindFramebuffer}, @fn_gl{DrawBuffers}
         * @requires_gles30 Draw attachments for default framebuffer are
         *      available only in OpenGL ES 3.0.
         */
        void mapForDraw(std::initializer_list<std::pair<GLuint, DrawAttachment>> attachments);

        /**
         * @brief Map shader output to buffer attachment
         * @param attachment        Buffer attachment
         *
         * Similar to above function, can be used in cases when shader has
         * only one (unnamed) output.
         * @see mapForRead(), @fn_gl{BindFramebuffer}, @fn_gl{DrawBuffer}
         * @requires_gles30 Draw attachments for default framebuffer are
         *      available only in OpenGL ES 3.0.
         */
        void mapForDraw(DrawAttachment attachment);
        #endif

        /**
         * @brief Map given attachment for reading
         * @param attachment        Buffer attachment
         *
         * @see mapForDraw(), @fn_gl{BindFramebuffer}, @fn_gl{ReadBuffer}
         * @requires_gles30 %Extension @es_extension2{NV,read_buffer,GL_NV_read_buffer}
         */
        void mapForRead(ReadAttachment attachment);

    private:
        static void MAGNUM_LOCAL initializeContextBasedFunctionality(Context* context);
};

/** @brief Default framebuffer instance */
extern DefaultFramebuffer MAGNUM_EXPORT defaultFramebuffer;

}

#endif
