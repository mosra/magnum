#ifndef Magnum_Platform_AbstractScreen_h
#define Magnum_Platform_AbstractScreen_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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
 * @brief Class @ref Magnum::Platform::BasicScreen
 */

#include <Corrade/Containers/EnumSet.h>

#include "Magnum/Platform/ScreenedApplication.h"

namespace Magnum { namespace Platform {

namespace Implementation {
    enum class PropagatedScreenEvent: UnsignedByte {
        Draw = 1 << 0,
        Input = 1 << 1
    };

    typedef Containers::EnumSet<PropagatedScreenEvent> PropagatedScreenEvents;
    CORRADE_ENUMSET_OPERATORS(PropagatedScreenEvents)
}

/**
@brief Base for application screens

See @ref BasicScreenedApplication for more information.

If exactly one application header is included, this class is also aliased to
`Platform::Screen`.

## Explicit template specializations

The following specialization are explicitly compiled into each particular
`*Application` library. For other specializations you have to use
@ref ScreenedApplication.hpp implementation file to avoid linker errors. See
@ref compilation-speedup-hpp for more information.

-   @ref GlutApplication "BasicScreen<GlutApplication>"
-   @ref GlxApplication "BasicScreen<GlxApplication>"
-   @ref Sdl2Application "BasicScreen<Sdl2Application>"
-   @ref XEglApplication "BasicScreen<XEglApplication>"
*/
template<class Application> class BasicScreen: private Containers::LinkedListItem<BasicScreen<Application>, BasicScreenedApplication<Application>> {
    friend Containers::LinkedListItem<BasicScreen<Application>, BasicScreenedApplication<Application>>;
    friend Containers::LinkedList<BasicScreen<Application>>;
    friend BasicScreenedApplication<Application>;

    public:
        #ifdef DOXYGEN_GENERATING_OUTPUT
        /**
         * @brief Event propagated to given screen
         *
         * @see @ref PropagatedEvents, @ref BasicScreen::setPropagatedEvents()
         */
        enum class PropagatedEvent: UnsignedByte {
            /**
             * Draw event.
             *
             * When enabled, @ref drawEvent() is propagated to this screen.
             */
            Draw = 1 << 0,

            /**
             * Input events.
             *
             * When enabled, @ref keyPressEvent(), @ref keyReleaseEvent(),
             * @ref mousePressEvent(), @ref mouseReleaseEvent() and
             * @ref mouseMoveEvent() are propagated to this screen.
             */
            Input = 1 << 1
        };

        /**
         * @brief Events propagated to given screen
         *
         * @see @ref setPropagatedEvents()
         */
        typedef Containers::EnumSet<PropagatedEvent> PropagatedEvents;
        #else
        typedef Implementation::PropagatedScreenEvent PropagatedEvent;
        typedef Implementation::PropagatedScreenEvents PropagatedEvents;
        #endif

        /** @brief Input event */
        typedef typename BasicScreenedApplication<Application>::InputEvent InputEvent;

        /** @brief Key event */
        typedef typename BasicScreenedApplication<Application>::KeyEvent KeyEvent;

        /** @brief Mouse event */
        typedef typename BasicScreenedApplication<Application>::MouseEvent MouseEvent;

        /** @brief Mouse move event */
        typedef typename BasicScreenedApplication<Application>::MouseMoveEvent MouseMoveEvent;

        explicit BasicScreen();
        ~BasicScreen();

        /** @brief Events propagated to this screen */
        PropagatedEvents propagatedEvents() const { return _propagatedEvents; }

        /**
         * @brief Set events propagated to this screen
         *
         * For non-propagated events related event functions are not called.
         * No events are propagated by default, call this function in
         * @ref focusEvent() and @ref blurEvent() to reflect focus changes.
         */
        void setPropagatedEvents(PropagatedEvents events) { _propagatedEvents = events; }

        /** @brief Application holding this screen */
        template<class T = BasicScreenedApplication<Application>> T* application() {
            return static_cast<T*>(Containers::LinkedListItem<BasicScreen<Application>, BasicScreenedApplication<Application>>::list());
        }
        /** @overload */
        template<class T = BasicScreenedApplication<Application>> const T* application() const {
            return static_cast<const T*>(Containers::LinkedListItem<BasicScreen<Application>, BasicScreenedApplication<Application>>::list());
        }

        /**
         * @brief Next nearer screen
         *
         * @see @ref BasicScreenedApplication::frontScreen(),
         *      @ref BasicScreenedApplication::backScreen()
         */
        BasicScreen<Application>* nextNearerScreen() {
            return Containers::LinkedListItem<BasicScreen<Application>, BasicScreenedApplication<Application>>::previous();
        }
        /** @overload */
        const BasicScreen<Application>* nextNearerScreen() const {
            return Containers::LinkedListItem<BasicScreen<Application>, BasicScreenedApplication<Application>>::previous();
        }

        /**
         * @brief Next farther screen
         *
         * @see @ref BasicScreenedApplication::frontScreen(),
         *      @ref BasicScreenedApplication::backScreen()
         */
        BasicScreen<Application>* nextFartherScreen() {
            return Containers::LinkedListItem<BasicScreen<Application>, BasicScreenedApplication<Application>>::next();
        }
        /** @overload */
        const BasicScreen<Application>* nextFartherScreen() const {
            return Containers::LinkedListItem<BasicScreen<Application>, BasicScreenedApplication<Application>>::next();
        }

    protected:
        /** @brief Request redraw */
        virtual void redraw() { application()->redraw(); }

        /**
         * @brief Focus event
         *
         * Called when screen is focused using @ref BasicScreenedApplication::focusScreen()
         * or @ref BasicScreenedApplication::addScreen().
         */
        virtual void focusEvent() = 0;

        /**
         * @brief Blur event
         *
         * Called when another screen is focused using @ref BasicScreenedApplication::focusScreen(),
         * @ref BasicScreenedApplication::addScreen() or before the screen is
         * removed from application using @ref BasicScreenedApplication::removeScreen().
         */
        virtual void blurEvent() = 0;

        /**
         * @brief Viewport event
         *
         * Called from holder application when viewport size changes. See
         * @ref Sdl2Application::viewportEvent() "*Application::viewportEvent()"
         * for more information. Setting viewport on default framebuffer should
         * be done by the holder application.
         */
        virtual void viewportEvent(const Vector2i& size) = 0;

        /**
         * @brief Draw event
         *
         * Called when @ref PropagatedEvent::Draw is enabled and the screen is
         * redrawn. See @ref Sdl2Application::viewportEvent() "*Application::viewportEvent()"
         * for more information. Buffer swapping and clearing of default
         * framebufer should be done by the holder application.
         */
        virtual void drawEvent() = 0;

        /**
         * @brief Key press event
         *
         * Called when @ref PropagatedEvent::Input is enabled and an key is
         * pressed. See @ref Sdl2Application::keyPressEvent() "*Application::keyPressEvent()"
         * for more information.
         */
        virtual void keyPressEvent(KeyEvent& event);

        /**
         * @brief Key release event
         *
         * Called when @ref PropagatedEvent::Input is enabled and an key is
         * released. See @ref Sdl2Application::keyReleaseEvent() "*Application::keyReleaseEvent()"
         * for more information.
         */
        virtual void keyReleaseEvent(KeyEvent& event);

        /**
         * @brief Mouse press event
         *
         * Called when @ref PropagatedEvent::Input is enabled and mouse button
         * is pressed. See @ref Sdl2Application::mousePressEvent() "*Application::mousePressEvent()"
         * for more information.
         */
        virtual void mousePressEvent(MouseEvent& event);

        /**
         * @brief Mouse release event
         *
         * Called when @ref PropagatedEvent::Input is enabled and mouse button
         * is released. See @ref Sdl2Application::mouseReleaseEvent() "*Application::mouseReleaseEvent()"
         * for more information.
         */
        virtual void mouseReleaseEvent(MouseEvent& event);

        /**
         * @brief Mouse move event
         *
         * Called when @ref PropagatedEvent::Input is enabled and mouse is
         * moved. See @ref Sdl2Application::mouseMoveEvent() "*Application::mouseMoveEvent()"
         * for more information.
         */
        virtual void mouseMoveEvent(MouseMoveEvent& event);

    private:
        PropagatedEvents _propagatedEvents;
};

}}

#endif
