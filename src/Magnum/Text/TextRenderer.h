#ifndef Magnum_Text_TextRenderer_h
#define Magnum_Text_TextRenderer_h

#include "Magnum/Text/Renderer.h"

#ifdef MAGNUM_BUILD_DEPRECATED
namespace Magnum { namespace Text {

/**
@copybrief Renderer2D
@deprecated Use @ref Magnum::Text::Renderer2D "Renderer2D" instead.
*/
typedef CORRADE_DEPRECATED("use Renderer2D instead") Renderer<2> TextRenderer2D;

/**
@copybrief Renderer3D
@deprecated Use @ref Magnum::Text::Renderer3D "Renderer3D" instead.
*/
typedef CORRADE_DEPRECATED("use Renderer2D instead") Renderer<3> TextRenderer3D;

}}
#else
#error
#endif

#endif
