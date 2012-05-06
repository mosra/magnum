#ifndef Magnum_Camera_h
#define Magnum_Camera_h
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
 * @brief Class Magnum::Camera
 */

#include "Object.h"

#ifdef WIN32 /* I so HATE windows.h */
#undef near
#undef far
#endif

namespace Magnum {

/**
@brief %Camera object
 */
class MAGNUM_EXPORT Camera: public Object {
    public:
        /** @brief Aspect ratio policy */
        enum AspectRatioPolicy {
            NotPreserved,       /**< @brief Don't preserve aspect ratio */
            Extend,             /**< @brief Extend on larger side of view */
            Clip                /**< @brief Clip on smaller side of view */
        };

        /**
         * @brief Constructor
         * @param parent        Parent object
         *
         * Sets orthographic projection to the default OpenGL cube (range
         * @f$ [-1; 1] @f$ in all directions) and clear color to black.
         * @see setOrthographic(), setClearColor()
         */
        Camera(Object* parent = nullptr);

        /** @brief Aspect ratio policy */
        AspectRatioPolicy aspectRatioPolicy() const { return _aspectRatioPolicy; }

        /** @brief Set aspect ratio policy */
        void setAspectRatioPolicy(AspectRatioPolicy policy) { _aspectRatioPolicy = policy; }

        /**
         * @brief Set orthographic projection
         * @param size      Size of (square) view
         * @param near      Near clipping plane
         * @param far       Far clipping plane
         *
         * The volume of given size will be scaled down to range
         * @f$ [-1; 1] @f$ on all directions.
         */
        void setOrthographic(GLfloat size, GLfloat near, GLfloat far);

        /**
         * @brief Set perspective projection
         * @param fov       Field of view angle
         * @param near      Near clipping plane
         * @param far       Far clipping plane
         */
        void setPerspective(GLfloat fov, GLfloat near, GLfloat far);

        /** @brief Near clipping plane */
        inline GLfloat near() const { return _near; }

        /** @brief Far clipping plane */
        inline GLfloat far() const { return _far; }

        /**
         * @brief Camera matrix
         *
         * Camera matrix describes world position relative to the camera and is
         * applied as first.
         */
        inline Matrix4 cameraMatrix() {
            setClean();
            return _cameraMatrix;
        }

        /**
         * @brief Projection matrix
         *
         * Projection matrix handles e.g. perspective distortion and is applied
         * as last.
         */
        inline Matrix4 projectionMatrix() const { return _projectionMatrix; }

        /** @brief Viewport size */
        inline Math::Vector2<GLsizei> viewport() const { return _viewport; }

        /**
         * @brief Set viewport size
         *
         * Called when assigning the camera to the scene or when window
         * size changes.
         */
        virtual void setViewport(const Math::Vector2<GLsizei>& size);

        /** @brief Clear color */
        inline Vector4 clearColor() const { return _clearColor; }

        /** @brief Set clear color */
        void setClearColor(const Vector4& color);

        /**
         * @brief Draw the scene
         *
         * Clears the color, depth and stencil buffer and draws the scene
         * using drawChildren().
         */
        virtual void draw();

        /* It's here just so the function is not hidden by draw() */
        virtual void draw(const Matrix4& transformationMatrix, Camera* camera) {}

    protected:
        /**
         * Recalculates camera matrix.
         */
        void clean(const Matrix4& absoluteTransformation);

        /**
         * @brief Draw object children
         *
         * Recursively draws all children of the object.
         */
        void drawChildren(Object* object, const Matrix4& transformationMatrix);

    private:
        Matrix4 rawProjectionMatrix;
        Matrix4 _projectionMatrix;
        Matrix4 _cameraMatrix;
        Vector4 _clearColor;
        GLfloat _near, _far;

        Math::Vector2<GLsizei> _viewport;
        AspectRatioPolicy _aspectRatioPolicy;

        void fixAspectRatio();
};

}

#endif
