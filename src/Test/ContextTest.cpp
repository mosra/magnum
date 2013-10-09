/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

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

#include "Test/AbstractOpenGLTester.h"
#include "Context.h"
#include <Extensions.h>

namespace Magnum { namespace Test {

class ContextTest: public AbstractOpenGLTester {
    public:
        explicit ContextTest();

        void version();
        void versionList();
        void supportedExtension();
        void unsupportedExtension();
        void pastExtension();
        void versionDependentExtension();
};

ContextTest::ContextTest() {
    addTests({&ContextTest::version,
              &ContextTest::versionList,
              &ContextTest::supportedExtension,
              &ContextTest::unsupportedExtension,
              &ContextTest::pastExtension,
              &ContextTest::versionDependentExtension});
}

void ContextTest::version() {
    const Version v = Context::current()->version();
    CORRADE_VERIFY(Context::current()->isVersionSupported(v));
    CORRADE_VERIFY(Context::current()->isVersionSupported(Version(Int(v)-1)));
    CORRADE_VERIFY(!Context::current()->isVersionSupported(Version(Int(v)+1)));

    /* No assertions should be fired */
    MAGNUM_ASSERT_VERSION_SUPPORTED(v);
    MAGNUM_ASSERT_VERSION_SUPPORTED(Version(Int(v)-1));
}

void ContextTest::versionList() {
    const Version v = Context::current()->version();

    /* Selects first supported version (thus not necessarily the highest) */
    CORRADE_VERIFY(Context::current()->supportedVersion({Version(Int(v)+1), v, Version(Int(v)-1)}) == v);
    CORRADE_VERIFY(Context::current()->supportedVersion({Version(Int(v)+1), Version(Int(v)-1), v}) == Version(Int(v)-1));
}

void ContextTest::supportedExtension() {
    if(!Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_filter_anisotropic>())
        CORRADE_SKIP(Extensions::GL::EXT::texture_filter_anisotropic::string() + std::string(" extension should be supported, can't test"));

    std::string extensions(reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS)));
    CORRADE_VERIFY(extensions.find(Extensions::GL::EXT::texture_filter_anisotropic::string()) != std::string::npos);
}

void ContextTest::unsupportedExtension() {
    #ifndef MAGNUM_TARGET_GLES
    if(Context::current()->isExtensionSupported<Extensions::GL::GREMEDY::string_marker>())
        CORRADE_SKIP(Extensions::GL::GREMEDY::string_marker::string() + std::string(" extension shouldn't be supported, can't test"));

    std::string extensions(reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS)));
    CORRADE_VERIFY(extensions.find(Extensions::GL::GREMEDY::string_marker::string()) == std::string::npos);
    #elif !defined(CORRADE_TARGET_NACL)
    if(Context::current()->isExtensionSupported<Extensions::GL::CHROMIUM::map_sub>())
        CORRADE_SKIP(Extensions::GL::CHROMIUM::map_sub::string() + std::string(" extension shouldn't be supported, can't test"));

    std::string extensions(reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS)));
    CORRADE_VERIFY(extensions.find(Extensions::GL::CHROMIUM::map_sub::string()) == std::string::npos);
    #else
    if(Context::current()->isExtensionSupported<Extensions::GL::NV::read_buffer_front>())
        CORRADE_SKIP(Extensions::GL::NV::read_buffer_front::string() + std::string(" extension shouldn't be supported, can't test"));

    std::string extensions(reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS)));
    CORRADE_VERIFY(extensions.find(Extensions::GL::NV::read_buffer_front::string()) == std::string::npos);
    #endif
}

void ContextTest::pastExtension() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current()->isVersionSupported(Version::GL300))
        CORRADE_SKIP("No already supported extensions exist in OpenGL 2.1");

    CORRADE_VERIFY(Context::current()->isExtensionSupported<Extensions::GL::APPLE::vertex_array_object>());
    /* No assertion should be fired */
    MAGNUM_ASSERT_EXTENSION_SUPPORTED(Extensions::GL::APPLE::vertex_array_object);
    #else
    if(!Context::current()->isVersionSupported(Version::GLES300))
        CORRADE_SKIP("No already supported extensions exist in OpenGL ES 2.0");

    CORRADE_VERIFY(Context::current()->isExtensionSupported<Extensions::GL::EXT::texture_rg>());
    /* No assertion should be fired */
    MAGNUM_ASSERT_EXTENSION_SUPPORTED(Extensions::GL::EXT::texture_rg);
    #endif
}

void ContextTest::versionDependentExtension() {
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(Extensions::GL::ARB::get_program_binary::requiredVersion(), Version::GL300);
    if(!Context::current()->isExtensionSupported<Extensions::GL::ARB::get_program_binary>())
        CORRADE_SKIP(Extensions::GL::ARB::get_program_binary::string() + std::string("extension isn't supported, can't test"));

    CORRADE_VERIFY(Context::current()->isExtensionSupported<Extensions::GL::ARB::get_program_binary>(Context::current()->version()));
    CORRADE_VERIFY(!Context::current()->isExtensionSupported<Extensions::GL::ARB::get_program_binary>(Version::GL210));
    #else
    CORRADE_SKIP("No OpenGL ES 3.0-only extensions exist yet");
    #endif
}

}}

CORRADE_TEST_MAIN(Magnum::Test::ContextTest)
