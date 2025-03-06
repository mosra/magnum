/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2022 Pablo Escobar <mail@rvrs.in>

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

#include <Corrade/Containers/Iterable.h>
#include <Corrade/Containers/ScopeGuard.h>
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/Containers/StringView.h>
#include <Corrade/TestSuite/Compare/Numeric.h>
#include <Corrade/Utility/Format.h>

#include "Magnum/Image.h"
#include "Magnum/Mesh.h"
#include "Magnum/GL/AbstractShaderProgram.h"
#include "Magnum/GL/Buffer.h"
#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/Framebuffer.h"
#include "Magnum/GL/Mesh.h"
#include "Magnum/GL/MeshView.h"
#include "Magnum/GL/OpenGLTester.h"
#include "Magnum/GL/PixelFormat.h"
#include "Magnum/GL/Renderbuffer.h"
#include "Magnum/GL/RenderbufferFormat.h"
#include "Magnum/GL/Shader.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Math/Half.h"
#include "Magnum/Math/Matrix.h"
#include "Magnum/Math/Vector4.h"

#ifndef MAGNUM_TARGET_WEBGL
#include <Corrade/Containers/String.h>
#endif

namespace Magnum { namespace GL { namespace Test { namespace {

/* Tests also the MeshView class. */

struct MeshGLTest: OpenGLTester {
    explicit MeshGLTest();

    void construct();
    void constructMove();
    void wrap();

    void destructMovedOutInstance();

    template<class T> void primitive();

    #ifndef MAGNUM_TARGET_WEBGL
    void label();
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    void addVertexBufferUnsignedInt();
    void addVertexBufferInt();
    #endif
    void addVertexBufferFloat();
    #ifndef MAGNUM_TARGET_GLES
    void addVertexBufferDouble();
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    void addVertexBufferVectorNui();
    void addVertexBufferVectorNi();
    #endif
    void addVertexBufferVectorN();
    #ifndef MAGNUM_TARGET_GLES
    void addVertexBufferVectorNd();
    #endif
    void addVertexBufferMatrixNxN();
    #ifndef MAGNUM_TARGET_GLES
    void addVertexBufferMatrixNxNd();
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    void addVertexBufferMatrixMxN();
    #endif
    #ifndef MAGNUM_TARGET_GLES
    void addVertexBufferMatrixMxNd();
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    void addVertexBufferUnsignedIntWithUnsignedShort();
    void addVertexBufferUnsignedIntWithShort();
    void addVertexBufferIntWithUnsignedShort();
    void addVertexBufferIntWithShort();
    #endif
    #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    /* Other Float types omitted (covered by addVertexBufferNormalized()) */
    void addVertexBufferFloatWithHalf();
    #endif
    #ifndef MAGNUM_TARGET_GLES
    void addVertexBufferFloatWithDouble();
    void addVertexBufferVector3WithUnsignedInt10f11f11fRev();
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    void addVertexBufferVector4WithUnsignedInt2101010Rev();
    void addVertexBufferVector4WithInt2101010Rev();
    #endif

    void addVertexBufferLessVectorComponents();
    void addVertexBufferNormalized();
    #ifndef MAGNUM_TARGET_GLES
    void addVertexBufferBGRA();
    #endif

    void addVertexBufferMultiple();
    void addVertexBufferMultipleGaps();

    void addVertexBufferMovedOutInstance();
    void addVertexBufferTransferOwnwership();
    void addVertexBufferInstancedTransferOwnwership();
    void addVertexBufferDynamicTransferOwnwership();
    void addVertexBufferInstancedDynamicTransferOwnwership();

    template<class T> void setIndexBuffer();
    template<class T> void setIndexBufferRange();
    void setIndexBufferUnsignedInt();

    void setIndexBufferMovedOutInstance();
    template<class T> void setIndexBufferTransferOwnership();
    template<class T> void setIndexBufferRangeTransferOwnership();

    void setIndexOffset();

    void indexTypeSetIndexOffsetNotIndexed();

    void unbindVAOWhenSettingIndexBufferData();
    void unbindIndexBufferWhenBindingVao();
    void resetIndexBufferBindingWhenBindingVao();
    void unbindVAOBeforeEnteringExternalSection();
    void bindScratchVaoWhenEnteringExternalSection();

    #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    void setBaseVertex();
    #endif
    #ifdef MAGNUM_TARGET_GLES
    void setBaseVertexNoExtensionAvailable();
    void setBaseVertexRangeNoExtensionAvailable();
    #endif
    void setInstanceCount();
    #ifndef MAGNUM_TARGET_GLES2
    void setInstanceCountBaseInstance();
    #ifdef MAGNUM_TARGET_GLES
    void setInstanceCountBaseInstanceNoExtensionAvailable();
    #endif
    #endif
    void setInstanceCountIndexed();
    #ifndef MAGNUM_TARGET_GLES2
    void setInstanceCountIndexedBaseInstance();
    #ifdef MAGNUM_TARGET_GLES
    void setInstanceCountIndexedBaseInstanceNoExtensionAvailable();
    #endif
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    void setInstanceCountIndexedBaseVertex();
    #endif
    #ifdef MAGNUM_TARGET_GLES
    void setInstanceCountIndexedBaseVertexNoExtensionAvailable();
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    void setInstanceCountIndexedBaseVertexBaseInstance();
    #ifdef MAGNUM_TARGET_GLES
    void setInstanceCountIndexedBaseVertexBaseInstanceNoExtensionAvailable();
    #endif
    #endif

    void addVertexBufferInstancedFloat();
    #ifndef MAGNUM_TARGET_GLES2
    void addVertexBufferInstancedInteger();
    #endif
    #ifndef MAGNUM_TARGET_GLES
    void addVertexBufferInstancedDouble();
    #endif
    void resetDivisorAfterInstancedDraw();
    void drawInstancedAttributeSingleInstance();

    void multiDraw();
    void multiDrawSparseArrays();
    void multiDrawViews();
    template<class T> void multiDrawIndexed();
    template<class T> void multiDrawIndexedSparseArrays();
    void multiDrawIndexedViews();
    void multiDrawWrongVertexOffsetSize();
    void multiDrawIndexedWrongVertexOffsetSize();
    void multiDrawIndexedWrongIndexOffsetSize();
    #ifdef MAGNUM_TARGET_GLES
    void multiDrawIndexedBaseVertexNoExtensionAvailable();
    void multiDrawIndexedViewsBaseVertexNoExtensionAvailable();
    #endif
    void multiDrawViewsInstanced();
    void multiDrawViewsDifferentMeshes();
    #ifdef MAGNUM_TARGET_GLES
    void multiDrawInstanced();
    void multiDrawInstancedSparseArrays();
    /* no MeshView support for instanced multidraw */
    template<class T> void multiDrawInstancedIndexed();
    template<class T> void multiDrawInstancedIndexedSparseArrays();
    /* no MeshView support for instanced multidraw */
    void multiDrawInstancedWrongInstanceCountSize();
    void multiDrawInstancedWrongVertexOffsetSize();
    #ifndef MAGNUM_TARGET_GLES2
    void multiDrawInstancedWrongInstanceOffsetSize();
    #endif
    void multiDrawInstancedIndexedWrongInstanceCountSize();
    void multiDrawInstancedIndexedWrongVertexOffsetSize();
    void multiDrawInstancedIndexedWrongIndexOffsetSize();
    #ifndef MAGNUM_TARGET_GLES2
    void multiDrawInstancedIndexedWrongInstanceOffsetSize();
    void multiDrawInstancedBaseVertexNoExtensionAvailable();
    void multiDrawInstancedBaseInstanceNoExtensionAvailable();
    #endif
    #endif
};

const struct {
    const char* name;
    bool vertexId;
    bool drawId;
    Vector4 values[4];
    UnsignedInt counts[4];
    UnsignedInt vertexOffsets[4];
    Vector4 expected;
} MultiDrawData[] {
    {"all skipped", false, false,
        {{0.25f, 0.0f, 0.0f, 0.0f},
         {0.5f, 0.0f, 0.0f, 0.0f},
         {0.75f, 0.0f, 0.0f, 0.0f},
         {1.0f, 0.0f, 0.0f, 0.0f}},
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0.0f, 0.0f, 0.0f, 0.0f}},
    {"single draw", false, false,
        {{0.25f, 0.0f, 0.0f, 0.0f},
         {0.5f, 0.0f, 0.0f, 0.0f},
         {0.75f, 0.0f, 0.0f, 0.0f},
         {1.0f, 0.0f, 0.0f, 0.0f}},
        {4, 0, 0, 0},
        {0, 0, 0, 0},
        {0.25f, 0.5f, 0.75f, 1.0f}},
    #ifndef MAGNUM_TARGET_GLES2
    {"single draw, vertex ID", true, false,
        {{0.25f, 0.0f, 0.0f, 0.0f},
         {0.0f, 0.5f, 0.0f, 0.0f},
         {0.0f, 0.0f, 0.75f, 0.0f},
         {0.0f, 0.0f, 0.0f, 1.0f}},
        {4, 0, 0, 0},
        {0, 0, 0, 0},
        {0.25f, 0.5f, 0.75f, 1.0f}},
    #endif
    {"single draw, draw ID", false, true,
        {{0.25f, 0.0f, 0.0f, 0.0f},
         {0.5f, 0.0f, 0.0f, 0.0f},
         {0.75f, 0.0f, 0.0f, 0.0f},
         {1.0f, 0.0f, 0.0f, 0.0f}},
        {4, 0, 0, 0},
        {0, 0, 0, 0},
        {0.25f, 0.5f, 0.75f, 1.0f}},
    {"multi draw", false, false,
        {{0.25f, 0.0f, 0.0f, 0.0f},
         {0.5f, 0.0f, 0.0f, 0.0f},
         {0.75f, 0.0f, 0.0f, 0.0f},
         {1.0f, 0.0f, 0.0f, 0.0f}},
        {1, 1, 1, 1},
        {0, 1, 2, 3},
        {0.25f, 0.5f, 0.75f, 1.0f}},
    #ifndef MAGNUM_TARGET_GLES2
    {"multi draw, vertex ID", true, false,
        /* Interestingly enough, the gl_VertexID includes the vertexOffset
           (or the `first` parameter in glDrawArrays / glMultiDrawArrays). */
        {{0.25f, 0.0f, 0.0f, 0.0f},
         {0.0f, 0.5f, 0.0f, 0.0f},
         {0.0f, 0.0f, 0.75f, 0.0f},
         {0.0f, 0.0f, 0.0f, 1.0f}},
        {1, 1, 1, 1},
        {0, 1, 2, 3},
        {0.25f, 0.5f, 0.75f, 1.0f}},
    #endif
    {"multi draw, draw ID", false, true,
        {{0.25f, 0.0f, 0.0f, 0.0f},
         {0.0f, 0.5f, 0.0f, 0.0f},
         {0.0f, 0.0f, 0.75f, 0.0f},
         {0.0f, 0.0f, 0.0f, 1.0f}},
        {1, 1, 1, 1},
        {0, 1, 2, 3},
        {0.25f, 0.5f, 0.75f, 1.0f}},
    {"multi draw, random order, one skipped", false, false,
        {{0.25f, 0.0f, 0.0f, 0.0f},
         {0.5f, 0.0f, 0.0f, 0.0f},
         {0.75f, 0.0f, 0.0f, 0.0f},
         {1.0f, 0.0f, 0.0f, 0.0f}},
        {1, 0, 1, 1},
        {1, 0, 3, 2},
        /* The positions are fixed so this still renders in the same order */
        {0.0f, 0.5f, 0.75f, 1.0f}},
};

const struct {
    const char* name;
    bool vertexId;
    Vector4 values[4];
    UnsignedInt indices[4];
    UnsignedInt counts[4];
    UnsignedInt indexOffsetsInBytes[4];
    UnsignedInt vertexOffsets[4];
    Vector4 expected;
} MultiDrawIndexedData[] {
    {"single draw", false,
        {{0.25f, 0.0f, 0.0f, 0.0f},
         {0.5f, 0.0f, 0.0f, 0.0f},
         {0.75f, 0.0f, 0.0f, 0.0f},
         {1.0f, 0.0f, 0.0f, 0.0f}},
        {0, 1, 2, 3},
        {4, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0.25f, 0.5f, 0.75f, 1.0f}},
    {"multi draw", false,
        {{0.25f, 0.0f, 0.0f, 0.0f},
         {0.5f, 0.0f, 0.0f, 0.0f},
         {0.75f, 0.0f, 0.0f, 0.0f},
         {1.0f, 0.0f, 0.0f, 0.0f}},
        {0, 1, 2, 3},
        {1, 1, 1, 1},
        {0, 4, 8, 12},
        {0, 0, 0, 0},
        {0.25f, 0.5f, 0.75f, 1.0f}},
    #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    {"multi draw, vertex offset", false,
        {{0.25f, 0.0f, 0.0f, 0.0f},
         {0.5f, 0.0f, 0.0f, 0.0f},
         {0.75f, 0.0f, 0.0f, 0.0f},
         {1.0f, 0.0f, 0.0f, 0.0f}},
        {0, 1, 0, 1},
        {1, 1, 1, 1},
        {0, 4, 8, 12},
        {0, 0, 2, 2},
        {0.25f, 0.5f, 0.75f, 1.0f}},
    #ifndef MAGNUM_TARGET_GLES2
    {"multi draw, vertex offset, vertex ID", true,
        /* Same as in the non-indexed case, gl_VertexID includes the baseVertex
           as well */
        {{0.25f, 0.0f, 0.0f, 0.0f},
         {0.0f, 0.5f, 0.0f, 0.0f},
         {0.0f, 0.0f, 0.75f, 0.0f},
         {0.0f, 0.0f, 0.0f, 1.0f}},
        {0, 1, 0, 1},
        {1, 1, 1, 1},
        {0, 4, 8, 12},
        {0, 0, 2, 2},
        {0.25f, 0.5f, 0.75f, 1.0f}},
    #endif
    #endif
    {"multi draw, random index order & offset, one skipped", false,
        {{0.25f, 0.0f, 0.0f, 0.0f},
         {0.5f, 0.0f, 0.0f, 0.0f},
         {0.75f, 0.0f, 0.0f, 0.0f},
         {1.0f, 0.0f, 0.0f, 0.0f}},
        {5, 1, 0, 3},
        {1, 0, 1, 1},
        {12, 0, 8, 4},
        {0, 0, 0, 0},
        /* The positions are fixed so this still renders in the same order */
        {0.25f, 0.5f, 0.0f, 1.0f}}
};

#ifdef MAGNUM_TARGET_GLES
const struct {
    const char* name;
    bool vertexId;
    bool drawId;
    Vector3 values[2];
    UnsignedInt counts[2];
    UnsignedInt instanceCounts[2];
    UnsignedInt vertexOffsets[2];
    UnsignedInt instanceOffsets[2];
    Vector4 expected;
} MultiDrawInstancedData[] {
    {"all zero vertex counts", false, false,
        {{0.25f, 0.75f, 0.0f},
         {0.5f, 1.0f, 0.0f}},
        {0, 0},
        {1, 1},
        {0, 0},
        {0, 0},
        {0.0f, 0.0f, 0.0f, 0.0f}},
    {"all zero instance counts", false, false,
        {{0.25f, 0.75f, 0.0f},
         {0.5f, 1.0f, 0.0f}},
        {1, 1},
        {0, 0},
        {0, 0},
        {0, 0},
        {0.0f, 0.0f, 0.0f, 0.0f}},
    {"single draw", false, false,
        {{0.25f, 0.75f, 0.0f},
         {0.5f, 1.0f, 0.0f}},
        {2, 0},
        {2, 0},
        {0, 0},
        {0, 0},
        {0.25f, 0.5f, 0.75f, 1.0f}},
    #ifndef MAGNUM_TARGET_GLES2
    {"single draw, vertex ID", true, false,
        {{0.25f, 0.75f, 0.0f},
         {0.0f, 0.5f, 1.0f}},
        {2, 0},
        {2, 0},
        {0, 0},
        {0, 0},
        {0.25f, 0.5f, 0.75f, 1.0f}},
    #endif
    {"single draw, draw ID", false, true,
        {{0.25f, 0.75f, 0.0f},
         {0.5f, 1.0f, 0.0f}},
        {2, 0},
        {2, 0},
        {0, 0},
        {0, 0},
        {0.25f, 0.5f, 0.75f, 1.0f}},
    {"multi draw", false, false,
        {{0.25f, 0.75f, 0.0f},
         {0.5f, 1.0f, 0.0f}},
        {1, 1},
        {2, 2},
        {0, 1},
        {0, 0},
        {0.25f, 0.5f, 0.75f, 1.0f}},
    #ifndef MAGNUM_TARGET_GLES2
    {"multi draw, vertex ID", true, false,
        {{0.25f, 0.75f, 0.0f},
         {0.0f, 0.5f, 1.0f}},
        {1, 1},
        {2, 2},
        {0, 1},
        {0, 0},
        {0.25f, 0.5f, 0.75f, 1.0f}},
    #endif
    {"multi draw, draw ID", false, true,
        {{0.25f, 0.75f, 0.0f},
         {0.0f, 0.5f, 1.0f}},
        {1, 1},
        {2, 2},
        {0, 1},
        {0, 0},
        {0.25f, 0.5f, 0.75f, 1.0f}},
    #ifndef MAGNUM_TARGET_GLES2
    {"multi draw, instance offset", false, false,
        {{0.25f, 0.75f, 0.0f},
         {0.5f, 1.0f, 0.0f}},
        {2, 2},
        {1, 1},
        {0, 0},
        {0, 1},
        {0.25f, 0.5f, 0.75f, 1.0f}}
    #endif
};

const struct {
    const char* name;
    bool vertexId;
    Vector3 values[2];
    UnsignedInt indices[2];
    UnsignedInt counts[2];
    UnsignedInt instanceCounts[2];
    UnsignedInt indexOffsetsInBytes[2];
    UnsignedInt vertexOffsets[2];
    UnsignedInt instanceOffsets[2];
    Vector4 expected;
} MultiDrawInstancedIndexedData[] {
    {"single draw", false,
        {{0.25f, 0.75f, 0.0f},
         {0.5f, 1.0f, 0.0f}},
        {0, 1},
        {2, 0},
        {2, 0},
        {0, 0},
        {0, 0},
        {0, 0},
        {0.25f, 0.5f, 0.75f, 1.0f}},
    {"multi draw", false,
        {{0.25f, 0.75f, 0.0f},
         {0.5f, 1.0f, 0.0f}},
        {0, 1},
        {1, 1},
        {2, 2},
        {0, 4},
        {0, 0},
        {0, 0},
        {0.25f, 0.5f, 0.75f, 1.0f}},
    #ifndef MAGNUM_TARGET_GLES2
    {"multi draw, vertex offset", false,
        {{0.25f, 0.75f, 0.0f},
         {0.5f, 1.0f, 0.0f}},
        {0, 0},
        {1, 1},
        {2, 2},
        {0, 0},
        {0, 1},
        {0, 0},
        {0.25f, 0.5f, 0.75f, 1.0f}},
    {"multi draw, vertex offset, vertex ID", true,
        {{0.25f, 0.75f, 0.0f},
         {0.0f, 0.5f, 1.0f}},
        /* Same as in the non-indexed case, gl_VertexID includes the baseVertex
           as well */
        {0, 0},
        {1, 1},
        {2, 2},
        {0, 0},
        {0, 1},
        {0, 0},
        {0.25f, 0.5f, 0.75f, 1.0f}},
    {"multi draw, instance offset", false,
        {{0.25f, 0.75f, 0.0f},
         {0.5f, 1.0f, 0.0f}},
        {0, 1},
        {2, 2},
        {1, 1},
        {0, 0},
        {0, 0},
        {0, 1},
        {0.25f, 0.5f, 0.75f, 1.0f}},
    #endif
};
#endif

using namespace Math::Literals;

MeshGLTest::MeshGLTest() {
    addTests({&MeshGLTest::construct,
              &MeshGLTest::constructMove,
              &MeshGLTest::wrap,

              &MeshGLTest::destructMovedOutInstance,

              &MeshGLTest::primitive<GL::MeshPrimitive>,
              &MeshGLTest::primitive<Magnum::MeshPrimitive>,

              #ifndef MAGNUM_TARGET_WEBGL
              &MeshGLTest::label
              #endif
              });

    /* First instance is always using Attribute, second DynamicAttribute */
    addInstancedTests({
        #ifndef MAGNUM_TARGET_GLES2
        &MeshGLTest::addVertexBufferUnsignedInt,
        &MeshGLTest::addVertexBufferInt,
        #endif
        &MeshGLTest::addVertexBufferFloat,
        #ifndef MAGNUM_TARGET_GLES
        &MeshGLTest::addVertexBufferDouble,
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        &MeshGLTest::addVertexBufferVectorNui,
        &MeshGLTest::addVertexBufferVectorNi,
        #endif
        &MeshGLTest::addVertexBufferVectorN,
        #ifndef MAGNUM_TARGET_GLES
        &MeshGLTest::addVertexBufferVectorNd,
        #endif
        &MeshGLTest::addVertexBufferMatrixNxN,
        #ifndef MAGNUM_TARGET_GLES
        &MeshGLTest::addVertexBufferMatrixNxNd,
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        &MeshGLTest::addVertexBufferMatrixMxN,
        #endif
        #ifndef MAGNUM_TARGET_GLES
        &MeshGLTest::addVertexBufferMatrixMxNd,
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        &MeshGLTest::addVertexBufferUnsignedIntWithUnsignedShort,
        &MeshGLTest::addVertexBufferUnsignedIntWithShort,
        &MeshGLTest::addVertexBufferIntWithUnsignedShort,
        &MeshGLTest::addVertexBufferIntWithShort,
        #endif
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        &MeshGLTest::addVertexBufferFloatWithHalf,
        #endif
        #ifndef MAGNUM_TARGET_GLES
        &MeshGLTest::addVertexBufferFloatWithDouble,
        &MeshGLTest::addVertexBufferVector3WithUnsignedInt10f11f11fRev,
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        &MeshGLTest::addVertexBufferVector4WithUnsignedInt2101010Rev,
        &MeshGLTest::addVertexBufferVector4WithInt2101010Rev,
        #endif

        &MeshGLTest::addVertexBufferLessVectorComponents,
        &MeshGLTest::addVertexBufferNormalized,
        #ifndef MAGNUM_TARGET_GLES
        &MeshGLTest::addVertexBufferBGRA
        #endif
        }, 2);

    addTests({&MeshGLTest::addVertexBufferMultiple,
              &MeshGLTest::addVertexBufferMultipleGaps,

              &MeshGLTest::addVertexBufferMovedOutInstance,
              &MeshGLTest::addVertexBufferTransferOwnwership,
              &MeshGLTest::addVertexBufferInstancedTransferOwnwership,
              &MeshGLTest::addVertexBufferDynamicTransferOwnwership,
              &MeshGLTest::addVertexBufferInstancedDynamicTransferOwnwership,

              &MeshGLTest::setIndexBuffer<GL::MeshIndexType>,
              &MeshGLTest::setIndexBuffer<Magnum::MeshIndexType>,
              &MeshGLTest::setIndexBufferRange<GL::MeshIndexType>,
              &MeshGLTest::setIndexBufferRange<Magnum::MeshIndexType>,
              &MeshGLTest::setIndexBufferUnsignedInt,

              &MeshGLTest::setIndexBufferMovedOutInstance,
              &MeshGLTest::setIndexBufferTransferOwnership<GL::MeshIndexType>,
              &MeshGLTest::setIndexBufferTransferOwnership<Magnum::MeshIndexType>,
              &MeshGLTest::setIndexBufferRangeTransferOwnership<GL::MeshIndexType>,
              &MeshGLTest::setIndexBufferRangeTransferOwnership<Magnum::MeshIndexType>,

              &MeshGLTest::setIndexOffset,

              &MeshGLTest::indexTypeSetIndexOffsetNotIndexed,

              &MeshGLTest::unbindVAOWhenSettingIndexBufferData,
              &MeshGLTest::unbindIndexBufferWhenBindingVao,
              &MeshGLTest::resetIndexBufferBindingWhenBindingVao,
              &MeshGLTest::unbindVAOBeforeEnteringExternalSection,
              &MeshGLTest::bindScratchVaoWhenEnteringExternalSection,

              #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
              &MeshGLTest::setBaseVertex,
              #endif
              #ifdef MAGNUM_TARGET_GLES
              &MeshGLTest::setBaseVertexNoExtensionAvailable,
              &MeshGLTest::setBaseVertexRangeNoExtensionAvailable,
              #endif
              &MeshGLTest::setInstanceCount,
              #ifndef MAGNUM_TARGET_GLES2
              &MeshGLTest::setInstanceCountBaseInstance,
              #ifdef MAGNUM_TARGET_GLES
              &MeshGLTest::setInstanceCountBaseInstanceNoExtensionAvailable,
              #endif
              #endif
              &MeshGLTest::setInstanceCountIndexed,
              #ifndef MAGNUM_TARGET_GLES2
              &MeshGLTest::setInstanceCountIndexedBaseInstance,
              #ifdef MAGNUM_TARGET_GLES
              &MeshGLTest::setInstanceCountIndexedBaseInstanceNoExtensionAvailable,
              #endif
              #endif
              #ifndef MAGNUM_TARGET_GLES2
              &MeshGLTest::setInstanceCountIndexedBaseVertex,
              #endif
              #ifdef MAGNUM_TARGET_GLES
              &MeshGLTest::setInstanceCountIndexedBaseVertexNoExtensionAvailable,
              #endif
              #ifndef MAGNUM_TARGET_GLES2
              &MeshGLTest::setInstanceCountIndexedBaseVertexBaseInstance,
              #ifdef MAGNUM_TARGET_GLES
              &MeshGLTest::setInstanceCountIndexedBaseVertexBaseInstanceNoExtensionAvailable,
              #endif
              #endif

              &MeshGLTest::addVertexBufferInstancedFloat,
              #ifndef MAGNUM_TARGET_GLES2
              &MeshGLTest::addVertexBufferInstancedInteger,
              #endif
              #ifndef MAGNUM_TARGET_GLES
              &MeshGLTest::addVertexBufferInstancedDouble,
              #endif
              &MeshGLTest::resetDivisorAfterInstancedDraw,
              &MeshGLTest::drawInstancedAttributeSingleInstance});

    addInstancedTests({&MeshGLTest::multiDraw,
                       &MeshGLTest::multiDrawSparseArrays,
                       &MeshGLTest::multiDrawViews},
        Containers::arraySize(MultiDrawData));

    addInstancedTests({
        &MeshGLTest::multiDrawIndexed<UnsignedInt>,
        #ifndef CORRADE_TARGET_32BIT
        &MeshGLTest::multiDrawIndexed<UnsignedLong>,
        #endif
        &MeshGLTest::multiDrawIndexedSparseArrays<UnsignedInt>,
        #ifndef CORRADE_TARGET_32BIT
        &MeshGLTest::multiDrawIndexedSparseArrays<UnsignedLong>,
        #endif
        &MeshGLTest::multiDrawIndexedViews
    }, Containers::arraySize(MultiDrawIndexedData));

    addTests({
        &MeshGLTest::multiDrawWrongVertexOffsetSize,
        &MeshGLTest::multiDrawIndexedWrongVertexOffsetSize,
        &MeshGLTest::multiDrawIndexedWrongIndexOffsetSize,
        #ifdef MAGNUM_TARGET_GLES
        &MeshGLTest::multiDrawIndexedBaseVertexNoExtensionAvailable,
        &MeshGLTest::multiDrawIndexedViewsBaseVertexNoExtensionAvailable,
        #endif
        &MeshGLTest::multiDrawViewsInstanced,
        &MeshGLTest::multiDrawViewsDifferentMeshes
    });

    #ifdef MAGNUM_TARGET_GLES
    addInstancedTests({&MeshGLTest::multiDrawInstanced,
                       &MeshGLTest::multiDrawInstancedSparseArrays},
        Containers::arraySize(MultiDrawInstancedData));

    addInstancedTests<MeshGLTest>({
        &MeshGLTest::multiDrawInstancedIndexed<UnsignedInt>,
        #ifndef CORRADE_TARGET_32BIT
        &MeshGLTest::multiDrawInstancedIndexed<UnsignedLong>,
        #endif
        &MeshGLTest::multiDrawInstancedIndexedSparseArrays<UnsignedInt>,
        #ifndef CORRADE_TARGET_32BIT
        &MeshGLTest::multiDrawInstancedIndexedSparseArrays<UnsignedLong>
        #endif
    }, Containers::arraySize(MultiDrawInstancedIndexedData));

    addTests({
        &MeshGLTest::multiDrawInstancedWrongInstanceCountSize,
        &MeshGLTest::multiDrawInstancedWrongVertexOffsetSize,
        #ifndef MAGNUM_TARGET_GLES2
        &MeshGLTest::multiDrawInstancedWrongInstanceOffsetSize,
        #endif
        &MeshGLTest::multiDrawInstancedIndexedWrongInstanceCountSize,
        &MeshGLTest::multiDrawInstancedIndexedWrongVertexOffsetSize,
        &MeshGLTest::multiDrawInstancedIndexedWrongIndexOffsetSize,
        #ifndef MAGNUM_TARGET_GLES2
        &MeshGLTest::multiDrawInstancedIndexedWrongInstanceOffsetSize,
        &MeshGLTest::multiDrawInstancedBaseVertexNoExtensionAvailable,
        &MeshGLTest::multiDrawInstancedBaseInstanceNoExtensionAvailable
        #endif
    });
    #endif

    /* Reset clear color to something trivial first */
    Renderer::setClearColor(0x000000_rgbf);
}

#ifndef MAGNUM_TARGET_WEBGL
using namespace Containers::Literals;
#endif

void MeshGLTest::construct() {
    {
        const Mesh mesh;

        MAGNUM_VERIFY_NO_GL_ERROR();

        #ifndef MAGNUM_TARGET_GLES
        if(Context::current().isExtensionSupported<Extensions::ARB::vertex_array_object>())
        #elif defined(MAGNUM_TARGET_GLES2)
        if(Context::current().isExtensionSupported<Extensions::OES::vertex_array_object>())
        #endif
        {
            CORRADE_VERIFY(mesh.id() > 0);
        }
    }

    MAGNUM_VERIFY_NO_GL_ERROR();
}

struct FloatShader: AbstractShaderProgram {
    explicit FloatShader(Containers::StringView type, Containers::StringView conversion
        /* See the definition for details */
        #if defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2)
        , bool dummy = false
        #endif
    );
};

void MeshGLTest::constructMove() {
    const Float data = Math::unpack<Float, UnsignedByte>(96);
    Buffer buffer1, buffer2;
    buffer1.setData({&data, 1}, BufferUsage::StaticDraw);
    buffer2.setData({nullptr, 8}, BufferUsage::StaticDraw);

    Mesh a;
    a.addVertexBuffer(buffer1, 0, Attribute<0, Float>{});
    const Int id = a.id();

    MAGNUM_VERIFY_NO_GL_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    if(Context::current().isExtensionSupported<Extensions::ARB::vertex_array_object>())
    #elif defined(MAGNUM_TARGET_GLES2)
    if(Context::current().isExtensionSupported<Extensions::OES::vertex_array_object>())
    #endif
    {
        CORRADE_VERIFY(id > 0);
    }

    /* Move construct */
    Mesh b(Utility::move(a));

    CORRADE_COMPARE(a.id(), 0);
    CORRADE_COMPARE(b.id(), id);

    /* Move assign */
    Mesh c;
    c.addVertexBuffer(buffer2, 4, Attribute<1, Float>{});
    const Int cId = c.id();
    c = Utility::move(b);

    MAGNUM_VERIFY_NO_GL_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    if(Context::current().isExtensionSupported<Extensions::ARB::vertex_array_object>())
    #elif defined(MAGNUM_TARGET_GLES2)
    if(Context::current().isExtensionSupported<Extensions::OES::vertex_array_object>())
    #endif
    {
        CORRADE_VERIFY(cId > 0);
    }

    CORRADE_COMPARE(b.id(), cId);
    CORRADE_COMPARE(c.id(), id);

    /* Move assign to a NoCreate instance */
    Mesh d{NoCreate};
    d = Utility::move(c);

    CORRADE_COMPARE(c.id(), 0);
    CORRADE_COMPARE(d.id(), id);

    /* Destroy */
    b = Mesh{NoCreate};

    /* Test that drawing still works properly */
    {
        MAGNUM_VERIFY_NO_GL_ERROR();

        Renderbuffer renderbuffer;
        renderbuffer.setStorage(
            #ifndef MAGNUM_TARGET_GLES2
            RenderbufferFormat::RGBA8,
            #else
            RenderbufferFormat::RGBA4,
            #endif
            Vector2i(1));
        Framebuffer framebuffer{{{}, Vector2i(1)}};
        framebuffer.attachRenderbuffer(Framebuffer::ColorAttachment(0), renderbuffer)
                   .bind();

        d.setPrimitive(MeshPrimitive::Points)
         .setCount(1);
        FloatShader shader{"float", "vec4(valueInterpolated, 0.0, 0.0, 0.0)"};
        shader.draw(d);

        MAGNUM_VERIFY_NO_GL_ERROR();

        #ifndef MAGNUM_TARGET_GLES2
        CORRADE_COMPARE(Containers::arrayCast<UnsignedByte>(framebuffer.read({{}, Vector2i{1}}, {PixelFormat::RGBA, PixelType::UnsignedByte}).data())[0], 96);
        #else /* RGBA4, so less precision */
        CORRADE_COMPARE_WITH(Containers::arrayCast<UnsignedByte>(framebuffer.read({{}, Vector2i{1}}, {PixelFormat::RGBA, PixelType::UnsignedByte}).data())[0], 96,
            TestSuite::Compare::around(16));
        #endif
    }

    CORRADE_VERIFY(std::is_nothrow_move_constructible<Mesh>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<Mesh>::value);
}

void MeshGLTest::wrap() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::vertex_array_object>())
        CORRADE_SKIP(Extensions::ARB::vertex_array_object::string() << "is not supported.");
    #elif defined(MAGNUM_TARGET_GLES2)
    if(!Context::current().isExtensionSupported<Extensions::OES::vertex_array_object>())
        CORRADE_SKIP(Extensions::OES::vertex_array_object::string() << "is not supported.");
    #endif

