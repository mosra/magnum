/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
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

#include "GlutApplication.h"

#include <tuple>

#include "Magnum/GL/Version.h"
#include "Magnum/Platform/GLContext.h"
#include "Magnum/Platform/ScreenedApplication.hpp"

namespace Magnum { namespace Platform {

GlutApplication* GlutApplication::_instance = nullptr;

GlutApplication::GlutApplication(const Arguments& arguments): GlutApplication{arguments, Configuration{}, GLConfiguration{}} {}

GlutApplication::GlutApplication(const Arguments& arguments, const Configuration& configuration): GlutApplication{arguments, configuration, GLConfiguration{}} {}

GlutApplication::GlutApplication(const Arguments& arguments, const Configuration& configuration, const GLConfiguration& glConfiguration): GlutApplication{arguments, NoCreate} {
    create(configuration, glConfiguration);
}

GlutApplication::GlutApplication(const Arguments& arguments, NoCreateT): _context{new GLContext{NoCreate, arguments.argc, arguments.argv}} {
    /* Save global instance */
    _instance = this;

    /* Init GLUT */
    glutInit(&arguments.argc, arguments.argv);
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
}

void GlutApplication::create() {
    create(Configuration{}, GLConfiguration{});
}

void GlutApplication::create(const Configuration& configuration) {
    create(configuration, GLConfiguration{});
}

void GlutApplication::create(const Configuration& configuration, const GLConfiguration& glConfiguration) {
    if(!tryCreate(configuration, glConfiguration)) std::exit(1);
}

bool GlutApplication::tryCreate(const Configuration& configuration) {
    return tryCreate(configuration, GLConfiguration{});
}

bool GlutApplication::tryCreate(const Configuration& configuration, const GLConfiguration&
    #ifndef MAGNUM_BUILD_DEPRECATED
    glConfiguration
    #else
    _glConfiguration
    #endif
) {
    #ifdef MAGNUM_BUILD_DEPRECATED
    GLConfiguration glConfiguration{_glConfiguration};
    CORRADE_IGNORE_DEPRECATED_PUSH
    if(configuration.flags() && !glConfiguration.flags())
        glConfiguration.setFlags(configuration.flags());
    if(configuration.version() != GL::Version::None && glConfiguration.version() == GL::Version::None)
        glConfiguration.setVersion(configuration.version());
    if(configuration.sampleCount() && !glConfiguration.sampleCount())
        glConfiguration.setSampleCount(configuration.sampleCount());
    CORRADE_IGNORE_DEPRECATED_POP
    #endif

    CORRADE_ASSERT(_context->version() == GL::Version::None, "Platform::GlutApplication::tryCreate(): context already created", false);

    unsigned int flags = GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH|GLUT_STENCIL;

    /* Multisampling */
    if(glConfiguration.sampleCount()) flags |= GLUT_MULTISAMPLE;

    glutInitDisplayMode(flags);
    glutInitWindowSize(configuration.size().x(), configuration.size().y());

    /* Set context version, if requested */
    if(glConfiguration.version() != GL::Version::None) {
        Int major, minor;
        std::tie(major, minor) = version(glConfiguration.version());
        glutInitContextVersion(major, minor);
        #ifndef MAGNUM_TARGET_GLES
        if(glConfiguration.version() >= GL::Version::GL310)
            glutInitContextProfile(GLUT_CORE_PROFILE);
        #endif
    }

    /* Set context flags */
    glutInitContextFlags(int(glConfiguration.flags()));

    if(!glutCreateWindow(configuration.title().data())) {
        Error() << "Platform::GlutApplication::tryCreate(): cannot create context";
        return false;
    }
    glutReshapeFunc(staticViewportEvent);
    glutKeyboardFunc(staticKeyPressEvent);
    glutKeyboardUpFunc(staticKeyReleaseEvent);
    glutSpecialFunc(staticSpecialKeyPressEvent);
    glutSpecialUpFunc(staticSpecialKeyReleaseEvent);
    glutMouseFunc(staticMouseEvent);
    glutMotionFunc(staticMouseMoveEvent);
    glutDisplayFunc(staticDrawEvent);

    /* Return true if the initialization succeeds */
    return _context->tryCreate();
}

GlutApplication::~GlutApplication() = default;

void GlutApplication::staticKeyPressEvent(unsigned char key, int x, int y) {
    KeyEvent e(static_cast<KeyEvent::Key>(key), {x, y});
    _instance->keyPressEvent(e);
}

void GlutApplication::staticKeyReleaseEvent(unsigned char key, int x, int y) {
    KeyEvent e(static_cast<KeyEvent::Key>(key), {x, y});
    _instance->keyReleaseEvent(e);
}

void GlutApplication::staticSpecialKeyPressEvent(int key, int x, int y){
    KeyEvent e(static_cast<KeyEvent::Key>(key << 16), {x, y});
    _instance->keyPressEvent(e);
}

void GlutApplication::staticSpecialKeyReleaseEvent(int key, int x, int y){
    KeyEvent e(static_cast<KeyEvent::Key>(key << 16), {x, y});
    _instance->keyReleaseEvent(e);
}

void GlutApplication::staticMouseEvent(int button, int state, int x, int y) {
    MouseEvent e(static_cast<MouseEvent::Button>(button), {x, y});
    if(state == GLUT_DOWN)
        _instance->mousePressEvent(e);
    else
        _instance->mouseReleaseEvent(e);
}

void GlutApplication::staticMouseMoveEvent(int x, int y) {
    MouseMoveEvent e({x, y}, MouseMoveEvent::Button::Left);
    _instance->mouseMoveEvent(e);
}

void GlutApplication::viewportEvent(const Vector2i&) {}
void GlutApplication::keyPressEvent(KeyEvent&) {}
void GlutApplication::keyReleaseEvent(KeyEvent&) {}
void GlutApplication::mousePressEvent(MouseEvent&) {}
void GlutApplication::mouseReleaseEvent(MouseEvent&) {}
void GlutApplication::mouseMoveEvent(MouseMoveEvent&) {}

GlutApplication::GLConfiguration::GLConfiguration(): _sampleCount(0), _version(GL::Version::None) {}
GlutApplication::GLConfiguration::~GLConfiguration() = default;

GlutApplication::Configuration::Configuration():
    _title("Magnum GLUT Application"), _size(800, 600)
    #ifdef MAGNUM_BUILD_DEPRECATED
    , _sampleCount(0), _version(GL::Version::None)
    #endif
    {}
GlutApplication::Configuration::~Configuration() = default;

template class BasicScreen<GlutApplication>;
template class BasicScreenedApplication<GlutApplication>;

}}
