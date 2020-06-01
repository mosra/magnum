#ifndef Magnum_SceneGraph_Object_hpp
#define Magnum_SceneGraph_Object_hpp
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
 * @brief @ref compilation-speedup-hpp "Template implementation" for @ref AbstractObject.h, @ref AbstractTransformation.h and @ref Object.h
 */

#include <algorithm>
#include <stack>

#include "Magnum/SceneGraph/AbstractTransformation.h"
#include "Magnum/SceneGraph/Object.h"
#include "Magnum/SceneGraph/Scene.h"

namespace Magnum { namespace SceneGraph {

template<UnsignedInt dimensions, class T> AbstractObject<dimensions, T>::AbstractObject() {}
template<UnsignedInt dimensions, class T> AbstractObject<dimensions, T>::~AbstractObject() {}

template<UnsignedInt dimensions, class T> AbstractTransformation<dimensions, T>::AbstractTransformation() {}

template<class Transformation> Object<Transformation>::Object(Object<Transformation>* parent): counter(0xFFFFu), flags(Flag::Dirty) {
    setParent(parent);
}

template<class Transformation> Object<Transformation>::~Object() = default;

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

template<class Transformation> Object<Transformation>* Object<Transformation>::doScene() {
    return scene();
}

template<class Transformation> const Object<Transformation>* Object<Transformation>::doScene() const {
    return scene();
}

template<class Transformation> Object<Transformation>* Object<Transformation>::doParent() {
    return parent();
}

template<class Transformation> const Object<Transformation>* Object<Transformation>::doParent() const {
    return parent();
}

template<class Transformation> Object<Transformation>& Object<Transformation>::setParent(Object<Transformation>* parent) {
    /* Skip if parent is already parent or this is scene (which cannot have parent) */
    /** @todo Assert for setting parent to scene */
    if(this->parent() == parent || isScene()) return *this;

    /* Object cannot be parented to its child */
    Object<Transformation>* p = parent;
    while(p) {
        /** @todo Assert for this */
        if(p == this) return *this;
        p = p->parent();
    }

    /* Remove the object from old parent children list */
    if(this->parent()) this->parent()->Containers::template LinkedList<Object<Transformation>>::cut(this);

    /* Add the object to list of new parent */
    if(parent) parent->Containers::LinkedList<Object<Transformation>>::insert(this);

    setDirty();
    return *this;
}

template<class Transformation> Object<Transformation>& Object<Transformation>::setParentKeepTransformation(Object<Transformation>* parent) {
    CORRADE_ASSERT(scene() == parent->scene(), "SceneGraph::Object::setParentKeepTransformation(): both parents must be in the same scene", *this);

    const auto transformation = Implementation::Transformation<Transformation>::compose(
        Implementation::Transformation<Transformation>::inverted(parent->absoluteTransformation()), absoluteTransformation());
    setParent(parent);
    Transformation::setTransformation(transformation);

    return *this;
}

template<class Transformation> typename Transformation::DataType Object<Transformation>::absoluteTransformation() const {
    if(!parent()) return Transformation::transformation();
    return Implementation::Transformation<Transformation>::compose(parent()->absoluteTransformation(), Transformation::transformation());
}

template<class Transformation> void Object<Transformation>::setDirty() {
    /* The transformation of this object (and all children) is already dirty,
       nothing to do */
    if(flags & Flag::Dirty) return;

    /* Make all features dirty */
    for(AbstractFeature<Transformation::Dimensions, typename Transformation::Type>& feature: this->features())
        feature.markDirty();

    /* Make all children dirty */
    for(Object<Transformation>& child: children())
        child.setDirty();

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

        /* Compose transformation and clean object */
        absoluteTransformation = Implementation::Transformation<Transformation>::compose(absoluteTransformation, o->transformation());
        CORRADE_INTERNAL_ASSERT(o->isDirty());
        o->setCleanInternal(absoluteTransformation);
        CORRADE_ASSERT(!o->isDirty(), "SceneGraph::Object::setClean(): original implementation was not called", );
    }
}

template<class Transformation> auto Object<Transformation>::doTransformationMatrices(const std::vector<std::reference_wrapper<AbstractObject<Transformation::Dimensions, typename Transformation::Type>>>& objects, const MatrixType& finalTransformationMatrix) const -> std::vector<MatrixType> {
    std::vector<std::reference_wrapper<Object<Transformation>>> castObjects;
    castObjects.reserve(objects.size());
    /** @todo Ensure this doesn't crash, somehow */
    for(auto o: objects) castObjects.push_back(static_cast<Object<Transformation>&>(o.get()));

    return transformationMatrices(std::move(castObjects), finalTransformationMatrix);
}

template<class Transformation> auto Object<Transformation>::transformationMatrices(const std::vector<std::reference_wrapper<Object<Transformation>>>& objects, const MatrixType& finalTransformationMatrix) const -> std::vector<MatrixType> {
    std::vector<typename Transformation::DataType> transformations = this->transformations(std::move(objects), Implementation::Transformation<Transformation>::fromMatrix(finalTransformationMatrix));
    std::vector<MatrixType> transformationMatrices(transformations.size());
    for(std::size_t i = 0; i != objects.size(); ++i)
        transformationMatrices[i] = Implementation::Transformation<Transformation>::toMatrix(transformations[i]);

    return transformationMatrices;
}

/*
Computing absolute transformations for given list of objects

The goal is to compute absolute transformation only once for each object
involved. Objects contained in the subtree specified by `object` list are
divided into two groups:
 - "joints", which are either part of `object` list or they have more than one
   child in the subtree
 - "non-joints", i.e. paths between joints

Then for all joints their transformation (relative to parent joint) is
computed and recursively concatenated together. Resulting transformations for
joints which were originally in `object` list is then returned.
*/
template<class Transformation> std::vector<typename Transformation::DataType> Object<Transformation>::transformations(std::vector<std::reference_wrapper<Object<Transformation>>> objects, const typename Transformation::DataType& finalTransformation) const {
    CORRADE_ASSERT(objects.size() < 0xFFFFu, "SceneGraph::Object::transformations(): too large scene", {});

    /* Remember object count for later */
    std::size_t objectCount = objects.size();

    /* Mark all original objects as joints and create initial list of joints
       from them */
    for(std::size_t i = 0; i != objects.size(); ++i) {
        /* Multiple occurences of one object in the array, don't overwrite it
           with different counter */
        if(objects[i].get().counter != 0xFFFFu) continue;

        objects[i].get().counter = UnsignedShort(i);
        objects[i].get().flags |= Flag::Joint;
    }
    std::vector<std::reference_wrapper<Object<Transformation>>> jointObjects(objects);

    #ifndef CORRADE_NO_ASSERT
    /* Scene object */
    const Scene<Transformation>* scene = this->scene();
    #endif

    /* Nearest common ancestor not yet implemented - assert this is done on scene */
    CORRADE_ASSERT(scene == this, "SceneGraph::Object::transformationMatrices(): currently implemented only for Scene", {});

    /* Mark all objects up the hierarchy as visited */
    auto it = objects.begin();
    while(!objects.empty()) {
        /* Already visited, remove and continue to next (duplicate occurence) */
        if(it->get().flags & Flag::Visited) {
            it = objects.erase(it);
            continue;
        }

        /* Mark the object as visited */
        it->get().flags |= Flag::Visited;

        Object<Transformation>* parent = it->get().parent();

        /* If this is root object, remove from list */
        if(!parent) {
            CORRADE_ASSERT(&it->get() == scene, "SceneGraph::Object::transformations(): the objects are not part of the same tree", {});
            it = objects.erase(it);

        /* Parent is an joint or already visited - remove current from list */
        } else if(parent->flags & (Flag::Visited|Flag::Joint)) {
            it = objects.erase(it);

            /* If not already marked as joint, mark it as such and add it to
               list of joint objects */
            if(!(parent->flags & Flag::Joint)) {
                CORRADE_ASSERT(jointObjects.size() < 0xFFFFu,
                               "SceneGraph::Object::transformations(): too large scene", {});
                CORRADE_INTERNAL_ASSERT(parent->counter == 0xFFFFu);
                parent->counter = UnsignedShort(jointObjects.size());
                parent->flags |= Flag::Joint;
                jointObjects.push_back(*parent);
            }

        /* Else go up the hierarchy */
        } else *it = *parent;

        /* Cycle if reached end */
        if(it == objects.end()) it = objects.begin();
    }

    /* Array of absolute transformations in joints */
    std::vector<typename Transformation::DataType> jointTransformations(jointObjects.size());

    /* Compute transformations for all joints */
    for(std::size_t i = 0; i != jointTransformations.size(); ++i)
        computeJointTransformation(jointObjects, jointTransformations, i, finalTransformation);

    /* Copy transformation for second or next occurences from first occurence
       of duplicate object */
    for(std::size_t i = 0; i != objectCount; ++i) {
        if(jointObjects[i].get().counter != i)
            jointTransformations[i] = jointTransformations[jointObjects[i].get().counter];
    }

    /* All visited marks are now cleaned, clean joint marks and counters */
    for(auto i: jointObjects) {
        /* All not-already cleaned objects (...duplicate occurences) should
           have joint mark */
        CORRADE_INTERNAL_ASSERT(i.get().counter == 0xFFFFu || i.get().flags & Flag::Joint);
        i.get().flags &= ~Flag::Joint;
        i.get().counter = 0xFFFFu;
    }

    /* Shrink the array to contain only transformations of requested objects and return */
    jointTransformations.resize(objectCount);
    return jointTransformations;
}

template<class Transformation> typename Transformation::DataType Object<Transformation>::computeJointTransformation(const std::vector<std::reference_wrapper<Object<Transformation>>>& jointObjects, std::vector<typename Transformation::DataType>& jointTransformations, const std::size_t joint, const typename Transformation::DataType& finalTransformation) const {
    std::reference_wrapper<Object<Transformation>> o = jointObjects[joint];

    /* Transformation already computed ("unvisited" by this function before
       either due to recursion or duplicate object occurences), done */
    if(!(o.get().flags & Flag::Visited)) return jointTransformations[joint];

    /* Initialize transformation */
    jointTransformations[joint] = o.get().transformation();

    /* Go up until next joint or root */
    for(;;) {
        /* Clean visited mark */
        CORRADE_INTERNAL_ASSERT(o.get().flags & Flag::Visited);
        o.get().flags &= ~Flag::Visited;

        Object<Transformation>* parent = o.get().parent();

        /* Root object, compose transformation with final, done */
        if(!parent) {
            CORRADE_INTERNAL_ASSERT(o.get().isScene());
            return (jointTransformations[joint] =
                Implementation::Transformation<Transformation>::compose(finalTransformation, jointTransformations[joint]));

        /* Joint object, compose transformation with the joint, done */
        } else if(parent->flags & Flag::Joint) {
            return (jointTransformations[joint] =
                Implementation::Transformation<Transformation>::compose(computeJointTransformation(jointObjects, jointTransformations, parent->counter, finalTransformation), jointTransformations[joint]));

        /* Else compose transformation with parent, go up the hierarchy */
        } else {
            jointTransformations[joint] = Implementation::Transformation<Transformation>::compose(parent->transformation(), jointTransformations[joint]);
            o = *parent;
        }
    }
}

template<class Transformation> void Object<Transformation>::doSetClean(const std::vector<std::reference_wrapper<AbstractObject<Transformation::Dimensions, typename Transformation::Type>>>& objects) {
    std::vector<std::reference_wrapper<Object<Transformation>>> castObjects;
    castObjects.reserve(objects.size());
    /** @todo Ensure this doesn't crash, somehow */
    for(auto o: objects) castObjects.push_back(static_cast<Object<Transformation>&>(o.get()));

    setClean(std::move(castObjects));
}

template<class Transformation> void Object<Transformation>::setClean(std::vector<std::reference_wrapper<Object<Transformation>>> objects) {
    /* Remove all clean objects from the list */
    auto firstClean = std::remove_if(objects.begin(), objects.end(), [](Object<Transformation>& o) { return !o.isDirty(); });
    objects.erase(firstClean, objects.end());

    /* No dirty objects left, done */
    if(objects.empty()) return;

    /* Add non-clean parents to the list. Mark each added object as visited, so
       they aren't added more than once */
    for(std::size_t end = objects.size(), i = 0; i != end; ++i) {
        Object<Transformation>& o = objects[i];
        o.flags |= Flag::Visited;

        Object<Transformation>* parent = o.parent();
        while(parent && !(parent->flags & Flag::Visited) && parent->isDirty()) {
            objects.push_back(*parent);
            parent = parent->parent();
        }
    }

    /* Cleanup all marks */
    for(auto o: objects) o.get().flags &= ~Flag::Visited;

    /* Compute absolute transformations */
    Scene<Transformation>* scene = objects[0].get().scene();
    CORRADE_ASSERT(scene, "Object::setClean(): objects must be part of some scene", );
    std::vector<typename Transformation::DataType> transformations(scene->transformations(objects));

    /* Go through all objects and clean them */
    for(std::size_t i = 0; i != objects.size(); ++i) {
        /* The object might be duplicated in the list, don't clean it more than once */
        if(!objects[i].get().isDirty()) continue;

        objects[i].get().setCleanInternal(transformations[i]);
        CORRADE_ASSERT(!objects[i].get().isDirty(), "SceneGraph::Object::setClean(): original implementation was not called", );
    }
}

template<class Transformation> void Object<Transformation>::setCleanInternal(const typename Transformation::DataType& absoluteTransformation) {
    /* "Lazy storage" for transformation matrix and inverted transformation matrix */
    CachedTransformations cached;
    MatrixType matrix, invertedMatrix;

    /* Clean all features */
    for(AbstractFeature<Transformation::Dimensions, typename Transformation::Type>& feature: this->features()) {
        /* Cached absolute transformation, compute it if it wasn't
            computed already */
        if(feature.cachedTransformations() & CachedTransformation::Absolute) {
            if(!(cached & CachedTransformation::Absolute)) {
                cached |= CachedTransformation::Absolute;
                matrix = Implementation::Transformation<Transformation>::toMatrix(absoluteTransformation);
            }

            feature.clean(matrix);
        }

        /* Cached inverse absolute transformation, compute it if it wasn't
            computed already */
        if(feature.cachedTransformations() & CachedTransformation::InvertedAbsolute) {
            if(!(cached & CachedTransformation::InvertedAbsolute)) {
                cached |= CachedTransformation::InvertedAbsolute;
                invertedMatrix = Implementation::Transformation<Transformation>::toMatrix(
                    Implementation::Transformation<Transformation>::inverted(absoluteTransformation));
            }

            feature.cleanInverted(invertedMatrix);
        }
    }

    /* Mark object as clean */
    flags &= ~Flag::Dirty;
}

}}

#endif
