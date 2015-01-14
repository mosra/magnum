/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015
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
 * @brief @ref compilation-speedup-hpp "Template implementation" for @ref ScreenedApplication.h and @ref Screen.h
 */

#include "Magnum/Platform/Screen.h"
#include "Magnum/Platform/ScreenedApplication.h"

namespace Magnum { namespace Platform {

template<class Application> BasicScreen<Application>::BasicScreen() = default;
template<class Application> BasicScreen<Application>::~BasicScreen() = default;

template<class Application> void BasicScreen<Application>::keyPressEvent(KeyEvent&) {}
template<class Application> void BasicScreen<Application>::keyReleaseEvent(KeyEvent&) {}
template<class Application> void BasicScreen<Application>::mousePressEvent(MouseEvent&) {}
template<class Application> void BasicScreen<Application>::mouseReleaseEvent(MouseEvent&) {}
template<class Application> void BasicScreen<Application>::mouseMoveEvent(MouseMoveEvent&) {}

template<class Application> BasicScreenedApplication<Application>::BasicScreenedApplication(const typename Application::Arguments& arguments, const typename Application::Configuration& configuration): Application(arguments, configuration) {}

template<class Application> BasicScreenedApplication<Application>::BasicScreenedApplication(const typename Application::Arguments& arguments, std::nullptr_t): Application(arguments, nullptr) {}

template<class Application> BasicScreenedApplication<Application>::~BasicScreenedApplication() = default;

template<class Application> BasicScreenedApplication<Application>& BasicScreenedApplication<Application>::addScreen(BasicScreen<Application>& screen) {
    Containers::LinkedList<BasicScreen<Application>>::insert(&screen);
    if(screens().first() == &screen) screen.focusEvent();
    Application::redraw();
    return *this;
}

template<class Application> BasicScreenedApplication<Application>& BasicScreenedApplication<Application>::removeScreen(BasicScreen<Application>& screen) {
    screen.blurEvent();
    Containers::LinkedList<BasicScreen<Application>>::erase(&screen);
    Application::redraw();
    return *this;
}

template<class Application> BasicScreenedApplication<Application>& BasicScreenedApplication<Application>::focusScreen(BasicScreen<Application>& screen) {
    /* Already focused, nothing to do */
    if(screens().first() == &screen) return *this;

    screens().first()->blurEvent();
    Containers::LinkedList<BasicScreen<Application>>::move(&screen, screens().first());
    screen.focusEvent();
    Application::redraw();
    return *this;
}

template<class Application> void BasicScreenedApplication<Application>::globalViewportEvent(const Vector2i&) {}

template<class Application> void BasicScreenedApplication<Application>::viewportEvent(const Vector2i& size) {
    /* Call viewport event after all other (because of framebuffer resizing) */
    globalViewportEvent(size);

    for(BasicScreen<Application>* s = screens().first(); s; s = s->nextFartherScreen())
        s->viewportEvent(size);
}

template<class Application> void BasicScreenedApplication<Application>::drawEvent() {
    /* Back-to-front rendering */
    for(BasicScreen<Application>* s = screens().last(); s; s = s->nextNearerScreen())
        if(s->propagatedEvents() & Implementation::PropagatedScreenEvent::Draw) s->drawEvent();

    /* Call global event after all other (because of buffer swapping) */
    globalDrawEvent();
}

template<class Application> void BasicScreenedApplication<Application>::keyPressEvent(typename Application::KeyEvent& event) {
    /* Front-to-back event propagation, stop when the event gets accepted */
    for(BasicScreen<Application>* s = screens().first(); s; s = s->nextFartherScreen()) {
        if(s->propagatedEvents() & Implementation::PropagatedScreenEvent::Input) {
            s->keyPressEvent(event);
            if(event.isAccepted()) break;
        }
    }
}

template<class Application> void BasicScreenedApplication<Application>::keyReleaseEvent(typename Application::KeyEvent& event) {
    /* Front-to-back event propagation, stop when the event gets accepted */
    for(BasicScreen<Application>* s = screens().first(); s; s = s->nextFartherScreen()) {
        if(s->propagatedEvents() & Implementation::PropagatedScreenEvent::Input) {
            s->keyReleaseEvent(event);
            if(event.isAccepted()) break;
        }
    }
}

template<class Application> void BasicScreenedApplication<Application>::mousePressEvent(typename Application::MouseEvent& event) {
    /* Front-to-back event propagation, stop when the event gets accepted */
    for(BasicScreen<Application>* s = screens().first(); s; s = s->nextFartherScreen()) {
        if(s->propagatedEvents() & Implementation::PropagatedScreenEvent::Input) {
            s->mousePressEvent(event);
            if(event.isAccepted()) break;
        }
    }
}

template<class Application> void BasicScreenedApplication<Application>::mouseReleaseEvent(typename Application::MouseEvent& event) {
    /* Front-to-back event propagation, stop when the event gets accepted */
    for(BasicScreen<Application>* s = screens().first(); s; s = s->nextFartherScreen()) {
        if(s->propagatedEvents() & Implementation::PropagatedScreenEvent::Input) {
            s->mouseReleaseEvent(event);
            if(event.isAccepted()) break;
        }
    }
}

template<class Application> void BasicScreenedApplication<Application>::mouseMoveEvent(typename Application::MouseMoveEvent& event) {
    /* Front-to-back event propagation, stop when the event gets accepted */
    for(BasicScreen<Application>* s = screens().first(); s; s = s->nextFartherScreen()) {
        if(s->propagatedEvents() & Implementation::PropagatedScreenEvent::Input) {
            s->mouseMoveEvent(event);
            if(event.isAccepted()) break;
        }
    }
}

}}
