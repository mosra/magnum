#ifndef Magnum_Trade_Trade_h
#define Magnum_Trade_Trade_h
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
 * @brief Forward declarations for the @ref Magnum::Trade namespace
 */

#include <Corrade/Containers/Containers.h>

#include "Magnum/Magnum.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <Corrade/Utility/Macros.h>
#endif

namespace Magnum { namespace Trade {

#ifndef DOXYGEN_GENERATING_OUTPUT
class AbstractImageConverter;
class AbstractImporter;
class AbstractSceneConverter;

#ifdef MAGNUM_BUILD_DEPRECATED
typedef CORRADE_DEPRECATED("use InputFileCallbackPolicy instead") InputFileCallbackPolicy ImporterFileCallbackPolicy;
#endif

enum class MaterialAttribute: UnsignedInt;
enum class MaterialTextureSwizzle: UnsignedInt;
enum class MaterialAttributeType: UnsignedByte;
enum class MaterialLayer: UnsignedInt;
enum class MaterialType: UnsignedInt;
enum class MaterialAlphaMode: UnsignedByte;
class MaterialAttributeData;
class MaterialData;
#ifdef MAGNUM_BUILD_DEPRECATED
typedef CORRADE_DEPRECATED("use MaterialData instead") MaterialData AbstractMaterialData;
#endif

enum class AnimationTrackTargetType: UnsignedByte;
enum class AnimationTrackType: UnsignedByte;
class AnimationTrackData;
class AnimationData;

enum class CameraType: UnsignedByte;
class CameraData;

enum class DataFlag: UnsignedByte;
typedef Containers::EnumSet<DataFlag> DataFlags;

class FlatMaterialData;

template<UnsignedInt> class ImageData;
typedef ImageData<1> ImageData1D;
typedef ImageData<2> ImageData2D;
typedef ImageData<3> ImageData3D;

class LightData;

enum class MeshAttribute: UnsignedShort;
class MeshIndexData;
class MeshAttributeData;
class MeshData;

#ifdef MAGNUM_BUILD_DEPRECATED
class CORRADE_DEPRECATED("use MeshData instead") MeshData2D;
class CORRADE_DEPRECATED("use MeshData instead") MeshData3D;
#endif
class MeshObjectData2D;
class MeshObjectData3D;
class ObjectData2D;
class ObjectData3D;
class PbrClearCoatMaterialData;
class PbrMetallicRoughnessMaterialData;
class PbrSpecularGlossinessMaterialData;
class PhongMaterialData;
class TextureData;
class SceneData;

template<UnsignedInt> class SkinData;
typedef SkinData<2> SkinData2D;
typedef SkinData<3> SkinData3D;
#endif

}}

#endif
