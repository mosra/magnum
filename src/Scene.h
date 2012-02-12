#ifndef Magnum_Scene_h
#define Magnum_Scene_h
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
 * @brief Class Magnum::Scene
 */

#include "Camera.h"

namespace Magnum {

/** @brief %Scene */
class MAGNUM_EXPORT Scene: public Object {
    private:
        void setParent(Object* parent) = delete;
        void setTransformation(const Matrix4& transformation) = delete;
        void multiplyTransformation(const Matrix4& transformation, bool global = true) = delete;
        void setTransformationFrom(Object* another) = delete;
        void translate(Vector3 vec, bool global = true) = delete;
        void translate(GLfloat x, GLfloat y, GLfloat z, bool global = true) = delete;
        void scale(Vector3 vec, bool global = true) = delete;
        void scale(GLfloat x, GLfloat y, GLfloat z, bool global = true) = delete;
        void rotate(GLfloat angle, Vector3 vec, bool global = true) = delete;
        void rotate(GLfloat angle, GLfloat x, GLfloat y, GLfloat z, bool global = true) = delete;

    public:
        /** @brief Features */
        enum Feature {
            AlphaBlending   = 0x01, /**< @brief Alpha blending */
            DepthTest       = 0x02, /**< @brief Depth test */
            FaceCulling     = 0x04  /**< @brief Face culling */
        };

        /**
         * @brief Constructor
         *
         * Creates one default vertex array.
         */
        Scene();

        /**
         * @brief Destructor
         *
         * Deletes the default vertex array.
         */
        inline ~Scene() { glDeleteVertexArrays(1, &vao); }

        /** @brief Clear color */
        inline Vector4 clearColor() const { return _clearColor; }

        /** @brief Which features are set */
        inline unsigned int features() const { return _features; }

        /** @brief Set feature */
        void setFeature(Feature feature, bool enabled);

        /** @brief Set clear color */
        void setClearColor(const Vector4& color);

        /** @brief Set clear color */
        inline void setClearColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
            setClearColor(Vector4(r, g, b, a));
        }

        /**
         * @brief Draw whole scene
         *
         * Recursively draws all child objects with given camera.
         */
        virtual void draw(Camera* camera);

    private:
        unsigned int _features;
        Vector4 _clearColor;
        GLuint vao;

        inline virtual void draw(const Magnum::Matrix4& transformationMatrix, Camera* camera) {}
        void drawChildren(Object* object, const Matrix4& transformationMatrix, Camera* camera);
};

}

#endif
