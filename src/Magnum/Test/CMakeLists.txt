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

corrade_add_test(ArrayTest ArrayTest.cpp LIBRARIES Magnum)
corrade_add_test(FileCallbackTest FileCallbackTest.cpp LIBRARIES Magnum)
corrade_add_test(ImageTest ImageTest.cpp LIBRARIES MagnumTestLib)
corrade_add_test(ImageViewTest ImageViewTest.cpp LIBRARIES MagnumTestLib)
corrade_add_test(MeshTest MeshTest.cpp LIBRARIES MagnumTestLib)
corrade_add_test(PixelFormatTest PixelFormatTest.cpp LIBRARIES MagnumTestLib)
corrade_add_test(PixelStorageTest PixelStorageTest.cpp LIBRARIES Magnum)
corrade_add_test(ResourceManagerTest ResourceManagerTest.cpp LIBRARIES Magnum)
corrade_add_test(SamplerTest SamplerTest.cpp LIBRARIES MagnumTestLib)
corrade_add_test(TagsTest TagsTest.cpp LIBRARIES Magnum)
corrade_add_test(VersionTest VersionTest.cpp LIBRARIES Magnum)
corrade_add_test(VertexFormatTest VertexFormatTest.cpp LIBRARIES MagnumTestLib)

set_target_properties(
    ArrayTest
    ImageTest
    ImageViewTest
    MeshTest
    PixelFormatTest
    PixelStorageTest
    ResourceManagerTest
    SamplerTest
    TagsTest
    VertexFormatTest
    PROPERTIES FOLDER "Magnum/Test")

set_property(TARGET
    MeshTest
    PixelFormatTest
    ResourceManagerTest
    VertexFormatTest
    APPEND PROPERTY COMPILE_DEFINITIONS "CORRADE_GRACEFUL_ASSERT")
