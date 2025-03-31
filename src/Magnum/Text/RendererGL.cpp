/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
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

#include "RendererGL.h"

#include <Corrade/Containers/EnumSet.hpp>
#include <Corrade/Containers/StringView.h>

#include "Magnum/Shaders/GenericGL.h" /* no link-time dependency here */
#include "Magnum/Text/AbstractGlyphCache.h"
#include "Magnum/Text/Implementation/rendererState.h"

/* Somehow on GCC 4.8 to 7 the {} passed as a default argument for
   ArrayView<const FeatureRange> causes "error: elements of array 'const class
   Magnum::Text::FeatureRange [0]' have incomplete type". GCC 9 is fine, no
   idea about version 8, but including the definition for it as well to be
   safe. Similar problem happens with MSVC STL, where the initializer_list is
   implemented as a (begin, end) pair and size() is a difference of those two
   pointers. Which needs to know the type size to calculate the actual element
   count. */
#if (defined(CORRADE_TARGET_GCC) && __GNUC__ <= 8) || defined(CORRADE_TARGET_DINKUMWARE)
#include "Magnum/Text/Feature.h"
#endif

namespace Magnum { namespace Text {

Debug& operator<<(Debug& debug, const RendererGLFlag value) {
    debug << "Text::RendererGLFlag" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(v) case RendererGLFlag::v: return debug << "::" #v;
        _c(GlyphPositionsClusters)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << Debug::hex << UnsignedByte(value) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const RendererGLFlags value) {
    return Containers::enumSetDebugOutput(debug, value, "Text::RendererGLFlags{}", {
        RendererGLFlag::GlyphPositionsClusters
    });
}

struct RendererGL::State: Renderer::State {
    explicit State(const AbstractGlyphCache& glyphCache, RendererGLFlags flags);

    GL::Buffer indices{GL::Buffer::TargetHint::ElementArray},
        vertices{GL::Buffer::TargetHint::Array};
    GL::Mesh mesh;

