#ifndef Magnum_Object_h
#define Magnum_Object_h
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
 * @brief Class Magnum::Object
 */

#include <set>

#include "Magnum.h"

namespace Magnum {

class Scene;
class Camera;

/**
 * @brief Base for all positioned objects
 *
 * @todo Transform transformation when changing parent, so the object stays in
 * place.
 */
class MAGNUM_EXPORT Object {
    Object(const Object& other) = delete;
    Object(Object&& other) = delete;
    Object& operator=(const Object& other) = delete;
    Object& operator=(Object&& other) = delete;

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

        /** @{ @name Scene hierarchy */

        /**
         * @brief %Scene
         * @return If the object is not assigned to any scene, returns nullptr.
         */
        Scene* scene();

        /** @brief Parent object */
        inline Object* parent() { return _parent; }

        /** @brief Child objects */
        inline const std::set<Object*>& children() { return _children; }

        /** @brief Set parent object */
        Object* setParent(Object* parent);

        /*@}*/

        /** @{ @name Object transformation
         *
         * All transformations (except absoluteTransformation()) are relative
         * to parent.
         */

        /** @brief Transformation */
        inline Matrix4 transformation() const {
            return _transformation;
        }

        /**
         * @brief Absolute transformation
         *
         * If both this object and the camera is part of the same scene,
         * returns absolute transformation matrix (relative to the camera).
         * Otherwise returns transformation matrix relative to root object
         * (in most cases scene of this object).
         *
         * Note that the absolute transformation is computed from all parent
         * objects every time it is asked, unless this function is
         * reimplemented in a different way.
         */
        virtual Matrix4 absoluteTransformation(Camera* camera = nullptr);

        /** @brief Set transformation */
        Object* setTransformation(const Matrix4& transformation);

        /**
         * @brief Multiply transformation
         * @param transformation    Transformation
         * @param global            Whether to apply transformation as global
         *      (multiply from left side) or as local (multiply from right
         *      side)
         */
        inline Object* multiplyTransformation(const Matrix4& transformation, bool global = true) {
            setTransformation(global ? transformation*_transformation : _transformation*transformation);
            return this;
        }

        /**
         * @brief Translate object
         *
         * Same as calling multiplyTransformation() with Matrix4::translation().
         */
        inline Object* translate(Vector3 vec, bool global = true) {
            multiplyTransformation(Matrix4::translation(vec), global);
            return this;
        }

        /**
         * @brief Scale object
         *
         * Same as calling multiplyTransformation() with Matrix4::scaling().
         */
        inline Object* scale(Vector3 vec, bool global = true) {
            multiplyTransformation(Matrix4::scaling(vec), global);
            return this;
        }

        /**
         * @brief Rotate object
         *
         * Same as calling multiplyTransformation() with Matrix4::rotation().
         */
        inline Object* rotate(GLfloat angle, Vector3 vec, bool global = true) {
            multiplyTransformation(Matrix4::rotation(angle, vec), global);
            return this;
        }

        /*@}*/

        /** @{ @name Caching helpers
         *
         * If the object (absolute) transformation or anything depending on it
         * is used many times when drawing (such as e.g. position of light
         * object), it's good to cache these values, so they don't have to be
         * computed again on every request.
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
         * @attention Reimplementations must call also this function!
         *
         * @see setClean()
         */
        virtual void setDirty();

        /**
         * @brief Set object and all its parents as clean
         *
         * Recursively calls setClean() on every parent. Default implementation
         * only marks the object as clean, but if the object does any caching,
         * this function should be reimplemented to regenerate the cache.
         * @attention Reimplementations must call also this function!
         */
        virtual void setClean();

        /*@}*/

        /**
         * @brief Draw object
         * @param transformationMatrix      %Matrix specifying object
         *      transformation relative to the scene.
         * @param camera                    Active camera (containing
         *      projection matrix)
         *
         * Default implementation does nothing.
         */
        virtual void draw(const Matrix4& transformationMatrix, Camera* camera) {}

    private:
        Object* _parent;
        std::set<Object*> _children;
        Matrix4 _transformation;
        bool dirty;
};

}

#endif