    GLuint id;
    #ifndef MAGNUM_TARGET_GLES2
    glGenVertexArrays(1, &id);
    #else
    glGenVertexArraysOES(1, &id);
    #endif

    /* Releasing won't delete anything */
    {
        auto mesh = Mesh::wrap(id, ObjectFlag::DeleteOnDestruction);
        CORRADE_COMPARE(mesh.release(), id);
    }

    /* ...so we can wrap it again */
    Mesh::wrap(id);
    #ifndef MAGNUM_TARGET_GLES2
    glDeleteVertexArrays(1, &id);
    #else
    glDeleteVertexArraysOES(1, &id);
    #endif
}

void MeshGLTest::destructMovedOutInstance() {
    {
        Containers::ScopeGuard restoreCurrentContext{&GL::Context::current(), GL::Context::makeCurrent};

        Mesh a = Mesh::wrap(0xabcd);
        CORRADE_COMPARE(a.id(), 0xabcd);

        a = Mesh{NoCreate};
        CORRADE_COMPARE(a.id(), 0);

        GL::Context::makeCurrent(nullptr);
    }

    /* It shouldn't try to access the current context to decide anything */
    CORRADE_VERIFY(true);
}

template<class T> void MeshGLTest::primitive() {
    setTestCaseTemplateName(std::is_same<T, MeshPrimitive>::value ?
        "GL::MeshPrimitive" : "Magnum::MeshPrimitive");

    {
        Mesh mesh{T::LineLoop};
        CORRADE_COMPARE(mesh.primitive(), MeshPrimitive::LineLoop);
    } {
        Mesh mesh;
        mesh.setPrimitive(T::TriangleFan);
        CORRADE_COMPARE(mesh.primitive(), MeshPrimitive::TriangleFan);
    }
}

#ifndef MAGNUM_TARGET_WEBGL
void MeshGLTest::label() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::vertex_array_object>())
        CORRADE_SKIP(Extensions::ARB::vertex_array_object::string() << "is not supported.");
    #elif defined(MAGNUM_TARGET_GLES2)
    if(!Context::current().isExtensionSupported<Extensions::OES::vertex_array_object>())
        CORRADE_SKIP(Extensions::OES::vertex_array_object::string() << "is not supported.");
    #endif

    /* No-Op version is tested in AbstractObjectGLTest */
    if(!Context::current().isExtensionSupported<Extensions::KHR::debug>() &&
       !Context::current().isExtensionSupported<Extensions::EXT::debug_label>())
        CORRADE_SKIP("Required extension is not supported");

    Mesh mesh;
    CORRADE_COMPARE(mesh.label(), "");
    MAGNUM_VERIFY_NO_GL_ERROR();

    /* Test the string size gets correctly used, instead of relying on null
       termination */
    mesh.setLabel("MyMesh!"_s.exceptSuffix(1));
    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(mesh.label(), "MyMesh");
    MAGNUM_VERIFY_NO_GL_ERROR();
}
#endif

#ifndef MAGNUM_TARGET_GLES2
struct IntegerShader: AbstractShaderProgram {
    explicit IntegerShader(Containers::StringView type);
};
#endif

#ifndef MAGNUM_TARGET_GLES
struct DoubleShader: AbstractShaderProgram {
    explicit DoubleShader(Containers::StringView type, Containers::StringView outputType, Containers::StringView conversion);
};
#endif

/** @todo clean this up, it does too much implicitly and there's no way to
    check just a subset, or the getters, or ... */
struct Checker {
    Checker(AbstractShaderProgram&& shader, RenderbufferFormat format, Mesh& mesh);

    template<class T> T get(PixelFormat format, PixelType type);

    Renderbuffer renderbuffer;
    Framebuffer framebuffer;
};

FloatShader::FloatShader(Containers::StringView type, Containers::StringView conversion
    /* WebGL 1 requires that at least one attribute is not instanced. The
       addVertexBufferInstancedFloat() and drawInstancedAttributeSingleInstance()
       tests set this to true to add a dummy input (that isn't even present in
       the mesh) to fix that. */
    #if defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2)
    , bool dummy
    #endif
) {
    /* We need special version for ES3, because GLSL in ES2 doesn't support
       rectangle matrices */
    #ifndef MAGNUM_TARGET_GLES
    Shader vert(
        #ifndef CORRADE_TARGET_APPLE
        Version::GL210
        #else
        Version::GL310
        #endif
        , Shader::Type::Vertex);
    Shader frag(
        #ifndef CORRADE_TARGET_APPLE
        Version::GL210
        #else
        Version::GL310
        #endif
        , Shader::Type::Fragment);
    #elif defined(MAGNUM_TARGET_GLES2)
    Shader vert(Version::GLES200, Shader::Type::Vertex);
    Shader frag(Version::GLES200, Shader::Type::Fragment);
    #else
    Shader vert(Version::GLES300, Shader::Type::Vertex);
    Shader frag(Version::GLES300, Shader::Type::Fragment);
    #endif

    vert.addSource(Utility::format(
        #if defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2)
        dummy ?
            "#if !defined(GL_ES) && __VERSION__ == 120\n"
            "#define mediump\n"
            "#endif\n"
            "#if (defined(GL_ES) && __VERSION__ < 300) || __VERSION__ == 120\n"
            "#define in attribute\n"
            "#define out varying\n"
            "#endif\n"
            "in mediump {0} value;\n"
            "in mediump float dummy;\n"
            "out mediump {0} valueInterpolated;\n"
            "void main() {{\n"
            "    valueInterpolated = value;\n"
            "    gl_PointSize = 1.0;\n"
            "    gl_Position = vec4(0.0, 0.0, dummy, 1.0);\n"
            "}}\n" :
        #endif
            "#if !defined(GL_ES) && __VERSION__ == 120\n"
            "#define mediump\n"
            "#endif\n"
            "#if (defined(GL_ES) && __VERSION__ < 300) || __VERSION__ == 120\n"
            "#define in attribute\n"
            "#define out varying\n"
            "#endif\n"
            "in mediump {0} value;\n"
            "out mediump {0} valueInterpolated;\n"
            "void main() {{\n"
            "    valueInterpolated = value;\n"
            "    gl_PointSize = 1.0;\n"
            "    gl_Position = vec4(0.0, 0.0, 0.0, 1.0);\n"
            "}}\n",
        type));
    frag.addSource(Utility::format(
        "#if !defined(GL_ES) && __VERSION__ == 120\n"
        "#define mediump\n"
        "#endif\n"
        "#if (defined(GL_ES) && __VERSION__ < 300) || __VERSION__ == 120\n"
        "#define in varying\n"
        "#define result gl_FragColor\n"
        "#endif\n"
        "in mediump {0} valueInterpolated;\n"
        "#if (defined(GL_ES) && __VERSION__ >= 300) || (!defined(GL_ES) && __VERSION__ >= 130)\n"
        "out mediump vec4 result;\n"
        "#endif\n"
        "void main() {{ result = {1}; }}\n", type, conversion));

    CORRADE_INTERNAL_ASSERT_OUTPUT(vert.compile() && frag.compile());

    attachShaders({vert, frag});

    bindAttributeLocation(0, "value");

    CORRADE_INTERNAL_ASSERT_OUTPUT(link());
}

#ifndef MAGNUM_TARGET_GLES2
IntegerShader::IntegerShader(const Containers::StringView type) {
    #ifndef MAGNUM_TARGET_GLES
    Shader vert(
        #ifndef CORRADE_TARGET_APPLE
        Version::GL300
        #else
        Version::GL310
        #endif
        , Shader::Type::Vertex);
    Shader frag(
        #ifndef CORRADE_TARGET_APPLE
        Version::GL300
        #else
        Version::GL310
        #endif
        , Shader::Type::Fragment);
    #else
    Shader vert(Version::GLES300, Shader::Type::Vertex);
    Shader frag(Version::GLES300, Shader::Type::Fragment);
    #endif

    vert.addSource(Utility::format(
        "in mediump {0} value;\n"
        "flat out mediump {0} valueInterpolated;\n"
        "void main() {{\n"
        "    valueInterpolated = value;\n"
        "    gl_PointSize = 1.0;\n"
        "    gl_Position = vec4(0.0, 0.0, 0.0, 1.0);\n"
        "}}\n", type));
    frag.addSource(Utility::format(
        "flat in mediump {0} valueInterpolated;\n"
        "out mediump {0} result;\n"
        "void main() {{ result = valueInterpolated; }}\n", type));

    CORRADE_INTERNAL_ASSERT_OUTPUT(vert.compile() && frag.compile());

    attachShaders({vert, frag});

    bindAttributeLocation(0, "value");

    CORRADE_INTERNAL_ASSERT_OUTPUT(link());
}
#endif

#ifndef MAGNUM_TARGET_GLES
DoubleShader::DoubleShader(const Containers::StringView type, const Containers::StringView outputType, const Containers::StringView conversion) {
    constexpr const Version version =
        #ifndef CORRADE_TARGET_APPLE
        Version::GL300;
        #else
        Version::GL400;
        #endif
    Shader vert{version, Shader::Type::Vertex};
    Shader frag(version, Shader::Type::Fragment);

    vert.addSource(Utility::format(
        "#extension GL_ARB_vertex_attrib_64bit: require\n"
        "#extension GL_ARB_gpu_shader_fp64: require\n"
        "in {0} value;\n"
        "out {1} valueInterpolated;\n"
        "void main() {{\n"
        "    valueInterpolated = {2};\n"
        "    gl_PointSize = 1.0;\n"
        "    gl_Position = vec4(0.0, 0.0, 0.0, 1.0);\n"
        "}}\n", type, outputType, conversion));
    frag.addSource(Utility::format(
        "in {0} valueInterpolated;\n"
        "out {0} result;\n"
        "void main() {{ result = valueInterpolated; }}\n", outputType));

    CORRADE_INTERNAL_ASSERT_OUTPUT(vert.compile() && frag.compile());

    attachShaders({vert, frag});

    bindAttributeLocation(0, "value");

    CORRADE_INTERNAL_ASSERT_OUTPUT(link());
}
#endif

Checker::Checker(AbstractShaderProgram&& shader, RenderbufferFormat format, Mesh& mesh): framebuffer({{}, Vector2i(1)}) {
    renderbuffer.setStorage(format, Vector2i(1));
    framebuffer.attachRenderbuffer(Framebuffer::ColorAttachment(0), renderbuffer);

    framebuffer.bind();
    mesh.setPrimitive(MeshPrimitive::Points)
        .setCount(2);

    /* Skip first vertex so we test also offsets */
    MeshView view{mesh};
    view.setCount(1)
        .setBaseVertex(mesh.baseVertex())
        .setInstanceCount(mesh.instanceCount())
        #ifndef MAGNUM_TARGET_GLES
        .setBaseInstance(mesh.baseInstance())
        #endif
        ;

    if(view.mesh().isIndexed()) view.setIndexOffset(1);

    shader.draw(view);
}

template<class T> T Checker::get(PixelFormat format, PixelType type) {
    return Containers::arrayCast<T>(framebuffer.read({{}, Vector2i{1}}, {format, type}).data())[0];
}

#ifndef MAGNUM_TARGET_GLES2
void MeshGLTest::addVertexBufferUnsignedInt() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    constexpr UnsignedInt data[] = { 0, 157, 35681 };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setBaseVertex(1);

    if(testCaseInstanceId() == 0) {
        setTestCaseDescription("Attribute");
        mesh.addVertexBuffer(buffer, 4, Attribute<0, UnsignedInt>{});
    } else if(testCaseInstanceId() == 1) {
        setTestCaseDescription("DynamicAttribute");
        mesh.addVertexBuffer(buffer, 4, 4, DynamicAttribute{
            DynamicAttribute::Kind::Integral, 0,
            DynamicAttribute::Components::One,
            DynamicAttribute::DataType::UnsignedInt});
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    const auto value = Checker(IntegerShader("uint"), RenderbufferFormat::R32UI, mesh).get<UnsignedInt>(
        #ifndef MAGNUM_TARGET_WEBGL
        PixelFormat::RedInteger,
        #else
        PixelFormat::RGBAInteger,
        #endif
        PixelType::UnsignedInt);

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(value, 35681);
}

void MeshGLTest::addVertexBufferInt() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    constexpr Int data[] = { 0, 457931, 27530 };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setBaseVertex(1);

    if(testCaseInstanceId() == 0) {
        setTestCaseDescription("Attribute");
        mesh.addVertexBuffer(buffer, 4, Attribute<0, Int>{});
    } else if(testCaseInstanceId() == 1) {
        setTestCaseDescription("DynamicAttribute");
        mesh.addVertexBuffer(buffer, 4, 4, DynamicAttribute{
            DynamicAttribute::Kind::Integral, 0,
            DynamicAttribute::Components::One,
            DynamicAttribute::DataType::Int});
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    const auto value = Checker(IntegerShader("int"), RenderbufferFormat::R32I, mesh).get<Int>(
        #ifndef MAGNUM_TARGET_WEBGL
        PixelFormat::RedInteger,
        #else
        PixelFormat::RGBAInteger,
        #endif
        PixelType::Int);

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(value, 27530);
}
#endif

