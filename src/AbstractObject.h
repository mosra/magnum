#ifndef Magnum_AbstractObject_h
#define Magnum_AbstractObject_h
/*
    Copyright © 2010 Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Class Magnum::AbstractObject
 */

#include <set>

#include "Magnum.h"

namespace Magnum {

/**
 * @brief Base for all positioned objects
 *
 * @todo Transform transformation when changing parent, so the object stays in
 * place.
 */
class AbstractObject {
    DISABLE_COPY(AbstractObject)

    public:
        /**
         * @brief Constructor
         * @param parent    Parent object
         *
         * Sets all transformations to their default values.
         */
        inline AbstractObject(AbstractObject* parent = 0): _parent(0) {
            setParent(parent);
        }

        /**
         * @brief Destructor
         *
         * Removes itself from parent's children list and destroys all own
         * children.
         */
        virtual ~AbstractObject();

        /** @brief Parent object */
        inline AbstractObject* parent() const { return _parent; }

        /** @brief Child objects */
        const std::set<AbstractObject*>& children() const { return _children; }

        /** @brief Set parent object */
        void setParent(AbstractObject* parent);

        /** @brief Transformation matrix */
        inline Matrix4 transformation() const { return _transformation; }

        /** @brief Set transformation matrix */
        inline void setTransformation(const Matrix4& transformation) { _transformation = transformation; }

        /**
         * @brief Multiply transformation matrix
         * @param transformation    Transformation matrix
         * @param global            Whether to apply transformation as global
         *      (multiply from left side) or as local (multiply from right
         *      side)
         *
         * Multiplies current transformation matrix by new matrix.
         */
        inline void multiplyTransformation(const Matrix4& transformation, bool global = true) {
            _transformation = global ? transformation*_transformation : _transformation*transformation;
        }

        /**
         * @brief Set transformation and parent from another object
         *
         * Sets parent and transformation from another object, so they will
         * appear in the same place.
         */
        inline void setTransformationFrom(const AbstractObject& another) {
            setParent(another.parent());
            setTransformation(another.transformation());
        }

        /**
         * @brief Translate object
         *
         * Same as calling multiplyTransformation() with Matrix4::translation().
         */
        inline void translate(Vector3 vec, bool global = true) {
            multiplyTransformation(Matrix4::translation(vec), global);
        }

        /** @copydoc translate(Vector3, bool) */
        inline void translate(GLfloat x, GLfloat y, GLfloat z, bool global = true) {
            translate(Vector3(x, y, z), global);
        }

        /**
         * @brief Scale object
         *
         * Same as calling multiplyTransformation() with Matrix4::scaling().
         */
        inline void scale(Vector3 vec, bool global = true) {
            multiplyTransformation(Matrix4::scaling(vec), global);
        }

        /** @copydoc scale(Vector3, bool) */
        inline void scale(GLfloat x, GLfloat y, GLfloat z, bool global = true) {
            scale(Vector3(x, y, z), global);
        }

        /**
         * @brief Rotate object
         *
         * Same as calling multiplyTransformation() with Matrix4::rotation().
         */
        inline void rotate(GLfloat angle, Vector3 vec, bool global = true) {
            multiplyTransformation(Matrix4::rotation(angle, vec), global);
        }

        /** @copydoc rotate(GLfloat, Vector3, bool) */
        inline void rotate(GLfloat angle, GLfloat x, GLfloat y, GLfloat z, bool global = true) {
            rotate(angle, Vector3(x, y, z), global);
        }

        /** @brief Draw object */
        virtual void draw(const Matrix4& transformationMatrix, const Matrix4& projectionMatrix) = 0;

    private:
        AbstractObject* _parent;
        std::set<AbstractObject*> _children;
        Matrix4 _transformation;
};

}

#endif
