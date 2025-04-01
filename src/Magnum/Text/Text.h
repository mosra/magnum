#ifndef Magnum_Text_Text_h
#define Magnum_Text_Text_h
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

/** @file
 * @brief Forward declarations for the @ref Magnum::Text namespace
 */

#include "Magnum/Types.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <Corrade/Utility/Macros.h>
#endif

#ifndef DOXYGEN_GENERATING_OUTPUT
namespace Magnum { namespace Text {

class AbstractFont;
class AbstractFontConverter;
class AbstractGlyphCache;
class AbstractLayouter;
class AbstractShaper;

enum class Alignment: UnsignedByte;
enum class ShapeDirection: UnsignedByte;
enum class LayoutDirection: UnsignedByte;

class AbstractGlyphCache;

enum class Feature: UnsignedInt;
enum class Script: UnsignedInt;

class FeatureRange;

class RendererCore;
class Renderer;

#ifdef MAGNUM_TARGET_GL
class DistanceFieldGlyphCacheGL;
class GlyphCacheGL;
#ifdef MAGNUM_BUILD_DEPRECATED
typedef CORRADE_DEPRECATED("use DistanceFieldGlyphCacheGL instead") DistanceFieldGlyphCacheGL DistanceFieldGlyphCache;
typedef CORRADE_DEPRECATED("use GlyphCacheGL instead") GlyphCacheGL GlyphCache;
#endif
#ifndef MAGNUM_TARGET_GLES2
class GlyphCacheArrayGL;
#endif
class RendererGL;
#ifdef MAGNUM_BUILD_DEPRECATED
class CORRADE_DEPRECATED("use Renderer or RendererGL instead") AbstractRenderer;
CORRADE_IGNORE_DEPRECATED_PUSH /* idiotic MSVC warns for deprecated APIs using deprecated APIs */
typedef CORRADE_DEPRECATED("use Renderer or RendererGL instead") AbstractRenderer Renderer2D;
typedef CORRADE_DEPRECATED("use Renderer or RendererGL instead") AbstractRenderer Renderer3D;
CORRADE_IGNORE_DEPRECATED_POP
#endif
#endif

}}
#endif

#endif
