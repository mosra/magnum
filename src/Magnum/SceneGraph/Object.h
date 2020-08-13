#ifndef Magnum_SceneGraph_Object_h
#define Magnum_SceneGraph_Object_h
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

    typedef Containers::EnumSet<ObjectFlag> ObjectFlags;

    CORRADE_ENUMSET_OPERATORS(ObjectFlags)
}

/**
@brief Object

@m_keywords{Object2D Object3D}

Base of scene graph. Contains specific transformation implementation, takes
care of parent/children relationship and contains features. See @ref scenegraph
for introduction.

Common usage is to typedef @ref Object with desired transformation type to save
unnecessary typing later, along with @ref Scene and possibly other types, e.g.:

@snippet MagnumSceneGraph.cpp Object-typedef

Uses @ref Corrade::Containers::LinkedList for efficient hierarchy management.
Traversing through the list of child objects can be done using range-based for:

@snippet MagnumSceneGraph.cpp Object-children-range

Or, if you need more flexibility, like in the following code. It is also
possible to go in reverse order using @ref Corrade::Containers::LinkedList::last()
and @ref previousSibling().

@snippet MagnumSceneGraph.cpp Object-children

@section SceneGraph-Object-explicit-specializations Explicit template specializations

The following specializations are explicitly compiled into the @ref SceneGraph
library. For other specializations (e.g. using @ref Magnum::Double "Double"
type or special transformation class) you have to use the @ref Object.hpp
implementation file (and possibly others) to avoid linker errors. See also
relevant sections in the @ref SceneGraph-AbstractObject-explicit-specializations "AbstractObject" and
@ref SceneGraph-AbstractTransformation-explicit-specializations "AbstractTransformation"
class documentation or @ref compilation-speedup-hpp for more information.

-   @ref DualComplexTransformation "Object<DualComplexTransformation>"
-   @ref DualQuaternionTransformation "Object<DualQuaternionTransformation>"
-   @ref MatrixTransformation2D "Object<MatrixTransformation2D>" (custom
    specializations need also @ref MatrixTransformation2D.hpp)
-   @ref MatrixTransformation3D "Object<MatrixTransformation3D>" (custom
    specializations need also @ref MatrixTransformation3D.hpp)
-   @ref RigidMatrixTransformation2D "Object<RigidMatrixTransformation2D>"
    (custom specializations need also @ref RigidMatrixTransformation2D.hpp)
-   @ref RigidMatrixTransformation3D "Object<RigidMatrixTransformation3D>"
    (custom specializations need also @ref RigidMatrixTransformation3D.hpp)
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
    public:
        /** @brief Matrix type */
        typedef MatrixTypeFor<Transformation::Dimensions, typename Transformation::Type> MatrixType;

        /**
         * @brief Constructor
         * @param parent    Parent object
         */
        explicit Object(Object<Transformation>* parent = nullptr);

        /** @brief Copying is not allowed */
        Object(const Object<Transformation>&) = delete;

        /** @brief Moving is not allowed */
        Object(Object<Transformation>&&) = delete;

        /**
         * @brief Destructor
         *
         * Removes itself from parent's children list and destroys all own
         * children.
         */
        ~Object();

        /** @brief Copying is not allowed */
        Object<Transformation>& operator=(const Object<Transformation>&) = delete;

        /** @brief Moving is not allowed */
        Object<Transformation>& operator=(Object<Transformation>&&) = delete;

        /**
         * @{ @name Scene hierarchy
         *
         * See @ref scenegraph-hierarchy for more information.
         */

        /** @copydoc AbstractObject::scene() */
        Scene<Transformation>* scene();
        const Scene<Transformation>* scene() const; /**< @overload */

        /** @brief Parent object or `nullptr`, if this is the root object */
        Object<Transformation>* parent() {
            return Containers::LinkedListItem<Object<Transformation>, Object<Transformation>>::list();
        }

        /** @overload */
        const Object<Transformation>* parent() const {
            return Containers::LinkedListItem<Object<Transformation>, Object<Transformation>>::list();
        }

        /**
         * @brief Move a child object before another
         * @return Reference to self (for method chaining)
         * @m_since_latest
         *
         * Doesn't have any effect on draw order, only on the child order when
         * iterating through @ref children(). The @p child is expected to be a
         * child of this object, @p before is either a child of this object or
         * @cpp nullptr @ce in which case the @p child is moved to the last
         * position in the child list.
         * @see @ref Corrade::Containers::LinkedList::move()
         */
        Object<Transformation>& move(Object<Transformation>& child, Object<Transformation>* before) {
            Containers::LinkedList<Object<Transformation>>::move(&child, before);
            return *this;
        }

        /** @brief Previous sibling object or `nullptr`, if this is the first object */
        Object<Transformation>* previousSibling() {
            return Containers::LinkedListItem<Object<Transformation>, Object<Transformation>>::previous();
        }

        /** @overload */
        const Object<Transformation>* previousSibling() const {
            return Containers::LinkedListItem<Object<Transformation>, Object<Transformation>>::previous();
        }

        /** @brief Next sibling object or `nullptr`, if this is the last object */
        Object<Transformation>* nextSibling() {
            return Containers::LinkedListItem<Object<Transformation>, Object<Transformation>>::next();
        }

        /** @overload */
        const Object<Transformation>* nextSibling() const {
            return Containers::LinkedListItem<Object<Transformation>, Object<Transformation>>::next();
        }

        /**
         * @brief Child objects
         *
         * @see @ref parent(), @ref previousSibling(), @ref nextSibling()
         */
        Containers::LinkedList<Object<Transformation>>& children() {
            return static_cast<Containers::LinkedList<Object<Transformation>>&>(*this);
        }

        /** @overload */
        const Containers::LinkedList<Object<Transformation>>& children() const {
            return static_cast<const Containers::LinkedList<Object<Transformation>>&>(*this);
        }

        /**
         * @brief Add a child
         *
         * Calling `object.addChild<MyObject>(args...)` is equivalent to
         * `new MyObject{args...}` followed by an appropriate @ref setParent()
         * call.
         */
        template<class T, class ...Args> T& addChild(Args&&... args) {
            T* child = new T{std::forward<Args>(args)...};
            child->setParent(this);
            return *child;
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

        /* Since 1.8.17, the original short-hand group closing doesn't work
           anymore. FFS. */
        /**
         * @}
         */

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
         * @brief Transformation matrix relative to the root object
         *
         * @see @ref absoluteTransformation()
         */
        MatrixType absoluteTransformationMatrix() const {
            return Implementation::Transformation<Transformation>::toMatrix(absoluteTransformation());
        }

        /**
         * @brief Transformation relative to the root object
         *
         * @see @ref absoluteTransformationMatrix()
         */
        typename Transformation::DataType absoluteTransformation() const;

        /**
         * @brief Transformation matrices of given set of objects relative to this object
         *
         * All transformations are post-multiplied with
         * @p finalTransformationMatrix, if specified (it gets applied on the
         * left-most side, suitable for example for an inverse camera
         * transformation or a projection matrix).
         * @see @ref transformations()
         */
        std::vector<MatrixType> transformationMatrices(const std::vector<std::reference_wrapper<Object<Transformation>>>& objects, const MatrixType& finalTransformationMatrix = MatrixType()) const;

        /**
         * @brief Transformations of given group of objects relative to this object
         *
         * All transformations are post-multiplied with
         * @p finalTransformation, if specified (it gets applied on the
         * left-most side, suitable for example for an inverse camera
         * transformation).
         * @see @ref transformationMatrices()
         */
        /* `objects` passed by copy intentionally (to allow move from
           transformationMatrices() and avoid copy in the function itself) */
        std::vector<typename Transformation::DataType> transformations(std::vector<std::reference_wrapper<Object<Transformation>>> objects, const typename Transformation::DataType& finalTransformation =
            #ifndef CORRADE_MSVC2015_COMPATIBILITY /* I hate this inconsistency */
            typename Transformation::DataType()
            #else
            Transformation::DataType()
            #endif
            ) const;

        /* Since 1.8.17, the original short-hand group closing doesn't work
           anymore. FFS. */
        /**
         * @}
         */

        /**
         * @{ @name Transformation caching
         *
         * See @ref scenegraph-features-caching for more information.
         */

        /**
         * @brief Clean absolute transformations of given set of objects
         *
         * Only dirty objects in the list are cleaned.
         * @see @ref setClean()
         */
        /* `objects` passed by copy intentionally (to avoid copy internally) */
        static void setClean(std::vector<std::reference_wrapper<Object<Transformation>>> objects);

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
         * @see @ref scenegraph-features-caching, @ref setDirty(),
         *      @ref isDirty()
         */
        /* note: doc verbatim copied from AbstractObject::setClean() */
        void setClean();

        /* Since 1.8.17, the original short-hand group closing doesn't work
           anymore. FFS. */
        /**
         * @}
         */

    #ifndef DOXYGEN_GENERATING_OUTPUT
    public:
        virtual bool isScene() const { return false; }

        /* Since the class inherits from both in order to implement a tree,
           the compilers complain that erase() is ambiguous (because each is
           from a different base, *not* because both have the same signature).
           Fixing that by bringing them both locally, then the overload works
           because of a different signature. */
        using Containers::LinkedList<Object<Transformation>>::erase;
    private:
        using Containers::LinkedListItem<Object<Transformation>, Object<Transformation>>::erase;
    #endif

    private:
        #ifndef DOXYGEN_GENERATING_OUTPUT /* https://bugzilla.gnome.org/show_bug.cgi?id=776986 */
        friend Containers::LinkedList<Object<Transformation>>;
        friend Containers::LinkedListItem<Object<Transformation>, Object<Transformation>>;
        #endif

        Object<Transformation>* doScene() override final;
        const Object<Transformation>* doScene() const override final;

        Object<Transformation>* doParent() override final;
        const Object<Transformation>* doParent() const override final;

        MatrixType MAGNUM_SCENEGRAPH_LOCAL doTransformationMatrix() const override final {
            return transformationMatrix();
        }
        MatrixType MAGNUM_SCENEGRAPH_LOCAL doAbsoluteTransformationMatrix() const override final {
            return absoluteTransformationMatrix();
        }

        std::vector<MatrixType> doTransformationMatrices(const std::vector<std::reference_wrapper<AbstractObject<Transformation::Dimensions, typename Transformation::Type>>>& objects, const MatrixType& finalTransformationMatrix) const override final;

        typename Transformation::DataType MAGNUM_SCENEGRAPH_LOCAL computeJointTransformation(const std::vector<std::reference_wrapper<Object<Transformation>>>& jointObjects, std::vector<typename Transformation::DataType>& jointTransformations, const std::size_t joint, const typename Transformation::DataType& finalTransformation) const;

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
