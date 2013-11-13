#ifndef Magnum_Text_TextRenderer_h
#define Magnum_Text_TextRenderer_h

#include "Text/Renderer.h"

namespace Magnum { namespace Text {

#ifdef MAGNUM_BUILD_DEPRECATED
/**
@copybrief Renderer2D
@deprecated Use @ref Magnum::Text::Renderer2D "Renderer2D" instead.
*/
typedef Renderer<2> TextRenderer2D;

/**
@copybrief Renderer3D
@deprecated Use @ref Magnum::Text::Renderer3D "Renderer3D" instead.
*/
typedef Renderer<3> TextRenderer3D;
#else
#error
#endif

}}

#endif
