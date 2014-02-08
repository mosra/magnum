#ifndef Magnum_SceneGraph_Object_h
#define Magnum_SceneGraph_Object_h
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
 * @brief Class @ref Magnum::SceneGraph::Object
 */

#include <Corrade/Containers/EnumSet.h>

#include "Magnum/SceneGraph/AbstractFeature.h"
#include "Magnum/SceneGraph/AbstractObject.h"
#include "Magnum/SceneGraph/visibility.h"

namespace Magnum { namespace SceneGraph {

namespace Implementation {
    enum class ObjectFlag: UnsignedByte {
        Dirty = 1 << 0,
        Visited = 1 << 1,
        Joint = 1 << 2
    };

    typedef Containers::EnumSet<ObjectFlag, UnsignedByte> ObjectFlags;

    CORRADE_ENUMSET_OPERATORS(ObjectFlags)
}

/**
@brief %Object

Base of scene graph. Contains specific transformation implementation, takes
care of parent/children relationship and contains features. See @ref scenegraph
for introduction.

Common usage is to typedef @ref Object with desired transformation type to save
unnecessary typing later, along with @ref Scene and possibly other types, e.g.:
@code
typedef SceneGraph::Scene<SceneGraph::MatrixTransformation3D> Scene3D;
typedef SceneGraph::Object<SceneGraph::MatrixTransformation3D> Object3D;
@endcode

Uses @ref Corrade::Containers::LinkedList for parent/children relationship.
Traversing through the list is done like in the following code. It is also
possible to go in reverse order using @ref lastChild() and @ref previousSibling().
@code
for(Object* child = o->firstChild(); child; child = child->nextSibling()) {
    // ...
}
@endcode

@section Object-explicit-specializations Explicit template specializations

The following specialization are explicitly compiled into @ref SceneGraph
library. For other specializations (e.g. using @ref Double type or special
transformation class) you have to use @ref Object.hpp implementation file to
avoid linker errors. See also relevant sections in
@ref AbstractObject-explicit-specializations "AbstractObject" and
@ref AbstractTransformation-explicit-specializations "AbstractTransformation"
class documentation or @ref compilation-speedup-hpp for more information.

-   @ref DualComplexTransformation "Object<DualComplexTransformation>"
-   @ref DualQuaternionTransformation "Object<DualQuaternionTransformation>"
-   @ref MatrixTransformation2D "Object<MatrixTransformation2D>"
-   @ref MatrixTransformation3D "Object<MatrixTransformation3D>"
-   @ref RigidMatrixTransformation2D "Object<RigidMatrixTransformation2D>"
-   @ref RigidMatrixTransformation3D "Object<RigidMatrixTransformation3D>"
-   @ref TranslationTransformation2D "Object<TranslationTransformation2D>"
-   @ref TranslationTransformation3D "Object<TranslationTransformation3D>"

@see @ref Scene, @ref AbstractFeature, @ref AbstractTransformation,
    @ref DebugTools::ObjectRenderer
@todo Consider using `mutable` for flags to make transformation computation
    available on const refs
*/
template<class Transformation> class Object: public AbstractObject<Transformation::Dimensions, typename Transformation::Type>, public Transformation
    #ifndef DOXYGEN_GENERATING_OUTPUT
    , private Containers::LinkedList<Object<Transformation>>, private Containers::LinkedListItem<Object<Transformation>, Object<Transformation>>
    #endif
{
    friend class Containers::LinkedList<Object<Transformation>>;
    friend class Containers::LinkedListItem<Object<Transformation>, Object<Transformation>>;

    #ifndef DOXYGEN_GENERATING_OUTPUT
    Object(const Object<Transformation>&) = delete;
    Object(Object<Transformation>&&) = delete;
    Object<Transformation>& operator=(const Object<Transformation>&) = delete;
    Object<Transformation>& operator=(Object<Transformation>&&) = delete;
    #endif

    public:
        /** @brief Matrix type */
        typedef typename DimensionTraits<Transformation::Dimensions, typename Transformation::Type>::MatrixType MatrixType;

        /**
         * @brief Constructor
         * @param parent    Parent object
         */
        explicit Object(Object<Transformation>* parent = nullptr);

        /**
         * @brief Destructor
         *
         * Removes itself from parent's children list and destroys all own
         * children.
         */
        ~Object();

        /**
         * @{ @name Scene hierarchy
         *
         * See @ref scenegraph-hierarchy for more information.
         */

        /** @copydoc AbstractObject::scene() */
        Scene<Transformation>* scene();
        const Scene<Transformation>* scene() const; /**< @overload */

        /** @brief Parent object or `nullptr`, if this is root object */
        Object<Transformation>* parent() {
            return Containers::LinkedListItem<Object<Transformation>, Object<Transformation>>::list();
        }

        /** @overload */
        const Object<Transformation>* parent() const {
            return Containers::LinkedListItem<Object<Transformation>, Object<Transformation>>::list();
        }

        /** @brief Previous sibling object or `nullptr`, if this is first object */
        Object<Transformation>* previousSibling() {
            return Containers::LinkedListItem<Object<Transformation>, Object<Transformation>>::previous();
        }

        /** @overload */
        const Object<Transformation>* previousSibling() const {
            return Containers::LinkedListItem<Object<Transformation>, Object<Transformation>>::previous();
        }

        /** @brief Next sibling object or `nullptr`, if this is last object */
        Object<Transformation>* nextSibling() {
            return Containers::LinkedListItem<Object<Transformation>, Object<Transformation>>::next();
        }

        /** @overload */
        const Object<Transformation>* nextSibling() const {
            return Containers::LinkedListItem<Object<Transformation>, Object<Transformation>>::next();
        }

        /** @brief Whether this object has children */
        bool hasChildren() const {
            return !Containers::LinkedList<Object<Transformation>>::isEmpty();
        }

        /** @brief First child object or `nullptr`, if this object has no children */
        Object<Transformation>* firstChild() {
            return Containers::LinkedList<Object<Transformation>>::first();
        }

        /** @overload */
        const Object<Transformation>* firstChild() const {
            return Containers::LinkedList<Object<Transformation>>::first();
        }

        /** @brief Last child object or `nullptr`, if this object has no children */
        Object<Transformation>* lastChild() {
            return Containers::LinkedList<Object<Transformation>>::last();
        }

        /** @overload */
        const Object<Transformation>* lastChild() const {
            return Containers::LinkedList<Object<Transformation>>::last();
        }

        /**
         * @brief Set parent object
         * @return Reference to self (for method chaining)
         *
         * @see @ref setParentKeepTransformation()
         */
        Object<Transformation>& setParent(Object<Transformation>* parent);

        /**
         * @brief Set parent object and keep absolute transformation
         * @return Reference to self (for method chaining)
         *
         * While @ref setParent() preserves relative transformation of the
         * object, this function preserves absolute transformation (i.e., the
         * object stays in place after reparenting).
         */
        Object<Transformation>& setParentKeepTransformation(Object<Transformation>* parent);

        /*@}*/

        /** @{ @name Object transformation */

        /**
         * @brief Transformation matrix
         *
         * See also `transformation()` function of various transformation
         * classes.
         */
        MatrixType transformationMatrix() const {
            return Implementation::Transformation<Transformation>::toMatrix(Transformation::transformation());
        }

        /**
         * @brief Transformation matrix relative to root object
         *
         * @see @ref absoluteTransformation()
         */
        MatrixType absoluteTransformationMatrix() const {
            return Implementation::Transformation<Transformation>::toMatrix(absoluteTransformation());
        }

        /**
         * @brief Transformation relative to root object
         *
         * @see @ref absoluteTransformationMatrix()
         */
        typename Transformation::DataType absoluteTransformation() const;

        /**
         * @brief Transformation matrices of given set of objects relative to this object
         *
         * All transformations are premultiplied with @p initialTransformationMatrix,
         * if specified.
         * @see @ref transformations()
         */
        std::vector<MatrixType> transformationMatrices(const std::vector<std::reference_wrapper<Object<Transformation>>>& objects, const MatrixType& initialTransformationMatrix = MatrixType()) const;

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @copybrief transformationMatrices(const std::vector<std::reference_wrapper<Object<Transformation>>>&, const MatrixType&)
         * @deprecated Use @ref Magnum::SceneGraph::Object::transformationMatrices(const std::vector<std::reference_wrapper<Object<Transformation>>>&, const MatrixType&) "transformationMatrices(const std::vector<std::reference_wrapper<Object<Transformation>>>&, const MatrixType&)" instead.
         */
        CORRADE_DEPRECATED("use transformationMatrices(const std::vector<std::reference_wrapper<Object<Transformation>>>&, const MatrixType&) instead") std::vector<MatrixType> transformationMatrices(const std::vector<Object<Transformation>*>& objects, const MatrixType& initialTransformationMatrix = MatrixType()) const;

        #ifdef CORRADE_GCC47_COMPATIBILITY
        /* Workarounds to avoid ambiguous overload errors on GCC < 4.8. And I
           thought 4.7 was bug-free. */
        std::vector<MatrixType> transformationMatrices(std::initializer_list<std::reference_wrapper<Object<Transformation>>> objects, const MatrixType& initialTransformationMatrix = MatrixType()) const {
            return transformationMatrices(std::vector<std::reference_wrapper<Object<Transformation>>>{objects}, initialTransformationMatrix);
        }
        CORRADE_DEPRECATED("use transformationMatrices(const std::vector<std::reference_wrapper<Object<Transformation>>>&, const MatrixType&) instead") std::vector<MatrixType> transformationMatrices(std::initializer_list<Object<Transformation>*> objects, const MatrixType& initialTransformationMatrix = MatrixType()) const;
        #endif
        #endif

        /**
         * @brief Transformations of given group of objects relative to this object
         *
         * All transformations can be premultiplied with @p initialTransformation,
         * if specified.
         * @see @ref transformationMatrices()
         */
        /* `objects` passed by copy intentionally (to allow move from
           transformationMatrices() and avoid copy in the function itself) */
        std::vector<typename Transformation::DataType> transformations(std::vector<std::reference_wrapper<Object<Transformation>>> objects, const typename Transformation::DataType& initialTransformation = typename Transformation::DataType()) const;

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @copybrief transformations(std::vector<std::reference_wrapper<Object<Transformation>>>, const typename Transformation::DataType&)
         * @deprecated Use @ref Magnum::SceneGraph::Object::transformations(std::vector<std::reference_wrapper<Object<Transformation>>>, const typename Transformation::DataType&) "transformations(std::vector<std::reference_wrapper<Object<Transformation>>>, const typename Transformation::DataType&)" instead.
         */
        CORRADE_DEPRECATED("use transformations(std::vector<std::reference_wrapper<Object<Transformation>>>, const typename Transformation::DataType&) instead") std::vector<typename Transformation::DataType> transformations(const std::vector<Object<Transformation>*>& objects, const typename Transformation::DataType& initialTransformation = typename Transformation::DataType()) const;

        #ifdef CORRADE_GCC47_COMPATIBILITY
        /* Workarounds to avoid ambiguous overload errors on GCC < 4.8. And I
           thought 4.7 was bug-free. */
        std::vector<typename Transformation::DataType> transformations(std::initializer_list<std::reference_wrapper<Object<Transformation>>> objects, const typename Transformation::DataType& initialTransformation = typename Transformation::DataType()) const {
            return transformations(std::vector<std::reference_wrapper<Object<Transformation>>>{objects}, initialTransformation);
        }
        CORRADE_DEPRECATED("use transformations(std::vector<std::reference_wrapper<Object<Transformation>>>, const typename Transformation::DataType&) instead") std::vector<typename Transformation::DataType> transformations(std::initializer_list<Object<Transformation>*> objects, const typename Transformation::DataType& initialTransformation = typename Transformation::DataType()) const;
        #endif
        #endif

        /*@}*/

        /**
         * @{ @name Transformation caching
         *
         * See @ref scenegraph-caching for more information.
         */

        /**
         * @brief Clean absolute transformations of given set of objects
         *
         * Only dirty objects in the list are cleaned.
         * @see @ref setClean()
         */
        /* `objects` passed by copy intentionally (to avoid copy internally) */
        static void setClean(std::vector<std::reference_wrapper<Object<Transformation>>> objects);

        #ifdef MAGNUM_BUILD_DEPRECATED
        /**
         * @copybrief setClean(std::vector<std::reference_wrapper<Object<Transformation>>>)
         * @deprecated Use @ref Magnum::SceneGraph::Object::setClean(std::vector<std::reference_wrapper<Object<Transformation>>> "setClean(std::vector<std::reference_wrapper<Object<Transformation>>>" instead.
         */
        CORRADE_DEPRECATED("use setClean(std::vector<std::reference_wrapper<Object<Transformation>>>) instead") static void setClean(const std::vector<Object<Transformation>*>& objects);

        #ifdef CORRADE_GCC47_COMPATIBILITY
        /* Workarounds to avoid ambiguous overload errors on GCC < 4.8. And I
           thought 4.7 was bug-free. */
        static void setClean(std::initializer_list<std::reference_wrapper<Object<Transformation>>> objects) {
            setClean(std::vector<std::reference_wrapper<Object<Transformation>>>{objects});
        }
        static CORRADE_DEPRECATED("use setClean(std::vector<std::reference_wrapper<Object<Transformation>>>) instead") void setClean(std::initializer_list<Object<Transformation>*> objects);
        #endif
        #endif

        /** @copydoc AbstractObject::isDirty() */
        bool isDirty() const { return !!(flags & Flag::Dirty); }

        /** @copydoc AbstractObject::setDirty() */
        void setDirty();

        /**
         * @brief Clean object absolute transformation
         *
         * Calls @ref AbstractFeature::clean() and/or @ref AbstractFeature::cleanInverted()
         * on all object features which have caching enabled and recursively
         * calls @ref setClean() on every parent which is not already clean. If
         * the object is already clean, the function does nothing.
         *
         * See also @ref setClean(std::vector<std::reference_wrapper<Object<Transformation>>>),
         * which cleans given set of objects more efficiently than when calling
         * @ref setClean() on each object individually.
         * @see @ref scenegraph-caching, @ref setDirty(), @ref isDirty()
         */
        /* note: doc verbatim copied from AbstractObject::setClean() */
        void setClean();

        /*@}*/

    #ifndef DOXYGEN_GENERATING_OUTPUT
    public:
        virtual bool isScene() const { return false; }
    #endif

    private:
        Object<Transformation>* doScene() override final;
        const Object<Transformation>* doScene() const override final;

        MatrixType MAGNUM_SCENEGRAPH_LOCAL doTransformationMatrix() const override final {
            return transformationMatrix();
        }
        MatrixType MAGNUM_SCENEGRAPH_LOCAL doAbsoluteTransformationMatrix() const override final {
            return absoluteTransformationMatrix();
        }

        std::vector<MatrixType> doTransformationMatrices(const std::vector<std::reference_wrapper<AbstractObject<Transformation::Dimensions, typename Transformation::Type>>>& objects, const MatrixType& initialTransformationMatrix) const override final;

        typename Transformation::DataType MAGNUM_SCENEGRAPH_LOCAL computeJointTransformation(const std::vector<std::reference_wrapper<Object<Transformation>>>& jointObjects, std::vector<typename Transformation::DataType>& jointTransformations, const std::size_t joint, const typename Transformation::DataType& initialTransformation) const;

        bool MAGNUM_SCENEGRAPH_LOCAL doIsDirty() const override final { return isDirty(); }
        void MAGNUM_SCENEGRAPH_LOCAL doSetDirty() override final { setDirty(); }
        void MAGNUM_SCENEGRAPH_LOCAL doSetClean() override final { setClean(); }
        void doSetClean(const std::vector<std::reference_wrapper<AbstractObject<Transformation::Dimensions, typename Transformation::Type>>>& objects) override final;

        void MAGNUM_SCENEGRAPH_LOCAL setCleanInternal(const typename Transformation::DataType& absoluteTransformation);

        typedef Implementation::ObjectFlag Flag;
        typedef Implementation::ObjectFlags Flags;
        UnsignedShort counter;
        Flags flags;
};

}}

#endif
