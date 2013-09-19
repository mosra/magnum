#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>
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

corrade_add_test(AbstractImageTest AbstractImageTest.cpp LIBRARIES Magnum)
corrade_add_test(AbstractShaderProgramTest AbstractShaderProgramTest.cpp LIBRARIES Magnum)
corrade_add_test(ArrayTest ArrayTest.cpp)
corrade_add_test(ColorTest ColorTest.cpp LIBRARIES MagnumMathTestLib)
corrade_add_test(DefaultFramebufferTest DefaultFramebufferTest.cpp LIBRARIES Magnum)
corrade_add_test(FramebufferTest FramebufferTest.cpp LIBRARIES Magnum)
corrade_add_test(ImageTest ImageTest.cpp LIBRARIES Magnum)
corrade_add_test(MeshTest MeshTest.cpp LIBRARIES Magnum)
corrade_add_test(RendererTest RendererTest.cpp LIBRARIES Magnum)
corrade_add_test(ResourceManagerTest ResourceManagerTest.cpp LIBRARIES MagnumTestLib)
corrade_add_test(SamplerTest SamplerTest.cpp LIBRARIES Magnum)
corrade_add_test(SwizzleTest SwizzleTest.cpp LIBRARIES MagnumMathTestLib)

if(BUILD_GL_TESTS)
    corrade_add_test(BufferGLTest BufferGLTest.cpp LIBRARIES ${GL_TEST_LIBRARIES})
    corrade_add_test(ContextTest ContextTest.cpp LIBRARIES ${GL_TEST_LIBRARIES})
endif()

set_target_properties(ResourceManagerTest PROPERTIES COMPILE_FLAGS -DCORRADE_GRACEFUL_ASSERT)

# Install bootstrap header for GL tests to be used in dependent projects
install(FILES AbstractOpenGLTester.h DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/Test)
