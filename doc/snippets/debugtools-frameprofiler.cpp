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

#include <sstream>
#include <Corrade/Utility/DebugStl.h>

#include "Magnum/DebugTools/FrameProfiler.h"

/* Hacking around the fugly windowlessapp setup by including OpenGLTester */
#include "Magnum/GL/OpenGLTester.h"

using namespace Magnum;

class FrameProfiler: public Platform::WindowlessApplication {
    public:
        explicit FrameProfiler(const Arguments& arguments);

        int exec() override { return 0; }
};

FrameProfiler::FrameProfiler(const Arguments& arguments): Platform::WindowlessApplication{arguments} {
    /* Enable everything in the GL profiler and then introspect it to fake
       its output 1:1 */
    DebugTools::GLFrameProfiler glProfiler{
        DebugTools::GLFrameProfiler::Value::FrameTime|
        DebugTools::GLFrameProfiler::Value::CpuDuration|
        DebugTools::GLFrameProfiler::Value::GpuDuration|
        DebugTools::GLFrameProfiler::Value::VertexFetchRatio|
        DebugTools::GLFrameProfiler::Value::PrimitiveClipRatio
    , 50};

    DebugTools::FrameProfiler profiler{{
        DebugTools::FrameProfiler::Measurement{
            glProfiler.measurementName(0),
            glProfiler.measurementUnits(0),
            glProfiler.measurementDelay(2),
            [](void*, UnsignedInt) {},
            [](void*, UnsignedInt) {},
            [](void*, UnsignedInt, UnsignedInt) {
                return UnsignedLong{16651567};
            }, nullptr},
        DebugTools::FrameProfiler::Measurement{
            glProfiler.measurementName(1),
            glProfiler.measurementUnits(1),
            glProfiler.measurementDelay(2),
            [](void*, UnsignedInt) {},
            [](void*, UnsignedInt) {},
            [](void*, UnsignedInt, UnsignedInt) {
                return UnsignedLong{14720000};
            }, nullptr},
        DebugTools::FrameProfiler::Measurement{
            glProfiler.measurementName(2),
            glProfiler.measurementUnits(2),
            glProfiler.measurementDelay(2),
            [](void*, UnsignedInt) {},
            [](void*, UnsignedInt) {},
            [](void*, UnsignedInt, UnsignedInt) {
                return UnsignedLong{10890000};
            }, nullptr},
        DebugTools::FrameProfiler::Measurement{
            glProfiler.measurementName(3),
            glProfiler.measurementUnits(3),
            glProfiler.measurementDelay(3),
            [](void*, UnsignedInt) {},
            [](void*, UnsignedInt) {},
            [](void*, UnsignedInt, UnsignedInt) {
                return UnsignedLong{240};
            }, nullptr},
        DebugTools::FrameProfiler::Measurement{
            glProfiler.measurementName(4),
            glProfiler.measurementUnits(4),
            glProfiler.measurementDelay(4),
            [](void*, UnsignedInt) {},
            [](void*, UnsignedInt) {},
            [](void*, UnsignedInt, UnsignedInt) {
                return UnsignedLong{59670};
            }, nullptr},
    }, 50};

    for(std::size_t i = 0; i != 100; ++i) {
        profiler.beginFrame();
        profiler.endFrame();
    }

    std::ostringstream out; /* we don't want a TTY */
    profiler.printStatistics(Debug{&out}, 1);
    Debug{Debug::Flag::NoNewlineAtTheEnd} << out.str();
}

MAGNUM_WINDOWLESSAPPLICATION_MAIN(FrameProfiler)