void MeshGLTest::addVertexBufferFloat() {
    const Float data[] = { 0.0f, -0.7f, Math::unpack<Float, UnsignedByte>(96) };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setBaseVertex(1);

    if(testCaseInstanceId() == 0) {
        setTestCaseDescription("Attribute");
        mesh.addVertexBuffer(buffer, 4, Attribute<0, Float>{});
    } else if(testCaseInstanceId() == 1) {
        setTestCaseDescription("DynamicAttribute");
        mesh.addVertexBuffer(buffer, 4, 4, DynamicAttribute{
            DynamicAttribute::Kind::Generic, 0,
            DynamicAttribute::Components::One,
            DynamicAttribute::DataType::Float});
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    const auto value = Checker(FloatShader("float", "vec4(valueInterpolated, 0.0, 0.0, 0.0)"),
        #ifndef MAGNUM_TARGET_GLES2
        RenderbufferFormat::RGBA8,
        #else
        RenderbufferFormat::RGBA4,
        #endif
        mesh).get<UnsignedByte>(PixelFormat::RGBA, PixelType::UnsignedByte);

    MAGNUM_VERIFY_NO_GL_ERROR();
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_COMPARE(value, 96);
    #else /* RGBA4, so less precision */
    CORRADE_COMPARE_WITH(value, 96, TestSuite::Compare::around(16));
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void MeshGLTest::addVertexBufferDouble() {
    if(!Context::current().isExtensionSupported<Extensions::ARB::vertex_attrib_64bit>())
        CORRADE_SKIP(Extensions::ARB::vertex_attrib_64bit::string() << "is not supported.");

    const Double data[] = { 0.0, -0.7, Math::unpack<Double, UnsignedShort>(45828) };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setBaseVertex(1);

    if(testCaseInstanceId() == 0) {
        setTestCaseDescription("Attribute");
        mesh.addVertexBuffer(buffer, 8, Attribute<0, Double>{});
    } else if(testCaseInstanceId() == 1) {
        setTestCaseDescription("DynamicAttribute");
        mesh.addVertexBuffer(buffer, 8, 8, DynamicAttribute{
            DynamicAttribute::Kind::Long, 0,
            DynamicAttribute::Components::One,
            DynamicAttribute::DataType::Double});
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    const auto value = Checker(DoubleShader("double", "float", "float(value)"),
        RenderbufferFormat::R16, mesh).get<UnsignedShort>(PixelFormat::Red, PixelType::UnsignedShort);

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(value, 45828);
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void MeshGLTest::addVertexBufferVectorNui() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    constexpr Vector3ui data[] = { {}, {37448, 547686, 156}, {27592, 157, 25} };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setBaseVertex(1);

    if(testCaseInstanceId() == 0) {
        setTestCaseDescription("Attribute");
        mesh.addVertexBuffer(buffer, 3*4, Attribute<0, Vector3ui>{});
    } else if(testCaseInstanceId() == 1) {
        setTestCaseDescription("DynamicAttribute");
        mesh.addVertexBuffer(buffer, 3*4, 3*4, DynamicAttribute{
            DynamicAttribute::Kind::Integral, 0,
            DynamicAttribute::Components::Three,
            DynamicAttribute::DataType::UnsignedInt});
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    const auto value = Checker(IntegerShader("uvec3"), RenderbufferFormat::RGBA32UI, mesh)
        .get<Vector4ui>(PixelFormat::RGBAInteger, PixelType::UnsignedInt);

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(value.xyz(), Vector3ui(27592, 157, 25));
}

void MeshGLTest::addVertexBufferVectorNi() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    constexpr Vector2i data[] = { {}, {-37448, 547686}, {27592, -157} };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setBaseVertex(1);

    if(testCaseInstanceId() == 0) {
        setTestCaseDescription("Attribute");
        mesh.addVertexBuffer(buffer, 2*4, Attribute<0, Vector2i>{});
    } else if(testCaseInstanceId() == 1) {
        setTestCaseDescription("DynamicAttribute");
        mesh.addVertexBuffer(buffer, 2*4, 2*4, DynamicAttribute{
            DynamicAttribute::Kind::Integral, 0,
            DynamicAttribute::Components::Two,
            DynamicAttribute::DataType::Int});
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    const auto value = Checker(IntegerShader("ivec2"), RenderbufferFormat::RG32I, mesh).get<Vector2i>(
        #ifndef MAGNUM_TARGET_WEBGL
        PixelFormat::RGInteger
        #else
        PixelFormat::RGBAInteger
        #endif
    , PixelType::Int);

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(value, Vector2i(27592, -157));
}
#endif

void MeshGLTest::addVertexBufferVectorN() {
    const Vector3 data[] = { {}, {0.0f, -0.9f, 1.0f}, 0x60189c_rgbf };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setBaseVertex(1);

    if(testCaseInstanceId() == 0) {
        setTestCaseDescription("Attribute");
        mesh.addVertexBuffer(buffer, 3*4, Attribute<0, Vector3>{});
    } else if(testCaseInstanceId() == 1) {
        setTestCaseDescription("DynamicAttribute");
        mesh.addVertexBuffer(buffer, 3*4, 3*4, DynamicAttribute{
            DynamicAttribute::Kind::Generic, 0,
            DynamicAttribute::Components::Three,
            DynamicAttribute::DataType::Float});
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    const auto value = Checker(FloatShader("vec3", "vec4(valueInterpolated, 0.0)"),
        #ifndef MAGNUM_TARGET_GLES2
        RenderbufferFormat::RGBA8,
        #else
        RenderbufferFormat::RGBA4,
        #endif
        mesh).get<Color4ub>(PixelFormat::RGBA, PixelType::UnsignedByte);

    MAGNUM_VERIFY_NO_GL_ERROR();
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_COMPARE(value.xyz(), 0x60189c_rgb);
    #else /* RGBA4, so less precision */
    CORRADE_COMPARE_WITH(value.xyz(), 0x60189c_rgb, TestSuite::Compare::around(0x101010_rgb));
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void MeshGLTest::addVertexBufferVectorNd() {
    if(!Context::current().isExtensionSupported<Extensions::ARB::vertex_attrib_64bit>())
        CORRADE_SKIP(Extensions::ARB::vertex_attrib_64bit::string() << "is not supported.");

    const Vector4d data[] = {
        {}, {0.0, -0.9, 1.0, 1.25},
        Math::unpack<Vector4d>(Vector4us{315, 65201, 2576, 12})
    };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setBaseVertex(1);

    if(testCaseInstanceId() == 0) {
        setTestCaseDescription("Attribute");
        mesh.addVertexBuffer(buffer, 4*8, Attribute<0, Vector4d>{});
    } else if(testCaseInstanceId() == 1) {
        setTestCaseDescription("DynamicAttribute");
        mesh.addVertexBuffer(buffer, 4*8, 4*8, DynamicAttribute{
            DynamicAttribute::Kind::Long, 0,
            DynamicAttribute::Components::Four,
            DynamicAttribute::DataType::Double});
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    const auto value = Checker(DoubleShader("dvec4", "vec4", "vec4(value)"),
        RenderbufferFormat::RGBA16, mesh).get<Vector4us>(PixelFormat::RGBA, PixelType::UnsignedShort);

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(value, (Vector4us{315, 65201, 2576, 12}));
}
#endif

void MeshGLTest::addVertexBufferMatrixNxN() {
    const Matrix3x3 data[] = {
        {},
        Matrix3x3::fromDiagonal({0.0f, -0.9f, 1.0f}),
        Matrix3x3::fromDiagonal(0x60189c_rgbf)
    };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setBaseVertex(1);

    if(testCaseInstanceId() == 0) {
        setTestCaseDescription("Attribute");
        mesh.addVertexBuffer(buffer, 3*3*4, Attribute<0, Matrix3x3>{});
    } else if(testCaseInstanceId() == 1) {
        setTestCaseDescription("DynamicAttribute");
        mesh.addVertexBuffer(buffer, 3*3*4, 3*3*4, DynamicAttribute{
            DynamicAttribute::Kind::Generic, 0,
            DynamicAttribute::Components::Three, 3,
            DynamicAttribute::DataType::Float});
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    const auto value = Checker(FloatShader("mat3",
        "vec4(valueInterpolated[0][0], valueInterpolated[1][1], valueInterpolated[2][2], 0.0)"),
        #ifndef MAGNUM_TARGET_GLES2
        RenderbufferFormat::RGBA8,
        #else
        RenderbufferFormat::RGBA4,
        #endif
        mesh).get<Color4ub>(PixelFormat::RGBA, PixelType::UnsignedByte);

    MAGNUM_VERIFY_NO_GL_ERROR();
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_COMPARE(value.xyz(), 0x60189c_rgb);
    #else /* RGBA4, so less precision */
    CORRADE_COMPARE_WITH(value.xyz(), 0x60189c_rgb, TestSuite::Compare::around(0x101010_rgb));
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void MeshGLTest::addVertexBufferMatrixNxNd() {
    if(!Context::current().isExtensionSupported<Extensions::ARB::vertex_attrib_64bit>())
        CORRADE_SKIP(Extensions::ARB::vertex_attrib_64bit::string() << "is not supported.");

    const Matrix3x3d data[] = {
        {},
        Matrix3x3d::fromDiagonal({0.0, -0.9, 1.0}),
        Matrix3x3d::fromDiagonal(Math::unpack<Vector3d>(Vector3us{315, 65201, 2576}))
    };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setBaseVertex(1);

    if(testCaseInstanceId() == 0) {
        setTestCaseDescription("Attribute");
        mesh.addVertexBuffer(buffer, 3*3*8, Attribute<0, Matrix3x3d>{});
    } else if(testCaseInstanceId() == 1) {
        setTestCaseDescription("DynamicAttribute");
        mesh.addVertexBuffer(buffer, 3*3*8, 3*3*8, DynamicAttribute{
            DynamicAttribute::Kind::Long, 0,
            DynamicAttribute::Components::Three, 3,
            DynamicAttribute::DataType::Double});
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    const auto value = Checker(DoubleShader("dmat3", "vec4",
        "vec4(value[0][0], value[1][1], value[2][2], 0.0)"),
        RenderbufferFormat::RGBA16, mesh).get<Vector4us>(PixelFormat::RGBA, PixelType::UnsignedShort);

    MAGNUM_VERIFY_NO_GL_ERROR();

    {
        /* Used to be a problem on Intel Windows drivers 23, not a problem on
           26 anymore */
        CORRADE_EXPECT_FAIL_IF(Context::current().detectedDriver() & (Context::DetectedDriver::Amd|Context::DetectedDriver::NVidia), "Somehow only first two values are extracted on AMD and NVidia drivers.");
        CORRADE_COMPARE(value.xyz(), (Vector3us{315, 65201, 2576}));
    }

    /* This is wrong, but check if it's still the right wrong. Fails on AMD
       15.201.1151 but seems to be fixed in 15.300.1025.0 */
    if(Context::current().detectedDriver() & (Context::DetectedDriver::Amd|Context::DetectedDriver::NVidia))
        CORRADE_COMPARE(value.xyz(), (Vector3us{315, 65201, 0}));
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void MeshGLTest::addVertexBufferMatrixMxN() {
    const Matrix3x4 data[] = {
        {},
        Matrix3x4::fromDiagonal({0.0f, -0.9f, 1.0f}),
        Matrix3x4::fromDiagonal(Math::unpack<Vector3>(Color3ub(96, 24, 156)))
    };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setBaseVertex(1);

    if(testCaseInstanceId() == 0) {
        setTestCaseDescription("Attribute");
        mesh.addVertexBuffer(buffer, 3*4*4, Attribute<0, Matrix3x4>{});
    } else if(testCaseInstanceId() == 1) {
        setTestCaseDescription("DynamicAttribute");
        mesh.addVertexBuffer(buffer, 3*4*4, 3*4*4, DynamicAttribute{
            DynamicAttribute::Kind::Generic, 0,
            DynamicAttribute::Components::Four, 3,
            DynamicAttribute::DataType::Float});
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    const auto value = Checker(FloatShader("mat3x4",
        "vec4(valueInterpolated[0][0], valueInterpolated[1][1], valueInterpolated[2][2], 0.0)"),
        RenderbufferFormat::RGBA8, mesh).get<Color4ub>(PixelFormat::RGBA, PixelType::UnsignedByte);

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(value.xyz(), Color3ub(96, 24, 156));
}
#endif

#ifndef MAGNUM_TARGET_GLES
void MeshGLTest::addVertexBufferMatrixMxNd() {
    if(!Context::current().isExtensionSupported<Extensions::ARB::vertex_attrib_64bit>())
        CORRADE_SKIP(Extensions::ARB::vertex_attrib_64bit::string() << "is not supported.");

    const Matrix3x4d data[] = {
        {},
        Matrix3x4d::fromDiagonal({0.0, -0.9, 1.0}),
        Matrix3x4d::fromDiagonal(Math::unpack<Vector3d>(Vector3us{315, 65201, 2576}))
    };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setBaseVertex(1);

    if(testCaseInstanceId() == 0) {
        setTestCaseDescription("Attribute");
        mesh.addVertexBuffer(buffer, 3*4*8, Attribute<0, Matrix3x4d>{});
    } else if(testCaseInstanceId() == 1) {
        setTestCaseDescription("DynamicAttribute");
        mesh.addVertexBuffer(buffer, 3*4*8, 3*4*8, DynamicAttribute{
            DynamicAttribute::Kind::Long, 0,
            DynamicAttribute::Components::Four, 3,
            DynamicAttribute::DataType::Double});
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    const auto value = Checker(DoubleShader("dmat3x4", "vec4",
        "vec4(value[0][0], value[1][1], value[2][2], 0.0)"),
        RenderbufferFormat::RGBA16, mesh).get<Vector4us>(PixelFormat::RGBA, PixelType::UnsignedShort);

    MAGNUM_VERIFY_NO_GL_ERROR();

    {
        /* Used to be a problem on Intel Windows drivers 23, not a problem on
           26 anymore */
        CORRADE_EXPECT_FAIL_IF(Context::current().detectedDriver() & (Context::DetectedDriver::Amd|Context::DetectedDriver::NVidia), "Somehow only first two values are extracted on AMD and NVidia drivers.");
        CORRADE_COMPARE(value.xyz(), (Vector3us{315, 65201, 2576}));
    }

    /* This is wrong, but check if it's still the right wrong. Fails on AMD
       15.201.1151 but seems to be fixed in 15.300.1025.0 */
    if(Context::current().detectedDriver() & (Context::DetectedDriver::Amd|Context::DetectedDriver::NVidia))
        CORRADE_COMPARE(value.xyz(), (Vector3us{315, 65201, 0}));
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void MeshGLTest::addVertexBufferUnsignedIntWithUnsignedShort() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    constexpr UnsignedShort data[] = { 0, 49563, 2128, 3821, 16583 };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setBaseVertex(1);

    if(testCaseInstanceId() == 0) {
        setTestCaseDescription("Attribute");
        mesh.addVertexBuffer(buffer, 2, 2, Attribute<0, UnsignedInt>{Attribute<0, UnsignedInt>::DataType::UnsignedShort});
    } else if(testCaseInstanceId() == 1) {
        setTestCaseDescription("DynamicAttribute");
        mesh.addVertexBuffer(buffer, 4, 4, DynamicAttribute{
            DynamicAttribute::Kind::Integral, 0,
            DynamicAttribute::Components::One,
            DynamicAttribute::DataType::UnsignedShort});
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    const UnsignedShort value = Checker(IntegerShader("uint"), RenderbufferFormat::R16UI, mesh)
        #ifndef MAGNUM_TARGET_WEBGL
        .get<UnsignedShort>(PixelFormat::RedInteger, PixelType::UnsignedShort)
        #else
        .get<UnsignedInt>(PixelFormat::RGBAInteger, PixelType::UnsignedInt)
        #endif
    ;

    #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL)
    CORRADE_EXPECT_FAIL_IF(Context::current().detectedDriver() & Context::DetectedDriver::SwiftShader,
        "SwiftShader doesn't like integer buffers with anything else than (Unsigned)Int");
    #endif

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(value, 16583);
}

void MeshGLTest::addVertexBufferUnsignedIntWithShort() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    #ifdef MAGNUM_TARGET_WEBGL
    CORRADE_SKIP("WebGL doesn't allow supplying signed data to an unsigned attribute.");
    #endif

    /* Signed DataType is now deprecated for unsigned attributes, so build the
       rest only on non-WebGL or on deprecated WebGL builds */
    #if !defined(MAGNUM_TARGET_WEBGL) || defined(MAGNUM_BUILD_DEPRECATED)
    constexpr Short data[] = { 0, 24563, 2128, 3821, 16583 };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setBaseVertex(1);

    if(testCaseInstanceId() == 0) {
        setTestCaseDescription("Attribute");
        #ifdef MAGNUM_TARGET_WEBGL
        CORRADE_IGNORE_DEPRECATED_PUSH
        #endif
        mesh.addVertexBuffer(buffer, 2, 2, Attribute<0, UnsignedInt>{Attribute<0, UnsignedInt>::DataType::Short});
        #ifdef MAGNUM_TARGET_WEBGL
        CORRADE_IGNORE_DEPRECATED_POP
        #endif
    } else if(testCaseInstanceId() == 1) {
        setTestCaseDescription("DynamicAttribute");
        mesh.addVertexBuffer(buffer, 4, 4, DynamicAttribute{
            DynamicAttribute::Kind::Integral, 0,
            DynamicAttribute::Components::One,
            DynamicAttribute::DataType::Short});
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    const auto value = Checker(IntegerShader("uint"), RenderbufferFormat::R16UI, mesh)
        .get<UnsignedShort>(PixelFormat::RedInteger, PixelType::UnsignedShort);

    #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL)
    CORRADE_EXPECT_FAIL_IF(Context::current().detectedDriver() & Context::DetectedDriver::SwiftShader,
        "SwiftShader doesn't like integer buffers with anything else than (Unsigned)Int");
    #endif

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(value, 16583);
    #endif
}

void MeshGLTest::addVertexBufferIntWithUnsignedShort() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    #ifdef MAGNUM_TARGET_WEBGL
    CORRADE_SKIP("WebGL doesn't allow supplying unsigned data to a signed attribute.");
    #endif

    /* Unsigned DataType is now deprecated for signed attributes, so build the
       rest only on non-WebGL or on deprecated WebGL builds */
    #if !defined(MAGNUM_TARGET_WEBGL) || defined(MAGNUM_BUILD_DEPRECATED)
    constexpr UnsignedShort data[] = { 0, 49563, 2128, 3821, 16583 };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setBaseVertex(1);

    if(testCaseInstanceId() == 0) {
        setTestCaseDescription("Attribute");
        #ifdef MAGNUM_TARGET_WEBGL
        CORRADE_IGNORE_DEPRECATED_PUSH
        #endif
        mesh.addVertexBuffer(buffer, 2, 2, Attribute<0, Int>{Attribute<0, Int>::DataType::UnsignedShort});
        #ifdef MAGNUM_TARGET_WEBGL
        CORRADE_IGNORE_DEPRECATED_POP
        #endif
    } else if(testCaseInstanceId() == 1) {
        setTestCaseDescription("DynamicAttribute");
        mesh.addVertexBuffer(buffer, 4, 4, DynamicAttribute{
            DynamicAttribute::Kind::Integral, 0,
            DynamicAttribute::Components::One,
            DynamicAttribute::DataType::UnsignedShort});
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    const auto value = Checker(IntegerShader("int"), RenderbufferFormat::R16I, mesh)
        .get<Short>(PixelFormat::RedInteger, PixelType::Short);

    #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL)
    CORRADE_EXPECT_FAIL_IF(Context::current().detectedDriver() & Context::DetectedDriver::SwiftShader,
        "SwiftShader doesn't like integer buffers with anything else than (Unsigned)Int");
    #endif

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(value, 16583);
    #endif
}

void MeshGLTest::addVertexBufferIntWithShort() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    constexpr Short data[] = { 0, 24563, 2128, 3821, -16583 };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setBaseVertex(1);

    if(testCaseInstanceId() == 0) {
        setTestCaseDescription("Attribute");
        mesh.addVertexBuffer(buffer, 2, 2, Attribute<0, Int>{Attribute<0, Int>::DataType::Short});
    } else if(testCaseInstanceId() == 1) {
        setTestCaseDescription("DynamicAttribute");
        mesh.addVertexBuffer(buffer, 4, 4, DynamicAttribute{
            DynamicAttribute::Kind::Integral, 0,
            DynamicAttribute::Components::One,
            DynamicAttribute::DataType::Short});
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    const Short value = Checker(IntegerShader("int"), RenderbufferFormat::R16I, mesh)
        #ifndef MAGNUM_TARGET_WEBGL
        .get<Short>(PixelFormat::RedInteger, PixelType::Short)
        #else
        .get<Int>(PixelFormat::RGBAInteger, PixelType::Int)
        #endif
    ;

    #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL)
    CORRADE_EXPECT_FAIL_IF(Context::current().detectedDriver() & Context::DetectedDriver::SwiftShader,
        "SwiftShader doesn't like integer buffers with anything else than (Unsigned)Int");
    #endif

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(value, -16583);
}
#endif

#if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
void MeshGLTest::addVertexBufferFloatWithHalf() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::half_float_vertex>())
        CORRADE_SKIP(Extensions::ARB::half_float_vertex::string() << "is not supported.");
    #elif defined(MAGNUM_TARGET_GLES2)
    if(!Context::current().isExtensionSupported<Extensions::OES::vertex_half_float>())
        CORRADE_SKIP(Extensions::OES::vertex_half_float::string() << "is not supported.");
    #endif

    const Half data[] = { 0.0_h, -0.7_h, Half(Math::unpack<Float, UnsignedByte>(186)) };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setBaseVertex(1);

    if(testCaseInstanceId() == 0) {
        setTestCaseDescription("Attribute");
        mesh.addVertexBuffer(buffer, 2, Attribute<0, Float>{Attribute<0, Float>::DataType::Half});
    } else if(testCaseInstanceId() == 1) {
        setTestCaseDescription("DynamicAttribute");
        mesh.addVertexBuffer(buffer, 2, 2, DynamicAttribute{
            DynamicAttribute::Kind::Generic, 0,
            DynamicAttribute::Components::One,
            DynamicAttribute::DataType::Half});
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    const auto value = Checker(FloatShader("float", "vec4(valueInterpolated, 0.0, 0.0, 0.0)"),
        RenderbufferFormat::RGBA8, mesh).get<UnsignedByte>(PixelFormat::RGBA, PixelType::UnsignedByte);

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(value, 186);
}
#endif

#ifndef MAGNUM_TARGET_GLES
void MeshGLTest::addVertexBufferFloatWithDouble() {
    const Double data[] = { 0.0, -0.7, Math::unpack<Double, UnsignedByte>(186) };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setBaseVertex(1)
        .addVertexBuffer(buffer, 8, Attribute<0, Float>(Attribute<0, Float>::DataType::Double));

    if(testCaseInstanceId() == 0) {
        setTestCaseDescription("Attribute");
        mesh.addVertexBuffer(buffer, 8, Attribute<0, Float>{Attribute<0, Float>::DataType::Double});
    } else if(testCaseInstanceId() == 1) {
        setTestCaseDescription("DynamicAttribute");
        mesh.addVertexBuffer(buffer, 8, 8, DynamicAttribute{
            DynamicAttribute::Kind::Generic, 0,
            DynamicAttribute::Components::One,
            DynamicAttribute::DataType::Double});
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    const auto value = Checker(FloatShader("float", "vec4(valueInterpolated, 0.0, 0.0, 0.0)"),
        RenderbufferFormat::RGBA8, mesh).get<UnsignedByte>(PixelFormat::RGBA, PixelType::UnsignedShort);

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(value, 186);
}

void MeshGLTest::addVertexBufferVector3WithUnsignedInt10f11f11fRev() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::vertex_type_10f_11f_11f_rev>())
        CORRADE_SKIP(Extensions::ARB::vertex_type_10f_11f_11f_rev::string() << "is not supported.");
    #endif

    Buffer buffer;
    buffer.setData({nullptr, 12}, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setBaseVertex(1);

    if(testCaseInstanceId() == 0) {
        setTestCaseDescription("Attribute");
        mesh.addVertexBuffer(buffer, 4, Attribute<0, Vector3>{Attribute<0, Vector3>::DataType::UnsignedInt10f11f11fRev});
    } else if(testCaseInstanceId() == 1) {
        setTestCaseDescription("DynamicAttribute");
        mesh.addVertexBuffer(buffer, 4, 4, DynamicAttribute{
            DynamicAttribute::Kind::Generic, 0,
            DynamicAttribute::Components::Three,
            DynamicAttribute::DataType::UnsignedInt10f11f11fRev});
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();
    /* Won't test the actual values */
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void MeshGLTest::addVertexBufferVector4WithUnsignedInt2101010Rev() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::vertex_type_2_10_10_10_rev>())
        CORRADE_SKIP(Extensions::ARB::vertex_type_2_10_10_10_rev::string() << "is not supported.");
    #endif

    Buffer buffer;
    buffer.setData({nullptr, 12}, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setBaseVertex(1);

    if(testCaseInstanceId() == 0) {
        setTestCaseDescription("Attribute");
        mesh.addVertexBuffer(buffer, 4, Attribute<0, Vector4>{Attribute<0, Vector4>::DataType::UnsignedInt2101010Rev});
    } else if(testCaseInstanceId() == 1) {
        setTestCaseDescription("DynamicAttribute");
        mesh.addVertexBuffer(buffer, 4, 4, DynamicAttribute{
            DynamicAttribute::Kind::Generic, 0,
            DynamicAttribute::Components::Four,
            DynamicAttribute::DataType::UnsignedInt2101010Rev});
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();
    /* Won't test the actual values */
}

void MeshGLTest::addVertexBufferVector4WithInt2101010Rev() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::vertex_type_2_10_10_10_rev>())
        CORRADE_SKIP(Extensions::ARB::vertex_type_2_10_10_10_rev::string() << "is not supported.");
    #endif

    Buffer buffer;
    buffer.setData({nullptr, 12}, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setBaseVertex(1);

    if(testCaseInstanceId() == 0) {
        setTestCaseDescription("Attribute");
        mesh.addVertexBuffer(buffer, 4, Attribute<0, Vector4>{Attribute<0, Vector4>::DataType::Int2101010Rev});
    } else if(testCaseInstanceId() == 1) {
        setTestCaseDescription("DynamicAttribute");
        mesh.addVertexBuffer(buffer, 4, 4, DynamicAttribute{
            DynamicAttribute::Kind::Generic, 0,
            DynamicAttribute::Components::Four,
            DynamicAttribute::DataType::Int2101010Rev});
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();
    /* Won't test the actual values */
}
#endif

void MeshGLTest::addVertexBufferLessVectorComponents() {
    const Vector3 data[] = { {}, {0.0f, -0.9f, 1.0f}, 0x60189c_rgbf };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setBaseVertex(1);

    if(testCaseInstanceId() == 0) {
        setTestCaseDescription("Attribute");
        mesh.addVertexBuffer(buffer, 3*4, Attribute<0, Vector4>{Attribute<0, Vector4>::Components::Three});
    } else if(testCaseInstanceId() == 1) {
        setTestCaseDescription("DynamicAttribute");
        mesh.addVertexBuffer(buffer, 3*4, 3*4, DynamicAttribute{
            DynamicAttribute::Kind::Generic, 0,
            DynamicAttribute::Components::Three,
            DynamicAttribute::DataType::Float});
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    const auto value = Checker(FloatShader("vec4", "valueInterpolated"),
        #ifndef MAGNUM_TARGET_GLES2
        RenderbufferFormat::RGBA8,
        #else
        RenderbufferFormat::RGBA4,
        #endif
        mesh).get<Color4ub>(PixelFormat::RGBA, PixelType::UnsignedByte);

    MAGNUM_VERIFY_NO_GL_ERROR();
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_COMPARE(value, 0x60189cff_rgba);
    #else /* RGBA4, so less precision */
    CORRADE_COMPARE_WITH(value.xyz(), 0x60189cff_rgba, TestSuite::Compare::around(0x10101000_rgba));
    #endif
}

void MeshGLTest::addVertexBufferNormalized() {
    constexpr Color4ub data[] = { {}, {0, 128, 64}, 0x209ce4_rgb };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setBaseVertex(1);

    if(testCaseInstanceId() == 0) {
        setTestCaseDescription("Attribute");
        typedef Attribute<0, Vector3> Attribute;
        mesh.addVertexBuffer(buffer, 4, Attribute(Attribute::DataType::UnsignedByte, Attribute::DataOption::Normalized), 1);
    } else if(testCaseInstanceId() == 1) {
        setTestCaseDescription("DynamicAttribute");
        mesh.addVertexBuffer(buffer, 4, 4, DynamicAttribute{
            DynamicAttribute::Kind::GenericNormalized, 0,
            DynamicAttribute::Components::Three,
            DynamicAttribute::DataType::UnsignedByte});
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    const auto value = Checker(FloatShader("vec3", "vec4(valueInterpolated, 0.0)"),
        #ifndef MAGNUM_TARGET_GLES2
        RenderbufferFormat::RGBA8,
        #else
        RenderbufferFormat::RGBA4,
        #endif
        mesh).get<Color4ub>(PixelFormat::RGBA, PixelType::UnsignedByte);

    MAGNUM_VERIFY_NO_GL_ERROR();
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_COMPARE(value.xyz(), 0x209ce4_rgb);
    #else /* RGBA4, so less precision */
    CORRADE_COMPARE_WITH(value.xyz(), 0x209ce4_rgb, TestSuite::Compare::around(0x101010_rgb));
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void MeshGLTest::addVertexBufferBGRA() {
    if(!Context::current().isExtensionSupported<Extensions::ARB::vertex_array_bgra>())
        CORRADE_SKIP(Extensions::ARB::vertex_array_bgra::string() << "is not supported.");

    constexpr Color4ub data[] = { {}, {0, 128, 64, 161}, {96, 24, 156, 225} };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setBaseVertex(1);

    if(testCaseInstanceId() == 0) {
        setTestCaseDescription("Attribute");
        typedef Attribute<0, Vector4> Attribute;
        mesh.addVertexBuffer(buffer, 4, Attribute(Attribute::Components::BGRA, Attribute::DataType::UnsignedByte, Attribute::DataOption::Normalized));
    } else if(testCaseInstanceId() == 1) {
        setTestCaseDescription("DynamicAttribute");
        mesh.addVertexBuffer(buffer, 4, 4, DynamicAttribute{
            DynamicAttribute::Kind::GenericNormalized, 0,
            DynamicAttribute::Components::BGRA,
            DynamicAttribute::DataType::UnsignedByte});
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE();

    MAGNUM_VERIFY_NO_GL_ERROR();

    const auto value = Checker(FloatShader("vec4", "valueInterpolated"),
        RenderbufferFormat::RGBA8, mesh).get<Color4ub>(PixelFormat::RGBA, PixelType::UnsignedByte);

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(value, Color4ub(156, 24, 96, 225));
}
#endif

struct MultipleShader: AbstractShaderProgram {
    typedef Attribute<0, Vector3> Position;
    typedef Attribute<1, Vector3> Normal;
    typedef Attribute<2, Vector2> TextureCoordinates;

    explicit MultipleShader();
};

MultipleShader::MultipleShader() {
    #ifndef MAGNUM_TARGET_GLES
    Shader vert(
        #ifndef CORRADE_TARGET_APPLE
        Version::GL210
        #else
        Version::GL310
        #endif
        , Shader::Type::Vertex);
    Shader frag(
        #ifndef CORRADE_TARGET_APPLE
        Version::GL210
        #else
        Version::GL310
        #endif
        , Shader::Type::Fragment);
    #else
    Shader vert(Version::GLES200, Shader::Type::Vertex);
    Shader frag(Version::GLES200, Shader::Type::Fragment);
    #endif

    vert.addSource(
        "#if !defined(GL_ES) && __VERSION__ == 120\n"
        "#define mediump\n"
        "#endif\n"
        "#if defined(GL_ES) || __VERSION__ == 120\n"
        "#define in attribute\n"
        "#define out varying\n"
        "#endif\n"
        "in mediump vec4 position;\n"
        "in mediump vec3 normal;\n"
        "in mediump vec2 textureCoordinates;\n"
        "out mediump vec4 valueInterpolated;\n"
        "void main() {\n"
        "    valueInterpolated = position + vec4(normal, 0.0) + vec4(textureCoordinates, 0.0, 0.0);\n"
        "    gl_PointSize = 1.0;\n"
        "    gl_Position = vec4(0.0, 0.0, 0.0, 1.0);\n"
        "}\n");
    frag.addSource(
        "#if !defined(GL_ES) && __VERSION__ == 120\n"
        "#define mediump\n"
        "#endif\n"
        "#if defined(GL_ES) || __VERSION__ == 120\n"
        "#define in varying\n"
        "#define result gl_FragColor\n"
        "#endif\n"
        "in mediump vec4 valueInterpolated;\n"
        "#if !defined(GL_ES) && __VERSION__ >= 130\n"
        "out mediump vec4 result;\n"
        "#endif\n"
        "void main() { result = valueInterpolated; }\n");

    CORRADE_INTERNAL_ASSERT_OUTPUT(vert.compile() && frag.compile());

    attachShaders({vert, frag});

    bindAttributeLocation(Position::Location, "position");
    bindAttributeLocation(Normal::Location, "normal");
    bindAttributeLocation(TextureCoordinates::Location, "textureCoordinates");

    CORRADE_INTERNAL_ASSERT_OUTPUT(link());
}

void MeshGLTest::addVertexBufferMultiple() {
    const Float data[] = {
        0.0f, /* Offset */

        /* First attribute */
        0.3f, 0.1f, 0.5f,
            0.4f, 0.0f, -0.9f,
                1.0f, -0.5f,

        /* Second attribute */
        Math::unpack<Float, UnsignedByte>(64),
            Math::unpack<Float, UnsignedByte>(17),
                Math::unpack<Float, UnsignedByte>(56),
        Math::unpack<Float, UnsignedByte>(15),
            Math::unpack<Float, UnsignedByte>(164),
                Math::unpack<Float, UnsignedByte>(17),
        Math::unpack<Float, UnsignedByte>(97),
            Math::unpack<Float, UnsignedByte>(28)
    };

    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setBaseVertex(1)
        .addVertexBuffer(buffer, 1*4, MultipleShader::Position(),
        MultipleShader::Normal(), MultipleShader::TextureCoordinates());

    MAGNUM_VERIFY_NO_GL_ERROR();

    const auto value = Checker(MultipleShader(),
        #ifndef MAGNUM_TARGET_GLES2
        RenderbufferFormat::RGBA8,
        #else
        RenderbufferFormat::RGBA4,
        #endif
        mesh).get<Color4ub>(PixelFormat::RGBA, PixelType::UnsignedByte);

    MAGNUM_VERIFY_NO_GL_ERROR();
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_COMPARE(value, Color4ub(64 + 15 + 97, 17 + 164 + 28, 56 + 17, 255));
    #else /* RGBA4, so less precision */
    CORRADE_COMPARE_WITH(value,
        (Color4ub{64 + 15 + 97, 17 + 164 + 28, 56 + 17, 255}),
        TestSuite::Compare::around(0x10101000_rgba));
    #endif
}

void MeshGLTest::addVertexBufferMultipleGaps() {
     const Float data[] = {
        0.0f, 0.0f, 0.0f, 0.0f, /* Offset */

        /* First attribute */
        0.3f, 0.1f, 0.5f, 0.0f,
            0.4f, 0.0f, -0.9f, 0.0f,
                1.0f, -0.5f, 0.0f, 0.0f,

        /* Second attribute */
        Math::unpack<Float, UnsignedByte>(64),
            Math::unpack<Float, UnsignedByte>(17),
                Math::unpack<Float, UnsignedByte>(56), 0.0f,
        Math::unpack<Float, UnsignedByte>(15),
            Math::unpack<Float, UnsignedByte>(164),
                Math::unpack<Float, UnsignedByte>(17), 0.0f,
        Math::unpack<Float, UnsignedByte>(97),
            Math::unpack<Float, UnsignedByte>(28), 0.0f, 0.0f
    };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setBaseVertex(1)
        .addVertexBuffer(buffer, 4*4,
        MultipleShader::Position(), 1*4,
        MultipleShader::Normal(), 1*4,
        MultipleShader::TextureCoordinates(), 2*4);

    MAGNUM_VERIFY_NO_GL_ERROR();

    const auto value = Checker(MultipleShader(),
        #ifndef MAGNUM_TARGET_GLES2
        RenderbufferFormat::RGBA8,
        #else
        RenderbufferFormat::RGBA4,
        #endif
        mesh).get<Color4ub>(PixelFormat::RGBA, PixelType::UnsignedByte);

    MAGNUM_VERIFY_NO_GL_ERROR();
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_COMPARE(value, Color4ub(64 + 15 + 97, 17 + 164 + 28, 56 + 17, 255));
    #else /* RGBA4, so less precision */
    CORRADE_COMPARE_WITH(value,
        (Color4ub{64 + 15 + 97, 17 + 164 + 28, 56 + 17, 255}),
        TestSuite::Compare::around(0x10101000_rgba));
    #endif
}

void MeshGLTest::addVertexBufferMovedOutInstance() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Buffer buffer{NoCreate};
    Mesh mesh;

    Containers::String out;
    Error redirectError{&out};

    mesh.addVertexBuffer(buffer, 0, Attribute<0, Float>{});

    CORRADE_COMPARE(out, "GL::Mesh::addVertexBuffer(): empty or moved-out Buffer instance was passed\n");
}

void MeshGLTest::addVertexBufferTransferOwnwership() {
    const Float data = 1.0f;
    Buffer buffer;
    buffer.setData({&data, 1}, BufferUsage::StaticDraw);

    const GLuint id = buffer.id();
    CORRADE_VERIFY(glIsBuffer(id));

    {
        Mesh mesh;
        mesh.addVertexBuffer(buffer, 0, Attribute<0, Float>{});
        CORRADE_VERIFY(buffer.id());
        CORRADE_VERIFY(glIsBuffer(id));
    }

    CORRADE_VERIFY(glIsBuffer(id));

    {
        Mesh mesh;
        mesh.addVertexBuffer(Utility::move(buffer), 0, Attribute<0, Float>{});
        CORRADE_VERIFY(!buffer.id());
        CORRADE_VERIFY(glIsBuffer(id));
    }

    CORRADE_VERIFY(!glIsBuffer(id));
}

void MeshGLTest::addVertexBufferInstancedTransferOwnwership() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::draw_instanced>())
        CORRADE_SKIP(Extensions::ARB::draw_instanced::string() << "is not supported.");
    #elif defined(MAGNUM_TARGET_GLES2)
    #ifndef MAGNUM_TARGET_WEBGL
    if(!Context::current().isExtensionSupported<Extensions::ANGLE::instanced_arrays>() &&
       !Context::current().isExtensionSupported<Extensions::EXT::instanced_arrays>() &&
       !Context::current().isExtensionSupported<Extensions::NV::instanced_arrays>())
        CORRADE_SKIP("Required extension is not supported.");
    #else
    if(!Context::current().isExtensionSupported<Extensions::ANGLE::instanced_arrays>())
        CORRADE_SKIP(Extensions::ANGLE::instanced_arrays::string() << "is not supported.");
    #endif
    #endif

    const Float data = 1.0f;
    Buffer buffer;
    buffer.setData({&data, 1}, BufferUsage::StaticDraw);

    const GLuint id = buffer.id();
    CORRADE_VERIFY(glIsBuffer(id));

    {
        Mesh mesh;
        mesh.addVertexBufferInstanced(buffer, 1, 0, Attribute<0, Float>{});
        CORRADE_VERIFY(buffer.id());
        CORRADE_VERIFY(glIsBuffer(id));
    }

    CORRADE_VERIFY(glIsBuffer(id));

    {
        Mesh mesh;
        mesh.addVertexBufferInstanced(Utility::move(buffer), 1, 0, Attribute<0, Float>{});
        CORRADE_VERIFY(!buffer.id());
        CORRADE_VERIFY(glIsBuffer(id));
    }

    CORRADE_VERIFY(!glIsBuffer(id));
}

void MeshGLTest::addVertexBufferDynamicTransferOwnwership() {
    const Float data = 1.0f;
    Buffer buffer;
    buffer.setData({&data, 1}, BufferUsage::StaticDraw);

    const GLuint id = buffer.id();
    CORRADE_VERIFY(glIsBuffer(id));

    {
        Mesh mesh;
        mesh.addVertexBuffer(buffer, 0, 4, DynamicAttribute{
            DynamicAttribute::Kind::GenericNormalized, 0,
            DynamicAttribute::Components::One,
            DynamicAttribute::DataType::Float});
        CORRADE_VERIFY(buffer.id());
        CORRADE_VERIFY(glIsBuffer(id));
    }

    CORRADE_VERIFY(glIsBuffer(id));

    {
        Mesh mesh;
        mesh.addVertexBuffer(Utility::move(buffer), 0, 4, DynamicAttribute{
            DynamicAttribute::Kind::GenericNormalized, 0,
            DynamicAttribute::Components::One,
            DynamicAttribute::DataType::Float});
        CORRADE_VERIFY(!buffer.id());
        CORRADE_VERIFY(glIsBuffer(id));
    }

    CORRADE_VERIFY(!glIsBuffer(id));
}

void MeshGLTest::addVertexBufferInstancedDynamicTransferOwnwership() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::draw_instanced>())
        CORRADE_SKIP(Extensions::ARB::draw_instanced::string() << "is not supported.");
    #elif defined(MAGNUM_TARGET_GLES2)
    #ifndef MAGNUM_TARGET_WEBGL
    if(!Context::current().isExtensionSupported<Extensions::ANGLE::instanced_arrays>() &&
       !Context::current().isExtensionSupported<Extensions::EXT::instanced_arrays>() &&
       !Context::current().isExtensionSupported<Extensions::NV::instanced_arrays>())
        CORRADE_SKIP("Required extension is not supported.");
    #else
    if(!Context::current().isExtensionSupported<Extensions::ANGLE::instanced_arrays>())
        CORRADE_SKIP(Extensions::ANGLE::instanced_arrays::string() << "is not supported.");
    #endif
    #endif

    const Float data = 1.0f;
    Buffer buffer;
    buffer.setData({&data, 1}, BufferUsage::StaticDraw);

    const GLuint id = buffer.id();
    CORRADE_VERIFY(glIsBuffer(id));

    {
        Mesh mesh;
        mesh.addVertexBufferInstanced(buffer, 1, 0, 4, DynamicAttribute{
            DynamicAttribute::Kind::GenericNormalized, 0,
            DynamicAttribute::Components::One,
            DynamicAttribute::DataType::Float});
        CORRADE_VERIFY(buffer.id());
        CORRADE_VERIFY(glIsBuffer(id));
    }

    CORRADE_VERIFY(glIsBuffer(id));

    {
        Mesh mesh;
        mesh.addVertexBufferInstanced(Utility::move(buffer), 1, 0, 4, DynamicAttribute{
            DynamicAttribute::Kind::GenericNormalized, 0,
            DynamicAttribute::Components::One,
            DynamicAttribute::DataType::Float});
        CORRADE_VERIFY(!buffer.id());
        CORRADE_VERIFY(glIsBuffer(id));
    }

    CORRADE_VERIFY(!glIsBuffer(id));
}

const Float indexedVertexData[] = {
    0.0f, /* Offset */

    /* First vertex */
    Math::unpack<Float, UnsignedByte>(64),
        Math::unpack<Float, UnsignedByte>(17),
            Math::unpack<Float, UnsignedByte>(56),
    Math::unpack<Float, UnsignedByte>(15),
        Math::unpack<Float, UnsignedByte>(164),
            Math::unpack<Float, UnsignedByte>(17),
    Math::unpack<Float, UnsignedByte>(97),
        Math::unpack<Float, UnsignedByte>(28),

    /* Second vertex */
    0.3f, 0.1f, 0.5f,
        0.4f, 0.0f, -0.9f,
            1.0f, -0.5f
};

const Float indexedVertexDataBaseVertex[] = {
    0.0f, 0.0f, /* Offset */

    /* First vertex */
    0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
            0.0f, 0.0f,

    /* Second vertex */
    0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
            0.0f, 0.0f,

    /* Third vertex */
    Math::unpack<Float, UnsignedByte>(64),
        Math::unpack<Float, UnsignedByte>(17),
            Math::unpack<Float, UnsignedByte>(56),
    Math::unpack<Float, UnsignedByte>(15),
        Math::unpack<Float, UnsignedByte>(164),
            Math::unpack<Float, UnsignedByte>(17),
    Math::unpack<Float, UnsignedByte>(97),
        Math::unpack<Float, UnsignedByte>(28),

    /* Fourth vertex */
    0.3f, 0.1f, 0.5f,
        0.4f, 0.0f, -0.9f,
            1.0f, -0.5f
};

constexpr Color4ub indexedResult{64 + 15 + 97, 17 + 164 + 28, 56 + 17, 255};

template<class T> void MeshGLTest::setIndexBuffer() {
    setTestCaseTemplateName(std::is_same<T, MeshIndexType>::value ?
        "GL::MeshIndexType" : "Magnum::MeshIndexType");

    Buffer vertices;
    vertices.setData(indexedVertexData, BufferUsage::StaticDraw);

    constexpr UnsignedByte indexData[] = { 2, 1, 0 };
    Buffer indices{Buffer::TargetHint::ElementArray};
    indices.setData(indexData, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.addVertexBuffer(vertices, 1*4,  MultipleShader::Position(),
                         MultipleShader::Normal(), MultipleShader::TextureCoordinates())
        .setIndexBuffer(indices, 1, T::UnsignedByte);

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(mesh.indexType(), MeshIndexType::UnsignedByte);

    const auto value = Checker(MultipleShader{},
        #ifndef MAGNUM_TARGET_GLES2
        RenderbufferFormat::RGBA8,
        #else
        RenderbufferFormat::RGBA4,
        #endif
        mesh).get<Color4ub>(PixelFormat::RGBA, PixelType::UnsignedByte);

    MAGNUM_VERIFY_NO_GL_ERROR();
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_COMPARE(value, indexedResult);
    #else /* RGBA4, so less precision */
    CORRADE_COMPARE_WITH(value, indexedResult, TestSuite::Compare::around(0x10101000_rgba));
    #endif
}

template<class T> void MeshGLTest::setIndexBufferRange() {
    setTestCaseTemplateName(std::is_same<T, MeshIndexType>::value ?
        "GL::MeshIndexType" : "Magnum::MeshIndexType");

    Buffer vertices;
    vertices.setData(indexedVertexData, BufferUsage::StaticDraw);

    constexpr UnsignedShort indexData[] = { 2, 1, 0 };
    Buffer indices{Buffer::TargetHint::ElementArray};
    indices.setData(indexData, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.addVertexBuffer(vertices, 1*4,  MultipleShader::Position(),
                         MultipleShader::Normal(), MultipleShader::TextureCoordinates())
        .setIndexBuffer(indices, 2, T::UnsignedShort, 0, 1);

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(mesh.indexType(), GL::MeshIndexType::UnsignedShort);

    const auto value = Checker(MultipleShader{},
        #ifndef MAGNUM_TARGET_GLES2
        RenderbufferFormat::RGBA8,
        #else
        RenderbufferFormat::RGBA4,
        #endif
        mesh).get<Color4ub>(PixelFormat::RGBA, PixelType::UnsignedByte);

    MAGNUM_VERIFY_NO_GL_ERROR();
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_COMPARE(value, indexedResult);
    #else /* RGBA4, so less precision */
    CORRADE_COMPARE_WITH(value, indexedResult, TestSuite::Compare::around(0x10101000_rgba));
    #endif
}

void MeshGLTest::setIndexBufferUnsignedInt() {
    #ifdef MAGNUM_TARGET_GLES2
    if(!Context::current().isExtensionSupported<Extensions::OES::element_index_uint>())
        CORRADE_SKIP(Extensions::OES::element_index_uint::string() << "is not supported.");
    #endif

    Buffer vertices;
    vertices.setData(indexedVertexData, BufferUsage::StaticDraw);

    constexpr UnsignedInt indexData[] = { 2, 1, 0 };
    Buffer indices{Buffer::TargetHint::ElementArray};
    indices.setData(indexData, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.addVertexBuffer(vertices, 1*4,  MultipleShader::Position(),
                         MultipleShader::Normal(), MultipleShader::TextureCoordinates())
        .setIndexBuffer(indices, 4, MeshIndexType::UnsignedInt);

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(mesh.indexType(), GL::MeshIndexType::UnsignedInt);

    const auto value = Checker(MultipleShader{},
        #ifndef MAGNUM_TARGET_GLES2
        RenderbufferFormat::RGBA8,
        #else
        RenderbufferFormat::RGBA4,
        #endif
        mesh).get<Color4ub>(PixelFormat::RGBA, PixelType::UnsignedByte);

    MAGNUM_VERIFY_NO_GL_ERROR();
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_COMPARE(value, indexedResult);
    #else /* RGBA4, so less precision */
    CORRADE_COMPARE_WITH(value, indexedResult, TestSuite::Compare::around(0x10101000_rgba));
    #endif
}

void MeshGLTest::setIndexBufferMovedOutInstance() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Buffer buffer{NoCreate};
    Mesh mesh;

    Containers::String out;
    Error redirectError{&out};

    mesh.setIndexBuffer(buffer, 0, MeshIndexType::UnsignedByte);

    CORRADE_COMPARE(out, "GL::Mesh::setIndexBuffer(): empty or moved-out Buffer instance was passed\n");
}

template<class T> void MeshGLTest::setIndexBufferTransferOwnership() {
    setTestCaseTemplateName(std::is_same<T, MeshIndexType>::value ?
        "GL::MeshIndexType" : "Magnum::MeshIndexType");

    const UnsignedShort data = 0;
    Buffer buffer{Buffer::TargetHint::ElementArray};
    buffer.setData({&data, 1}, BufferUsage::StaticDraw);

    const GLuint id = buffer.id();
    CORRADE_VERIFY(glIsBuffer(id));

    {

        Mesh mesh;
        mesh.setIndexBuffer(buffer, 0, T::UnsignedShort);
        CORRADE_VERIFY(buffer.id());
        CORRADE_VERIFY(glIsBuffer(id));
    }

    CORRADE_VERIFY(glIsBuffer(id));

    {

        Mesh mesh;
        mesh.setIndexBuffer(Utility::move(buffer), 0, T::UnsignedShort);
        CORRADE_VERIFY(!buffer.id());
        CORRADE_VERIFY(glIsBuffer(id));
    }

    CORRADE_VERIFY(!glIsBuffer(id));
}

template<class T> void MeshGLTest::setIndexBufferRangeTransferOwnership() {
    setTestCaseTemplateName(std::is_same<T, MeshIndexType>::value ?
        "GL::MeshIndexType" : "Magnum::MeshIndexType");

    const UnsignedShort data = 0;
    Buffer buffer{Buffer::TargetHint::ElementArray};
    buffer.setData({&data, 1}, BufferUsage::StaticDraw);

    const GLuint id = buffer.id();
    CORRADE_VERIFY(glIsBuffer(id));

    {
        Mesh mesh;
        mesh.setIndexBuffer(buffer, 0, T::UnsignedShort, 0, 1);
        CORRADE_VERIFY(buffer.id());
        CORRADE_VERIFY(glIsBuffer(id));
    }

    CORRADE_VERIFY(glIsBuffer(id));

    {
        Mesh mesh;
        mesh.setIndexBuffer(Utility::move(buffer), 0, T::UnsignedShort, 0, 1);
        CORRADE_VERIFY(!buffer.id());
        CORRADE_VERIFY(glIsBuffer(id));
    }

    CORRADE_VERIFY(!glIsBuffer(id));
}

void MeshGLTest::setIndexOffset() {
    /* Like setIndexBuffer(), but with a four-byte index type and the Checker
       internals unwrapped to call setIndexOffset() on the Mesh directly
       instead of the view */

    Buffer vertices;
    vertices.setData(indexedVertexData, BufferUsage::StaticDraw);

    constexpr UnsignedInt indexData[] = { 2, 267276, 2653, 282675, 0, 221987 };
    Buffer indices{Buffer::TargetHint::ElementArray};
    indices.setData(indexData, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.addVertexBuffer(vertices, 1*4,  MultipleShader::Position(),
                         MultipleShader::Normal(), MultipleShader::TextureCoordinates())
        .setIndexBuffer(indices, 4, MeshIndexType::UnsignedInt);

    Renderbuffer renderbuffer;
    renderbuffer.setStorage(
        #ifndef MAGNUM_TARGET_GLES2
        RenderbufferFormat::RGBA8,
        #else
        RenderbufferFormat::RGBA4,
        #endif
        Vector2i(1));
    Framebuffer framebuffer{{{}, Vector2i{1}}};
    framebuffer.attachRenderbuffer(Framebuffer::ColorAttachment(0), renderbuffer);

    framebuffer.bind();
    mesh.setPrimitive(MeshPrimitive::Points)
        .setCount(1)
        .setIndexOffset(3, 0, 1);
    CORRADE_COMPARE(mesh.indexOffset(), 3);

    MultipleShader{}.draw(mesh);

    const auto value = framebuffer.read({{}, Vector2i{1}}, {PixelFormat::RGBA, PixelType::UnsignedByte}).pixels<Color4ub>()[0][0];

    MAGNUM_VERIFY_NO_GL_ERROR();
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_COMPARE(value, indexedResult);
    #else /* RGBA4, so less precision */
    CORRADE_COMPARE_WITH(value, indexedResult, TestSuite::Compare::around(0x10101000_rgba));
    #endif
}

void MeshGLTest::indexTypeSetIndexOffsetNotIndexed() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Mesh mesh;
    MeshView view{mesh};

    Containers::String out;
    Error redirectError{&out};
    mesh.indexType();
    mesh.setIndexOffset(3);
    view.setIndexOffset(3);
    CORRADE_COMPARE(out,
        "GL::Mesh::indexType(): mesh is not indexed\n"
        "GL::Mesh::setIndexOffset(): mesh is not indexed\n"
        "GL::MeshView::setIndexOffset(): mesh is not indexed\n");
}

void MeshGLTest::unbindVAOWhenSettingIndexBufferData() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::vertex_array_object>())
        CORRADE_SKIP(Extensions::ARB::vertex_array_object::string() << "is not supported.");
    if(Context::current().isExtensionSupported<Extensions::ARB::direct_state_access>())
        CORRADE_SKIP(Extensions::ARB::direct_state_access::string() << "is active which circumvents the issue tested here.");
    #elif defined(MAGNUM_TARGET_GLES2)
    if(!Context::current().isExtensionSupported<Extensions::OES::vertex_array_object>())
        CORRADE_SKIP(Extensions::OES::vertex_array_object::string() << "is not supported.");
    #endif

    typedef Attribute<0, Float> Attribute;

    const Float data[] = {
        -0.7f,
        Math::unpack<Float, UnsignedByte>(92),
        Math::unpack<Float, UnsignedByte>(32)
    };
    Buffer buffer{Buffer::TargetHint::Array};
    buffer.setData(data, BufferUsage::StaticDraw);

    Buffer indices{Buffer::TargetHint::ElementArray};
    indices.setData(std::initializer_list<UnsignedByte>{5, 0}, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.addVertexBuffer(buffer, 4, Attribute{})
        .setIndexBuffer(indices, 0, MeshIndexType::UnsignedByte);

    /* This buffer should have no effect on the mesh above */
    Buffer otherIndices{Buffer::TargetHint::ElementArray};
    otherIndices.setData(std::initializer_list<UnsignedByte>{100, 1}, BufferUsage::StaticDraw);

    MAGNUM_VERIFY_NO_GL_ERROR();

    const auto value = Checker(FloatShader("float", "vec4(valueInterpolated, 0.0, 0.0, 0.0)"),
        #ifndef MAGNUM_TARGET_GLES2
        RenderbufferFormat::RGBA8,
        #else
        RenderbufferFormat::RGBA4,
        #endif
        mesh).get<UnsignedByte>(PixelFormat::RGBA, PixelType::UnsignedByte);

    MAGNUM_VERIFY_NO_GL_ERROR();
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_COMPARE(value, 92);
    #else /* RGBA4, so less precision */
    CORRADE_COMPARE_WITH(value, 92, TestSuite::Compare::around(16));
    #endif
}

void MeshGLTest::unbindIndexBufferWhenBindingVao() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::vertex_array_object>())
        CORRADE_SKIP(Extensions::ARB::vertex_array_object::string() << "is not supported.");
    if(Context::current().isExtensionSupported<Extensions::ARB::direct_state_access>())
        CORRADE_SKIP(Extensions::ARB::direct_state_access::string() << "is active which circumvents the issue tested here.");
    #elif defined(MAGNUM_TARGET_GLES2)
    if(!Context::current().isExtensionSupported<Extensions::OES::vertex_array_object>())
        CORRADE_SKIP(Extensions::OES::vertex_array_object::string() << "is not supported.");
    #endif

    typedef Attribute<0, Float> Attribute;

    const Float data[] = {
        -0.7f,
        Math::unpack<Float, UnsignedByte>(92),
        Math::unpack<Float, UnsignedByte>(32)
    };
    Buffer vertices{Buffer::TargetHint::Array};
    vertices.setData(data, BufferUsage::StaticDraw);

    Buffer indices{Buffer::TargetHint::ElementArray};
    /* Just reserve the memory first */
    indices.setData({nullptr, 2}, BufferUsage::StaticDraw);

    /* Create an indexed mesh first */
    Mesh indexed;
    indexed.addVertexBuffer(vertices, 0, Attribute{})
        .setIndexBuffer(indices, 0, MeshIndexType::UnsignedByte);

    /* Now bind an nonindexed mesh */
    Mesh nonindexed;
    nonindexed.addVertexBuffer(vertices, 0, Attribute{});

    /* Fill index buffer for the indexed mesh */
    indices.setData(std::initializer_list<UnsignedByte>{5, 1}, BufferUsage::StaticDraw);

    MAGNUM_VERIFY_NO_GL_ERROR();

    /* Draw the indexed mesh. The index buffer should be correctly updated,
       picking the second vertex with value of 92. */
    const auto value = Checker(FloatShader("float", "vec4(valueInterpolated, 0.0, 0.0, 0.0)"),
        #ifndef MAGNUM_TARGET_GLES2
        RenderbufferFormat::RGBA8,
        #else
        RenderbufferFormat::RGBA4,
        #endif
        indexed).get<UnsignedByte>(PixelFormat::RGBA, PixelType::UnsignedByte);

    MAGNUM_VERIFY_NO_GL_ERROR();
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_COMPARE(value, 92);
    #else /* RGBA4, so less precision */
    CORRADE_COMPARE_WITH(value, 92, TestSuite::Compare::around(16));
    #endif
}

void MeshGLTest::resetIndexBufferBindingWhenBindingVao() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::vertex_array_object>())
        CORRADE_SKIP(Extensions::ARB::vertex_array_object::string() << "is not supported.");
    if(Context::current().isExtensionSupported<Extensions::ARB::direct_state_access>())
        CORRADE_SKIP(Extensions::ARB::direct_state_access::string() << "is active which circumvents the issue tested here.");
    #elif defined(MAGNUM_TARGET_GLES2)
    if(!Context::current().isExtensionSupported<Extensions::OES::vertex_array_object>())
        CORRADE_SKIP(Extensions::OES::vertex_array_object::string() << "is not supported.");
    #endif

    typedef Attribute<0, Float> Attribute;

    const Float data[] = {
        -0.7f,
        Math::unpack<Float, UnsignedByte>(92),
        Math::unpack<Float, UnsignedByte>(32)
    };
    Buffer vertices{Buffer::TargetHint::Array};
    vertices.setData(data);

    /* Create an indexed mesh */
    Mesh indexed;
    indexed.addVertexBuffer(vertices, 0, Attribute{});

    /* Create an index buffer and fill it (the VAO is bound now, so it'll get
       unbound to avoid messing with its state). */
    Buffer indices{Buffer::TargetHint::ElementArray};
    indices.setData(std::initializer_list<UnsignedByte>{5, 1});

    /* Add the index buffer. The VAO is unbound, so it gets bound. That resets
       the element array buffer binding and then the buffer gets bound to the
       VAO. */
    indexed.setIndexBuffer(indices, 0, MeshIndexType::UnsignedByte);

    MAGNUM_VERIFY_NO_GL_ERROR();

    /* Draw the indexed mesh. The index buffer should be correctly bound,
       picking the second vertex with value of 92. */
    const auto value = Checker(FloatShader("float", "vec4(valueInterpolated, 0.0, 0.0, 0.0)"),
        #ifndef MAGNUM_TARGET_GLES2
        RenderbufferFormat::RGBA8,
        #else
        RenderbufferFormat::RGBA4,
        #endif
        indexed).get<UnsignedByte>(PixelFormat::RGBA, PixelType::UnsignedByte);

    MAGNUM_VERIFY_NO_GL_ERROR();
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_COMPARE(value, 92);
    #else /* RGBA4, so less precision */
    CORRADE_COMPARE_WITH(value, 92, TestSuite::Compare::around(16));
    #endif
}

void MeshGLTest::unbindVAOBeforeEnteringExternalSection() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::vertex_array_object>())
        CORRADE_SKIP(Extensions::ARB::vertex_array_object::string() << "is not supported.");
    #elif defined(MAGNUM_TARGET_GLES2)
    if(!Context::current().isExtensionSupported<Extensions::OES::vertex_array_object>())
        CORRADE_SKIP(Extensions::OES::vertex_array_object::string() << "is not supported.");
    #endif

    typedef Attribute<0, Float> Attribute;

    const Float data[] = {
        -0.7f,
        Math::unpack<Float, UnsignedByte>(92),
        Math::unpack<Float, UnsignedByte>(32)
    };
    Buffer buffer{Buffer::TargetHint::Array};
    buffer.setData(data, BufferUsage::StaticDraw);

    Buffer indices{Buffer::TargetHint::ElementArray};
    indices.setData(std::initializer_list<UnsignedByte>{5, 0}, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.addVertexBuffer(buffer, 4, Attribute{})
        .setIndexBuffer(indices, 0, MeshIndexType::UnsignedByte);

    {
        /* Comment this out to watch the world burn */
        Context::current().resetState(Context::State::MeshVao);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        /* Be nice to the other tests */
        Context::current().resetState(Context::State::ExitExternal);
    }

    MAGNUM_VERIFY_NO_GL_ERROR();

    const auto value = Checker(FloatShader("float", "vec4(valueInterpolated, 0.0, 0.0, 0.0)"),
        #ifndef MAGNUM_TARGET_GLES2
        RenderbufferFormat::RGBA8,
        #else
        RenderbufferFormat::RGBA4,
        #endif
        mesh).get<UnsignedByte>(PixelFormat::RGBA, PixelType::UnsignedByte);

    MAGNUM_VERIFY_NO_GL_ERROR();
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_COMPARE(value, 92);
    #else /* RGBA4, so less precision */
    CORRADE_COMPARE_WITH(value, 92, TestSuite::Compare::around(16));
    #endif
}

void MeshGLTest::bindScratchVaoWhenEnteringExternalSection() {
    typedef Attribute<0, Float> Attribute;

    const Float data[] = {
        -0.7f,
        Math::unpack<Float, UnsignedByte>(92),
        Math::unpack<Float, UnsignedByte>(32)
    };
    Buffer buffer{Buffer::TargetHint::Array};
    buffer.setData(data, BufferUsage::StaticDraw);

    Buffer indices{Buffer::TargetHint::ElementArray};
    indices.setData(std::initializer_list<UnsignedByte>{5, 0}, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.addVertexBuffer(buffer, 4, Attribute{})
        .setIndexBuffer(indices, 0, MeshIndexType::UnsignedByte);

    {
        /* Bind a scratch framebuffer so glDrawArrays() doesn't complain about
           an incomplete framebuffer in case we're on a framebuffer-less
           context */
        Renderbuffer renderbuffer;
        renderbuffer.setStorage(
            #ifndef MAGNUM_TARGET_GLES2
            RenderbufferFormat::RGBA8,
            #else
            RenderbufferFormat::RGBA4,
            #endif
            Vector2i{1});
        Framebuffer framebuffer{{{}, Vector2i{1}}};
        framebuffer.attachRenderbuffer(Framebuffer::ColorAttachment{0}, renderbuffer)
            .bind();

        /* Should bind a scratch VAO only on desktop with core profile and be
           a no-op everywhere else */
        Context::current().resetState(Context::State::EnterExternal
            |Context::State::BindScratchVao /* Comment this out to watch the world burn */
            );

        /* Should throw no GL error if scratch VAO is bound */
        glDrawArrays(GL_POINTS, 0, 0);

        /* Be nice to the other tests */
        Context::current().resetState(Context::State::ExitExternal);
    }

    MAGNUM_VERIFY_NO_GL_ERROR();

    const auto value = Checker(FloatShader("float", "vec4(valueInterpolated, 0.0, 0.0, 0.0)"),
        #ifndef MAGNUM_TARGET_GLES2
        RenderbufferFormat::RGBA8,
        #else
        RenderbufferFormat::RGBA4,
        #endif
        mesh).get<UnsignedByte>(PixelFormat::RGBA, PixelType::UnsignedByte);

    MAGNUM_VERIFY_NO_GL_ERROR();
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_COMPARE(value, 92);
    #else /* RGBA4, so less precision */
    CORRADE_COMPARE_WITH(value, 92, TestSuite::Compare::around(16));
    #endif
}

#if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
void MeshGLTest::setBaseVertex() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::draw_elements_base_vertex>())
        CORRADE_SKIP(Extensions::ARB::draw_elements_base_vertex::string() << "is not supported.");
    #elif !defined(MAGNUM_TARGET_WEBGL)
    #ifndef MAGNUM_TARGET_GLES2
    if(!Context::current().isExtensionSupported<Extensions::OES::draw_elements_base_vertex>() &&
       !Context::current().isExtensionSupported<Extensions::EXT::draw_elements_base_vertex>() &&
       !Context::current().isExtensionSupported<Extensions::ANGLE::base_vertex_base_instance>()
    )
        CORRADE_SKIP("Neither" << Extensions::OES::draw_elements_base_vertex::string() << "nor" << Extensions::EXT::draw_elements_base_vertex::string() << "nor" << Extensions::ANGLE::base_vertex_base_instance::string() << "is supported.");
    #else
    if(!Context::current().isExtensionSupported<Extensions::OES::draw_elements_base_vertex>() &&
       !Context::current().isExtensionSupported<Extensions::EXT::draw_elements_base_vertex>()
    )
        CORRADE_SKIP("Neither" << Extensions::OES::draw_elements_base_vertex::string() << "nor" << Extensions::EXT::draw_elements_base_vertex::string() << "is supported.");
    #endif
    #else
    if(!Context::current().isExtensionSupported<Extensions::WEBGL::draw_instanced_base_vertex_base_instance>())
        CORRADE_SKIP(Extensions::WEBGL::draw_instanced_base_vertex_base_instance::string() << "is not supported.");
    #endif

    Buffer vertices;
    vertices.setData(indexedVertexDataBaseVertex, BufferUsage::StaticDraw);

    constexpr UnsignedShort indexData[] = { 2, 1, 0 };
    Buffer indices{Buffer::TargetHint::ElementArray};
    indices.setData(indexData, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setBaseVertex(2)
        .addVertexBuffer(vertices, 2*4,  MultipleShader::Position(),
                         MultipleShader::Normal(), MultipleShader::TextureCoordinates())
        .setIndexBuffer(indices, 2, MeshIndexType::UnsignedShort);

    MAGNUM_VERIFY_NO_GL_ERROR();

    const auto value = Checker(MultipleShader{}, RenderbufferFormat::RGBA8,
        mesh).get<Color4ub>(PixelFormat::RGBA, PixelType::UnsignedByte);

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(value, indexedResult);
}
#endif

#ifdef MAGNUM_TARGET_GLES
void MeshGLTest::setBaseVertexNoExtensionAvailable() {
    #ifndef MAGNUM_TARGET_WEBGL
    if(Context::current().isVersionSupported(Version::GLES320))
        CORRADE_SKIP("OpenGL ES 3.2 is supported.");
    if(Context::current().isExtensionSupported<Extensions::EXT::draw_elements_base_vertex>())
        CORRADE_SKIP(Extensions::EXT::draw_elements_base_vertex::string() << "is supported.");
    if(Context::current().isExtensionSupported<Extensions::OES::draw_elements_base_vertex>())
        CORRADE_SKIP(Extensions::OES::draw_elements_base_vertex::string() << "is supported.");
    #ifndef MAGNUM_TARGET_GLES2
    if(Context::current().isExtensionSupported<Extensions::ANGLE::base_vertex_base_instance>())
        CORRADE_SKIP(Extensions::ANGLE::base_vertex_base_instance::string() << "is supported.");
    #endif
    #elif !defined(MAGNUM_TARGET_GLES2)
    if(Context::current().isExtensionSupported<Extensions::WEBGL::draw_instanced_base_vertex_base_instance>())
        CORRADE_SKIP(Extensions::WEBGL::draw_instanced_base_vertex_base_instance::string() << "is supported.");
    #endif

    constexpr UnsignedShort indexData[] = { 2, 1, 0 };
    Buffer indices{Buffer::TargetHint::ElementArray};
    indices.setData(indexData, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setCount(3)
        .setBaseVertex(1)
        .setIndexBuffer(indices, 0, MeshIndexType::UnsignedShort);

    Containers::String out;
    Error redirectError{&out};
    MultipleShader{}.draw(mesh);
    #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    CORRADE_COMPARE(out, "GL::AbstractShaderProgram::draw(): no extension available for indexed mesh draw with base vertex specification\n");
    #else
    CORRADE_COMPARE(out, "GL::AbstractShaderProgram::draw(): indexed mesh draw with base vertex specification possible only since WebGL 2.0\n");
    #endif
}

void MeshGLTest::setBaseVertexRangeNoExtensionAvailable() {
    #ifndef MAGNUM_TARGET_WEBGL
    if(Context::current().isVersionSupported(Version::GLES320))
        CORRADE_SKIP("OpenGL ES 3.2 is supported.");
    if(Context::current().isExtensionSupported<Extensions::EXT::draw_elements_base_vertex>())
        CORRADE_SKIP(Extensions::EXT::draw_elements_base_vertex::string() << "is supported.");
    if(Context::current().isExtensionSupported<Extensions::OES::draw_elements_base_vertex>())
        CORRADE_SKIP(Extensions::OES::draw_elements_base_vertex::string() << "is supported.");
    #ifndef MAGNUM_TARGET_GLES2
    if(Context::current().isExtensionSupported<Extensions::ANGLE::base_vertex_base_instance>())
        CORRADE_SKIP(Extensions::ANGLE::base_vertex_base_instance::string() << "is supported.");
    #endif
    #elif !defined(MAGNUM_TARGET_GLES2)
    if(Context::current().isExtensionSupported<Extensions::WEBGL::draw_instanced_base_vertex_base_instance>())
        CORRADE_SKIP(Extensions::WEBGL::draw_instanced_base_vertex_base_instance::string() << "is supported.");
    #endif

    constexpr UnsignedShort indexData[] = { 2, 1, 0 };
    Buffer indices{Buffer::TargetHint::ElementArray};
    indices.setData(indexData, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setCount(3)
        .setBaseVertex(1)
        .setIndexBuffer(indices, 0, MeshIndexType::UnsignedShort, 0, 2);

    Containers::String out;
    Error redirectError{&out};
    MultipleShader{}.draw(mesh);
    #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    CORRADE_COMPARE(out, "GL::AbstractShaderProgram::draw(): no extension available for indexed mesh draw with base vertex specification\n");
    #else
    CORRADE_COMPARE(out, "GL::AbstractShaderProgram::draw(): indexed mesh draw with base vertex specification possible only since WebGL 2.0\n");
    #endif
}
#endif

void MeshGLTest::setInstanceCount() {
    /* Verbatim copy of addVertexBufferFloat() with added extension check and
       setInstanceCount() call. It would just render three times the same
       value. I'm too lazy to invent proper test case, so I'll just check that
       it didn't generate any error and rendered something */

    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::draw_instanced>())
        CORRADE_SKIP(Extensions::ARB::draw_instanced::string() << "is not supported.");
    #elif defined(MAGNUM_TARGET_GLES2)
    #ifndef MAGNUM_TARGET_WEBGL
    if(!Context::current().isExtensionSupported<Extensions::ANGLE::instanced_arrays>() &&
       !Context::current().isExtensionSupported<Extensions::EXT::instanced_arrays>() &&
       !Context::current().isExtensionSupported<Extensions::EXT::draw_instanced>() &&
       !Context::current().isExtensionSupported<Extensions::NV::instanced_arrays>() &&
       !Context::current().isExtensionSupported<Extensions::NV::draw_instanced>())
        CORRADE_SKIP("Required extension is not supported.");
    #else
    if(!Context::current().isExtensionSupported<Extensions::ANGLE::instanced_arrays>())
        CORRADE_SKIP(Extensions::ANGLE::instanced_arrays::string() << "is not supported.");
    #endif
    #endif

    typedef Attribute<0, Float> Attribute;

    const Float data[] = { 0.0f, -0.7f, Math::unpack<Float, UnsignedByte>(96) };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setBaseVertex(1)
        .setInstanceCount(3)
        .addVertexBuffer(buffer, 4, Attribute());

    MAGNUM_VERIFY_NO_GL_ERROR();

    const auto value = Checker(FloatShader("float", "vec4(valueInterpolated, 0.0, 0.0, 0.0)"),
        #ifndef MAGNUM_TARGET_GLES2
        RenderbufferFormat::RGBA8,
        #else
        RenderbufferFormat::RGBA4,
        #endif
        mesh).get<UnsignedByte>(PixelFormat::RGBA, PixelType::UnsignedByte);

    MAGNUM_VERIFY_NO_GL_ERROR();
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_COMPARE(value, 96);
    #else /* RGBA4, so less precision */
    CORRADE_COMPARE_WITH(value, 96, TestSuite::Compare::around(16));
    #endif
}

#ifndef MAGNUM_TARGET_GLES2
void MeshGLTest::setInstanceCountBaseInstance() {
    /* Verbatim copy of setInstanceCount() with additional extension check and
       setBaseInstance() call. It would just render three times the same
       value. I'm too lazy to invent proper test case, so I'll just check that
       it didn't generate any error and rendered something */

    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::draw_instanced>())
        CORRADE_SKIP(Extensions::ARB::draw_instanced::string() << "is not supported.");
    if(!Context::current().isExtensionSupported<Extensions::ARB::base_instance>())
        CORRADE_SKIP(Extensions::ARB::base_instance::string() << "is not supported.");
    #elif !defined(MAGNUM_TARGET_WEBGL)
    if(!Context::current().isExtensionSupported<Extensions::ANGLE::base_vertex_base_instance>())
        CORRADE_SKIP(Extensions::ANGLE::base_vertex_base_instance::string() << "is not supported.");
    #else
    if(!Context::current().isExtensionSupported<Extensions::WEBGL::draw_instanced_base_vertex_base_instance>())
        CORRADE_SKIP(Extensions::WEBGL::draw_instanced_base_vertex_base_instance::string() << "is not supported.");
    #endif

    typedef Attribute<0, Float> Attribute;

    const Float data[] = { 0.0f, -0.7f, Math::unpack<Float, UnsignedByte>(96) };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setBaseVertex(1)
        .setInstanceCount(3)
        .setBaseInstance(72)
        .addVertexBuffer(buffer, 4, Attribute());

    MAGNUM_VERIFY_NO_GL_ERROR();

    const auto value = Checker(FloatShader("float", "vec4(valueInterpolated, 0.0, 0.0, 0.0)"),
        RenderbufferFormat::RGBA8,
        mesh).get<UnsignedByte>(PixelFormat::RGBA, PixelType::UnsignedByte);

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(value, 96);
}

#ifdef MAGNUM_TARGET_GLES
void MeshGLTest::setInstanceCountBaseInstanceNoExtensionAvailable() {
    #ifndef MAGNUM_TARGET_WEBGL
    if(Context::current().isExtensionSupported<Extensions::ANGLE::base_vertex_base_instance>())
        CORRADE_SKIP(Extensions::ANGLE::base_vertex_base_instance::string() << "is supported.");
    #else
    if(Context::current().isExtensionSupported<Extensions::WEBGL::draw_instanced_base_vertex_base_instance>())
        CORRADE_SKIP(Extensions::WEBGL::draw_instanced_base_vertex_base_instance::string() << "is supported.");
    #endif

    Mesh mesh;
    mesh.setCount(3)
        .setInstanceCount(2)
        .setBaseInstance(1);

    Containers::String out;
    Error redirectError{&out};
    MultipleShader{}.draw(mesh);
    CORRADE_COMPARE(out, "GL::AbstractShaderProgram::draw(): no extension available for instanced mesh draw with base instance specification\n");
}
#endif
#endif

void MeshGLTest::setInstanceCountIndexed() {
    /* Verbatim copy of setIndexBuffer() with added extension check and
       setInstanceCount() call. It would just render three times the same
       value. I'm too lazy to invent proper test case, so I'll just check that
       it didn't generate any error and rendered something */

    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::draw_instanced>())
        CORRADE_SKIP(Extensions::ARB::draw_instanced::string() << "is not supported.");
    #elif defined(MAGNUM_TARGET_GLES2)
    #ifndef MAGNUM_TARGET_WEBGL
    if(!Context::current().isExtensionSupported<Extensions::ANGLE::instanced_arrays>() &&
       !Context::current().isExtensionSupported<Extensions::EXT::instanced_arrays>() &&
       !Context::current().isExtensionSupported<Extensions::EXT::draw_instanced>() &&
       !Context::current().isExtensionSupported<Extensions::NV::instanced_arrays>() &&
       !Context::current().isExtensionSupported<Extensions::NV::draw_instanced>())
        CORRADE_SKIP("Required extension is not supported.");
    #else
    if(!Context::current().isExtensionSupported<Extensions::ANGLE::instanced_arrays>())
        CORRADE_SKIP(Extensions::ANGLE::instanced_arrays::string() << "is not supported.");
    #endif
    #endif

    Buffer vertices;
    vertices.setData(indexedVertexData, BufferUsage::StaticDraw);

    constexpr UnsignedShort indexData[] = { 2, 1, 0 };
    Buffer indices{Buffer::TargetHint::ElementArray};
    indices.setData(indexData, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setInstanceCount(3)
        .addVertexBuffer(vertices, 1*4,  MultipleShader::Position(),
                         MultipleShader::Normal(), MultipleShader::TextureCoordinates())
        .setIndexBuffer(indices, 2, MeshIndexType::UnsignedShort);

    MAGNUM_VERIFY_NO_GL_ERROR();

    const auto value = Checker(MultipleShader{},
        #ifndef MAGNUM_TARGET_GLES2
        RenderbufferFormat::RGBA8,
        #else
        RenderbufferFormat::RGBA4,
        #endif
        mesh).get<Color4ub>(PixelFormat::RGBA, PixelType::UnsignedByte);

    MAGNUM_VERIFY_NO_GL_ERROR();
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_COMPARE(value, indexedResult);
    #else /* RGBA4, so less precision */
    CORRADE_COMPARE_WITH(value, indexedResult, TestSuite::Compare::around(0x10101000_rgba));
    #endif
}

#ifndef MAGNUM_TARGET_GLES2
void MeshGLTest::setInstanceCountIndexedBaseInstance() {
    /* Verbatim copy of setInstanceCountIndexed() with additional extension
       check and setBaseInstance() call. It would just render three times the
       same value. I'm too lazy to invent proper test case, so I'll just check
       that it didn't generate any error and rendered something */

    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::draw_instanced>())
        CORRADE_SKIP(Extensions::ARB::draw_instanced::string() << "is not supported.");
    if(!Context::current().isExtensionSupported<Extensions::ARB::base_instance>())
        CORRADE_SKIP(Extensions::ARB::base_instance::string() << "is not supported.");
    #elif !defined(MAGNUM_TARGET_WEBGL)
    if(!Context::current().isExtensionSupported<Extensions::ANGLE::base_vertex_base_instance>())
        CORRADE_SKIP(Extensions::ANGLE::base_vertex_base_instance::string() << "is not supported.");
    #else
    if(!Context::current().isExtensionSupported<Extensions::WEBGL::draw_instanced_base_vertex_base_instance>())
        CORRADE_SKIP(Extensions::WEBGL::draw_instanced_base_vertex_base_instance::string() << "is not supported.");
    #endif

    Buffer vertices;
    vertices.setData(indexedVertexData, BufferUsage::StaticDraw);

    constexpr UnsignedShort indexData[] = { 2, 1, 0 };
    Buffer indices{Buffer::TargetHint::ElementArray};
    indices.setData(indexData, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setInstanceCount(3)
        .setBaseInstance(72)
        .addVertexBuffer(vertices, 1*4,  MultipleShader::Position(),
                         MultipleShader::Normal(), MultipleShader::TextureCoordinates())
        .setIndexBuffer(indices, 2, MeshIndexType::UnsignedShort);

    MAGNUM_VERIFY_NO_GL_ERROR();

    const auto value = Checker(MultipleShader{}, RenderbufferFormat::RGBA8,
        mesh).get<Color4ub>(PixelFormat::RGBA, PixelType::UnsignedByte);

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(value, indexedResult);
}

#ifdef MAGNUM_TARGET_GLES
void MeshGLTest::setInstanceCountIndexedBaseInstanceNoExtensionAvailable() {
    #ifndef MAGNUM_TARGET_WEBGL
    if(Context::current().isExtensionSupported<Extensions::ANGLE::base_vertex_base_instance>())
        CORRADE_SKIP(Extensions::ANGLE::base_vertex_base_instance::string() << "is supported.");
    #else
    if(Context::current().isExtensionSupported<Extensions::WEBGL::draw_instanced_base_vertex_base_instance>())
        CORRADE_SKIP(Extensions::WEBGL::draw_instanced_base_vertex_base_instance::string() << "is supported.");
    #endif

    constexpr UnsignedShort indexData[] = { 2, 1, 0 };
    Buffer indices{Buffer::TargetHint::ElementArray};
    indices.setData(indexData, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setCount(3)
        .setInstanceCount(2)
        .setBaseInstance(1)
        .setIndexBuffer(indices, 0, MeshIndexType::UnsignedShort);

    Containers::String out;
    Error redirectError{&out};
    MultipleShader{}.draw(mesh);
    CORRADE_COMPARE(out, "GL::AbstractShaderProgram::draw(): no extension available for instanced indexed mesh draw with base instance specification\n");
}
#endif
#endif

#ifndef MAGNUM_TARGET_GLES2
void MeshGLTest::setInstanceCountIndexedBaseVertex() {
    /* Verbatim copy of setBaseVertex() with additional extension check and
       setInstanceCount() call. It would just render three times the same
       value. I'm too lazy to invent proper test case, so I'll just check
       that it didn't generate any error and rendered something */

    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::draw_instanced>())
        CORRADE_SKIP(Extensions::ARB::draw_instanced::string() << "is not supported.");
    if(!Context::current().isExtensionSupported<Extensions::ARB::draw_elements_base_vertex>())
        CORRADE_SKIP(Extensions::ARB::draw_elements_base_vertex::string() << "is not supported.");
    #elif !defined(MAGNUM_TARGET_WEBGL)
    #ifndef MAGNUM_TARGET_GLES2
    if(!Context::current().isExtensionSupported<Extensions::OES::draw_elements_base_vertex>() &&
       !Context::current().isExtensionSupported<Extensions::EXT::draw_elements_base_vertex>() &&
       !Context::current().isExtensionSupported<Extensions::ANGLE::base_vertex_base_instance>()
    )
        CORRADE_SKIP("Neither" << Extensions::OES::draw_elements_base_vertex::string() << "nor" << Extensions::EXT::draw_elements_base_vertex::string() << "nor" << Extensions::ANGLE::base_vertex_base_instance::string() << "is supported.");
    #else
    if(!Context::current().isExtensionSupported<Extensions::OES::draw_elements_base_vertex>() &&
       !Context::current().isExtensionSupported<Extensions::EXT::draw_elements_base_vertex>()
    )
        CORRADE_SKIP("Neither" << Extensions::OES::draw_elements_base_vertex::string() << "nor" << Extensions::EXT::draw_elements_base_vertex::string() << "is supported.");
    #endif
    #else
    if(!Context::current().isExtensionSupported<Extensions::WEBGL::draw_instanced_base_vertex_base_instance>())
        CORRADE_SKIP(Extensions::WEBGL::draw_instanced_base_vertex_base_instance::string() << "is not supported.");
    #endif

    Buffer vertices;
    vertices.setData(indexedVertexDataBaseVertex, BufferUsage::StaticDraw);

    constexpr UnsignedShort indexData[] = { 2, 1, 0 };
    Buffer indices{Buffer::TargetHint::ElementArray};
    indices.setData(indexData, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setBaseVertex(2)
        .setInstanceCount(3)
        .addVertexBuffer(vertices, 2*4,  MultipleShader::Position(),
                         MultipleShader::Normal(), MultipleShader::TextureCoordinates())
        .setIndexBuffer(indices, 2, MeshIndexType::UnsignedShort);

    MAGNUM_VERIFY_NO_GL_ERROR();

    const auto value = Checker(MultipleShader{}, RenderbufferFormat::RGBA8,
        mesh).get<Color4ub>(PixelFormat::RGBA, PixelType::UnsignedByte);

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(value, indexedResult);
}
#endif

#ifdef MAGNUM_TARGET_GLES
void MeshGLTest::setInstanceCountIndexedBaseVertexNoExtensionAvailable() {
    #ifndef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_WEBGL
    if(Context::current().isVersionSupported(Version::GLES320))
        CORRADE_SKIP("OpenGL ES 3.2 is supported.");
    if(Context::current().isExtensionSupported<Extensions::EXT::draw_elements_base_vertex>())
        CORRADE_SKIP(Extensions::EXT::draw_elements_base_vertex::string() << "is supported.");
    if(Context::current().isExtensionSupported<Extensions::OES::draw_elements_base_vertex>())
        CORRADE_SKIP(Extensions::OES::draw_elements_base_vertex::string() << "is supported.");
    if(Context::current().isExtensionSupported<Extensions::ANGLE::base_vertex_base_instance>())
        CORRADE_SKIP(Extensions::ANGLE::base_vertex_base_instance::string() << "is supported.");
    #else
    if(Context::current().isExtensionSupported<Extensions::WEBGL::draw_instanced_base_vertex_base_instance>())
        CORRADE_SKIP(Extensions::WEBGL::draw_instanced_base_vertex_base_instance::string() << "is supported.");
    #endif
    #endif

    constexpr UnsignedShort indexData[] = { 2, 1, 0 };
    Buffer indices{Buffer::TargetHint::ElementArray};
    indices.setData(indexData, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setCount(3)
        .setInstanceCount(2)
        .setBaseVertex(1)
        .setIndexBuffer(indices, 0, MeshIndexType::UnsignedShort);

    Containers::String out;
    Error redirectError{&out};
    MultipleShader{}.draw(mesh);
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_COMPARE(out, "GL::AbstractShaderProgram::draw(): no extension available for instanced indexed mesh draw with base vertex specification\n");
    #else
    CORRADE_COMPARE(out, "GL::AbstractShaderProgram::draw(): instanced indexed mesh draw with base vertex specification possible only since OpenGL ES 3.0\n");
    #endif
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void MeshGLTest::setInstanceCountIndexedBaseVertexBaseInstance() {
    /* Verbatim copy of setInstanceCountBaseVertex() with added extension check
       and setBaseInstance() call. It would just render three times the same
       value. I'm too lazy to invent proper test case, so I'll just check
       that it didn't generate any error and rendered something */

    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::draw_instanced>())
        CORRADE_SKIP(Extensions::ARB::draw_instanced::string() << "is not supported.");
    if(!Context::current().isExtensionSupported<Extensions::ARB::draw_elements_base_vertex>())
        CORRADE_SKIP(Extensions::ARB::draw_elements_base_vertex::string() << "is not supported.");
    if(!Context::current().isExtensionSupported<Extensions::ARB::base_instance>())
        CORRADE_SKIP(Extensions::ARB::base_instance::string() << "is not supported.");
    #elif !defined(MAGNUM_TARGET_WEBGL)
    if(!Context::current().isExtensionSupported<Extensions::ANGLE::base_vertex_base_instance>())
        CORRADE_SKIP(Extensions::ANGLE::base_vertex_base_instance::string() << "is not supported.");
    #else
    if(!Context::current().isExtensionSupported<Extensions::WEBGL::draw_instanced_base_vertex_base_instance>())
        CORRADE_SKIP(Extensions::WEBGL::draw_instanced_base_vertex_base_instance::string() << "is not supported.");
    #endif

    Buffer vertices;
    vertices.setData(indexedVertexDataBaseVertex, BufferUsage::StaticDraw);

    constexpr UnsignedShort indexData[] = { 2, 1, 0 };
    Buffer indices{Buffer::TargetHint::ElementArray};
    indices.setData(indexData, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setBaseVertex(2)
        .setInstanceCount(3)
        .setBaseInstance(72)
        .addVertexBuffer(vertices, 2*4,  MultipleShader::Position(),
                         MultipleShader::Normal(), MultipleShader::TextureCoordinates())
        .setIndexBuffer(indices, 2, MeshIndexType::UnsignedShort);

    MAGNUM_VERIFY_NO_GL_ERROR();

    const auto value = Checker(MultipleShader{}, RenderbufferFormat::RGBA8,
        mesh).get<Color4ub>(PixelFormat::RGBA, PixelType::UnsignedByte);

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(value, indexedResult);
}

#ifdef MAGNUM_TARGET_GLES
void MeshGLTest::setInstanceCountIndexedBaseVertexBaseInstanceNoExtensionAvailable() {
    #ifndef MAGNUM_TARGET_WEBGL
    if(Context::current().isExtensionSupported<Extensions::ANGLE::base_vertex_base_instance>())
        CORRADE_SKIP(Extensions::ANGLE::base_vertex_base_instance::string() << "is supported.");
    #else
    if(Context::current().isExtensionSupported<Extensions::WEBGL::draw_instanced_base_vertex_base_instance>())
        CORRADE_SKIP(Extensions::WEBGL::draw_instanced_base_vertex_base_instance::string() << "is supported.");
    #endif

    constexpr UnsignedShort indexData[] = { 2, 1, 0 };
    Buffer indices{Buffer::TargetHint::ElementArray};
    indices.setData(indexData, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setCount(3)
        .setInstanceCount(2)
        .setBaseVertex(1)
        .setBaseInstance(1)
        .setIndexBuffer(indices, 0, MeshIndexType::UnsignedShort);

    Containers::String out;
    Error redirectError{&out};
    MultipleShader{}.draw(mesh);
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_COMPARE(out, "GL::AbstractShaderProgram::draw(): no extension available for instanced indexed mesh draw with base vertex and base instance specification\n");
    #else
    CORRADE_COMPARE(out, "GL::AbstractShaderProgram::draw(): instanced indexed mesh draw with base vertex specification possible only since OpenGL 3.0\n");
    #endif
}
#endif
#endif

void MeshGLTest::addVertexBufferInstancedFloat() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::draw_instanced>())
        CORRADE_SKIP(Extensions::ARB::draw_instanced::string() << "is not supported.");
    if(!Context::current().isExtensionSupported<Extensions::ARB::instanced_arrays>())
        CORRADE_SKIP(Extensions::ARB::instanced_arrays::string() << "is not supported.");
    #elif defined(MAGNUM_TARGET_GLES2)
    #ifndef MAGNUM_TARGET_WEBGL
    if(!Context::current().isExtensionSupported<Extensions::ANGLE::instanced_arrays>() &&
       !Context::current().isExtensionSupported<Extensions::EXT::instanced_arrays>() &&
       !Context::current().isExtensionSupported<Extensions::NV::instanced_arrays>())
        CORRADE_SKIP("Required extension is not supported.");
    #else
    if(!Context::current().isExtensionSupported<Extensions::ANGLE::instanced_arrays>())
        CORRADE_SKIP(Extensions::ANGLE::instanced_arrays::string() << "is not supported.");
    #endif
    #endif

    typedef Attribute<0, Float> Attribute;

    const Float data[] = {
        0.0f,   /* Offset */
                /* Base vertex is ignored for instanced arrays */
        -0.7f,  /* First instance */
        0.3f,   /* Second instance */
        Math::unpack<Float, UnsignedByte>(96) /* Third instance */
    };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setInstanceCount(3)
        .addVertexBufferInstanced(buffer, 1, 4, Attribute{});

    MAGNUM_VERIFY_NO_GL_ERROR();

    const auto value = Checker(FloatShader("float", "vec4(valueInterpolated, 0.0, 0.0, 0.0)"
        /* WebGL 1 requires that at least one attribute is not instanced. Add
           a dummy input (that isn't even present in the mesh) to fix that. */
        #if defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2)
        , /* dummy */ true
        #endif
    ),
        #ifndef MAGNUM_TARGET_GLES2
        RenderbufferFormat::RGBA8,
        #else
        RenderbufferFormat::RGBA4,
        #endif
        mesh).get<UnsignedByte>(PixelFormat::RGBA, PixelType::UnsignedByte);

    MAGNUM_VERIFY_NO_GL_ERROR();
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_COMPARE(value, 96);
    #else /* RGBA4, so less precision */
    CORRADE_COMPARE_WITH(value, 96, TestSuite::Compare::around(16));
    #endif
}

#ifndef MAGNUM_TARGET_GLES2
void MeshGLTest::addVertexBufferInstancedInteger() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::draw_instanced>())
        CORRADE_SKIP(Extensions::ARB::draw_instanced::string() << "is not supported.");
    if(!Context::current().isExtensionSupported<Extensions::ARB::instanced_arrays>())
        CORRADE_SKIP(Extensions::ARB::instanced_arrays::string() << "is not supported.");
    if(!Context::current().isExtensionSupported<Extensions::EXT::gpu_shader4>())
        CORRADE_SKIP(Extensions::EXT::gpu_shader4::string() << "is not supported.");
    #endif

    typedef Attribute<0, UnsignedInt> Attribute;

    constexpr UnsignedInt data[] = {
        0,      /* Offset */
                /* Base vertex is ignored for instanced arrays */
        157,    /* First instance */
        25,     /* Second instance */
        35681   /* Third instance */
    };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setInstanceCount(3)
        .addVertexBufferInstanced(buffer, 1, 4, Attribute{});

    MAGNUM_VERIFY_NO_GL_ERROR();

    const auto value = Checker(IntegerShader("uint"), RenderbufferFormat::R32UI, mesh).get<UnsignedInt>(
        #ifndef MAGNUM_TARGET_WEBGL
        PixelFormat::RedInteger,
        #else
        PixelFormat::RGBAInteger,
        #endif
        PixelType::UnsignedInt);

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(value, 35681);
}
#endif

#ifndef MAGNUM_TARGET_GLES
void MeshGLTest::addVertexBufferInstancedDouble() {
    if(!Context::current().isExtensionSupported<Extensions::ARB::draw_instanced>())
        CORRADE_SKIP(Extensions::ARB::draw_instanced::string() << "is not supported.");
    if(!Context::current().isExtensionSupported<Extensions::ARB::instanced_arrays>())
        CORRADE_SKIP(Extensions::ARB::instanced_arrays::string() << "is not supported.");
    if(!Context::current().isExtensionSupported<Extensions::ARB::vertex_attrib_64bit>())
        CORRADE_SKIP(Extensions::ARB::vertex_attrib_64bit::string() << "is not supported.");

    typedef Attribute<0, Double> Attribute;

    const Double data[] = {
        0.0,    /* Offset */
                /* Base vertex is ignored for instanced arrays */
        -0.7,   /* First instance */
        0.3,    /* Second instance */
        Math::unpack<Double, UnsignedShort>(45828) /* Third instance */
    };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Mesh mesh;
    mesh.setInstanceCount(3)
        .addVertexBufferInstanced(buffer, 1, 8, Attribute{});

    MAGNUM_VERIFY_NO_GL_ERROR();

    const auto value = Checker(DoubleShader("double", "float", "float(value)"),
        RenderbufferFormat::R16, mesh).get<UnsignedShort>(PixelFormat::Red, PixelType::UnsignedShort);

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(value, 45828);
}
#endif

void MeshGLTest::resetDivisorAfterInstancedDraw() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::draw_instanced>())
        CORRADE_SKIP(Extensions::ARB::draw_instanced::string() << "is not supported.");
    if(!Context::current().isExtensionSupported<Extensions::ARB::instanced_arrays>())
        CORRADE_SKIP(Extensions::ARB::instanced_arrays::string() << "is not supported.");
    #elif defined(MAGNUM_TARGET_GLES2)
    #ifndef MAGNUM_TARGET_WEBGL
    if(!Context::current().isExtensionSupported<Extensions::ANGLE::instanced_arrays>() &&
       !Context::current().isExtensionSupported<Extensions::EXT::instanced_arrays>() &&
       !Context::current().isExtensionSupported<Extensions::NV::instanced_arrays>())
        CORRADE_SKIP("Required extension is not supported.");
    #else
    if(!Context::current().isExtensionSupported<Extensions::ANGLE::instanced_arrays>())
        CORRADE_SKIP(Extensions::ANGLE::instanced_arrays::string() << "is not supported.");
    #endif
    #endif

    /* This doesn't affect VAOs, because they encapsulate the state */
    #ifndef MAGNUM_TARGET_GLES
    if(Context::current().isExtensionSupported<Extensions::ARB::vertex_array_object>())
        CORRADE_SKIP(Extensions::ARB::vertex_array_object::string() << "is enabled, can't test.");
    #elif defined(MAGNUM_TARGET_GLES2)
    if(Context::current().isExtensionSupported<Extensions::OES::vertex_array_object>())
        CORRADE_SKIP(Extensions::OES::vertex_array_object::string() << "is enabled, can't test.");
    #endif

    typedef Attribute<0, Float> Attribute;

    const Float data[]{
        0,
        Math::unpack<Float, UnsignedByte>(96),
        Math::unpack<Float, UnsignedByte>(48),
    };
    Buffer buffer;
    buffer.setData(data, BufferUsage::StaticDraw);

    Renderbuffer renderbuffer;
    renderbuffer.setStorage(
        #ifndef MAGNUM_TARGET_GLES2
        RenderbufferFormat::RGBA8,
        #else
        RenderbufferFormat::RGBA4,
        #endif
        Vector2i(1));
    Framebuffer framebuffer{{{}, Vector2i(1)}};
    framebuffer.attachRenderbuffer(Framebuffer::ColorAttachment(0), renderbuffer)
                .bind();

    FloatShader shader{"float", "vec4(valueInterpolated, 0.0, 0.0, 0.0)"};

    MAGNUM_VERIFY_NO_GL_ERROR();

    /* Draw instanced first. Two single-vertex instances of an attribute with
       divisor 1, first draws 0, second draws 96 */
    {
        Mesh mesh;
        mesh.setInstanceCount(2)
            .addVertexBufferInstanced(buffer, 1, 0, Attribute{})
            .setPrimitive(MeshPrimitive::Points)
            .setCount(1);
        shader.draw(mesh);

        MAGNUM_VERIFY_NO_GL_ERROR();

        CORRADE_COMPARE(Containers::arrayCast<UnsignedByte>(framebuffer.read({{}, Vector2i{1}}, {PixelFormat::RGBA, PixelType::UnsignedByte}).data())[0], 96);
    }

    /* Draw normal after. One two-vertex instance of an attribute with divisor
       0, first draws 96, second 48. In case divisor is not properly reset,
       I'll get 96 on both. */
    {
        Mesh mesh;
        mesh.setInstanceCount(1)
            .addVertexBuffer(buffer, 4, Attribute{})
            .setPrimitive(MeshPrimitive::Points)
            .setCount(2);
        shader.draw(mesh);

        MAGNUM_VERIFY_NO_GL_ERROR();

        CORRADE_COMPARE(Containers::arrayCast<UnsignedByte>(framebuffer.read({{}, Vector2i{1}}, {PixelFormat::RGBA, PixelType::UnsignedByte}).data())[0], 48);
    }
}

void MeshGLTest::drawInstancedAttributeSingleInstance() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::draw_instanced>())
        CORRADE_SKIP(Extensions::ARB::draw_instanced::string() << "is not supported.");
    if(!Context::current().isExtensionSupported<Extensions::ARB::instanced_arrays>())
        CORRADE_SKIP(Extensions::ARB::instanced_arrays::string() << "is not supported.");
    #elif defined(MAGNUM_TARGET_GLES2)
    #ifndef MAGNUM_TARGET_WEBGL
    if(!Context::current().isExtensionSupported<Extensions::ANGLE::instanced_arrays>() &&
       !Context::current().isExtensionSupported<Extensions::EXT::instanced_arrays>() &&
       !Context::current().isExtensionSupported<Extensions::NV::instanced_arrays>())
        CORRADE_SKIP("Required extension is not supported.");
    #else
    if(!Context::current().isExtensionSupported<Extensions::ANGLE::instanced_arrays>())
        CORRADE_SKIP(Extensions::ANGLE::instanced_arrays::string() << "is not supported.");
    #endif
    #endif

    typedef Attribute<0, Float> Attribute;

    const Float data[]{
        Math::unpack<Float, UnsignedByte>(96)
    };
    Buffer buffer;
    /* The ANGLE validation error can be only reproduced with DynamicDraw used
       here, not StaticDraw. Interesting. */
    buffer.setData(data, BufferUsage::DynamicDraw);

    Renderbuffer renderbuffer;
    renderbuffer.setStorage(
        #ifndef MAGNUM_TARGET_GLES2
        RenderbufferFormat::RGBA8,
        #else
        RenderbufferFormat::RGBA4,
        #endif
        Vector2i(1));
    Framebuffer framebuffer{{{}, Vector2i(1)}};
    framebuffer.attachRenderbuffer(Framebuffer::ColorAttachment{0}, renderbuffer)
               .bind();

    FloatShader shader{"float", "vec4(valueInterpolated, 0.0, 0.0, 0.0)"
        /* WebGL 1 requires that at least one attribute is not instanced. Add
           a dummy input (that isn't even present in the mesh) to fix that. */
        #if defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2)
        , /* dummy */ true
        #endif
    };

    MAGNUM_VERIFY_NO_GL_ERROR();

    /* Create a mesh with (implicitly) one instance and the buffer added as
       instanced. Drawing it 16 times should always draw 96 with no error.
       On ANGLE w/o the "angle-instanced-attributes-always-draw-instanced"
       workaround this would trigger a validation error where it would complain
       that the 4-byte buffer is not large enough to draw 16 vertices. */
    Mesh mesh;
    mesh.addVertexBufferInstanced(buffer, 1, 0, Attribute{})
        .setPrimitive(MeshPrimitive::Points)
        .setCount(16);
    shader.draw(mesh);

    MAGNUM_VERIFY_NO_GL_ERROR();

    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_COMPARE(Containers::arrayCast<UnsignedByte>(framebuffer.read({{}, Vector2i{1}}, {PixelFormat::RGBA, PixelType::UnsignedByte}).data())[0], 96);
    #else /* RGBA4, so less precision */
    CORRADE_COMPARE_WITH(Containers::arrayCast<UnsignedByte>(framebuffer.read({{}, Vector2i{1}}, {PixelFormat::RGBA, PixelType::UnsignedByte}).data())[0], 96,
        TestSuite::Compare::around(16));
    #endif
}

struct MultiDrawShader: AbstractShaderProgram {
    typedef Attribute<0, Vector2> Position;
    typedef Attribute<1, Vector4> Value;

    explicit MultiDrawShader(bool vertexId = false, bool drawId = false);
};

MultiDrawShader::MultiDrawShader(bool vertexId, bool drawId) {
    /* Pick GLSL 3.0 / ESSL 3.0 for gl_VertexID, if available */
    #ifndef MAGNUM_TARGET_GLES
    #ifndef CORRADE_TARGET_APPLE
    const Version version = Context::current().supportedVersion({Version::GL300, Version::GL210});
    #else
    const Version version = Version::GL310;
    #endif
    #else
    const Version version = Context::current().supportedVersion({Version::GLES300, Version::GLES200});
    #endif
    Shader vert{version, Shader::Type::Vertex};
    Shader frag{version, Shader::Type::Fragment};

    if(drawId) vert.addSource(
        "#ifndef GL_ES\n"
        "#extension GL_ARB_shader_draw_parameters: require\n"
        "#define vertexOrDrawId gl_DrawIDARB\n"
        "#else /* covers WebGL as well */\n"
        "#extension GL_ANGLE_multi_draw: require\n"
        "#define vertexOrDrawId gl_DrawID\n"
        "#endif\n");
    else if(vertexId) vert.addSource(
        "#define vertexOrDrawId gl_VertexID\n");
    else vert.addSource(
        "#define vertexOrDrawId 0\n");
    vert.addSource(
        "#if !defined(GL_ES) && __VERSION__ == 120\n"
        "#define mediump\n"
        "#endif\n"
        "#if (defined(GL_ES) && __VERSION__ == 100) || __VERSION__ == 120\n"
        "#define in attribute\n"
        "#define out varying\n"
        "#endif\n"
        "in mediump vec2 position;\n"
        "in mediump vec4 value;\n"
        "out mediump float valueInterpolated;\n"
        "void main() {\n"
        /* WebGL 1 doesn't allow dynamic indexing into a vec4. Similar thing is
           in MultiDrawInstancedShader below or in the SUBSCRIPTING_WORKAROUND
           in Shaders/MeshVisualizer.vert. */
        #if defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2)
        "         if(vertexOrDrawId == 0) valueInterpolated = value.x;\n"
        "    else if(vertexOrDrawId == 1) valueInterpolated = value.y;\n"
        "    else if(vertexOrDrawId == 2) valueInterpolated = value.z;\n"
        "    else                         valueInterpolated = value.w;\n"
        #else
        "    valueInterpolated = value[vertexOrDrawId];\n"
        #endif
        "    gl_PointSize = 1.0;\n"
        "    gl_Position = vec4(position, 0.0, 1.0);\n"
        "}\n");
    frag.addSource(
        "#if !defined(GL_ES) && __VERSION__ == 120\n"
        "#define mediump\n"
        "#endif\n"
        "#if (defined(GL_ES) && __VERSION__ == 100) || __VERSION__ == 120\n"
        "#define in varying\n"
        "#define result gl_FragColor\n"
        "#endif\n"
        "in mediump float valueInterpolated;\n"
        "#if (defined(GL_ES) && __VERSION__ >= 300) || __VERSION__ >= 130\n"
        "out mediump vec4 result;\n"
        "#endif\n"
        "void main() { result.r = valueInterpolated; }\n");

    CORRADE_INTERNAL_ASSERT_OUTPUT(vert.compile() && frag.compile());

    attachShaders({vert, frag});

    bindAttributeLocation(Position::Location, "position");
    bindAttributeLocation(Value::Location, "value");

    CORRADE_INTERNAL_ASSERT_OUTPUT(link());
}

struct MultiDrawChecker {
    explicit MultiDrawChecker();

    Vector4 get();

    Renderbuffer _renderbuffer;
    Framebuffer _framebuffer;
};

MultiDrawChecker::MultiDrawChecker(): _framebuffer{{{}, Vector2i{2}}} {
    _renderbuffer.setStorage(
        #ifndef MAGNUM_TARGET_GLES2
        RenderbufferFormat::RGBA8,
        #else
        RenderbufferFormat::RGBA4,
        #endif
        Vector2i{2});
    _framebuffer.attachRenderbuffer(Framebuffer::ColorAttachment{0}, _renderbuffer);

    _framebuffer
        .clear(FramebufferClear::Color)
        .bind();
}

Vector4 MultiDrawChecker::get() {
    const Image2D image = _framebuffer.read({{}, Vector2i{2}}, {PixelFormat::RGBA, PixelType::UnsignedByte});
    const Containers::StridedArrayView2D<const UnsignedByte> redChannel = Containers::arrayCast<const UnsignedByte>(image.pixels<Vector4ub>());
    return Math::unpack<Vector4>(Vector4ub{redChannel[0][0],
                                           redChannel[0][1],
                                           redChannel[1][0],
                                           redChannel[1][1]});
}

void MeshGLTest::multiDraw() {
    auto&& data = MultiDrawData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifdef MAGNUM_TARGET_GLES
    #ifndef MAGNUM_TARGET_WEBGL
    if(!Context::current().isExtensionSupported<Extensions::EXT::multi_draw_arrays>() &&
       !Context::current().isExtensionSupported<Extensions::ANGLE::multi_draw>())
        CORRADE_SKIP("Neither" << Extensions::EXT::multi_draw_arrays::string() << "nor" << Extensions::ANGLE::multi_draw::string() << "is supported.");
    #else
    if(!Context::current().isExtensionSupported<Extensions::WEBGL::multi_draw>())
        CORRADE_SKIP(Extensions::WEBGL::multi_draw::string() << "is not supported.");
    #endif
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    if(data.vertexId && !GL::Context::current().isExtensionSupported<GL::Extensions::MAGNUM::shader_vertex_id>())
        CORRADE_SKIP("gl_VertexID not supported");
    #endif

    if(data.drawId) {
        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_draw_parameters>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_draw_parameters::string() << "is not supported.");
        #elif !defined(MAGNUM_TARGET_WEBGL)
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ANGLE::multi_draw>())
            CORRADE_SKIP(GL::Extensions::ANGLE::multi_draw::string() << "is not supported.");
        #else
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::WEBGL::multi_draw>())
            CORRADE_SKIP(GL::Extensions::WEBGL::multi_draw::string() << "is not supported.");
        #endif
    }

    const struct {
        Vector2 position;
        Vector4 value;
    } vertexData[] {
        {}, /* initial offset */
        {{-1.0f/3.0f, -1.0f/3.0f}, data.values[0]},
        {{ 1.0f/3.0f, -1.0f/3.0f}, data.values[1]},
        {{-1.0f/3.0f,  1.0f/3.0f}, data.values[2]},
        {{ 1.0f/3.0f,  1.0f/3.0f}, data.values[3]},
    };

    Mesh mesh{MeshPrimitive::Points};
    mesh.addVertexBuffer(Buffer{vertexData}, sizeof(vertexData[0]), MultiDrawShader::Position{}, MultiDrawShader::Value{});

    MAGNUM_VERIFY_NO_GL_ERROR();

    MultiDrawChecker checker;
    MultiDrawShader{data.vertexId, data.drawId}.draw(mesh, data.counts, data.vertexOffsets, nullptr);
    Vector4 value = checker.get();

    MAGNUM_VERIFY_NO_GL_ERROR();
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_COMPARE_WITH(value, data.expected,
        TestSuite::Compare::around(Vector4{1.0f/255.0f}));
    #else
    CORRADE_COMPARE_WITH(value, data.expected, /* it's only RGBA4 */
        TestSuite::Compare::around(Vector4{1.0f/15.0f}));
    #endif
}

void MeshGLTest::multiDrawSparseArrays() {
    auto&& data = MultiDrawData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifdef MAGNUM_TARGET_GLES
    #ifndef MAGNUM_TARGET_WEBGL
    if(!Context::current().isExtensionSupported<Extensions::EXT::multi_draw_arrays>() &&
       !Context::current().isExtensionSupported<Extensions::ANGLE::multi_draw>())
        CORRADE_SKIP("Neither" << Extensions::EXT::multi_draw_arrays::string() << "nor" << Extensions::ANGLE::multi_draw::string() << "is supported.");
    #else
    if(!Context::current().isExtensionSupported<Extensions::WEBGL::multi_draw>())
        CORRADE_SKIP(Extensions::WEBGL::multi_draw::string() << "is not supported.");
    #endif
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    if(data.vertexId && !GL::Context::current().isExtensionSupported<GL::Extensions::MAGNUM::shader_vertex_id>())
        CORRADE_SKIP("gl_VertexID not supported");
    #endif

    if(data.drawId) {
        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_draw_parameters>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_draw_parameters::string() << "is not supported.");
        #elif !defined(MAGNUM_TARGET_WEBGL)
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ANGLE::multi_draw>())
            CORRADE_SKIP(GL::Extensions::ANGLE::multi_draw::string() << "is not supported.");
        #else
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::WEBGL::multi_draw>())
            CORRADE_SKIP(GL::Extensions::WEBGL::multi_draw::string() << "is not supported.");
        #endif
    }

    const struct {
        Vector2 position;
        Vector4 value;
    } vertexData[] {
        {}, /* initial offset */
        {{-1.0f/3.0f, -1.0f/3.0f}, data.values[0]},
        {{ 1.0f/3.0f, -1.0f/3.0f}, data.values[1]},
        {{-1.0f/3.0f,  1.0f/3.0f}, data.values[2]},
        {{ 1.0f/3.0f,  1.0f/3.0f}, data.values[3]},
    };

    Mesh mesh{MeshPrimitive::Points};
    mesh.addVertexBuffer(Buffer{vertexData}, sizeof(vertexData[0]), MultiDrawShader::Position{}, MultiDrawShader::Value{});

    MAGNUM_VERIFY_NO_GL_ERROR();

    /* The signature accepted by glMultiDrawArraysIndirect() */
    struct Command {
        UnsignedInt count;
        UnsignedInt instanceCount;
        UnsignedInt first;
        UnsignedInt baseInstance;
    } commands[] {
        {data.counts[0], 0, data.vertexOffsets[0], 0},
        {data.counts[1], 0, data.vertexOffsets[1], 0},
        {data.counts[2], 0, data.vertexOffsets[2], 0},
        {data.counts[3], 0, data.vertexOffsets[3], 0},
    };

    MultiDrawChecker checker;
    MultiDrawShader{data.vertexId, data.drawId}.draw(mesh,
        Containers::stridedArrayView(commands).slice(&Command::count),
        Containers::stridedArrayView(commands).slice(&Command::first),
        nullptr);
    Vector4 value = checker.get();

    MAGNUM_VERIFY_NO_GL_ERROR();
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_COMPARE_WITH(value, data.expected,
        TestSuite::Compare::around(Vector4{1.0f/255.0f}));
    #else
    CORRADE_COMPARE_WITH(value, data.expected, /* it's only RGBA4 */
        TestSuite::Compare::around(Vector4{1.0f/15.0f}));
    #endif
}

