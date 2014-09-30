/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014
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

#include "Magnum/Context.h"
#include "Magnum/Math/Range.h"

namespace Magnum {

auto Context::detectedDriver() -> DetectedDrivers {
    if(_detectedDrivers) return *_detectedDrivers;

    _detectedDrivers = DetectedDrivers{};

    /* AMD binary desktop drivers */
    #ifndef MAGNUM_TARGET_GLES
    const std::string vendor = vendorString();
    if(vendor.find("ATI Technologies Inc.") != std::string::npos)
        return *_detectedDrivers |= DetectedDriver::AMD;
    #endif

    /* OpenGL ES 2.0 implementation using ANGLE. Taken from
       http://stackoverflow.com/a/20149090 */
    #ifdef MAGNUM_TARGET_GLES2
    {
        Range1Di range;
        glGetIntegerv(GL_ALIASED_LINE_WIDTH_RANGE, range.data());
        if(range.min() == 1 && range.max() == 1)
            return *_detectedDrivers |= DetectedDriver::ProbablyAngle;
    }
    #endif

    return *_detectedDrivers;
}

}
