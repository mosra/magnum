#ifndef Magnum_DebugTools_ColorMap_h
#define Magnum_DebugTools_ColorMap_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
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
 * @brief Namespace @ref Magnum::DebugTools::ColorMap
 * @m_since_latest
 */

#include "Magnum/Magnum.h"
#include "Magnum/DebugTools/visibility.h"

namespace Magnum { namespace DebugTools {

/**
@brief Color maps
@m_since_latest

A collection of predefined color maps for visualization of gradient data.

This library is built if `WITH_DEBUGTOOLS` is enabled when building Magnum. To
use this library with CMake, request the `DebugTools` component of the `Magnum`
package and link to the `Magnum::DebugTools` target:

@code{.cmake}
find_package(Magnum REQUIRED DebugTools)

# ...
target_link_libraries(your-app PRIVATE Magnum::DebugTools)
@endcode

See @ref building, @ref cmake and @ref debug-tools for more information.

@m_class{m-row}

@parblock

@m_div{m-col-l-8 m-push-l-3 m-col-m-8 m-push-m-4 m-nopadt}
@image html colormap-turbo.png width=100%
@m_enddiv

@m_div{m-col-l-2 m-pull-l-7 m-col-m-4 m-pull-m-8 m-text-center m-nopadt} @ref turbo() @m_enddiv

@endparblock

@m_class{m-row}

@parblock

@m_div{m-col-l-8 m-push-l-3 m-col-m-8 m-push-m-4 m-nopadt}
@image html colormap-magma.png width=100%
@m_enddiv

@m_div{m-col-l-2 m-pull-l-7 m-col-m-4 m-pull-m-8 m-text-center m-nopadt} @ref magma() @m_enddiv

@endparblock

@m_class{m-row}

@parblock

@m_div{m-col-l-8 m-push-l-3 m-col-m-8 m-push-m-4 m-nopadt}
@image html colormap-plasma.png width=100%
@m_enddiv

@m_div{m-col-l-2 m-pull-l-7 m-col-m-4 m-pull-m-8 m-text-center m-nopadt} @ref plasma() @m_enddiv

@endparblock

@m_class{m-row}

@parblock

@m_div{m-col-l-8 m-push-l-3 m-col-m-8 m-push-m-4 m-nopadt}
@image html colormap-inferno.png width=100%
@m_enddiv

@m_div{m-col-l-2 m-pull-l-7 m-col-m-4 m-pull-m-8 m-text-center m-nopadt} @ref inferno() @m_enddiv

@endparblock

@m_class{m-row}

@parblock

@m_div{m-col-l-8 m-push-l-3 m-col-m-8 m-push-m-4 m-nopadt}
@image html colormap-viridis.png width=100%
@m_enddiv

@m_div{m-col-l-2 m-pull-l-7 m-col-m-4 m-pull-m-8 m-text-center m-nopadt} @ref viridis() @m_enddiv

@endparblock

For all color maps the returned data is the sRGB colorspace. Desired usage is
by uploading to a texture with linear filtering, depending on the use case with
either clamp or repeat wrapping. For a sRGB workflow don't forget to set the
texture format to sRGB, to ensure the values are interpreted and interpolated
done correctly.

@snippet MagnumDebugTools-gl.cpp ColorMap

*/
namespace ColorMap {

/**
@brief Turbo colormap
@m_since_latest

@image html colormap-turbo.png width=100%

@m_class{m-block m-success}

@thirdparty Data from [Turbo, An Improved Rainbow Colormap for Visualization](https://ai.googleblog.com/2019/08/turbo-improved-rainbow-colormap-for.html)
    by Anton Mikhailov, released under an
    @m_class{m-label m-success} **Apache-2.0** license
    ([choosealicense.com](https://choosealicense.com/licenses/apache-2.0/)).
*/
MAGNUM_DEBUGTOOLS_EXPORT Containers::StaticArrayView<256, const Vector3ub> turbo();

/**
@brief Magma colormap
@m_since_latest

@image html colormap-magma.png width=100%

@m_class{m-block m-primary}

@thirdparty Created by [Stéfan van der Walt](https://github.com/stefanv) and
    [Nathaniel Smith](https://github.com/njsmith) for the matplotlib library
    ([source](http://bids.github.io/colormap/)), released under a
    @m_class{m-label m-primary} **CC0** license
    ([choosealicense.com](https://choosealicense.com/licenses/cc0-1.0/)).
*/
MAGNUM_DEBUGTOOLS_EXPORT Containers::StaticArrayView<256, const Vector3ub> magma();

/**
@brief Plasma colormap
@m_since_latest

@image html colormap-plasma.png width=100%

@m_class{m-block m-primary}

@thirdparty Created by [Stéfan van der Walt](https://github.com/stefanv) and
    [Nathaniel Smith](https://github.com/njsmith) for the matplotlib library
    ([source](http://bids.github.io/colormap/)), released under a
    @m_class{m-label m-primary} **CC0** license
    ([choosealicense.com](https://choosealicense.com/licenses/cc0-1.0/)).
*/
MAGNUM_DEBUGTOOLS_EXPORT Containers::StaticArrayView<256, const Vector3ub> plasma();

/**
@brief Inferno colormap
@m_since_latest

@image html colormap-inferno.png width=100%

@m_class{m-block m-primary}

@thirdparty Created by [Stéfan van der Walt](https://github.com/stefanv) and
    [Nathaniel Smith](https://github.com/njsmith) for the matplotlib library
    ([source](http://bids.github.io/colormap/)), released under a
    @m_class{m-label m-primary} **CC0** license
    ([choosealicense.com](https://choosealicense.com/licenses/cc0-1.0/)).
*/
MAGNUM_DEBUGTOOLS_EXPORT Containers::StaticArrayView<256, const Vector3ub> inferno();

/**
@brief Viridis colormap
@m_since_latest

@image html colormap-viridis.png width=100%

@m_class{m-block m-primary}

@thirdparty Created by [Stéfan van der Walt](https://github.com/stefanv) and
    [Nathaniel Smith](https://github.com/njsmith) for the matplotlib library
    based on a design by Eric Firing ([source](http://bids.github.io/colormap/)),
    released under a @m_class{m-label m-primary} **CC0** license
    ([choosealicense.com](https://choosealicense.com/licenses/cc0-1.0/)).
*/
MAGNUM_DEBUGTOOLS_EXPORT Containers::StaticArrayView<256, const Vector3ub> viridis();

}

}}

#endif