void MeshGLTest::multiDrawViews() {
    auto&& data = MultiDrawData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES2
    if(data.vertexId && !GL::Context::current().isExtensionSupported<GL::Extensions::MAGNUM::shader_vertex_id>())
        CORRADE_SKIP("gl_VertexID not supported");
    #endif

    if(data.drawId) {
        #ifndef MAGNUM_TARGET_GLES
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ARB::shader_draw_parameters>())
            CORRADE_SKIP(GL::Extensions::ARB::shader_draw_parameters::string() << "is not supported.");
        #elif !defined(MAGNUM_TARGET_WEBGL)
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ANGLE::multi_draw>())
            CORRADE_SKIP(GL::Extensions::ANGLE::multi_draw::string() << "is not supported.");
        #else
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::WEBGL::multi_draw>())
            CORRADE_SKIP(GL::Extensions::WEBGL::multi_draw::string() << "is not supported.");
        #endif
    }

    #ifdef MAGNUM_TARGET_GLES
    #ifndef MAGNUM_TARGET_WEBGL
    if(!Context::current().isExtensionSupported<Extensions::EXT::multi_draw_arrays>() &&
       !Context::current().isExtensionSupported<Extensions::ANGLE::multi_draw>())
        CORRADE_INFO("Neither" << Extensions::EXT::multi_draw_arrays::string() << "nor" << Extensions::ANGLE::multi_draw::string() << "is supported, using fallback implementation");
    #else
    if(!Context::current().isExtensionSupported<Extensions::WEBGL::multi_draw>())
        CORRADE_INFO(Extensions::WEBGL::multi_draw::string() << "is not supported, using fallback implementation");
    #endif
    #endif

    const struct {
        Vector2 position;
        Vector4 value;
    } vertexData[] {
        {}, /* initial offset */
        {{-1.0f/3.0f, -1.0f/3.0f}, data.values[0]},
        {{ 1.0f/3.0f, -1.0f/3.0f}, data.values[1]},
        {{-1.0f/3.0f,  1.0f/3.0f}, data.values[2]},
        {{ 1.0f/3.0f,  1.0f/3.0f}, data.values[3]},
    };

    Mesh mesh{MeshPrimitive::Points};
    mesh.addVertexBuffer(Buffer{vertexData}, sizeof(vertexData[0]), MultiDrawShader::Position{}, MultiDrawShader::Value{});

    MeshView a{mesh}, b{mesh}, c{mesh}, d{mesh};
    a.setCount(data.counts[0])
     .setBaseVertex(data.vertexOffsets[0]);
    b.setCount(data.counts[1])
     .setBaseVertex(data.vertexOffsets[1]);
    c.setCount(data.counts[2])
     .setBaseVertex(data.vertexOffsets[2]);
    d.setCount(data.counts[3])
     .setBaseVertex(data.vertexOffsets[3]);

    MAGNUM_VERIFY_NO_GL_ERROR();

    MultiDrawChecker checker;
    MultiDrawShader{data.vertexId, data.drawId}.draw({a, b, c, d});
    Vector4 value = checker.get();

    MAGNUM_VERIFY_NO_GL_ERROR();
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_COMPARE_WITH(value, data.expected,
        TestSuite::Compare::around(Vector4{1.0f/255.0f}));
    #else
    CORRADE_COMPARE_WITH(value, data.expected, /* it's only RGBA4 */
        TestSuite::Compare::around(Vector4{1.0f/15.0f}));
    #endif
}

