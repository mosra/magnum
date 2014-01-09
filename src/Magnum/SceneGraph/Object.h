#ifndef Magnum_SceneGraph_Object_h
#define Magnum_SceneGraph_Object_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Class Magnum::SceneGraph::Object
 */

#include <Containers/EnumSet.h>

#include "AbstractFeature.h"
#include "AbstractObject.h"

#include "magnumSceneGraphVisibility.h"

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
         * @see setParentKeepTransformation()
         */
        Object<Transformation>& setParent(Object<Transformation>* parent);

        /**
         * @brief Set parent object and keep absolute transformation
         * @return Reference to self (for method chaining)
         *
         * While setParent() preserves only relative transformation of the
         * object, this funcition preserves absolute transformation.
         */
        Object<Transformation>& setParentKeepTransformation(Object<Transformation>* parent);

        /*@}*/

        /** @{ @name Object transformation */

        /**
         * @brief Transformation matrix
         *
         * @see transformation()
         */
        MatrixType transformationMatrix() const {
            return Implementation::Transformation<Transformation>::toMatrix(Transformation::transformation());
        }

        /**
         * @brief Transformation matrix relative to root object
         *
         * @see absoluteTransformation()
         */
        MatrixType absoluteTransformationMatrix() const {
            return Implementation::Transformation<Transformation>::toMatrix(absoluteTransformation());
        }

        /**
         * @brief Transformation relative to root object
         *
         * @see absoluteTransformationMatrix()
         */
        typename Transformation::DataType absoluteTransformation() const;

        /**
         * @brief Transformation matrices of given set of objects relative to this object
         *
         * All transformations are premultiplied with @p initialTransformationMatrix,
         * if specified.
         * @see transformations()
         */
        std::vector<MatrixType> transformationMatrices(const std::vector<Object<Transformation>*>& objects, const MatrixType& initialTransformationMatrix = MatrixType()) const;

        /**
         * @brief Transformations of given group of objects relative to this object
         *
         * All transformations can be premultiplied with @p initialTransformation,
         * if specified.
         * @see transformationMatrices()
         */
        /* `objects` passed by copy intentionally (to allow move from
           transformationMatrices() and avoid copy in the function itself) */
        std::vector<typename Transformation::DataType> transformations(std::vector<Object<Transformation>*> objects, const typename Transformation::DataType& initialTransformation = typename Transformation::DataType()) const;

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
         * @see setClean()
         */
        /* `objects` passed by copy intentionally (to avoid copy internally) */
        static void setClean(std::vector<Object<Transformation>*> objects);

        /** @copydoc AbstractObject::isDirty() */
        bool isDirty() const { return !!(flags & Flag::Dirty); }

        /** @copydoc AbstractObject::setDirty() */
        void setDirty();

        /** @copydoc AbstractObject::setClean() */
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

        std::vector<MatrixType> doTransformationMatrices(const std::vector<AbstractObject<Transformation::Dimensions, typename Transformation::Type>*>& objects, const MatrixType& initialTransformationMatrix) const override final;

        typename Transformation::DataType MAGNUM_SCENEGRAPH_LOCAL computeJointTransformation(const std::vector<Object<Transformation>*>& jointObjects, std::vector<typename Transformation::DataType>& jointTransformations, const std::size_t joint, const typename Transformation::DataType& initialTransformation) const;

        bool MAGNUM_SCENEGRAPH_LOCAL doIsDirty() const override final { return isDirty(); }
        void MAGNUM_SCENEGRAPH_LOCAL doSetDirty() override final { setDirty(); }
        void MAGNUM_SCENEGRAPH_LOCAL doSetClean() override final { setClean(); }
        void doSetClean(const std::vector<AbstractObject<Transformation::Dimensions, typename Transformation::Type>*>& objects) override final;

        void MAGNUM_SCENEGRAPH_LOCAL setClean(const typename Transformation::DataType& absoluteTransformation);

        typedef Implementation::ObjectFlag Flag;
        typedef Implementation::ObjectFlags Flags;
        UnsignedShort counter;
        Flags flags;
};

}}

#endif
