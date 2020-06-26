#ifndef Magnum_Platform_ScreenedApplication_h
#define Magnum_Platform_ScreenedApplication_h
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
 * @brief Class @ref Magnum::Platform::BasicScreenedApplication
 */

#include <Corrade/Containers/LinkedList.h>

#include "Magnum/Magnum.h"
#include "Magnum/Tags.h"
#include "Magnum/Platform/Platform.h"

namespace Magnum { namespace Platform {

namespace Implementation {

CORRADE_HAS_TYPE(HasKeyEvent, typename T::KeyEvent);
CORRADE_HAS_TYPE(HasMouseScrollEvent, typename T::MouseScrollEvent);
CORRADE_HAS_TYPE(HasTextInputEvent, typename T::TextInputEvent);
CORRADE_HAS_TYPE(HasTextEditingEvent, typename T::TextEditingEvent);

/* Calls into the screen in case the application has a key*Event(), otherwise
   provides a dummy virtual so the application can unconditionally override */
template<class Application, bool> struct ApplicationKeyEventMixin {
    typedef int KeyEvent;
    virtual void keyPressEvent(KeyEvent&) = 0;
    virtual void keyReleaseEvent(KeyEvent&) = 0;

    void callKeyPressEvent(KeyEvent&, Containers::LinkedList<BasicScreen<Application>>&);
    void callKeyReleaseEvent(KeyEvent&, Containers::LinkedList<BasicScreen<Application>>&);
};
template<class Application> struct ApplicationKeyEventMixin<Application, true> {
    void callKeyPressEvent(typename Application::KeyEvent& event, Containers::LinkedList<BasicScreen<Application>>& screens);
    void callKeyReleaseEvent(typename Application::KeyEvent& event, Containers::LinkedList<BasicScreen<Application>>& screens);
};

/* Calls into the screen in case the application has a mouseScrollEvent(),
   otherwise provides a dummy virtual so the application can unconditionally
   override */
template<class Application, bool> struct ApplicationMouseScrollEventMixin {
    typedef int MouseScrollEvent;
    virtual void mouseScrollEvent(MouseScrollEvent&) = 0;

    void callMouseScrollEvent(MouseScrollEvent&, Containers::LinkedList<BasicScreen<Application>>&);
};
template<class Application> struct ApplicationMouseScrollEventMixin<Application, true> {
    void callMouseScrollEvent(typename Application::MouseScrollEvent& event, Containers::LinkedList<BasicScreen<Application>>& screens);
};

/* Calls into the screen in case the application has a textInputEvent(),
   otherwise provides a dummy virtual so the application can unconditionally
   override */
template<class Application, bool> struct ApplicationTextInputEventMixin {
    typedef int TextInputEvent;
    virtual void textInputEvent(TextInputEvent&) = 0;

    void callTextInputEvent(TextInputEvent&, Containers::LinkedList<BasicScreen<Application>>&);
};
template<class Application> struct ApplicationTextInputEventMixin<Application, true> {
    void callTextInputEvent(typename Application::TextInputEvent& event, Containers::LinkedList<BasicScreen<Application>>& screens);
};

/* Calls into the screen in case the application has a textEditingEvent(),
   otherwise provides a dummy virtual so the application can unconditionally
   override */
template<class Application, bool> struct ApplicationTextEditingEventMixin {
    typedef int TextEditingEvent;
    virtual void textEditingEvent(TextEditingEvent&) = 0;

    void callTextEditingEvent(TextEditingEvent&, Containers::LinkedList<BasicScreen<Application>>&);
};
template<class Application> struct ApplicationTextEditingEventMixin<Application, true> {
    void callTextEditingEvent(typename Application::TextEditingEvent& event, Containers::LinkedList<BasicScreen<Application>>& screens);
};

}

/**
@brief Base for applications with screen management

@m_keywords{ScreenedApplication}

Manages list of screens and propagates events to them. If exactly one
application header is included, this class is also aliased to
@cpp Platform::ScreenedApplication @ce.

When you derive from this class, you're not allowed to implement any
usual application event handlers --- instead, these are propagated to
@ref BasicScreen "Screen" instances that get added using @ref addScreen(). Each
@ref BasicScreen "Screen" specifies which set of events should be propagated to
it using @ref BasicScreen::setPropagatedEvents(). When the application gets an
event, they are propagated to the screens:

-   @ref Sdl2Application::viewportEvent() "viewportEvent()" is propagated to
    all screens.
-   @ref Sdl2Application::drawEvent() "drawEvent()" is propagated in
    back-to-front order to screens which have @ref BasicScreen::PropagatedEvent::Draw
    enabled.
-   Input events (@ref Sdl2Application::keyPressEvent() "keyPressEvent()",
    @ref Sdl2Application::keyReleaseEvent() "keyReleaseEvent()",
    @ref Sdl2Application::mousePressEvent() "mousePressEvent()",
    @ref Sdl2Application::mouseReleaseEvent() "mouseReleaseEvent()",
    @ref Sdl2Application::mouseMoveEvent() "mouseMoveEvent()",
    @ref Sdl2Application::mouseMoveEvent() "mouseScrollEvent()",
    @ref Sdl2Application::textInputEvent() "textInputEvent()" and
    @ref Sdl2Application::textEditingEvent() "textEditingEvent()")
    are propagated in front-to-back order to screens which have
    @ref BasicScreen::PropagatedEvent::Input enabled. If any screen sets the
    event as accepted, it is not propagated further.

For the actual application, at the very least you need to implement
@ref globalDrawEvent(), and in case your application is resizable,
@ref globalViewportEvent() as well. The global draw event gets called *after*
all @ref BasicScreen::drawEvent() "Screen::drawEvent()" in order to make it
possible for you to do a buffer swap, while the global viewport event gets
called *before* all @ref BasicScreen::viewportEvent() "Screen::viewportEvent()",
in this case to make it possible to handle viewport changes on the default
framebuffer:

@snippet MagnumPlatform.cpp ScreenedApplication-global-events

Uses @ref Corrade::Containers::LinkedList for efficient screen management.
Traversing front-to-back through the list of screens can be done using
range-based for:

@snippet MagnumPlatform.cpp ScreenedApplication-for-range

Or, if you need more flexibility, like in the following code. Traversing
back-to-front can be done using @ref Corrade::Containers::LinkedList::last()
and @ref BasicScreen::nextNearerScreen().

@snippet MagnumPlatform.cpp ScreenedApplication-for

@section Platform-ScreenedApplication-template-specializations Explicit template specializations

The following specialization are explicitly compiled into each particular
`*Application` library. For other specializations you have to use the
@ref ScreenedApplication.hpp implementation file to avoid linker errors. See
@ref compilation-speedup-hpp for more information.

-   @ref AndroidApplication "BasicScreenedApplication<AndroidApplication>"
-   @ref EmscriptenApplication "BasicScreenedApplication<EmscriptenApplication>"
-   @ref GlfwApplication "BasicScreenedApplication<GlfwApplication>"
-   @ref GlxApplication "BasicScreenedApplication<GlxApplication>"
-   @ref Sdl2Application "BasicScreenedApplication<Sdl2Application>"
-   @ref XEglApplication "BasicScreenedApplication<XEglApplication>"
*/
template<class Application> class BasicScreenedApplication:
    public Application,
    private Containers::LinkedList<BasicScreen<Application>>,
    private Implementation::ApplicationKeyEventMixin<Application, Implementation::HasKeyEvent<Application>::value>,
    private Implementation::ApplicationMouseScrollEventMixin<Application, Implementation::HasMouseScrollEvent<Application>::value>,
    private Implementation::ApplicationTextInputEventMixin<Application, Implementation::HasTextInputEvent<Application>::value>,
    private Implementation::ApplicationTextEditingEventMixin<Application, Implementation::HasTextEditingEvent<Application>::value>
{
    public:
        #ifdef MAGNUM_TARGET_GL
        /**
         * @brief Construct with given configuration for OpenGL context
         *
         * Passes the arguments through to a particular application
         * constructor.
         *
         * @note This function is available only if Magnum is compiled with
         *      @ref MAGNUM_TARGET_GL enabled (done by default). See
         *      @ref building-features for more information.
         */
        explicit BasicScreenedApplication(const typename Application::Arguments& arguments, const typename Application::Configuration& configuration, const typename Application::GLConfiguration& glConfiguration);
        #endif

        /**
         * @brief Construct with given configuration
         *
         * Passes the arguments through to a particular application
         * constructor.
         */
        explicit BasicScreenedApplication(const typename Application::Arguments& arguments, const typename Application::Configuration& configuration = typename Application::Configuration{});

        /**
         * @brief Constructor
         * @param arguments         Application arguments
         *
         * Unlike above, the context is not created and must be created later
         * with @ref Sdl2Application::create() "create()" or
         * @ref Sdl2Application::tryCreate() "tryCreate()".
         */
        explicit BasicScreenedApplication(const typename Application::Arguments& arguments, NoCreateT);

        /**
         * @brief Add screen to application
         * @return Reference to self (for method chaining)
         *
         * The new screen is added as backmost. If this is the first screen
         * added, @ref BasicScreen::focusEvent() is called. If not, neither
         * @ref BasicScreen::blurEvent() nor @ref BasicScreen::focusEvent() is
         * called (i.e. the screen default state is used).
         *
         * Alternatively, a screen can be created using the
         * @ref BasicScreen::BasicScreen(BasicScreenedApplication<Application>&, PropagatedEvents)
         * constructor. In that case, the first @ref BasicScreen::focusEvent()
         * is not called, assuming the screen is put into desired state already
         * during construction.
         */
        BasicScreenedApplication<Application>& addScreen(BasicScreen<Application>& screen);

        /**
         * @brief Remove screen from application
         * @return Reference to self (for method chaining)
         *
         * The screen is blurred before removing. Deleting the object is left
         * up to the user.
         * @see @ref BasicScreen::blurEvent()
         */
        BasicScreenedApplication<Application>& removeScreen(BasicScreen<Application>& screen);

        /**
         * @brief Focus screen
         * @return Reference to self (for method chaining)
         *
         * Moves the screen to front. Previously focused screen is blurred and
         * this screen is focused.
         * @see @ref BasicScreen::blurEvent(), @ref BasicScreen::focusEvent()
         */
        BasicScreenedApplication<Application>& focusScreen(BasicScreen<Application>& screen);

        /**
         * @brief Application screens
         *
         * The screens are sorted front-to-back.
         * @see @ref BasicScreen::application(),
         *      @ref BasicScreen::nextFartherScreen(),
         *      @ref BasicScreen::nextNearerScreen()
         */
        Containers::LinkedList<BasicScreen<Application>>& screens() {
            return static_cast<Containers::LinkedList<BasicScreen<Application>>&>(*this);
        }

        /** @overload */
        const Containers::LinkedList<BasicScreen<Application>>& screens() const {
            return static_cast<const Containers::LinkedList<BasicScreen<Application>>&>(*this);
        }

        #if defined(MAGNUM_BUILD_DEPRECATED) && !defined(DOXYGEN_GENERATING_OUTPUT)
        CORRADE_DEPRECATED("Platform::Screen::application() returns a reference now") BasicScreenedApplication<Application>* operator->() { return this; }
        CORRADE_DEPRECATED("Platform::Screen::application() returns a reference now") const BasicScreenedApplication<Application>* operator->() const { return this; }
        CORRADE_DEPRECATED("Platform::Screen::application() returns a reference now") BasicScreenedApplication<Application>& operator*() { return *this; }
        CORRADE_DEPRECATED("Platform::Screen::application() returns a reference now") const BasicScreenedApplication<Application>& operator*() const { return *this; }
        CORRADE_DEPRECATED("Platform::Screen::application() returns a reference now") operator BasicScreenedApplication<Application>*() { return this; }
        CORRADE_DEPRECATED("Platform::Screen::application() returns a reference now") operator const BasicScreenedApplication<Application>*() const { return this; }
        template<class T, class = typename std::enable_if<std::is_base_of<BasicScreenedApplication<Application>, T>::value>::type> CORRADE_DEPRECATED("Platform::Screen::application() returns a reference now") operator T*() { return static_cast<T*>(this); }
        template<class T, class = typename std::enable_if<std::is_base_of<BasicScreenedApplication<Application>, T>::value>::type> CORRADE_DEPRECATED("Platform::Screen::application() returns a reference now") operator const T*() const { return static_cast<const T*>(this); }
        CORRADE_DEPRECATED("Platform::Screen::application() returns a reference now, use hasApplication() instead") bool operator!() const { return false; }
        #endif

    protected:
        /* Nobody will need to have (and delete) ScreenedApplication*, thus
           this is faster than public pure virtual destructor */
        ~BasicScreenedApplication();

    private:
        /**
         * @brief Global viewport event
         *
         * Called when window size changes, *before* all screens'
         * @ref BasicScreen::viewportEvent() "viewportEvent()". Default
         * implementation does nothing. See @ref Sdl2Application::viewportEvent() "*Application::viewportEvent()"
         * for more information.
         */
        virtual void globalViewportEvent(typename Application::ViewportEvent& size);

        /**
         * @brief Before draw event
         * @m_since{2020,06}
         *
         * Called *before* all screens' @ref BasicScreen::drawEvent() "drawEvent()".
         * Unlike @ref globalDrawEvent() doesn't need to be implemented.
         */
        virtual void globalBeforeDrawEvent();

        /**
         * @brief Draw event
         *
         * Called *after* all screens' @ref BasicScreen::drawEvent() "drawEvent()".
         * You should call at least @ref Sdl2Application::swapBuffers() "swapBuffers()".
         * If you want to draw immediately again, call also
         * @ref Sdl2Application::redraw() "redraw()". See also
         * @ref globalBeforeDrawEvent().
         */
        virtual void globalDrawEvent() = 0;

        #ifndef DOXYGEN_GENERATING_OUTPUT /* https://bugzilla.gnome.org/show_bug.cgi?id=776986 */
        friend Containers::LinkedList<BasicScreen<Application>>;
        friend Containers::LinkedListItem<BasicScreen<Application>, BasicScreenedApplication<Application>>;
        friend BasicScreen<Application>;
        #endif
        /* The user is supposed to override only globalViewportEvent(),
           globalDrawEvent() and possibly globalBeforeDrawEvent(), these
           implementations are dispatching the events to attached screens. */
        void viewportEvent(typename Application::ViewportEvent& event) override final;
        void drawEvent() override final;
        void mousePressEvent(typename Application::MouseEvent& event) override final;
        void mouseReleaseEvent(typename Application::MouseEvent& event) override final;
        void mouseMoveEvent(typename Application::MouseMoveEvent& event) override final;

        /* These events are not available in all cases, so if the Application
           doesn't have them, they're overriding a mixin dummy */
        void keyPressEvent(typename BasicScreenedApplication<Application>::KeyEvent& event) override final;
        void keyReleaseEvent(typename BasicScreenedApplication<Application>::KeyEvent& event) override final;
        void mouseScrollEvent(typename BasicScreenedApplication<Application>::MouseScrollEvent& event) override final;
        void textInputEvent(typename BasicScreenedApplication<Application>::TextInputEvent& event) override final;
        void textEditingEvent(typename BasicScreenedApplication<Application>::TextEditingEvent& event) override final;
};

}}

#endif