template<class T> void MeshGLTest::multiDrawIndexed() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    auto&& data = MultiDrawIndexedData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifdef MAGNUM_TARGET_GLES
    #ifndef MAGNUM_TARGET_WEBGL
    if(!Context::current().isExtensionSupported<Extensions::EXT::multi_draw_arrays>() &&
       !Context::current().isExtensionSupported<Extensions::ANGLE::multi_draw>())
        CORRADE_SKIP("Neither" << Extensions::EXT::multi_draw_arrays::string() << "nor" << Extensions::ANGLE::multi_draw::string() << "is supported.");
    #else
    if(!Context::current().isExtensionSupported<Extensions::WEBGL::multi_draw>())
        CORRADE_SKIP(Extensions::WEBGL::multi_draw::string() << "is not supported.");
    #endif
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    if(data.vertexId && !GL::Context::current().isExtensionSupported<GL::Extensions::MAGNUM::shader_vertex_id>())
        CORRADE_SKIP("gl_VertexID not supported");
    #endif

    bool hasBaseVertex = data.vertexOffsets[0] || data.vertexOffsets[1] || data.vertexOffsets[2] || data.vertexOffsets[3];
    if(hasBaseVertex) {
        #ifndef MAGNUM_TARGET_GLES
        if(!Context::current().isExtensionSupported<Extensions::ARB::draw_elements_base_vertex>())
            CORRADE_SKIP(Extensions::ARB::draw_elements_base_vertex::string() << "is not supported.");
        #elif !defined(MAGNUM_TARGET_WEBGL)
        #ifndef MAGNUM_TARGET_GLES2
        /* {OES,EXT}_draw_elements_base_vertex requires EXT_multi_draw_arrays
           for the multi-draw entrypoint */
        if((!Context::current().isExtensionSupported<Extensions::EXT::multi_draw_arrays>() || (
            !Context::current().isExtensionSupported<Extensions::OES::draw_elements_base_vertex>() &&
            !Context::current().isExtensionSupported<Extensions::EXT::draw_elements_base_vertex>()
        )) &&
            !Context::current().isExtensionSupported<Extensions::ANGLE::base_vertex_base_instance>()
        )
            CORRADE_SKIP("Neither" << Extensions::OES::draw_elements_base_vertex::string() << "nor" << Extensions::EXT::draw_elements_base_vertex::string() << "nor" << Extensions::ANGLE::base_vertex_base_instance::string() << "is supported.");
        #else
        /* {OES,EXT}_draw_elements_base_vertex requires EXT_multi_draw_arrays
           for the multi-draw entrypoint */
        if(!Context::current().isExtensionSupported<Extensions::EXT::multi_draw_arrays>() || (
            !Context::current().isExtensionSupported<Extensions::OES::draw_elements_base_vertex>() &&
            !Context::current().isExtensionSupported<Extensions::EXT::draw_elements_base_vertex>()
        ))
            CORRADE_SKIP("Neither" << Extensions::OES::draw_elements_base_vertex::string() << "nor" << Extensions::EXT::draw_elements_base_vertex::string() << "is supported.");
        #endif
        #elif !defined(MAGNUM_TARGET_GLES2)
        if(!Context::current().isExtensionSupported<Extensions::WEBGL::multi_draw_instanced_base_vertex_base_instance>())
            CORRADE_SKIP(Extensions::WEBGL::multi_draw_instanced_base_vertex_base_instance::string() << "is not supported.");
        #else
        CORRADE_FAIL("Can't do base vertex here.");
        #endif
    }

    const struct {
        Vector2 position;
        Vector4 value;
    } vertexData[] {
        {}, /* initial offset */
        {{-1.0f/3.0f, -1.0f/3.0f}, data.values[0]},
        {{ 1.0f/3.0f, -1.0f/3.0f}, data.values[1]},
        {{-1.0f/3.0f,  1.0f/3.0f}, data.values[2]},
        {{ 1.0f/3.0f,  1.0f/3.0f}, data.values[3]},
    };

    Mesh mesh{MeshPrimitive::Points};
    mesh.addVertexBuffer(Buffer{vertexData}, sizeof(vertexData[0]), MultiDrawShader::Position{}, MultiDrawShader::Value{})
        .setIndexBuffer(Buffer{Buffer::TargetHint::ElementArray, data.indices}, 0, MeshIndexType::UnsignedInt);

    MAGNUM_VERIFY_NO_GL_ERROR();

    /* Converted to either a 32bit or 64bit type */
    const T indexOffsetsInBytes[]{
        data.indexOffsetsInBytes[0],
        data.indexOffsetsInBytes[1],
        data.indexOffsetsInBytes[2],
        data.indexOffsetsInBytes[3]
    };

    MultiDrawChecker checker;
    MultiDrawShader{data.vertexId, false}.draw(mesh, data.counts, hasBaseVertex ? Containers::arrayView(data.vertexOffsets) : nullptr, indexOffsetsInBytes);
    Vector4 value = checker.get();

    MAGNUM_VERIFY_NO_GL_ERROR();
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_COMPARE_WITH(value, data.expected,
        TestSuite::Compare::around(Vector4{1.0f/255.0f}));
    #else
    CORRADE_COMPARE_WITH(value, data.expected, /* it's only RGBA4 */
        TestSuite::Compare::around(Vector4{1.0f/15.0f}));
    #endif
}

