/*
    Copyright © 2010, 2011, 2012 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

#include <Utility/Debug.h>

#include "Context.h"
#include "Platform/WindowlessGlxApplication.h"

namespace Magnum {

class MagnumInfo: public Platform::WindowlessGlxApplication {
    public:
        MagnumInfo(int& argc, char** argv);

        inline int exec() override { return 0; }
};

MagnumInfo::MagnumInfo(int& argc, char** argv): WindowlessGlxApplication(argc, argv) {
    Context* c = Context::current();

    Debug() << "";
    Debug() << "  +---------------------------------------------------------+";
    Debug() << "  | Information about Magnum engine and OpenGL capabilities |";
    Debug() << "  +---------------------------------------------------------+";
    Debug() << "";

    Debug() << "Used application: Platform::WindowlessGlxApplication";
    {
        Debug d;
        d << "Compilation flags:";
        #ifdef MAGNUM_TARGET_GLES
        d << "MAGNUM_TARGET_GLES";
        #endif
        #ifdef MAGNUM_TARGET_GLES2
        d << "MAGNUM_TARGET_GLES2";
        #endif
        #ifdef MAGNUM_TARGET_NACL
        d << "MAGNUM_TARGET_NACL";
        #endif
        #ifdef CORRADE_GCC46_COMPATIBILITY
        d << "CORRADE_GCC46_COMPATIBILITY";
        #endif
    }
    Debug() << "";

    Debug() << "Vendor:" << c->vendorString();
    Debug() << "Renderer:" << c->rendererString();
    Debug() << "OpenGL version:" << c->version() << '(' + c->versionString() + ')';
    Debug() << "GLSL version:" << c->version() << '(' + c->shadingLanguageVersionString() + ')';
    Debug() << "";

    /* Get first future (not supported) version */
    std::vector<Version> versions{
        #ifndef MAGNUM_TARGET_GLES
        Version::GL300,
        Version::GL310,
        Version::GL320,
        Version::GL330,
        Version::GL400,
        Version::GL410,
        Version::GL420,
        Version::GL430,
        #else
        Version::GLES200,
        Version::GLES300,
        #endif
        Version::None
    };
    std::size_t future = 0;
    while(versions[future] != Version::None && c->isVersionSupported(versions[future]))
        ++future;

    /* Display supported OpenGL extensions from unsupported versions */
    for(std::size_t i = future; i != versions.size(); ++i) {
        if(versions[i] != Version::None)
            Debug() << versions[i] << "extension support:";
        else Debug() << "Vendor extension support:";

        for(const auto& extension: Extension::extensions(versions[i])) {
            std::string extensionName = extension.string();
            Debug d;
            d << "  " << extensionName << std::string(60-extensionName.size(), ' ');
            if(c->isExtensionSupported(extension))
                d << "SUPPORTED";
            else if(c->isVersionSupported(extension.requiredVersion()))
                d << "    -";
            else
                d << "   ---";
        }

        Debug() << "";
    }
}

}

MAGNUM_WINDOWLESSGLXAPPLICATION_MAIN(Magnum::MagnumInfo)
