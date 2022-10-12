/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022 Vladimír Vondruš <mosra@centrum.cz>

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

#include "CompileLines.h"

#include "Magnum/GL/Buffer.h"
#include "Magnum/GL/Mesh.h"
#include "Magnum/MeshTools/Compile.h"
#include "Magnum/MeshTools/Implementation/GenerateLines.h"

/* This header is included only privately and doesn't introduce any linker
   dependency (taking just the PreviousPosition, NextPosition and Annotation
   attribute typedefs), thus it's completely safe to not link to the Shaders
   library */
#include "Magnum/Shaders/LineGL.h"

namespace Magnum { namespace MeshTools {

GL::Mesh compileLines(const Trade::MeshData& lineMesh) {
    Trade::MeshData mesh = Implementation::generateLines(lineMesh);

    #ifdef CORRADE_GRACEFUL_ASSERT
    /* If it asserted inside, bail */
    if(!mesh.attributeCount()) return GL::Mesh{};
    #endif

    /* Upload the buffers, bind the line-specific attributes manually */
    GL::Buffer indices{GL::Buffer::TargetHint::ElementArray, mesh.indexData()};
    GL::Buffer vertices{GL::Buffer::TargetHint::Array, mesh.vertexData()};
    GL::Mesh out = compile(mesh, std::move(indices), vertices);

    /* Warn about attributes that are conflicting with line-specific attributes
       and thus will get overwritten */
    static_assert(Shaders::GenericGL3D::TextureCoordinates::Location == UnsignedInt(Shaders::LineGL3D::Annotation::Location), "");
    static_assert(Shaders::GenericGL3D::Tangent::Location == UnsignedInt(Shaders::LineGL3D::PreviousPosition::Location), "");
    static_assert(Shaders::GenericGL3D::Normal::Location == UnsignedInt(Shaders::LineGL3D::NextPosition::Location), "");
    if(const Containers::Optional<UnsignedInt> id = lineMesh.findAttributeId(Trade::MeshAttribute::TextureCoordinates))
        Warning{} << "MeshTools::compileLines():" << lineMesh.attributeName(*id) << "conflicts with line annotation attribute, ignoring";
    if(const Containers::Optional<UnsignedInt> id = lineMesh.findAttributeId(Trade::MeshAttribute::Tangent))
        Warning{} << "MeshTools::compileLines():" << lineMesh.attributeName(*id) << "conflicts with line previous position attribute, ignoring";
    if(const Containers::Optional<UnsignedInt> id = lineMesh.findAttributeId(Trade::MeshAttribute::Normal))
        Warning{} << "MeshTools::compileLines():" << lineMesh.attributeName(*id) << "conflicts with line next position attribute, ignoring";

    /* PreviousPosition / NextPosition are bound to the same location in both
       2D and 3D, using the 3D variant so it can be trimmed to just two
       components in 2D (which wouldn't be possible the other way around) */
    out.addVertexBuffer(vertices,
        mesh.attributeOffset(Implementation::MeshAttributePreviousPosition),
        mesh.attributeStride(Implementation::MeshAttributePreviousPosition),
        GL::DynamicAttribute{Shaders::LineGL3D::PreviousPosition{}, mesh.attributeFormat(Implementation::MeshAttributePreviousPosition)});
    out.addVertexBuffer(vertices,
        mesh.attributeOffset(Implementation::MeshAttributeNextPosition),
        mesh.attributeStride(Implementation::MeshAttributeNextPosition),
        GL::DynamicAttribute{Shaders::LineGL3D::NextPosition{}, mesh.attributeFormat(Implementation::MeshAttributeNextPosition)});
    out.addVertexBuffer(std::move(vertices),
        mesh.attributeOffset(Implementation::MeshAttributeAnnotation),
        mesh.attributeStride(Implementation::MeshAttributeAnnotation),
        GL::DynamicAttribute{Shaders::LineGL3D::Annotation{}, mesh.attributeFormat(Implementation::MeshAttributeAnnotation)});
    return out;
}

}}
