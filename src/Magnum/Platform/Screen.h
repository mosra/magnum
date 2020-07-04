#ifndef Magnum_Platform_AbstractScreen_h
#define Magnum_Platform_AbstractScreen_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020 Vladimír Vondruš <mosra@centrum.cz>

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

/* These provide overrideable event handlers on the Screen side for events that
   are not implemented by all apps. The virtual *Event() function is defined
   only if the base Application has it. Calling into those is done through
   a corresponding Application*EventMixin defined in ScreenedApplication.h. */
template<class Application, bool> class ScreenKeyEventMixin {};
template<class Application> class ScreenKeyEventMixin<Application, true> {
    public:
        typedef typename BasicScreenedApplication<Application>::KeyEvent KeyEvent;

    private:
        friend ApplicationKeyEventMixin<Application, true>;

        virtual void keyPressEvent(KeyEvent& event);
        virtual void keyReleaseEvent(KeyEvent& event);
};

template<class Application, bool> class ScreenMouseScrollEventMixin {};
template<class Application> class ScreenMouseScrollEventMixin<Application, true> {
    public:
        typedef typename BasicScreenedApplication<Application>::MouseScrollEvent MouseScrollEvent;

    private:
        friend ApplicationMouseScrollEventMixin<Application, true>;

        virtual void mouseScrollEvent(MouseScrollEvent& event);
};

template<class Application, bool> class ScreenTextInputEventMixin {};
template<class Application> class ScreenTextInputEventMixin<Application, true> {
    public:
        typedef typename BasicScreenedApplication<Application>::TextInputEvent TextInputEvent;

    private:
        friend ApplicationTextInputEventMixin<Application, true>;

        virtual void textInputEvent(TextInputEvent& event);
};

template<class Application, bool> class ScreenTextEditingEventMixin {};
template<class Application> class ScreenTextEditingEventMixin<Application, true> {
    public:
        typedef typename BasicScreenedApplication<Application>::TextEditingEvent TextEditingEvent;

    private:
        friend ApplicationTextEditingEventMixin<Application, true>;

        virtual void textEditingEvent(TextEditingEvent& event);
};

}