template<class T> void MeshGLTest::multiDrawIndexedSparseArrays() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    auto&& data = MultiDrawIndexedData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifdef MAGNUM_TARGET_GLES
    #ifndef MAGNUM_TARGET_WEBGL
    if(!Context::current().isExtensionSupported<Extensions::EXT::multi_draw_arrays>() &&
       !Context::current().isExtensionSupported<Extensions::ANGLE::multi_draw>())
        CORRADE_SKIP("Neither" << Extensions::EXT::multi_draw_arrays::string() << "nor" << Extensions::ANGLE::multi_draw::string() << "is supported.");
    #else
    if(!Context::current().isExtensionSupported<Extensions::WEBGL::multi_draw>())
        CORRADE_SKIP(Extensions::WEBGL::multi_draw::string() << "is not supported.");
    #endif
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    if(data.vertexId && !GL::Context::current().isExtensionSupported<GL::Extensions::MAGNUM::shader_vertex_id>())
        CORRADE_SKIP("gl_VertexID not supported");
    #endif

    bool hasBaseVertex = data.vertexOffsets[0] || data.vertexOffsets[1] || data.vertexOffsets[2] || data.vertexOffsets[3];
    if(hasBaseVertex) {
        #ifndef MAGNUM_TARGET_GLES
        if(!Context::current().isExtensionSupported<Extensions::ARB::draw_elements_base_vertex>())
            CORRADE_SKIP(Extensions::ARB::draw_elements_base_vertex::string() << "is not supported.");
        #elif !defined(MAGNUM_TARGET_WEBGL)
        #ifndef MAGNUM_TARGET_GLES2
        /* {OES,EXT}_draw_elements_base_vertex requires EXT_multi_draw_arrays
           for the multi-draw entrypoint */
        if((!Context::current().isExtensionSupported<Extensions::EXT::multi_draw_arrays>() || (
            !Context::current().isExtensionSupported<Extensions::OES::draw_elements_base_vertex>() &&
            !Context::current().isExtensionSupported<Extensions::EXT::draw_elements_base_vertex>()
        )) &&
            !Context::current().isExtensionSupported<Extensions::ANGLE::base_vertex_base_instance>()
        )
            CORRADE_SKIP("Neither" << Extensions::OES::draw_elements_base_vertex::string() << "nor" << Extensions::EXT::draw_elements_base_vertex::string() << "nor" << Extensions::ANGLE::base_vertex_base_instance::string() << "is supported.");
        #else
        /* {OES,EXT}_draw_elements_base_vertex requires EXT_multi_draw_arrays
           for the multi-draw entrypoint */
        if(!Context::current().isExtensionSupported<Extensions::EXT::multi_draw_arrays>() || (
            !Context::current().isExtensionSupported<Extensions::OES::draw_elements_base_vertex>() &&
            !Context::current().isExtensionSupported<Extensions::EXT::draw_elements_base_vertex>()
        ))
            CORRADE_SKIP("Neither" << Extensions::OES::draw_elements_base_vertex::string() << "nor" << Extensions::EXT::draw_elements_base_vertex::string() << "is supported.");
        #endif
        #elif !defined(MAGNUM_TARGET_GLES2)
        if(!Context::current().isExtensionSupported<Extensions::WEBGL::multi_draw_instanced_base_vertex_base_instance>())
            CORRADE_SKIP(Extensions::WEBGL::multi_draw_instanced_base_vertex_base_instance::string() << "is not supported.");
        #else
        CORRADE_FAIL("Can't do base vertex here.");
        #endif
    }

    const struct {
        Vector2 position;
        Vector4 value;
    } vertexData[] {
        {}, /* initial offset */
        {{-1.0f/3.0f, -1.0f/3.0f}, data.values[0]},
        {{ 1.0f/3.0f, -1.0f/3.0f}, data.values[1]},
        {{-1.0f/3.0f,  1.0f/3.0f}, data.values[2]},
        {{ 1.0f/3.0f,  1.0f/3.0f}, data.values[3]},
    };

    Mesh mesh{MeshPrimitive::Points};
    mesh.addVertexBuffer(Buffer{vertexData}, sizeof(vertexData[0]), MultiDrawShader::Position{}, MultiDrawShader::Value{})
        .setIndexBuffer(Buffer{Buffer::TargetHint::ElementArray, data.indices}, 0, MeshIndexType::UnsignedInt);

    MAGNUM_VERIFY_NO_GL_ERROR();

    /* The signature accepted by glMultiDrawElementsIndirect() EXCEPT that
       here we need firstIndex to be in bytes */
    struct Command {
        UnsignedInt count;
        UnsignedInt instanceCount;
        T firstIndexInBytes; /* !! */
        UnsignedInt baseVertex;
        UnsignedInt baseInstance;
    } commands[] {
        {data.counts[0], 0, data.indexOffsetsInBytes[0], data.vertexOffsets[0], 0},
        {data.counts[1], 0, data.indexOffsetsInBytes[1], data.vertexOffsets[1], 0},
        {data.counts[2], 0, data.indexOffsetsInBytes[2], data.vertexOffsets[2], 0},
        {data.counts[3], 0, data.indexOffsetsInBytes[3], data.vertexOffsets[3], 0}
    };

    MultiDrawChecker checker;
    MultiDrawShader{data.vertexId, false}.draw(mesh,
        Containers::stridedArrayView(commands).slice(&Command::count),
        hasBaseVertex ? Containers::stridedArrayView(commands).slice(&Command::baseVertex) : nullptr,
        Containers::stridedArrayView(commands).slice(&Command::firstIndexInBytes));
    Vector4 value = checker.get();

    MAGNUM_VERIFY_NO_GL_ERROR();
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_COMPARE_WITH(value, data.expected,
        TestSuite::Compare::around(Vector4{1.0f/255.0f}));
    #else
    CORRADE_COMPARE_WITH(value, data.expected, /* it's only RGBA4 */
        TestSuite::Compare::around(Vector4{1.0f/15.0f}));
    #endif
}

