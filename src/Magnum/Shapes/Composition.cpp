/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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

#include "Composition.h"

#include <algorithm>
#include <Corrade/Utility/Assert.h>

#include "Magnum/Shapes/Implementation/CollisionDispatch.h"

namespace Magnum { namespace Shapes {

/*
Hierarchy implementation notes:

The hierarchy is stored in flat array to provide easy access for the user and
to save some allocations. Each node has zero, one or two subnodes. Value of
`Node::rightNode` describes which child nodes exist:

 *  0 - no child subnodes
 *  1 - only left subnode exists
 *  2 - only right subnode exists
 * >2 - both child nodes exist

If left node exists, it is right next to current one. If right node exists, it
is at position `Node::rightNode-1` relative to current one (this applies also
when `rightNode` is equal to 2, right node is right next to current one,
because there are no left nodes).

The node also specifies which shapes belong to it. Root node owns whole shape
array and `Node::rightShape` marks first shape belonging to the right child
node, relatively to begin. This recurses into child nodes, thus left child node
has shapes from parent's begin to parent's `rightShape`.

Shapes are merged together by concatenating its node and shape list and adding
new node at the beginning with properly set `rightNode` and `rightShape`.
Because these values are relative to parent, they don't need to be modified
when concatenating.
*/

template<UnsignedInt dimensions> Composition<dimensions>::Composition(const Composition<dimensions>& other): _shapes(other._shapes.size()), _nodes(other._nodes.size()) {
    copyShapes(0, other);
    copyNodes(0, other);
}

template<UnsignedInt dimensions> Composition<dimensions>::Composition(Composition<dimensions>&& other): _shapes(std::move(other._shapes)), _nodes(std::move(other._nodes)) {
    other._shapes = nullptr;
    other._nodes = nullptr;
}

template<UnsignedInt dimensions> Composition<dimensions>::~Composition() {
    for(std::size_t i = 0; i != _shapes.size(); ++i)
        delete _shapes[i];
}

template<UnsignedInt dimensions> Composition<dimensions>& Composition<dimensions>::operator=(const Composition<dimensions>& other) {
    for(std::size_t i = 0; i != _shapes.size(); ++i)
        delete _shapes[i];

    if(_shapes.size() != other._shapes.size())
        _shapes = Containers::Array<Implementation::AbstractShape<dimensions>*>(other._shapes.size());

    if(_nodes.size() != other._nodes.size())
        _nodes = Containers::Array<Node>(other._nodes.size());

    copyShapes(0, other);
    copyNodes(0, other);
    return *this;
}

template<UnsignedInt dimensions> Composition<dimensions>& Composition<dimensions>::operator=(Composition<dimensions>&& other) {
    using std::swap;
    swap(other._shapes, _shapes);
    swap(other._nodes, _nodes);
    return *this;
}

template<UnsignedInt dimensions> void Composition<dimensions>::copyShapes(const std::size_t offset, Composition<dimensions>&& other) {
    CORRADE_INTERNAL_ASSERT(_shapes.size() >= other._shapes.size()+offset);
    std::move(other._shapes.begin(), other._shapes.end(), _shapes.begin()+offset);
    other._shapes = nullptr;
}

template<UnsignedInt dimensions> void Composition<dimensions>::copyShapes(const std::size_t offset, const Composition<dimensions>& other) {
    CORRADE_INTERNAL_ASSERT(_shapes.size() >= other._shapes.size()+offset);
    for(Implementation::AbstractShape<dimensions> * const* i = other._shapes.begin(), ** o = _shapes.begin()+offset; i != other._shapes.end(); ++i, ++o)
        *o = (*i)->clone();
}

template<UnsignedInt dimensions> void Composition<dimensions>::copyNodes(std::size_t offset, const Composition<dimensions>& other) {
    CORRADE_INTERNAL_ASSERT(_nodes.size() >= other._nodes.size()+offset);
    std::copy(other._nodes.begin(), other._nodes.end(), _nodes.begin()+offset);
}

template<UnsignedInt dimensions> Composition<dimensions> Composition<dimensions>::transformed(const MatrixTypeFor<dimensions, Float>& matrix) const {
    Composition<dimensions> out(*this);
    for(Implementation::AbstractShape<dimensions> * const* i = _shapes.begin(), * const* o = out._shapes.begin(); i != _shapes.end(); ++i, ++o)
        (*i)->transform(matrix, *o);
    return out;
}

template<UnsignedInt dimensions> bool Composition<dimensions>::collides(const Implementation::AbstractShape<dimensions>& a, const std::size_t node, const std::size_t shapeBegin, const std::size_t shapeEnd) const {
    /* Empty group */
    if(shapeBegin == shapeEnd) return false;

    CORRADE_INTERNAL_ASSERT(node < _nodes.size() && shapeBegin < shapeEnd);

    /* Collision on the left child. If the node is leaf one (no left child
       exists), do it directly, recurse instead. */
    const bool collidesLeft = (_nodes[node].rightNode == 0 || _nodes[node].rightNode == 2) ?
        Implementation::collides(a, *_shapes[shapeBegin]) :
        collides(a, node+1, shapeBegin, shapeBegin+_nodes[node].rightShape);

    /* NOT operation */
    if(_nodes[node].operation == CompositionOperation::Not)
        return !collidesLeft;

    /* Short-circuit evaluation for AND/OR */
    if((_nodes[node].operation == CompositionOperation::Or) == collidesLeft)
        return collidesLeft;

    /* Now the collision result depends only on the right child. Similar to
       collision on the left child. */
    return (_nodes[node].rightNode < 2) ?
        Implementation::collides(a, *_shapes[shapeBegin+_nodes[node].rightShape]) :
        collides(a, node+_nodes[node].rightNode-1, shapeBegin+_nodes[node].rightShape, shapeEnd);
}

#ifndef DOXYGEN_GENERATING_OUTPUT
template class MAGNUM_SHAPES_EXPORT Composition<2>;
template class MAGNUM_SHAPES_EXPORT Composition<3>;
#endif

}}
