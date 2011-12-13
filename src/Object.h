#ifndef Magnum_Object_h
#define Magnum_Object_h
/*
    Copyright © 2010, 2011 Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Class Magnum::Object
 */

#include <set>

#include "Magnum.h"

namespace Magnum {

class Scene;

/**
 * @brief Base for all positioned objects
 *
 * @todo Transform transformation when changing parent, so the object stays in
 * place.
 */
class Object {
    DISABLE_COPY(Object)

    friend class Scene;

    public:
        /**
         * @brief Constructor
         * @param parent    Parent object
         *
         * Sets all transformations to their default values.
         */
        inline Object(Object* parent = nullptr): _parent(nullptr), dirty(true) {
            setParent(parent);
        }

        /**
         * @brief Destructor
         *
         * Removes itself from parent's children list and destroys all own
         * children.
         */
        virtual ~Object();

        /**
         * @brief Scene
         * @return If the object is not assigned to any scene, returns nullptr.
         */
        Scene* scene() const;

        /** @brief Parent object */
        inline Object* parent() const { return _parent; }

        /** @brief Child objects */
        inline const std::set<Object*>& children() const { return _children; }

        /** @brief Set parent object */
        void setParent(Object* parent);

        /**
         * @brief Transformation matrix
         *
         * If the object is part of an scene and @c absolute is set to true,
         * returns absolute transformation matrix (thus relative to actual
         * camera), if the object is not part of an scene, returns
         * transformation matrix composed of all matrices of parent objects.
         * If @c absolute is set to false, returns transformation matrix
         * relative to parent.
         */
        virtual Matrix4 transformation(bool absolute = false);

        /** @brief Set transformation matrix */
        inline void setTransformation(const Matrix4& transformation) {
            _transformation = transformation;
            setDirty();
        }

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
            setDirty();
        }

        /**
         * @brief Set transformation and parent from another object
         *
         * Sets parent and transformation from another object, so they will
         * appear in the same place.
         */
        inline void setTransformationFrom(Object* another) {
            setParent(another->parent());
            setTransformation(another->transformation());
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

        /** @{ @name Caching helpers
         *
         * If the object transformation is used many times when drawing (such
         * as e.g. position of light object), it's good to cache these values,
         * so they don't have to be computed again on every request.
         *
         * If the object or any parent is transformed, the transformed object
         * and all its children are marked as dirty. If currently active camera
         * is transformed, the scene goes through all children and calls
         * setDirty() recursively on every clean object (if the object is
         * already dirty, it and all its children are skipped, because they are
         * dirty too).
         *
         * These functions are used to manage dirty status of the object. If
         * the object doesn't cache anything, it's no need to bother about them,
         * but if does, setClean() should be reimplemented and used to
         * regenerate the cache. Thus the dirty status is managed only for
         * these objects, which are calling setClean().
         */

        /**
         * @brief Whether the object is dirty
         * @return  True, if transformation of the object, any parent or camera
         *      has changed since last asking, false otherwise.
         */
        inline bool isDirty() const { return dirty; }

        /**
         * @brief Set object and all its children as dirty
         *
         * Recursively calls setDirty() on every child. If the object is already
         * marked as dirty, the function does nothing.
         * @attention Reimplementations must also call this function!
         */
        virtual void setDirty();

        /**
         * @brief Set object and all its parents as clean
         *
         * Recursively calls setClean() on every parent. Default implementation
         * only marks the object as clean, but if the object does any caching,
         * this function should be reimplemented to regenerate the cache.
         * @attention Reimplementations must also call this function!
         */
        virtual void setClean();

        /*@}*/

        /**
         * @brief Draw object
         *
         * Default implementation does nothing.
         */
        virtual void draw(const Matrix4& transformationMatrix, const Matrix4& projectionMatrix) {}

    private:
        Object* _parent;
        std::set<Object*> _children;
        Matrix4 _transformation;
        bool dirty;
};

}

#endif