void MeshGLTest::multiDrawIndexedViews() {
    auto&& data = MultiDrawIndexedData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifdef MAGNUM_TARGET_GLES
    #ifndef MAGNUM_TARGET_WEBGL
    if(!Context::current().isExtensionSupported<Extensions::EXT::multi_draw_arrays>() &&
       !Context::current().isExtensionSupported<Extensions::ANGLE::multi_draw>())
        CORRADE_INFO("Neither" << Extensions::EXT::multi_draw_arrays::string() << "nor" << Extensions::ANGLE::multi_draw::string() << "is supported, using fallback implementation");
    #else
    if(!Context::current().isExtensionSupported<Extensions::WEBGL::multi_draw>())
        CORRADE_INFO(Extensions::WEBGL::multi_draw::string() << "is not supported, using fallback implementation");
    #endif
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    if(data.vertexId && !GL::Context::current().isExtensionSupported<GL::Extensions::MAGNUM::shader_vertex_id>())
        CORRADE_SKIP("gl_VertexID not supported");
    #endif

    if(data.vertexOffsets[0] || data.vertexOffsets[1] || data.vertexOffsets[2] || data.vertexOffsets[3]) {
        #ifndef MAGNUM_TARGET_GLES
        if(!Context::current().isExtensionSupported<Extensions::ARB::draw_elements_base_vertex>())
            CORRADE_SKIP(Extensions::ARB::draw_elements_base_vertex::string() << "is not supported.");
        #elif !defined(MAGNUM_TARGET_WEBGL)
        #ifndef MAGNUM_TARGET_GLES2
        /* {OES,EXT}_draw_elements_base_vertex requires EXT_multi_draw_arrays
           for the multi-draw entrypoint */
        if((!Context::current().isExtensionSupported<Extensions::EXT::multi_draw_arrays>() || (
            !Context::current().isExtensionSupported<Extensions::OES::draw_elements_base_vertex>() &&
            !Context::current().isExtensionSupported<Extensions::EXT::draw_elements_base_vertex>()
        )) &&
            !Context::current().isExtensionSupported<Extensions::ANGLE::base_vertex_base_instance>()
        )
            CORRADE_SKIP("Neither" << Extensions::OES::draw_elements_base_vertex::string() << "nor" << Extensions::EXT::draw_elements_base_vertex::string() << "nor" << Extensions::ANGLE::base_vertex_base_instance::string() << "is supported.");
        #else
        /* {OES,EXT}_draw_elements_base_vertex requires EXT_multi_draw_arrays
           for the multi-draw entrypoint */
        if(!Context::current().isExtensionSupported<Extensions::EXT::multi_draw_arrays>() || (
            !Context::current().isExtensionSupported<Extensions::OES::draw_elements_base_vertex>() &&
            !Context::current().isExtensionSupported<Extensions::EXT::draw_elements_base_vertex>()
        ))
            CORRADE_SKIP("Neither" << Extensions::OES::draw_elements_base_vertex::string() << "nor" << Extensions::EXT::draw_elements_base_vertex::string() << "is supported.");
        #endif
        #elif !defined(MAGNUM_TARGET_GLES2)
        if(!Context::current().isExtensionSupported<Extensions::WEBGL::multi_draw_instanced_base_vertex_base_instance>())
            CORRADE_SKIP(Extensions::WEBGL::multi_draw_instanced_base_vertex_base_instance::string() << "is not supported.");
        #else
        CORRADE_FAIL("Can't do base vertex here.");
        #endif
    }

    const struct {
        Vector2 position;
        Vector4 value;
    } vertexData[] {
        {}, /* initial offset */
        {{-1.0f/3.0f, -1.0f/3.0f}, data.values[0]},
        {{ 1.0f/3.0f, -1.0f/3.0f}, data.values[1]},
        {{-1.0f/3.0f,  1.0f/3.0f}, data.values[2]},
        {{ 1.0f/3.0f,  1.0f/3.0f}, data.values[3]},
    };

    Mesh mesh{MeshPrimitive::Points};
    mesh.addVertexBuffer(Buffer{vertexData}, sizeof(vertexData[0]), MultiDrawShader::Position{}, MultiDrawShader::Value{})
        .setIndexBuffer(Buffer{Buffer::TargetHint::ElementArray, data.indices}, 0, MeshIndexType::UnsignedInt);

    MeshView a{mesh}, b{mesh}, c{mesh}, d{mesh};
    a.setCount(data.counts[0])
     .setIndexOffset(data.indexOffsetsInBytes[0]/sizeof(UnsignedInt))
     .setBaseVertex(data.vertexOffsets[0]);
    b.setCount(data.counts[1])
     .setIndexOffset(data.indexOffsetsInBytes[1]/sizeof(UnsignedInt))
     .setBaseVertex(data.vertexOffsets[1]);
    c.setCount(data.counts[2])
     .setIndexOffset(data.indexOffsetsInBytes[2]/sizeof(UnsignedInt))
     .setBaseVertex(data.vertexOffsets[2]);
    d.setCount(data.counts[3])
     .setIndexOffset(data.indexOffsetsInBytes[3]/sizeof(UnsignedInt))
     .setBaseVertex(data.vertexOffsets[3]);

    MAGNUM_VERIFY_NO_GL_ERROR();

    MultiDrawChecker checker;
    MultiDrawShader{data.vertexId, false}.draw({a, b, c, d});
    Vector4 value = checker.get();

    MAGNUM_VERIFY_NO_GL_ERROR();
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_COMPARE_WITH(value, data.expected,
        TestSuite::Compare::around(Vector4{1.0f/255.0f}));
    #else
    CORRADE_COMPARE_WITH(value, data.expected, /* it's only RGBA4 */
        TestSuite::Compare::around(Vector4{1.0f/15.0f}));
    #endif
}

void MeshGLTest::multiDrawWrongVertexOffsetSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Mesh mesh;
    MultiDrawShader shader;
    UnsignedInt counts[3]{};
    UnsignedInt vertexOffsets[2]{};

    Containers::String out;
    Error redirectError{&out};
    shader.draw(mesh, counts, vertexOffsets, nullptr);
    shader.draw(mesh, counts, nullptr, nullptr);
    CORRADE_COMPARE(out,
        "GL::AbstractShaderProgram::draw(): expected 3 vertex offset items but got 2\n"
        "GL::AbstractShaderProgram::draw(): expected 3 vertex offset items but got 0\n");
}

void MeshGLTest::multiDrawIndexedWrongVertexOffsetSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Mesh mesh;
    mesh.setIndexBuffer(Buffer{Buffer::TargetHint::ElementArray, {2, 1, 0}}, 0, MeshIndexType::UnsignedInt);
    MultiDrawShader shader;
    UnsignedInt counts[3]{};
    UnsignedInt vertexOffsets[2]{};
    UnsignedInt indexOffsets[3]{};

    Containers::String out;
    Error redirectError{&out};
    shader.draw(mesh, counts, vertexOffsets, indexOffsets);
    /* Omitting vertex offsets altogether is okay */
    CORRADE_COMPARE(out,
        "GL::AbstractShaderProgram::draw(): expected 3 vertex offset items but got 2\n");
}

void MeshGLTest::multiDrawIndexedWrongIndexOffsetSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Mesh mesh;
    mesh.setIndexBuffer(Buffer{Buffer::TargetHint::ElementArray, {2, 1, 0}}, 0, MeshIndexType::UnsignedInt);
    MultiDrawShader shader;
    UnsignedInt counts[3]{};
    UnsignedInt indexOffsets[2]{};

    Containers::String out;
    Error redirectError{&out};
    shader.draw(mesh, counts, nullptr, indexOffsets);
    shader.draw(mesh, counts, nullptr, nullptr);
    CORRADE_COMPARE(out,
        "GL::AbstractShaderProgram::draw(): expected 3 index offset items but got 2\n"
        "GL::AbstractShaderProgram::draw(): expected 3 index offset items but got 0\n");
}

#ifdef MAGNUM_TARGET_GLES
void MeshGLTest::multiDrawIndexedBaseVertexNoExtensionAvailable() {
    #ifdef MAGNUM_TARGET_GLES
    /* If the multidraw extensions aren't available, we can't test this assert,
       only the assert in the fallback path, which is already tested above. */
    #ifndef MAGNUM_TARGET_WEBGL
    if(!Context::current().isExtensionSupported<Extensions::EXT::multi_draw_arrays>() &&
       !Context::current().isExtensionSupported<Extensions::ANGLE::multi_draw>())
        CORRADE_SKIP("Neither" << Extensions::EXT::multi_draw_arrays::string() << "nor" << Extensions::ANGLE::multi_draw::string() << "is supported.");
    #else
    if(!Context::current().isExtensionSupported<Extensions::WEBGL::multi_draw>())
        CORRADE_SKIP(Extensions::WEBGL::multi_draw::string() << "is not supported.");
    #endif
    #endif

    #ifndef MAGNUM_TARGET_WEBGL
    if(Context::current().isExtensionSupported<Extensions::EXT::draw_elements_base_vertex>())
        CORRADE_SKIP(Extensions::EXT::draw_elements_base_vertex::string() << "is supported.");
    if(Context::current().isExtensionSupported<Extensions::OES::draw_elements_base_vertex>())
        CORRADE_SKIP(Extensions::OES::draw_elements_base_vertex::string() << "is supported.");
    #ifndef MAGNUM_TARGET_GLES2
    if(Context::current().isExtensionSupported<Extensions::ANGLE::base_vertex_base_instance>())
        CORRADE_SKIP(Extensions::ANGLE::base_vertex_base_instance::string() << "is supported.");
    #endif
    #elif !defined(MAGNUM_TARGET_GLES2)
    if(Context::current().isExtensionSupported<Extensions::WEBGL::multi_draw_instanced_base_vertex_base_instance>())
        CORRADE_SKIP(Extensions::WEBGL::multi_draw_instanced_base_vertex_base_instance::string() << "is supported.");
    #endif

    Mesh mesh;
    mesh.setIndexBuffer(Buffer{Buffer::TargetHint::ElementArray, {2, 1, 0}}, 0, MeshIndexType::UnsignedInt);

    UnsignedInt counts[]{3};
    UnsignedInt vertexOffsets[]{0};
    UnsignedInt indexOffsets[]{0};

    Containers::String out;
    Error redirectError{&out};
    MultiDrawShader{}.draw(mesh, counts, vertexOffsets, indexOffsets);
    #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    CORRADE_COMPARE(out, "GL::AbstractShaderProgram::draw(): no extension available for indexed mesh multi-draw with base vertex specification\n");
    #else
    CORRADE_COMPARE(out, "GL::AbstractShaderProgram::draw(): indexed mesh multi-draw with base vertex specification possible only since WebGL 2.0\n");
    #endif
}

void MeshGLTest::multiDrawIndexedViewsBaseVertexNoExtensionAvailable() {
    #ifdef MAGNUM_TARGET_GLES
    /* If the multidraw extensions aren't available, we can't test this assert,
       only the assert in the fallback path, which is already tested above. */
    #ifndef MAGNUM_TARGET_WEBGL
    if(!Context::current().isExtensionSupported<Extensions::EXT::multi_draw_arrays>() &&
       !Context::current().isExtensionSupported<Extensions::ANGLE::multi_draw>())
        CORRADE_SKIP("Neither" << Extensions::EXT::multi_draw_arrays::string() << "nor" << Extensions::ANGLE::multi_draw::string() << "is supported.");
    #else
    if(!Context::current().isExtensionSupported<Extensions::WEBGL::multi_draw>())
        CORRADE_SKIP(Extensions::WEBGL::multi_draw::string() << "is not supported.");
    #endif
    #endif

    #ifndef MAGNUM_TARGET_WEBGL
    if(Context::current().isExtensionSupported<Extensions::EXT::draw_elements_base_vertex>())
        CORRADE_SKIP(Extensions::EXT::draw_elements_base_vertex::string() << "is supported.");
    if(Context::current().isExtensionSupported<Extensions::OES::draw_elements_base_vertex>())
        CORRADE_SKIP(Extensions::OES::draw_elements_base_vertex::string() << "is supported.");
    #ifndef MAGNUM_TARGET_GLES2
    if(Context::current().isExtensionSupported<Extensions::ANGLE::base_vertex_base_instance>())
        CORRADE_SKIP(Extensions::ANGLE::base_vertex_base_instance::string() << "is supported.");
    #endif
    #elif !defined(MAGNUM_TARGET_GLES2)
    if(Context::current().isExtensionSupported<Extensions::WEBGL::multi_draw_instanced_base_vertex_base_instance>())
        CORRADE_SKIP(Extensions::WEBGL::multi_draw_instanced_base_vertex_base_instance::string() << "is supported.");
    #endif

    Mesh mesh;
    mesh.setIndexBuffer(Buffer{Buffer::TargetHint::ElementArray, {2, 1, 0}}, 0, MeshIndexType::UnsignedInt);

    UnsignedInt counts[]{3};
    UnsignedInt vertexOffsets[]{1};
    UnsignedInt indexOffsets[]{0};

    Containers::String out;
    Error redirectError{&out};
    MultiDrawShader{}.draw(mesh, counts, vertexOffsets, indexOffsets);
    #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    CORRADE_COMPARE(out, "GL::AbstractShaderProgram::draw(): no extension available for indexed mesh multi-draw with base vertex specification\n");
    #else
    CORRADE_COMPARE(out, "GL::AbstractShaderProgram::draw(): indexed mesh multi-draw with base vertex specification possible only since WebGL 2.0\n");
    #endif
}
#endif

void MeshGLTest::multiDrawViewsInstanced() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Mesh mesh;
    MeshView view{mesh};
    view.setCount(3)
        .setInstanceCount(2);

    Containers::String out;
    Error redirectError{&out};
    MultiDrawShader{}.draw({view, view});
    CORRADE_COMPARE(out, "GL::AbstractShaderProgram::draw(): cannot multi-draw instanced meshes\n");
}

void MeshGLTest::multiDrawViewsDifferentMeshes() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Mesh a, b;
    MeshView viewA{a}, viewB{b};

    Containers::String out;
    Error redirectError{&out};
    MultiDrawShader{}.draw({viewA, viewB});
    CORRADE_COMPARE(out, Utility::format("GL::AbstractShaderProgram::draw(): all meshes must be views of the same original mesh, expected 0x{:x} but got 0x{:x} at index 1\n", reinterpret_cast<std::uintptr_t>(&a), reinterpret_cast<std::uintptr_t>(&b)));
}

#ifdef MAGNUM_TARGET_GLES
struct MultiDrawInstancedShader: AbstractShaderProgram {
    typedef Attribute<0, Float> PositionX;
    typedef Attribute<1, Float> PositionY;
    typedef Attribute<2, Vector3> Value;
    #ifdef MAGNUM_TARGET_GLES2
    /* ES2 has no integer attributes either */
    typedef Attribute<3, Float> InstanceId;
    #endif

    explicit MultiDrawInstancedShader(bool vertexId = false, bool drawId = false
        #ifndef MAGNUM_TARGET_GLES2
        , bool instanceOffset = false
        #endif
    );
};

MultiDrawInstancedShader::MultiDrawInstancedShader(bool vertexId, bool drawId
    #ifndef MAGNUM_TARGET_GLES2
    , bool instanceOffset
    #endif
) {
    /* Pick GLSL 3.0 / ESSL 3.0 for gl_VertexID, if available */
    #ifndef MAGNUM_TARGET_GLES
    #ifndef CORRADE_TARGET_APPLE
    const Version version = Context::current().supportedVersion({Version::GL300, Version::GL210});
    #else
    const Version version = Version::GL310;
    #endif
    #else
    const Version version = Context::current().supportedVersion({Version::GLES300, Version::GLES200});
    #endif
    Shader vert{version, Shader::Type::Vertex};
    Shader frag{version, Shader::Type::Fragment};

    if(drawId) vert.addSource(
        "#extension GL_ANGLE_multi_draw: require\n"
        "#define vertexOrDrawIdOrInstanceOffset gl_DrawID\n");
    else if(vertexId) vert.addSource(
        "#define vertexOrDrawIdOrInstanceOffset gl_VertexID\n");
    #ifndef MAGNUM_TARGET_GLES2
    else if(instanceOffset) vert.addSource(
        "#extension GL_ANGLE_base_vertex_base_instance: require\n"
        "#define vertexOrDrawIdOrInstanceOffset gl_BaseInstance\n");
    #endif
    else vert.addSource(
        "#define vertexOrDrawIdOrInstanceOffset 0\n");
    vert.addSource(
        "#if defined(GL_ES) && __VERSION__ == 100\n"
        "#define in attribute\n"
        "#define out varying\n"
        "#endif\n"
        "in mediump float positionX;\n"
        "in mediump float positionY;\n"
        "in mediump vec3 value;\n"
        #ifdef MAGNUM_TARGET_GLES2
        "in mediump float instanceId;\n"
        #endif
        "out mediump float valueInterpolated;\n"
        "void main() {\n"
        #ifndef MAGNUM_TARGET_GLES2
        "    valueInterpolated = value[vertexOrDrawIdOrInstanceOffset + gl_InstanceID];\n"
        /* WebGL 1 doesn't allow dynamic indexing into a vec3. Similar thing is
           in MultiDrawShader above or in the SUBSCRIPTING_WORKAROUND in
           Shaders/MeshVisualizer.vert. */
        #elif defined(MAGNUM_TARGET_WEBGL)
        "         if(vertexOrDrawIdOrInstanceOffset + int(instanceId) == 0) valueInterpolated = value.x;\n"
        "    else if(vertexOrDrawIdOrInstanceOffset + int(instanceId) == 1) valueInterpolated = value.y;\n"
        "    else                                                           valueInterpolated = value.z;\n"
        #else
        "    valueInterpolated = value[vertexOrDrawIdOrInstanceOffset + int(instanceId)];\n"
        #endif
        "    gl_PointSize = 1.0;\n"
        "    gl_Position = vec4(positionX, positionY, 0.0, 1.0);\n"
        "}\n");
    frag.addSource(
        "#if defined(GL_ES) && __VERSION__ == 100\n"
        "#define in varying\n"
        "#define result gl_FragColor\n"
        "#endif\n"
        "in mediump float valueInterpolated;\n"
        "#if defined(GL_ES) && __VERSION__ >= 300\n"
        "out mediump vec4 result;\n"
        "#endif\n"
        "void main() { result.r = valueInterpolated; }\n");

    CORRADE_INTERNAL_ASSERT_OUTPUT(vert.compile() && frag.compile());

    attachShaders({vert, frag});

    bindAttributeLocation(PositionX::Location, "positionX");
    bindAttributeLocation(PositionY::Location, "positionY");
    bindAttributeLocation(Value::Location, "value");
    #ifdef MAGNUM_TARGET_GLES2
    bindAttributeLocation(InstanceId::Location, "instanceId");
    #endif

    CORRADE_INTERNAL_ASSERT_OUTPUT(link());
}

