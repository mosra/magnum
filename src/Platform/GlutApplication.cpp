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

#include "GlutApplication.h"

#include "Context.h"
#include "ExtensionWrangler.h"

namespace Magnum { namespace Platform {

GlutApplication* GlutApplication::instance = nullptr;

GlutApplication::GlutApplication(int& argc, char** argv, const std::string& title, const Vector2i& size) {
    /* Save global instance */
    instance = this;

    /* Init GLUT */
    glutInit(&argc, argv);
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH|GLUT_STENCIL);
    glutInitWindowSize(size.x(), size.y());
    glutCreateWindow(title.c_str());
    glutReshapeFunc(staticViewportEvent);
    glutSpecialFunc(staticKeyEvent);
    glutMouseFunc(staticMouseEvent);
    glutMotionFunc(staticMouseMoveEvent);
    glutDisplayFunc(staticDrawEvent);

    ExtensionWrangler::initialize();

    c = new Context;
}

GlutApplication::~GlutApplication() {
    delete c;
}

void GlutApplication::staticKeyEvent(int key, int x, int y){
    KeyEvent e(static_cast<KeyEvent::Key>(key), {x, y});
    instance->keyPressEvent(e);
}

void GlutApplication::staticMouseEvent(int button, int state, int x, int y) {
    MouseEvent e(static_cast<MouseEvent::Button>(button), {x, y});
    if(state == GLUT_DOWN)
        instance->mousePressEvent(e);
    else
        instance->mouseReleaseEvent(e);
}

void GlutApplication::staticMouseMoveEvent(int x, int y) {
    MouseMoveEvent e({x, y});
    instance->mouseMoveEvent(e);
}

}}