    /* Because querying GL buffer size is not possible on all platforms and it
       may be slow, track the size here. It's used to know whether the buffer
       should be reuploaded as a whole or can be partially updated, updated in
       both reserve() and render(). */
    UnsignedInt bufferGlyphCapacity = 0;
};

RendererGL::State::State(const AbstractGlyphCache& glyphCache, RendererGLFlags flags): Renderer::State{glyphCache, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, RendererFlags{UnsignedByte(flags)}} {
    #ifdef MAGNUM_TARGET_GLES2
    CORRADE_ASSERT(glyphCache.size().z() == 1,
        "Text::RendererGL: array glyph caches are not supported in OpenGL ES 2.0 and WebGL 1 builds", );
    #endif

    /* As documented in RendererGL::setIndexType(), use of 8-bit indices is
       discouraged on contemporary GPUs */
    indexType = minIndexType = MeshIndexType::UnsignedShort;

    /* Set up the mesh with the initial index type and zero primitives to draw.
       The count gets updated on each renderer(), index buffer properties each
       time the index type changes. */
    mesh.setIndexBuffer(indices, 0, indexType)
        .setCount(0);
    #ifndef MAGNUM_TARGET_GLES2
    if(glyphCache.size().z() != 1) {
        mesh.addVertexBuffer(vertices, 0,
            Shaders::GenericGL2D::Position{},
            Shaders::GenericGL2D::TextureArrayCoordinates{});
    } else
    #endif
    {
        mesh.addVertexBuffer(vertices, 0,
            Shaders::GenericGL2D::Position{},
            Shaders::GenericGL2D::TextureCoordinates{});
    }
}

RendererGL::RendererGL(const AbstractGlyphCache& glyphCache, RendererGLFlags flags): Renderer{Containers::pointer<State>(glyphCache, flags)} {}

RendererGL::RendererGL(RendererGL&&) noexcept = default;

RendererGL::~RendererGL() = default;

RendererGL& RendererGL::operator=(RendererGL&&) noexcept = default;

RendererGLFlags RendererGL::flags() const {
    return RendererGLFlags{UnsignedByte(_state->flags)};
}

GL::Mesh& RendererGL::mesh() {
    return static_cast<State&>(*_state).mesh;
}

const GL::Mesh& RendererGL::mesh() const {
    return static_cast<const State&>(*_state).mesh;
}

RendererGL& RendererGL::setIndexType(MeshIndexType atLeast) {
    State& state = static_cast<State&>(*_state);

    Renderer::setIndexType(atLeast);

    /* Upload indices anew if the type is different from before. In this case
       it's also most likely that the size is bigger than before, so do it as
       a setData() call instead of having a specialized setSubData() code path
       if the total size shrinks.

       Besides the type, the capacity should not change compared to when the
       buffer was last updated in reserve() or render(). (Which only holds for
       builtin allocators, but RendererGL so far allows only builtin allocators
       so that's fine. It however does *not* hold for `state.indexData`, as
       that can stay larger if the index type becomes smaller, so verifying
       against `state.glyphPositions` instead.) */
    CORRADE_INTERNAL_ASSERT(state.bufferGlyphCapacity == state.glyphPositions.size());
    if(GL::meshIndexType(state.indexType) != state.mesh.indexType()) {
        state.indices.setData(state.indexData);
        state.mesh.setIndexBuffer(state.indices, 0, state.indexType);
    }

    return *this;
}

RendererGL& RendererGL::clear() {
    Renderer::clear();
    static_cast<State&>(*_state).mesh.setCount(0);
    return *this;
}

RendererGL& RendererGL::reset() {
    Renderer::reset();
    static_cast<State&>(*_state).mesh.setCount(0);
    return *this;
}

RendererGL& RendererGL::reserve(const UnsignedInt glyphCapacity, const UnsignedInt runCapacity) {
    State& state = static_cast<State&>(*_state);

    Renderer::reserve(glyphCapacity, runCapacity);

    /* Upload indices anew if the capacity is bigger than before */
    if(state.bufferGlyphCapacity < glyphCapacity) {
        state.indices.setData(state.indexData);
        /* Update the mesh index buffer reference if the type changed */
        if(GL::meshIndexType(state.indexType) != state.mesh.indexType())
            state.mesh.setIndexBuffer(state.indices, 0, state.indexType);

    /* If the capacity isn't bigger, the index type shouldn't have changed
       either and so no upload needs to be done. It can change only if the new
       capacity is too larger to fit the type used, or in a setIndexType()
       call, but there we handle the reupload directly. */
    } else CORRADE_INTERNAL_ASSERT(GL::meshIndexType(state.indexType) == state.mesh.indexType());

    /* Resize the vertex buffer and reupload its contents if the capacity is
       bigger than before */
    if(state.bufferGlyphCapacity < glyphCapacity) {
        const UnsignedInt glyphSize = 4*(
            #ifndef MAGNUM_TARGET_GLES2
            state.glyphCache.size().z() != 1 ?
                sizeof(Implementation::VertexArray) :
            #endif
            sizeof(Implementation::Vertex));

        /* The assumption in this case is that the capacity is bigger than the
           actually rendered glyph count, otherwise we'd have it all resized
           and uploaded in render() already. Thus we have to do a bigger
           setData() allocation first and then upload just a portion with
           setSubData(). */
        CORRADE_INTERNAL_ASSERT(glyphCapacity > state.glyphCount);
        state.vertices
            .setData({nullptr, glyphCapacity*glyphSize})
            .setSubData(0, state.vertexData.prefix(state.glyphCount*glyphSize));
    }

    /* Remember the currently used capacity if it grew. It can happen that
       reserve() is called with a smaller capacity, or with just runCapacity
       being larger, so this shouldn't reset that and cause needless reupload
       next time. */
    state.bufferGlyphCapacity = Math::max(state.bufferGlyphCapacity, glyphCapacity);

    return *this;
}

Containers::Pair<Range2D, Range1Dui> RendererGL::render() {
    State& state = static_cast<State&>(*_state);

    const Containers::Pair<Range2D, Range1Dui> out = Renderer::render();

    /* Upload indices anew if the glyph count is bigger than before */
    if(state.bufferGlyphCapacity < state.glyphCount) {
        state.indices.setData(state.indexData);
        /* Update the mesh index buffer reference if the type changed */
        if(GL::meshIndexType(state.indexType) != state.mesh.indexType())
            state.mesh.setIndexBuffer(state.indices, 0, state.indexType);

    /* If the glyph count isn't bigger, the index type shouldn't have changed
       either. Same reasoning as in reserve() above. */
    } else CORRADE_INTERNAL_ASSERT(GL::meshIndexType(state.indexType) == state.mesh.indexType());

    /* Upload vertices fully anew if the glyph count is bigger than before */
    const UnsignedInt glyphSize = 4*(
        #ifndef MAGNUM_TARGET_GLES2
        state.glyphCache.size().z() != 1 ?
            sizeof(Implementation::VertexArray) :
        #endif
        sizeof(Implementation::Vertex));
    if(state.bufferGlyphCapacity < state.glyphCount) {
        /* Unlike in render(), it's just setData() alone, with the assumption
           that the render() caused the capacity to grow to fit exactly all
           glyphs, and so we upload everything. (Which only holds for builtin
           vertex allocators, but RendererGL so far allows only builtin
           allocators so that's fine.) */
        CORRADE_INTERNAL_ASSERT(
            state.vertexPositions.size() == state.glyphCount*4 &&
            state.vertexTextureCoordinates.size() == state.glyphCount*4);
        state.vertices.setData(state.vertexData.prefix(state.glyphCount*glyphSize));

    /* Otherwise upload just what was rendered new */
    } else {
        const Range1Dui glyphRange = glyphsForRuns(out.second());
        state.vertices.setSubData(glyphRange.min()*glyphSize, state.vertexData.slice(glyphRange.min()*glyphSize, glyphRange.max()*glyphSize));
    }

    /* Remember the currently used capacity if it grew */
    state.bufferGlyphCapacity = Math::max(state.bufferGlyphCapacity, state.glyphCount);

    /* Set the mesh index count to exactly what was rendered in total */
    state.mesh.setCount(state.glyphCount*6);

    return out;
}

RendererGL& RendererGL::add(AbstractShaper& shaper, const Float size, const Containers::StringView text, const UnsignedInt begin, const UnsignedInt end, const Containers::ArrayView<const FeatureRange> features) {
    return static_cast<RendererGL&>(Renderer::add(shaper, size, text, begin, end, features));
}

RendererGL& RendererGL::add(AbstractShaper& shaper, const Float size, const Containers::StringView text, const UnsignedInt begin, const UnsignedInt end) {
    return static_cast<RendererGL&>(Renderer::add(shaper, size, text, begin, end));
}

RendererGL& RendererGL::add(AbstractShaper& shaper, const Float size, const Containers::StringView text, const UnsignedInt begin, const UnsignedInt end, const std::initializer_list<FeatureRange> features) {
    return static_cast<RendererGL&>(Renderer::add(shaper, size, text, begin, end, features));
}

RendererGL& RendererGL::add(AbstractShaper& shaper, const Float size, const Containers::StringView text, const Containers::ArrayView<const FeatureRange> features) {
    return static_cast<RendererGL&>(Renderer::add(shaper, size, text, features));
}

RendererGL& RendererGL::add(AbstractShaper& shaper, const Float size, const Containers::StringView text) {
    return static_cast<RendererGL&>(Renderer::add(shaper, size, text));
}

RendererGL& RendererGL::add(AbstractShaper& shaper, const Float size, const Containers::StringView text, const std::initializer_list<FeatureRange> features) {
    return static_cast<RendererGL&>(Renderer::add(shaper, size, text, features));
}

Containers::Pair<Range2D, Range1Dui> RendererGL::render(AbstractShaper& shaper, const Float size, const Containers::StringView text, const Containers::ArrayView<const FeatureRange> features) {
    /* Compared to Renderer::render() this calls our render() instead of
       Renderer::render() */
    add(shaper, size, text, features);
    return render();
}

Containers::Pair<Range2D, Range1Dui> RendererGL::render(AbstractShaper& shaper, const Float size, const Containers::StringView text) {
    return render(shaper, size, text, {});
}

Containers::Pair<Range2D, Range1Dui> RendererGL::render(AbstractShaper& shaper, const Float size, const Containers::StringView text, const std::initializer_list<FeatureRange> features) {
    return render(shaper, size, text, Containers::arrayView(features));
}

}}
