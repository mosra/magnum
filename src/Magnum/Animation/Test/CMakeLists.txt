#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
#               2020 Vladimír Vondruš <mosra@centrum.cz>
#
#   Permission is hereby granted, free of charge, to any person obtaining a
#   copy of this software and associated documentation files (the "Software"),
#   to deal in the Software without restriction, including without limitation
#   the rights to use, copy, modify, merge, publish, distribute, sublicense,
#   and/or sell copies of the Software, and to permit persons to whom the
#   Software is furnished to do so, subject to the following conditions:
#
#   The above copyright notice and this permission notice shall be included
#   in all copies or substantial portions of the Software.
#
#   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
#   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
#   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
#   DEALINGS IN THE SOFTWARE.
#

corrade_add_test(AnimationBenchmark Benchmark.cpp LIBRARIES Magnum)
corrade_add_test(AnimationEasingTest EasingTest.cpp LIBRARIES Magnum)
corrade_add_test(AnimationInterpolationTest InterpolationTest.cpp LIBRARIES MagnumTestLib)
corrade_add_test(AnimationPlayerTest PlayerTest.cpp LIBRARIES MagnumTestLib)
corrade_add_test(AnimationPlayerCustomTest PlayerCustomTest.cpp LIBRARIES MagnumTestLib)
corrade_add_test(AnimationTrackTest TrackTest.cpp LIBRARIES Magnum)
corrade_add_test(AnimationTrackViewTest TrackViewTest.cpp LIBRARIES Magnum)

set_property(TARGET
    AnimationInterpolationTest
    APPEND PROPERTY COMPILE_DEFINITIONS "CORRADE_GRACEFUL_ASSERT")

set_target_properties(
    AnimationBenchmark
    AnimationEasingTest
    AnimationInterpolationTest
    AnimationPlayerTest
    AnimationPlayerCustomTest
    AnimationTrackTest
    AnimationTrackViewTest
    PROPERTIES FOLDER "Magnum/Animation/Test")
