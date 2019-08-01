#ifndef Magnum_Platform_ScreenedApplication_hpp
#define Magnum_Platform_ScreenedApplication_hpp
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
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

namespace Implementation {

template<class Application, bool implements> void ApplicationKeyEventMixin<Application, implements>::callKeyPressEvent(KeyEvent&, Containers::LinkedList<BasicScreen<Application>>&) {}
template<class Application> void ApplicationKeyEventMixin<Application, true>::callKeyPressEvent(typename Application::KeyEvent& event, Containers::LinkedList<BasicScreen<Application>>& screens) {
    /* Front-to-back event propagation, stop when the event gets accepted */
    for(BasicScreen<Application>* s = screens.first(); s; s = s->nextFartherScreen()) {
        if(s->propagatedEvents() & Implementation::PropagatedScreenEvent::Input) {
            s->keyPressEvent(event);
            if(event.isAccepted()) break;
        }
    }
}

template<class Application, bool implements> void ApplicationKeyEventMixin<Application, implements>::callKeyReleaseEvent(KeyEvent&, Containers::LinkedList<BasicScreen<Application>>&) {}
template<class Application> void ApplicationKeyEventMixin<Application, true>::callKeyReleaseEvent(typename Application::KeyEvent& event, Containers::LinkedList<BasicScreen<Application>>& screens) {
    /* Front-to-back event propagation, stop when the event gets accepted */
    for(BasicScreen<Application>* s = screens.first(); s; s = s->nextFartherScreen()) {
        if(s->propagatedEvents() & Implementation::PropagatedScreenEvent::Input) {
            s->keyReleaseEvent(event);
            if(event.isAccepted()) break;
        }
    }
}

template<class Application, bool implements> void ApplicationMouseScrollEventMixin<Application, implements>::callMouseScrollEvent(MouseScrollEvent&, Containers::LinkedList<BasicScreen<Application>>&) {}
template<class Application> void ApplicationMouseScrollEventMixin<Application, true>::callMouseScrollEvent(typename Application::MouseScrollEvent& event, Containers::LinkedList<BasicScreen<Application>>& screens) {
    /* Front-to-back event propagation, stop when the event gets accepted */
    for(BasicScreen<Application>* s = screens.first(); s; s = s->nextFartherScreen()) {
        if(s->propagatedEvents() & Implementation::PropagatedScreenEvent::Input) {
            s->mouseScrollEvent(event);
            if(event.isAccepted()) break;
        }
    }
}

template<class Application, bool implements> void ApplicationTextInputEventMixin<Application, implements>::callTextInputEvent(TextInputEvent&, Containers::LinkedList<BasicScreen<Application>>&) {}
template<class Application> void ApplicationTextInputEventMixin<Application, true>::callTextInputEvent(typename Application::TextInputEvent& event, Containers::LinkedList<BasicScreen<Application>>& screens) {
    /* Front-to-back event propagation, stop when the event gets accepted */
    for(BasicScreen<Application>* s = screens.first(); s; s = s->nextFartherScreen()) {
        if(s->propagatedEvents() & Implementation::PropagatedScreenEvent::Input) {
            s->textInputEvent(event);
            if(event.isAccepted()) break;
        }
    }
}

template<class Application, bool implements> void ApplicationTextEditingEventMixin<Application, implements>::callTextEditingEvent(TextEditingEvent&, Containers::LinkedList<BasicScreen<Application>>&) {}
template<class Application> void ApplicationTextEditingEventMixin<Application,
true>::callTextEditingEvent(typename Application::TextEditingEvent& event, Containers::LinkedList<BasicScreen<Application>>& screens) {
    /* Front-to-back event propagation, stop when the event gets accepted */
    for(BasicScreen<Application>* s = screens.first(); s; s = s->nextFartherScreen()) {
        if(s->propagatedEvents() & Implementation::PropagatedScreenEvent::Input) {
            s->textEditingEvent(event);
            if(event.isAccepted()) break;
        }
    }
}

template<class Application> void ScreenKeyEventMixin<Application,
true>::keyPressEvent(KeyEvent&) {}
template<class Application> void ScreenKeyEventMixin<Application,
true>::keyReleaseEvent(KeyEvent&) {}
template<class Application> void ScreenMouseScrollEventMixin<Application,
true>::mouseScrollEvent(MouseScrollEvent&) {}
template<class Application> void ScreenTextInputEventMixin<Application,
true>::textInputEvent(TextInputEvent&) {}
template<class Application> void ScreenTextEditingEventMixin<Application,
true>::textEditingEvent(TextEditingEvent&) {}

}