/**
@brief Base for application screens

@m_keywords{Screen}

See @ref BasicScreenedApplication for more information. If exactly one
application header is included, this class is also aliased to
@cpp Platform::Screen @ce.

@section Platform-BasicScreen-template-specializations Explicit template specializations

The following specialization are explicitly compiled into each particular
`*Application` library. For other specializations you have to use the
@ref ScreenedApplication.hpp implementation file to avoid linker errors. See
@ref compilation-speedup-hpp for more information.

-   @ref AndroidApplication "BasicScreen<AndroidApplication>"
-   @ref EmscriptenApplication "BasicScreen<EmscriptenApplication>"
-   @ref GlfwApplication "BasicScreen<GlfwApplication>"
-   @ref GlxApplication "BasicScreen<GlxApplication>"
-   @ref Sdl2Application "BasicScreen<Sdl2Application>"
-   @ref XEglApplication "BasicScreen<XEglApplication>"
*/
template<class Application> class BasicScreen:
    private Containers::LinkedListItem<BasicScreen<Application>, BasicScreenedApplication<Application>>,
    public Implementation::ScreenKeyEventMixin<Application, Implementation::HasKeyEvent<Application>::value>,
    public Implementation::ScreenMouseScrollEventMixin<Application, Implementation::HasMouseScrollEvent<Application>::value>,
    public Implementation::ScreenTextInputEventMixin<Application, Implementation::HasTextInputEvent<Application>::value>,
    public Implementation::ScreenTextEditingEventMixin<Application, Implementation::HasTextEditingEvent<Application>::value>
{
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
             * @ref mousePressEvent(), @ref mouseReleaseEvent(),
             * @ref mouseMoveEvent(), @ref mouseScrollEvent(),
             * @ref textInputEvent() and @ref textEditingEvent() are propagated
             * to this screen.
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

        /** @brief Viewport event */
        typedef typename BasicScreenedApplication<Application>::ViewportEvent ViewportEvent;

        /** @brief Input event */
        typedef typename BasicScreenedApplication<Application>::InputEvent InputEvent;

        #ifdef DOXYGEN_GENERATING_OUTPUT
        /**
         * @brief Key event
         *
         * Defined only if the application has a
         * @ref Sdl2Application::KeyEvent "KeyEvent".
         */
        typedef typename BasicScreenedApplication<Application>::KeyEvent KeyEvent;
        #endif

        /** @brief Mouse event */
        typedef typename BasicScreenedApplication<Application>::MouseEvent MouseEvent;

        /** @brief Mouse move event */
        typedef typename BasicScreenedApplication<Application>::MouseMoveEvent MouseMoveEvent;

        #ifdef DOXYGEN_GENERATING_OUTPUT
        /**
         * @brief Mouse scroll event
         * @m_since{2019,10}
         *
         * Defined only if the application has a
         * @ref Sdl2Application::MouseScrollEvent "MouseScrollEvent".
         */
        typedef typename BasicScreenedApplication<Application>::MouseScrollEvent MouseScrollEvent;

        /**
         * @brief Text input event
         * @m_since{2019,10}
         *
         * Defined only if the application has a
         * @ref Sdl2Application::TextInputEvent "TextInputEvent".
         */
        typedef typename BasicScreenedApplication<Application>::TextInputEvent TextInputEvent;

        /**
         * @brief Text editing event
         * @m_since{2019,10}
         *
         * Defined only if the application has a
         * @ref Sdl2Application::TextEditingEvent "TextEditingEvent".
         */
        typedef typename BasicScreenedApplication<Application>::TextEditingEvent TextEditingEvent;
        #endif

        /**
         * @brief Construct a detached screen
         *
         * The screen is not attached to any application, use
         * @ref BasicScreenedApplication::addScreen() to add it. Alternatively,
         * use @ref BasicScreen(BasicScreenedApplication<Application>&, PropagatedEvents) to
         * attach the screen right during the construction.
         */
        explicit BasicScreen();

        /**
         * @brief Construct a screen and attach it to an application
         * @m_since{2019,10}
         *
         * Unlike with @ref BasicScreen(), the screen is added to the
         * application already during the construction, removing the need to
         * call @ref BasicScreenedApplication::addScreen() later. This also
         * means @ref focusEvent() is not called for the very first time,
         * assuming the screen is put into desired state already during
         * construction.
         * @see @ref setPropagatedEvents()
         */
        explicit BasicScreen(BasicScreenedApplication<Application>& application, PropagatedEvents events);

        /* A common use case is a list of screen derivatives, so allow deleting
           them through a base pointer */
        virtual ~BasicScreen();

        /** @brief Events propagated to this screen */
        PropagatedEvents propagatedEvents() const { return _propagatedEvents; }

        /**
         * @brief Set events propagated to this screen
         *
         * For non-propagated events related event functions are not called.
         * No events are propagated by default, call this function in
         * @ref focusEvent() and @ref blurEvent() to reflect focus changes.
         * @see @ref BasicScreen(BasicScreenedApplication<Application>&, PropagatedEvents)
         */
        void setPropagatedEvents(PropagatedEvents events) { _propagatedEvents = events; }

        /**
         * @brief Whether the screen is added to an application
         *
         * If not, the @ref application() accessor can't be used.
         * @see @ref BasicScreenedApplication::addScreen(),
         *      @ref BasicScreenedApplication::removeScreen()
         */
        bool hasApplication() {
            return Containers::LinkedListItem<BasicScreen<Application>, BasicScreenedApplication<Application>>::list();
        }

        /**
         * @brief Application holding this screen
         *
         * Expects that the screen is added to an application.
         * @see @ref hasApplication()
         */
        BasicScreenedApplication<Application>& application();
        /** @overload */
        const BasicScreenedApplication<Application>& application() const;
        /** @overload */
        template<class T = BasicScreenedApplication<Application>> T& application() {
            return static_cast<T&>(application());
        }
        /** @overload */
        template<class T = BasicScreenedApplication<Application>> const T& application() const {
            return static_cast<const T&>(application());
        }

        /**
         * @brief Next nearer screen
         *
         * Use @cpp application().screens().first() @ce to access the front
         * screen and @cpp application().screens().last() @ce to access the
         * back screen.
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
         * Use @cpp application().screens().first() @ce to access the front
         * screen and @cpp application().screens().last() @ce to access the
         * back screen.
         */
        BasicScreen<Application>* nextFartherScreen() {
            return Containers::LinkedListItem<BasicScreen<Application>, BasicScreenedApplication<Application>>::next();
        }
        /** @overload */
        const BasicScreen<Application>* nextFartherScreen() const {
            return Containers::LinkedListItem<BasicScreen<Application>, BasicScreenedApplication<Application>>::next();
        }

        /** @{ @name Screen handling */

    protected:
        /**
         * @brief Request redraw
         *
         * Expects that the screen is added to an application.
         * @see @ref hasApplication()
         */
        virtual void redraw();

    private:
        /**
         * @brief Focus event
         *
         * Called when screen is focused using @ref BasicScreenedApplication::focusScreen()
         * or @ref BasicScreenedApplication::addScreen().
         */
        virtual void focusEvent();

        /**
         * @brief Blur event
         *
         * Called when another screen is focused using @ref BasicScreenedApplication::focusScreen(),
         * @ref BasicScreenedApplication::addScreen() or before the screen is
         * removed from application using @ref BasicScreenedApplication::removeScreen().
         */
        virtual void blurEvent();

        /**
         * @brief Viewport event
         *
         * Called from holder application when viewport size changes. See
         * @ref Sdl2Application::viewportEvent() "*Application::viewportEvent()"
         * for more information. Setting viewport on default framebuffer should
         * be done by the holder application.
         */
        virtual void viewportEvent(ViewportEvent& event);

        /**
         * @brief Draw event
         *
         * Called when @ref PropagatedEvent::Draw is enabled and the screen is
         * redrawn. See @ref Sdl2Application::viewportEvent() "*Application::viewportEvent()"
         * for more information. Buffer swapping and clearing of default
         * framebufer should be done by the holder application.
         */
        virtual void drawEvent() = 0;

        /* Since 1.8.17, the original short-hand group closing doesn't work
           anymore. FFS. */
        /**
         * @}
         */

        /** @{ @name Keyboard handling */

        #ifdef DOXYGEN_GENERATING_OUTPUT
        /**
         * @brief Key press event
         *
         * Called when @ref PropagatedEvent::Input is enabled and an key is
         * pressed. See @ref Sdl2Application::keyPressEvent() "*Application::keyPressEvent()"
         * for more information. Defined only if the application has a
         * @ref Sdl2Application::KeyEvent "KeyEvent".
         */
        virtual void keyPressEvent(KeyEvent& event);

        /**
         * @brief Key release event
         *
         * Called when @ref PropagatedEvent::Input is enabled and an key is
         * released. See @ref Sdl2Application::keyReleaseEvent() "*Application::keyReleaseEvent()"
         * for more information. Defined only if the application has a
         * @ref Sdl2Application::KeyEvent "KeyEvent".
         */
        virtual void keyReleaseEvent(KeyEvent& event);
        #endif

        /* Since 1.8.17, the original short-hand group closing doesn't work
           anymore. FFS. */
        /**
         * @}
         */

        /** @{ @name Mouse handling */

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

        #ifdef DOXYGEN_GENERATING_OUTPUT
        /**
         * @brief Mouse scroll event
         * @m_since{2019,10}
         *
         * Called when @ref PropagatedEvent::Input is enabled and mouse wheel
         * is rotated. See @ref Sdl2Application::mouseScrollEvent() "*Application::mouseScrollEvent()"
         * for more information. Defined only if the application has a
         * @ref Sdl2Application::MouseScrollEvent "MouseScrollEvent".
         */
        virtual void mouseScrollEvent(MouseScrollEvent& event);
        #endif

        /* Since 1.8.17, the original short-hand group closing doesn't work
           anymore. FFS. */
        /**
         * @}
         */

        /** @{ @name Text input handling */

        #ifdef DOXYGEN_GENERATING_OUTPUT
        /**
         * @brief Text input event
         * @m_since{2019,10}
         *
         * Called when @ref PropagatedEvent::Input is enabled and text is being
         * input. Defined only if the application has a
         * @ref Sdl2Application::TextInputEvent "TextInputEvent".
         */
        virtual void textInputEvent(TextInputEvent& event);

        /**
         * @brief Text editing event
         * @m_since{2019,10}
         *
         * Called when @ref PropagatedEvent::Input and the text is being
         * edited. Defined only if the application has a
         * @ref Sdl2Application::TextEditingEvent "TextEditingEvent".
         */
        virtual void textEditingEvent(TextEditingEvent& event);
        #endif

        /* Since 1.8.17, the original short-hand group closing doesn't work
           anymore. FFS. */
        /**
         * @}
         */

    private:
        #ifndef DOXYGEN_GENERATING_OUTPUT /* https://bugzilla.gnome.org/show_bug.cgi?id=776986 */
        friend Containers::LinkedListItem<BasicScreen<Application>, BasicScreenedApplication<Application>>;
        friend Containers::LinkedList<BasicScreen<Application>>;
        friend BasicScreenedApplication<Application>;
        #endif

        PropagatedEvents _propagatedEvents;
};

}}

#endif