void MeshGLTest::multiDrawInstanced() {
    auto&& data = MultiDrawInstancedData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_WEBGL
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ANGLE::multi_draw>())
        CORRADE_SKIP(GL::Extensions::ANGLE::multi_draw::string() << "is not supported.");
    #else
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::WEBGL::multi_draw>())
        CORRADE_SKIP(GL::Extensions::WEBGL::multi_draw::string() << "is not supported.");
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    if(data.vertexId && !GL::Context::current().isExtensionSupported<GL::Extensions::MAGNUM::shader_vertex_id>())
        CORRADE_SKIP("gl_VertexID not supported");
    #endif

    bool hasBaseInstance = data.instanceOffsets[0] || data.instanceOffsets[1];
    if(hasBaseInstance) {
        #ifndef MAGNUM_TARGET_GLES2
        #ifndef MAGNUM_TARGET_WEBGL
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ANGLE::base_vertex_base_instance>())
            CORRADE_SKIP(GL::Extensions::ANGLE::base_vertex_base_instance::string() << "is not supported.");
        #else
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::WEBGL::multi_draw_instanced_base_vertex_base_instance>())
            CORRADE_SKIP(GL::Extensions::WEBGL::multi_draw_instanced_base_vertex_base_instance::string() << "is not supported.");
        /* https://github.com/KhronosGroup/WebGL/pull/3278 :( */
        CORRADE_SKIP(GL::Extensions::WEBGL::multi_draw_instanced_base_vertex_base_instance::string() << "is supported, but recent revisions of the extension don't support gl_BaseVertex / gl_BaseInstance anymore.");
        #endif
        #else
        CORRADE_FAIL("Can't do base instance here.");
        #endif
    }

    const struct {
        Float positionX;
        Vector3 value;
    } vertexData[] {
        {}, /* initial offset */
        {-1.0f/3.0f, data.values[0]},
        { 1.0f/3.0f, data.values[1]},
    };
    const Float instanceData[]{
        0, /* initial offset */
        -1.0f/3.0f,
         1.0f/3.0f
    };

    Mesh mesh{MeshPrimitive::Points};
    mesh.addVertexBuffer(Buffer{vertexData}, sizeof(vertexData[0]), MultiDrawInstancedShader::PositionX{}, MultiDrawInstancedShader::Value{})
        .addVertexBufferInstanced(Buffer{instanceData}, 1, sizeof(instanceData[0]), MultiDrawInstancedShader::PositionY{});

    #ifdef MAGNUM_TARGET_GLES2
    /* Because ANGLE_instanced_arrays on ES2 / WebGL 1 doesn't even provide
       gl_InstanceID ... and there are no integer attributes either */
    const Float instanceId[]{0.0f, 1.0f};
    mesh.addVertexBufferInstanced(Buffer{instanceId}, 1, 0, MultiDrawInstancedShader::InstanceId{});
    #endif

    MAGNUM_VERIFY_NO_GL_ERROR();

    MultiDrawChecker checker;
    MultiDrawInstancedShader{data.vertexId, data.drawId
        #ifndef MAGNUM_TARGET_GLES2
        , hasBaseInstance
        #endif
    }.draw(mesh, data.counts, data.instanceCounts, data.vertexOffsets, nullptr
        #ifndef MAGNUM_TARGET_GLES2
        , hasBaseInstance ? Containers::arrayView(data.instanceOffsets) : nullptr
        #endif
    );
    Vector4 value = checker.get();

    MAGNUM_VERIFY_NO_GL_ERROR();
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_COMPARE_WITH(value, data.expected,
        TestSuite::Compare::around(Vector4{1.0f/255.0f}));
    #else
    CORRADE_COMPARE_WITH(value, data.expected, /* it's only RGBA4 */
        TestSuite::Compare::around(Vector4{1.0f/15.0f}));
    #endif
}

void MeshGLTest::multiDrawInstancedSparseArrays() {
    auto&& data = MultiDrawInstancedData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_WEBGL
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ANGLE::multi_draw>())
        CORRADE_SKIP(GL::Extensions::ANGLE::multi_draw::string() << "is not supported.");
    #else
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::WEBGL::multi_draw>())
        CORRADE_SKIP(GL::Extensions::WEBGL::multi_draw::string() << "is not supported.");
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    if(data.vertexId && !GL::Context::current().isExtensionSupported<GL::Extensions::MAGNUM::shader_vertex_id>())
        CORRADE_SKIP("gl_VertexID not supported");
    #endif

    bool hasBaseInstance = data.instanceOffsets[0] || data.instanceOffsets[1];
    if(hasBaseInstance) {
        #ifndef MAGNUM_TARGET_GLES2
        #ifndef MAGNUM_TARGET_WEBGL
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::ANGLE::base_vertex_base_instance>())
            CORRADE_SKIP(GL::Extensions::ANGLE::base_vertex_base_instance::string() << "is not supported.");
        #else
        if(!GL::Context::current().isExtensionSupported<GL::Extensions::WEBGL::multi_draw_instanced_base_vertex_base_instance>())
            CORRADE_SKIP(GL::Extensions::WEBGL::multi_draw_instanced_base_vertex_base_instance::string() << "is not supported.");
        /* https://github.com/KhronosGroup/WebGL/pull/3278 :( */
        CORRADE_SKIP(GL::Extensions::WEBGL::multi_draw_instanced_base_vertex_base_instance::string() << "is supported, but recent revisions of the extension don't support gl_BaseVertex / gl_BaseInstance anymore.");
        #endif
        #else
        CORRADE_FAIL("Can't do base instance here.");
        #endif
    }

    const struct {
        Float positionX;
        Vector3 value;
    } vertexData[] {
        {}, /* initial offset */
        {-1.0f/3.0f, data.values[0]},
        { 1.0f/3.0f, data.values[1]},
    };
    const Float instanceData[]{
        0, /* initial offset */
        -1.0f/3.0f,
         1.0f/3.0f
    };

    Mesh mesh{MeshPrimitive::Points};
    mesh.addVertexBuffer(Buffer{vertexData}, sizeof(vertexData[0]), MultiDrawInstancedShader::PositionX{}, MultiDrawInstancedShader::Value{})
        .addVertexBufferInstanced(Buffer{instanceData}, 1, sizeof(instanceData[0]), MultiDrawInstancedShader::PositionY{});

    #ifdef MAGNUM_TARGET_GLES2
    /* Because ANGLE_instanced_arrays on ES2 / WebGL 1 doesn't even provide
       gl_InstanceID ... and there are no integer attributes either */
    const Float instanceId[]{0.0f, 1.0f};
    mesh.addVertexBufferInstanced(Buffer{instanceId}, 1, 0, MultiDrawInstancedShader::InstanceId{});
    #endif

    MAGNUM_VERIFY_NO_GL_ERROR();

    /* The signature accepted by glMultiDrawArraysIndirect() */
    struct Command {
        UnsignedInt count;
        UnsignedInt instanceCount;
        UnsignedInt first;
        UnsignedInt baseInstance;
    } commands[] {
        {data.counts[0], data.instanceCounts[0], data.vertexOffsets[0], data.instanceOffsets[0]},
        {data.counts[1], data.instanceCounts[1], data.vertexOffsets[1], data.instanceOffsets[1]}
    };

    MultiDrawChecker checker;
    MultiDrawInstancedShader{data.vertexId, data.drawId
        #ifndef MAGNUM_TARGET_GLES2
        , hasBaseInstance
        #endif
    }.draw(mesh,
        Containers::stridedArrayView(commands).slice(&Command::count),
        Containers::stridedArrayView(commands).slice(&Command::instanceCount),
        Containers::stridedArrayView(commands).slice(&Command::first),
        nullptr
        #ifndef MAGNUM_TARGET_GLES2
        ,
        hasBaseInstance ? Containers::stridedArrayView(commands).slice(&Command::baseInstance) : nullptr
        #endif
    );
    Vector4 value = checker.get();

    MAGNUM_VERIFY_NO_GL_ERROR();
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_COMPARE_WITH(value, data.expected,
        TestSuite::Compare::around(Vector4{1.0f/255.0f}));
    #else
    CORRADE_COMPARE_WITH(value, data.expected, /* it's only RGBA4 */
        TestSuite::Compare::around(Vector4{1.0f/15.0f}));
    #endif
}

template<class T> void MeshGLTest::multiDrawInstancedIndexed() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    auto&& data = MultiDrawInstancedIndexedData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_WEBGL
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ANGLE::multi_draw>())
        CORRADE_SKIP(GL::Extensions::ANGLE::multi_draw::string() << "is not supported.");
    #else
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::WEBGL::multi_draw>())
        CORRADE_SKIP(GL::Extensions::WEBGL::multi_draw::string() << "is not supported.");
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    if(data.vertexId && !GL::Context::current().isExtensionSupported<GL::Extensions::MAGNUM::shader_vertex_id>())
        CORRADE_SKIP("gl_VertexID not supported");
    #endif

    bool hasBaseVertex = data.vertexOffsets[0] || data.vertexOffsets[1];
    bool hasBaseInstance = data.instanceOffsets[0] || data.instanceOffsets[1];
    if(hasBaseVertex || hasBaseInstance) {
        #ifndef MAGNUM_TARGET_GLES2
        #ifndef MAGNUM_TARGET_WEBGL
        if(!Context::current().isExtensionSupported<Extensions::ANGLE::base_vertex_base_instance>())
            CORRADE_SKIP(Extensions::ANGLE::base_vertex_base_instance::string() << "is not supported.");
        #else
        if(!Context::current().isExtensionSupported<Extensions::WEBGL::multi_draw_instanced_base_vertex_base_instance>())
            CORRADE_SKIP(Extensions::WEBGL::multi_draw_instanced_base_vertex_base_instance::string() << "is not supported.");
        /* https://github.com/KhronosGroup/WebGL/pull/3278 :( */
        CORRADE_SKIP(GL::Extensions::WEBGL::multi_draw_instanced_base_vertex_base_instance::string() << "is supported, but recent revisions of the extension don't support gl_BaseVertex / gl_BaseInstance anymore.");
        #endif
        #else
        CORRADE_FAIL("Can't do base vertex or base instance here.");
        #endif
    }

    const struct {
        Float positionX;
        Vector3 value;
    } vertexData[] {
        {}, /* initial offset */
        {-1.0f/3.0f, data.values[0]},
        { 1.0f/3.0f, data.values[1]},
    };
    const Float instanceData[]{
        0, /* initial offset */
        -1.0f/3.0f,
         1.0f/3.0f
    };

    Mesh mesh{MeshPrimitive::Points};
    mesh.addVertexBuffer(Buffer{vertexData}, sizeof(vertexData[0]), MultiDrawInstancedShader::PositionX{}, MultiDrawInstancedShader::Value{})
        .addVertexBufferInstanced(Buffer{instanceData}, 1, sizeof(instanceData[0]), MultiDrawInstancedShader::PositionY{})
        .setIndexBuffer(Buffer{Buffer::TargetHint::ElementArray, data.indices}, 0, MeshIndexType::UnsignedInt);

    #ifdef MAGNUM_TARGET_GLES2
    /* Because ANGLE_instanced_arrays on ES2 / WebGL 1 doesn't even provide
       gl_InstanceID ... and there are no integer attributes either */
    const Float instanceId[]{0.0f, 1.0f};
    mesh.addVertexBufferInstanced(Buffer{instanceId}, 1, 0, MultiDrawInstancedShader::InstanceId{});
    #endif

    MAGNUM_VERIFY_NO_GL_ERROR();

    /* Converted to either a 32bit or 64bit type */
    const T indexOffsetsInBytes[]{
        data.indexOffsetsInBytes[0],
        data.indexOffsetsInBytes[1]
    };

    MultiDrawChecker checker;
    MultiDrawInstancedShader{data.vertexId, false
        #ifndef MAGNUM_TARGET_GLES2
        , hasBaseInstance
        #endif
    }.draw(mesh, data.counts, data.instanceCounts, hasBaseVertex ? Containers::arrayView(data.vertexOffsets) : nullptr, indexOffsetsInBytes
        #ifndef MAGNUM_TARGET_GLES2
        , hasBaseInstance ? Containers::arrayView(data.instanceOffsets) : nullptr
        #endif
    );
    Vector4 value = checker.get();

    MAGNUM_VERIFY_NO_GL_ERROR();
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_COMPARE_WITH(value, data.expected,
        TestSuite::Compare::around(Vector4{1.0f/255.0f}));
    #else
    CORRADE_COMPARE_WITH(value, data.expected, /* it's only RGBA4 */
        TestSuite::Compare::around(Vector4{1.0f/15.0f}));
    #endif
}

template<class T> void MeshGLTest::multiDrawInstancedIndexedSparseArrays() {
    setTestCaseTemplateName(Math::TypeTraits<T>::name());

    auto&& data = MultiDrawInstancedIndexedData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_WEBGL
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::ANGLE::multi_draw>())
        CORRADE_SKIP(GL::Extensions::ANGLE::multi_draw::string() << "is not supported.");
    #else
    if(!GL::Context::current().isExtensionSupported<GL::Extensions::WEBGL::multi_draw>())
        CORRADE_SKIP(GL::Extensions::WEBGL::multi_draw::string() << "is not supported.");
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    if(data.vertexId && !GL::Context::current().isExtensionSupported<GL::Extensions::MAGNUM::shader_vertex_id>())
        CORRADE_SKIP("gl_VertexID not supported");
    #endif

    bool hasBaseVertex = data.vertexOffsets[0] || data.vertexOffsets[1];
    bool hasBaseInstance = data.instanceOffsets[0] || data.instanceOffsets[1];
    if(hasBaseVertex || hasBaseInstance) {
        #ifndef MAGNUM_TARGET_GLES2
        #ifndef MAGNUM_TARGET_WEBGL
        if(!Context::current().isExtensionSupported<Extensions::ANGLE::base_vertex_base_instance>())
            CORRADE_SKIP(Extensions::ANGLE::base_vertex_base_instance::string() << "is not supported.");
        #else
        if(!Context::current().isExtensionSupported<Extensions::WEBGL::multi_draw_instanced_base_vertex_base_instance>())
            CORRADE_SKIP(Extensions::WEBGL::multi_draw_instanced_base_vertex_base_instance::string() << "is not supported.");
        /* https://github.com/KhronosGroup/WebGL/pull/3278 :( */
        CORRADE_SKIP(GL::Extensions::WEBGL::multi_draw_instanced_base_vertex_base_instance::string() << "is supported, but recent revisions of the extension don't support gl_BaseVertex / gl_BaseInstance anymore.");
        #endif
        #else
        CORRADE_FAIL("Can't do base vertex or base instance here.");
        #endif
    }

    const struct {
        Float positionX;
        Vector3 value;
    } vertexData[] {
        {}, /* initial offset */
        {-1.0f/3.0f, data.values[0]},
        { 1.0f/3.0f, data.values[1]},
    };
    const Float instanceData[]{
        0, /* initial offset */
        -1.0f/3.0f,
         1.0f/3.0f
    };

    Mesh mesh{MeshPrimitive::Points};
    mesh.addVertexBuffer(Buffer{vertexData}, sizeof(vertexData[0]), MultiDrawInstancedShader::PositionX{}, MultiDrawInstancedShader::Value{})
        .addVertexBufferInstanced(Buffer{instanceData}, 1, sizeof(instanceData[0]), MultiDrawInstancedShader::PositionY{})
        .setIndexBuffer(Buffer{Buffer::TargetHint::ElementArray, data.indices}, 0, MeshIndexType::UnsignedInt);

    #ifdef MAGNUM_TARGET_GLES2
    /* Because ANGLE_instanced_arrays on ES2 / WebGL 1 doesn't even provide
       gl_InstanceID ... and there are no integer attributes either */
    const Float instanceId[]{0.0f, 1.0f};
    mesh.addVertexBufferInstanced(Buffer{instanceId}, 1, 0, MultiDrawInstancedShader::InstanceId{});
    #endif

    MAGNUM_VERIFY_NO_GL_ERROR();

    /* The signature accepted by glMultiDrawElementsIndirect() EXCEPT that
       here we need firstIndex to be in bytes */
    struct Command {
        UnsignedInt count;
        UnsignedInt instanceCount;
        T firstIndexInBytes; /* !! */
        UnsignedInt baseVertex;
        UnsignedInt baseInstance;
    } commands[] {
        {data.counts[0], data.instanceCounts[0], data.indexOffsetsInBytes[0], data.vertexOffsets[0], data.instanceOffsets[0]},
        {data.counts[1], data.instanceCounts[1], data.indexOffsetsInBytes[1], data.vertexOffsets[1], data.instanceOffsets[1]}
    };

    MultiDrawChecker checker;
    MultiDrawInstancedShader{data.vertexId, false
        #ifndef MAGNUM_TARGET_GLES2
        , hasBaseInstance
        #endif
    }.draw(mesh,
        Containers::stridedArrayView(commands).slice(&Command::count),
        Containers::stridedArrayView(commands).slice(&Command::instanceCount),
        hasBaseVertex ? Containers::stridedArrayView(commands).slice(&Command::baseVertex) : nullptr,
        Containers::stridedArrayView(commands).slice(&Command::firstIndexInBytes)
        #ifndef MAGNUM_TARGET_GLES2
        ,
        hasBaseInstance ? Containers::stridedArrayView(commands).slice(&Command::baseInstance) : nullptr
        #endif
    );
    Vector4 value = checker.get();

    MAGNUM_VERIFY_NO_GL_ERROR();
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_COMPARE_WITH(value, data.expected,
        TestSuite::Compare::around(Vector4{1.0f/255.0f}));
    #else
    CORRADE_COMPARE_WITH(value, data.expected, /* it's only RGBA4 */
        TestSuite::Compare::around(Vector4{1.0f/15.0f}));
    #endif
}

void MeshGLTest::multiDrawInstancedWrongInstanceCountSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Mesh mesh;
    MultiDrawInstancedShader shader;
    UnsignedInt counts[3]{};
    UnsignedInt instanceCounts[2]{};

    Containers::String out;
    Error redirectError{&out};
    shader.draw(mesh, counts, instanceCounts, nullptr, nullptr);
    shader.draw(mesh, counts, nullptr, nullptr, nullptr);
    CORRADE_COMPARE(out,
        "GL::AbstractShaderProgram::draw(): expected 3 instance count items but got 2\n"
        "GL::AbstractShaderProgram::draw(): expected 3 instance count items but got 0\n");
}

void MeshGLTest::multiDrawInstancedWrongVertexOffsetSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Mesh mesh;
    MultiDrawInstancedShader shader;
    UnsignedInt counts[3]{};
    UnsignedInt instanceCounts[3]{};
    UnsignedInt vertexOffsets[2]{};

    Containers::String out;
    Error redirectError{&out};
    shader.draw(mesh, counts, instanceCounts, vertexOffsets, nullptr);
    shader.draw(mesh, counts, instanceCounts, nullptr, nullptr);
    CORRADE_COMPARE(out,
        "GL::AbstractShaderProgram::draw(): expected 3 vertex offset items but got 2\n"
        "GL::AbstractShaderProgram::draw(): expected 3 vertex offset items but got 0\n");
}

#ifndef MAGNUM_TARGET_GLES2
void MeshGLTest::multiDrawInstancedWrongInstanceOffsetSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Mesh mesh;
    MultiDrawInstancedShader shader;
    UnsignedInt counts[3]{};
    UnsignedInt instanceCounts[3]{};
    UnsignedInt vertexOffsets[3]{};
    UnsignedInt instanceOffsets[2]{};

    Containers::String out;
    Error redirectError{&out};
    shader.draw(mesh, counts, instanceCounts, vertexOffsets, nullptr, instanceOffsets);
    /* Omitting vertex offsets altogether is okay */
    CORRADE_COMPARE(out, "GL::AbstractShaderProgram::draw(): expected 3 instance offset items but got 2\n");
}
#endif

void MeshGLTest::multiDrawInstancedIndexedWrongInstanceCountSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Mesh mesh;
    mesh.setIndexBuffer(Buffer{Buffer::TargetHint::ElementArray, {2, 1, 0}}, 0, MeshIndexType::UnsignedInt);
    MultiDrawInstancedShader shader;
    UnsignedInt counts[3]{};
    UnsignedInt instanceCounts[2]{};

    Containers::String out;
    Error redirectError{&out};
    shader.draw(mesh, counts, instanceCounts, nullptr, nullptr);
    shader.draw(mesh, counts, nullptr, nullptr, nullptr);
    CORRADE_COMPARE(out,
        "GL::AbstractShaderProgram::draw(): expected 3 instance count items but got 2\n"
        "GL::AbstractShaderProgram::draw(): expected 3 instance count items but got 0\n");
}

void MeshGLTest::multiDrawInstancedIndexedWrongVertexOffsetSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Mesh mesh;
    mesh.setIndexBuffer(Buffer{Buffer::TargetHint::ElementArray, {2, 1, 0}}, 0, MeshIndexType::UnsignedInt);
    MultiDrawInstancedShader shader;
    UnsignedInt counts[3]{};
    UnsignedInt instanceCounts[3]{};
    UnsignedInt vertexOffsets[2]{};
    UnsignedInt indexOffsets[3]{};

    Containers::String out;
    Error redirectError{&out};
    shader.draw(mesh, counts, instanceCounts, vertexOffsets, indexOffsets);
    /* Omitting vertex offsets altogether is okay */
    CORRADE_COMPARE(out,
        "GL::AbstractShaderProgram::draw(): expected 3 vertex offset items but got 2\n");
}

void MeshGLTest::multiDrawInstancedIndexedWrongIndexOffsetSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Mesh mesh;
    mesh.setIndexBuffer(Buffer{Buffer::TargetHint::ElementArray, {2, 1, 0}}, 0, MeshIndexType::UnsignedInt);
    MultiDrawInstancedShader shader;
    UnsignedInt counts[3]{};
    UnsignedInt instanceCounts[3]{};
    UnsignedInt indexOffsets[2]{};

    Containers::String out;
    Error redirectError{&out};
    shader.draw(mesh, counts, instanceCounts, nullptr, indexOffsets);
    shader.draw(mesh, counts, instanceCounts, nullptr, nullptr);
    CORRADE_COMPARE(out,
        "GL::AbstractShaderProgram::draw(): expected 3 index offset items but got 2\n"
        "GL::AbstractShaderProgram::draw(): expected 3 index offset items but got 0\n");
}

#ifndef MAGNUM_TARGET_GLES2
void MeshGLTest::multiDrawInstancedIndexedWrongInstanceOffsetSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Mesh mesh;
    mesh.setIndexBuffer(Buffer{Buffer::TargetHint::ElementArray, {2, 1, 0}}, 0, MeshIndexType::UnsignedInt);
    MultiDrawInstancedShader shader;
    UnsignedInt counts[3]{};
    UnsignedInt instanceCounts[3]{};
    UnsignedInt indexOffsets[3]{};
    UnsignedInt instanceOffsets[2]{};

    Containers::String out;
    Error redirectError{&out};
    shader.draw(mesh, counts, instanceCounts, nullptr, indexOffsets, instanceOffsets);
    /* Omitting instance offsets altogether is okay */
    CORRADE_COMPARE(out, "GL::AbstractShaderProgram::draw(): expected 3 instance offset items but got 2\n");
}

void MeshGLTest::multiDrawInstancedBaseVertexNoExtensionAvailable() {
    /* The top-level multidraw extension isn't guarded (the user is expected to
       do so), but the base vertex is as the conditions are more complex */
    #ifndef MAGNUM_TARGET_WEBGL
    if(!Context::current().isExtensionSupported<Extensions::ANGLE::multi_draw>())
        CORRADE_SKIP(Extensions::ANGLE::multi_draw::string() << "is not supported.");
    if(Context::current().isExtensionSupported<Extensions::ANGLE::base_vertex_base_instance>())
        CORRADE_SKIP(Extensions::ANGLE::base_vertex_base_instance::string() << "is supported.");
    #else
    if(!Context::current().isExtensionSupported<Extensions::WEBGL::multi_draw>())
        CORRADE_SKIP(Extensions::WEBGL::multi_draw::string() << "is not supported.");
    if(Context::current().isExtensionSupported<Extensions::WEBGL::multi_draw_instanced_base_vertex_base_instance>())
        CORRADE_SKIP(Extensions::WEBGL::multi_draw_instanced_base_vertex_base_instance::string() << "is supported.");
    #endif

    Mesh mesh;
    mesh.setIndexBuffer(Buffer{Buffer::TargetHint::ElementArray, {2, 1, 0}}, 0, MeshIndexType::UnsignedInt);

    UnsignedInt counts[]{3};
    UnsignedInt instanceCounts[]{3};
    UnsignedInt vertexOffsets[]{0};
    UnsignedInt indexOffsets[]{0};

    Containers::String out;
    Error redirectError{&out};
    MultiDrawInstancedShader{}.draw(mesh, counts, instanceCounts, vertexOffsets, indexOffsets);
    #ifndef MAGNUM_TARGET_GLES2
    CORRADE_COMPARE(out, "GL::AbstractShaderProgram::draw(): no extension available for instanced indexed mesh multi-draw with base vertex and base instance specification\n");
    #else
    CORRADE_COMPARE(out, "GL::AbstractShaderProgram::draw(): instanced indexed mesh multi-draw with base vertex specification possible only since OpenGL ES 3.0 and WebGL 2.0\n");
    #endif
}

void MeshGLTest::multiDrawInstancedBaseInstanceNoExtensionAvailable() {
    /* The top-level multidraw extension isn't guarded (the user is expected to
       do so), but the base vertex is as the conditions are more complex */
    #ifndef MAGNUM_TARGET_WEBGL
    if(!Context::current().isExtensionSupported<Extensions::ANGLE::multi_draw>())
        CORRADE_SKIP(Extensions::ANGLE::multi_draw::string() << "is not supported.");
    if(Context::current().isExtensionSupported<Extensions::ANGLE::base_vertex_base_instance>())
        CORRADE_SKIP(Extensions::ANGLE::base_vertex_base_instance::string() << "is supported.");
    #else
    if(!Context::current().isExtensionSupported<Extensions::WEBGL::multi_draw>())
        CORRADE_SKIP(Extensions::WEBGL::multi_draw::string() << "is not supported.");
    if(Context::current().isExtensionSupported<Extensions::WEBGL::multi_draw_instanced_base_vertex_base_instance>())
        CORRADE_SKIP(Extensions::WEBGL::multi_draw_instanced_base_vertex_base_instance::string() << "is supported.");
    #endif

    Mesh nonIndexed;
    Mesh indexed;
    indexed.setIndexBuffer(Buffer{Buffer::TargetHint::ElementArray, {2, 1, 0}}, 0, MeshIndexType::UnsignedInt);

    UnsignedInt counts[]{3};
    UnsignedInt instanceCounts[]{3};
    UnsignedInt vertexOffsets[]{0};
    UnsignedInt indexOffsets[]{0};
    UnsignedInt instanceOffsets[]{0};

    Containers::String out;
    Error redirectError{&out};
    MultiDrawInstancedShader{}.draw(nonIndexed, counts, instanceCounts, vertexOffsets, nullptr, instanceOffsets);
    MultiDrawInstancedShader{}.draw(indexed, counts, instanceCounts, nullptr, indexOffsets, instanceOffsets);
    CORRADE_COMPARE(out,
        "GL::AbstractShaderProgram::draw(): no extension available for instanced mesh multi-draw with base instance specification\n"
        "GL::AbstractShaderProgram::draw(): no extension available for instanced indexed mesh multi-draw with base vertex and base instance specification\n");
}
#endif
#endif

}}}}

CORRADE_TEST_MAIN(Magnum::GL::Test::MeshGLTest)