template<class Application> BasicScreen<Application>::BasicScreen() = default;
template<class Application> BasicScreen<Application>::~BasicScreen() = default;

template<class Application> void BasicScreen<Application>::viewportEvent(ViewportEvent& event) {
    #ifdef MAGNUM_BUILD_DEPRECATED
    CORRADE_IGNORE_DEPRECATED_PUSH
    viewportEvent(event.windowSize());
    CORRADE_IGNORE_DEPRECATED_POP
    #else
    static_cast<void>(event);
    #endif
}

#ifdef MAGNUM_BUILD_DEPRECATED
template<class Application> void BasicScreen<Application>::viewportEvent(const Vector2i&) {}
#endif

template<class Application> void BasicScreen<Application>::mousePressEvent(MouseEvent&) {}
template<class Application> void BasicScreen<Application>::mouseReleaseEvent(MouseEvent&) {}
template<class Application> void BasicScreen<Application>::mouseMoveEvent(MouseMoveEvent&) {}

#ifdef MAGNUM_TARGET_GL
template<class Application> BasicScreenedApplication<Application>::BasicScreenedApplication(const typename Application::Arguments& arguments, const typename Application::Configuration& configuration, const typename Application::GLConfiguration& glConfiguration): Application(arguments, configuration, glConfiguration) {}
#endif

template<class Application> BasicScreenedApplication<Application>::BasicScreenedApplication(const typename Application::Arguments& arguments, const typename Application::Configuration& configuration): Application(arguments, configuration) {}

template<class Application> BasicScreenedApplication<Application>::BasicScreenedApplication(const typename Application::Arguments& arguments, NoCreateT): Application(arguments, NoCreate) {}

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

template<class Application> void BasicScreenedApplication<Application>::globalViewportEvent(typename Application::ViewportEvent&) {}

template<class Application> void BasicScreenedApplication<Application>::viewportEvent(typename Application::ViewportEvent& event) {
    /* Call global event before all other (to resize framebuffer first) */
    globalViewportEvent(event);

    for(BasicScreen<Application>& s: *this) s.viewportEvent(event);
}

template<class Application> void BasicScreenedApplication<Application>::drawEvent() {
    /* Back-to-front rendering */
    for(BasicScreen<Application>* s = screens().last(); s; s = s->nextNearerScreen())
        if(s->propagatedEvents() & Implementation::PropagatedScreenEvent::Draw) s->drawEvent();

    /* Call global event after all other (to swap buffers last) */
    globalDrawEvent();
}

template<class Application> void BasicScreenedApplication<Application>::keyPressEvent(typename BasicScreenedApplication<Application>::KeyEvent& event) {
    this->callKeyPressEvent(event, screens());
}

template<class Application> void BasicScreenedApplication<Application>::keyReleaseEvent(typename BasicScreenedApplication<Application>::KeyEvent& event) {
    this->callKeyReleaseEvent(event, screens());
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

template<class Application> void BasicScreenedApplication<Application>::mouseScrollEvent(typename BasicScreenedApplication<Application>::MouseScrollEvent& event) {
    this->callMouseScrollEvent(event, screens());
}

template<class Application> void BasicScreenedApplication<Application>::textInputEvent(typename BasicScreenedApplication<Application>::TextInputEvent& event) {
    this->callTextInputEvent(event, screens());
}

template<class Application> void BasicScreenedApplication<Application>::textEditingEvent(typename BasicScreenedApplication<Application>::TextEditingEvent& event) {
    this->callTextEditingEvent(event, screens());
}

}}

#endif
