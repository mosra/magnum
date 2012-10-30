#ifndef Magnum_SceneGraph_Object_hpp
#define Magnum_SceneGraph_Object_hpp
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
 * @brief @ref compilation-speedup-hpp "Template implementation" for Object.h
 */

#include "Object.h"

#include <stack>

#include "Scene.h"

namespace Magnum { namespace SceneGraph {

template<class Transformation> Scene<Transformation>* Object<Transformation>::scene() {
    Object<Transformation>* p(this);
    while(p && !p->isScene()) p = p->parent();
    return static_cast<Scene<Transformation>*>(p);
}

template<class Transformation> const Scene<Transformation>* Object<Transformation>::scene() const {
    const Object<Transformation>* p(this);
    while(p && !p->isScene()) p = p->parent();
    return static_cast<const Scene<Transformation>*>(p);
}

template<class Transformation> Object<Transformation>* Object<Transformation>::setParent(Object<Transformation>* parent) {
    /* Skip if parent is already parent or this is scene (which cannot have parent) */
    /** @todo Assert for setting parent to scene */
    if(this->parent() == parent || isScene()) return this;

    /* Object cannot be parented to its child */
    Object<Transformation>* p = parent;
    while(p) {
        /** @todo Assert for this */
        if(p == this) return this;
        p = p->parent();
    }

    /* Remove the object from old parent children list */
    if(this->parent()) this->parent()->Corrade::Containers::LinkedList<Object<Transformation>>::cut(this);

    /* Add the object to list of new parent */
    if(parent) parent->Corrade::Containers::LinkedList<Object<Transformation>>::insert(this);

    setDirty();
    return this;
}

template<class Transformation> typename Transformation::DataType Object<Transformation>::absoluteTransformation() const {
    if(!parent()) return Transformation::transformation();
    return Transformation::compose(parent()->absoluteTransformation(), Transformation::transformation());
}

template<class Transformation> void Object<Transformation>::setDirty() {
    /* The transformation of this object (and all children) is already dirty,
       nothing to do */
    if(flags & Flag::Dirty) return;

    Object<Transformation>* self = static_cast<Object<Transformation>*>(this);

    /* Make all features dirty */
    for(AbstractFeature<Transformation::Dimensions, typename Transformation::Type>* i = self->firstFeature(); i; i = i->nextFeature())
        i->markDirty();

    /* Make all children dirty */
    for(Object<Transformation>* i = self->firstChild(); i; i = i->nextSibling())
        i->setDirty();

    /* Mark object as dirty */
    flags |= Flag::Dirty;
}

template<class Transformation> void Object<Transformation>::setClean() {
    /* The object (and all its parents) are already clean, nothing to do */
    if(!(flags & Flag::Dirty)) return;

    /* Collect all parents, compute base transformation */
    std::stack<Object<Transformation>*> objects;
    typename Transformation::DataType absoluteTransformation;
    Object<Transformation>* p = static_cast<Object<Transformation>*>(this);
    for(;;) {
        objects.push(p);

        p = p->parent();

        /* On root object, base transformation is identity */
        if(!p) break;

        /* Parent object is clean, base transformation is its absolute
           transformation */
        if(!p->isDirty()) {
            absoluteTransformation = p->absoluteTransformation();
            break;
        }
    }

    /* Clean features on every collected object, going down from root object */
    while(!objects.empty()) {
        Object<Transformation>* o = objects.top();
        objects.pop();

        /* Compose transformations */
        absoluteTransformation = Transformation::compose(absoluteTransformation, o->transformation());

        /* "Lazy storage" for transformation matrix and inverted transformation matrix */
        typedef typename AbstractFeature<Transformation::Dimensions, typename Transformation::Type>::CachedTransformation CachedTransformation;
        typename AbstractFeature<Transformation::Dimensions, typename Transformation::Type>::CachedTransformations cached;
        typename DimensionTraits<Transformation::Dimensions, typename Transformation::Type>::MatrixType
            matrix, invertedMatrix;

        /* Clean all features */
        for(AbstractFeature<Transformation::Dimensions, typename Transformation::Type>* i = o->firstFeature(); i; i = i->nextFeature()) {
            /* Cached absolute transformation, compute it if it wasn't
               computed already */
            if(i->cachedTransformations() & CachedTransformation::Absolute) {
                if(!(cached & CachedTransformation::Absolute)) {
                    cached |= CachedTransformation::Absolute;
                    matrix = Transformation::toMatrix(absoluteTransformation);
                }

                i->clean(matrix);
            }

            /* Cached inverse absolute transformation, compute it if it wasn't
               computed already */
            if(i->cachedTransformations() & CachedTransformation::InvertedAbsolute) {
                if(!(cached & CachedTransformation::InvertedAbsolute)) {
                    cached |= CachedTransformation::InvertedAbsolute;
                    invertedMatrix = Transformation::toMatrix(Transformation::inverted(absoluteTransformation));
                }

                i->cleanInverted(invertedMatrix);
            }
        }

        /* Mark object as clean */
        o->flags &= ~Flag::Dirty;
    }
}

}}

#endif
