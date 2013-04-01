#ifndef Magnum_Platform_NaClApplication_h
#define Magnum_Platform_NaClApplication_h
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

/** @file
 * @brief Class Magnum::Platform::NaClApplication
 */

#include <string>
#include <Containers/EnumSet.h>
#include <ppapi/cpp/input_event.h>
#include <ppapi/cpp/instance.h>
#include <ppapi/cpp/module.h>
#include <ppapi/cpp/mouse_lock.h>
#include <ppapi/cpp/graphics_3d_client.h>
#include <ppapi/gles2/gl2ext_ppapi.h>

#include "Math/Vector2.h"
#include "Magnum.h"

#include "corradeCompatibility.h"

namespace pp {
    class Graphics3D;
    class Fullscreen;
}

namespace Magnum { namespace Platform {

/** @nosubgrouping
@brief NaCl application

Application running in [Google Chrome Native Client](https://developers.google.com/native-client/).
Supports keyboard and mouse handling. See @ref platform for brief introduction.

@section NaClApplication-usage Usage

You need to implement at least drawEvent() and viewportEvent() to be able to
draw on the screen. The subclass must be then registered to NaCl API using
MAGNUM_NACLAPPLICATION_MAIN() macro.
@code
class MyApplication: public Magnum::Platform::Sdl2Application {
    // implement required methods...
};
MAGNUM_NACLAPPLICATION_MAIN(MyApplication)
@endcode

If no other application header is included this class is also aliased to
`Platform::Application` and the macro is aliased to `MAGNUM_APPLICATION_MAIN()`
to simplify porting.
*/
class NaClApplication: public pp::Instance, public pp::Graphics3DClient, public pp::MouseLock {
    public:
        /** @brief Application arguments */
        typedef PP_Instance Arguments;

        class Configuration;
        class InputEvent;
        class KeyEvent;
        class MouseEvent;
        class MouseMoveEvent;

        /**
         * @brief Default constructor
         * @param arguments     Application arguments
         *
         * Creates application with default configuration. See Configuration
         * for more information.
         */
        explicit NaClApplication(const Arguments& arguments);

        /**
         * @brief Constructor
         * @param arguments     Application arguments
         * @param configuration Configuration
         *
         * The @p configuration is deleted afterwards. If `nullptr` is passed
         * as @p configuration, the context is not created and must be created
         * with createContext().
         */
        explicit NaClApplication(const Arguments& arguments, Configuration* configuration);

        ~NaClApplication();

        /** @brief Whether the application runs fullscreen */
        bool isFullscreen();

        /**
         * @brief Set fullscreen
         * @return `False` if switch to opposite mode is in progress or if the
         *      switch is not possible, `true` otherwise.
         *
         * The switch is done asynchronously, during the switch no event
         * processing is done.
         */
        bool setFullscreen(bool enabled);

    protected:
        /** @copydoc GlutApplication::createContext() */
        void createContext(Configuration* configuration);

        /** @{ @name Drawing functions */

        /** @copydoc GlutApplication::viewportEvent() */
        virtual void viewportEvent(const Vector2i& size) = 0;

        /** @copydoc GlutApplication::drawEvent() */
        virtual void drawEvent() = 0;

        /** @copydoc GlutApplication::swapBuffers() */
        void swapBuffers();

        /** @copydoc GlutApplication::redraw() */
        inline void redraw() { flags |= Flag::Redraw; }

        /*@}*/

        /** @{ @name Keyboard handling */

        /**
         * @brief Key press event
         *
         * Called when an key is pressed. Default implementation does nothing.
         * If you accept the event, call @ref InputEvent::setAccepted() "setAccepted()"
         * on it, otherwise the event will be propagated to the browser.
         */
        virtual void keyPressEvent(KeyEvent& event);

        /**
         * @brief Key release event
         *
         * Called when an key is released. Default implementation does nothing.
         * If you accept the event, call @ref InputEvent::setAccepted() "setAccepted()"
         * on it, otherwise the event will be propagated to the browser.
         */
        virtual void keyReleaseEvent(KeyEvent& event);

        /*@}*/

        /** @{ @name Mouse handling */

    public:
        /** @brief Whether mouse is locked */
        inline bool isMouseLocked() const {
            return flags & Flag::MouseLocked;
        }

        /**
         * @brief Enable or disable mouse locking
         *
         * When mouse is locked, the cursor is hidden and only
         * MouseMoveEvent::relativePosition() is changing, absolute position
         * stays the same.
         */
        void setMouseLocked(bool enabled);

    protected:
        /**
         * @brief Mouse press event
         *
         * Called when mouse button is pressed. Default implementation does
         * nothing. If you accept the event, call @ref InputEvent::setAccepted() "setAccepted()"
         * on it, otherwise the event will be propagated to the browser.
         */
        virtual void mousePressEvent(MouseEvent& event);

        /**
         * @brief Mouse release event
         *
         * Called when mouse button is released. Default implementation does
         * nothing. If you accept the event, call @ref InputEvent::setAccepted() "setAccepted()"
         * on it, otherwise the event will be propagated to the browser.
         */
        virtual void mouseReleaseEvent(MouseEvent& event);

        /**
         * @brief Mouse move event
         *
         * Called when mouse is moved. Default implementation does nothing. If
         * you accept the event, call @ref InputEvent::setAccepted() "setAccepted()"
         * on it, otherwise the event will be propagated to the browser.
         */
        virtual void mouseMoveEvent(MouseMoveEvent& event);

        /*@}*/

    private:
        enum class Flag: UnsignedByte {
            ViewportUpdated = 1 << 0,
            SwapInProgress = 1 << 1,
            Redraw = 1 << 2,
            FullscreenSwitchInProgress = 1 << 3,
            WillBeFullscreen = 1 << 4,
            MouseLocked = 1 << 5
        };
        typedef Corrade::Containers::EnumSet<Flag, UnsignedByte> Flags;

        inline void Graphics3DContextLost() override {
            CORRADE_ASSERT(false, "NaClApplication: context unexpectedly lost", );
        }

        inline void MouseLockLost() override {
            flags &= ~Flag::MouseLocked;
        }

        void DidChangeView(const pp::View& view) override;

        bool HandleInputEvent(const pp::InputEvent& event) override;

        static void swapCallback(void* applicationInstance, std::int32_t);
        static void mouseLockCallback(void* applicationInstance, std::int32_t);

        pp::Graphics3D* graphics;
        pp::Fullscreen* fullscreen;
        Context* c;
        Vector2i viewportSize;
        Flags flags;

        CORRADE_ENUMSET_FRIEND_OPERATORS(Flags)
};

/**
@brief %Configuration

Double-buffered RGBA canvas with depth and stencil buffers.
@see NaClApplication(), createContext()
*/
class NaClApplication::Configuration {
    Configuration(const Configuration&) = delete;
    Configuration(Configuration&&) = delete;
    Configuration& operator=(const Configuration&) = delete;
    Configuration& operator=(Configuration&&) = delete;

    public:
        explicit Configuration();
        ~Configuration();

        /** @brief Window size */
        inline Vector2i size() const { return _size; }

        /**
         * @brief Set window size
         * @return Pointer to self (for method chaining)
         *
         * Default is `{640, 480}`.
         */
        inline Configuration* setSize(const Vector2i& size) {
            _size = size;
            return this;
        }

    private:
        Vector2i _size;
};

/**
@brief Base for input events

If you accept the event, call setAccepted(), otherwise the event will be
propagated to the browser.
@see KeyEvent, MouseEvent, MouseMoveEvent, keyPressEvent(), keyReleaseEvent(),
    mousePressEvent(), mouseReleaseEvent(), mouseMoveEvent()
*/
class NaClApplication::InputEvent {
    InputEvent(const InputEvent&) = delete;
    InputEvent(InputEvent&&) = delete;
    InputEvent& operator=(const InputEvent&) = delete;
    InputEvent& operator=(InputEvent&&) = delete;

    public:
        /**
         * @brief %Modifier
         *
         * @todo AltGr + PP_INPUTEVENT_MODIFIER_ISKEYPAD, PP_INPUTEVENT_MODIFIER_ISAUTOREPEAT
         * @see Modifiers, modifiers()
         */
        enum class Modifier: std::uint32_t {
            Shift = PP_INPUTEVENT_MODIFIER_SHIFTKEY,    /**< Shift */
            Ctrl = PP_INPUTEVENT_MODIFIER_CONTROLKEY,   /**< Ctrl */
            Alt = PP_INPUTEVENT_MODIFIER_ALTKEY,        /**< Alt */
            Meta = PP_INPUTEVENT_MODIFIER_METAKEY,      /**< Meta */

            LeftButton = PP_INPUTEVENT_MODIFIER_LEFTBUTTONDOWN,     /**< Left mouse button */
            MiddleButton = PP_INPUTEVENT_MODIFIER_MIDDLEBUTTONDOWN, /**< Middle mouse button */
            RightButton = PP_INPUTEVENT_MODIFIER_RIGHTBUTTONDOWN,   /**< Right mouse button */

            CapsLock = PP_INPUTEVENT_MODIFIER_CAPSLOCKKEY,  /**< Caps lock */
            NumLock = PP_INPUTEVENT_MODIFIER_NUMLOCKKEY     /**< Num lock */
        };

        /**
         * @brief Set of modifiers
         *
         * @see modifiers()
         */
        typedef Corrade::Containers::EnumSet<Modifier, std::uint32_t> Modifiers;

        inline virtual ~InputEvent() {}

        /** @brief Modifiers */
        inline Modifiers modifiers() const { return _modifiers; }

        /**
         * @brief Set event as accepted
         *
         * If the event is ignored (i.e., not set as accepted), it is
         * propagated elsewhere (e.g. to the browser). By default is each
         * event ignored.
         */
        inline void setAccepted(bool accepted = true) { _accepted = accepted; }

        /** @brief Whether the event is accepted */
        inline bool isAccepted() { return _accepted; }

    #ifndef DOXYGEN_GENERATING_OUTPUT
    protected:
        inline InputEvent(Modifiers modifiers): _accepted(false), _modifiers(modifiers) {}
    #endif

    private:
        bool _accepted;
        const Modifiers _modifiers;
};

/**
@brief Key event

See InputEvent for more information.
@see keyPressEvent(), keyReleaseEvent()
*/
class NaClApplication::KeyEvent: public NaClApplication::InputEvent {
    friend class NaClApplication;

    public:
        /**
         * @brief Key
         *
         * @todo Slash, percent, equal to be compatible with *XApplication
         * @see key()
         */
        enum class Key: std::uint32_t {
            Enter = 0x0D,               /**< Enter */
            Esc = 0x1B,                 /**< Escape */

            Up = 0x26,                  /**< Up arrow */
            Down = 0x28,                /**< Down arrow */
            Left = 0x25,                /**< Left arrow */
            Right = 0x27,               /**< Right arrow */
            F1 = 0x70,                  /**< F1 */
            F2 = 0x71,                  /**< F2 */
            F3 = 0x72,                  /**< F3 */
            F4 = 0x73,                  /**< F4 */
            F5 = 0x74,                  /**< F5 */
            F6 = 0x75,                  /**< F6 */
            F7 = 0x76,                  /**< F7 */
            F8 = 0x77,                  /**< F8 */
            F9 = 0x78,                  /**< F9 */
            F10 = 0x79,                 /**< F10 */
            F11 = 0x7A,                 /**< F11 */
            F12 = 0x7B,                 /**< F12 */
            Home = 0x24,                /**< Home */
            End = 0x23,                 /**< End */
            PageUp = 0x21,              /**< Page up */
            PageDown = 0x22,            /**< Page down */

            Space = 0x20,               /**< Space */
            Comma = 0xBC,               /**< Comma */
            Period = 0xBE,              /**< Period */
            Minus = 0xBD,               /**< Minus */
            Plus = 0xBB,                /**< Plus */

            Zero = '0',                 /**< Zero */
            One = '1',                  /**< One */
            Two = '2',                  /**< Two */
            Three = '3',                /**< Three */
            Four = '4',                 /**< Four */
            Five = '5',                 /**< Five */
            Six = '6',                  /**< Six */
            Seven = '7',                /**< Seven */
            Eight = '8',                /**< Eight */
            Nine = '9',                 /**< Nine */

            A = 'A',                    /**< Letter A */
            B = 'B',                    /**< Letter B */
            C = 'C',                    /**< Letter C */
            D = 'D',                    /**< Letter D */
            E = 'E',                    /**< Letter E */
            F = 'F',                    /**< Letter F */
            G = 'G',                    /**< Letter G */
            H = 'H',                    /**< Letter H */
            I = 'I',                    /**< Letter I */
            J = 'J',                    /**< Letter J */
            K = 'K',                    /**< Letter K */
            L = 'L',                    /**< Letter L */
            M = 'M',                    /**< Letter M */
            N = 'N',                    /**< Letter N */
            O = 'O',                    /**< Letter O */
            P = 'P',                    /**< Letter P */
            Q = 'Q',                    /**< Letter Q */
            R = 'R',                    /**< Letter R */
            S = 'S',                    /**< Letter S */
            T = 'T',                    /**< Letter T */
            U = 'U',                    /**< Letter U */
            V = 'V',                    /**< Letter V */
            W = 'W',                    /**< Letter W */
            X = 'X',                    /**< Letter X */
            Y = 'Y',                    /**< Letter Y */
            Z = 'Z'                     /**< Letter Z */
        };

        /** @brief Key */
        inline Key key() const { return _key; }

    private:
        inline KeyEvent(Key key, Modifiers modifiers): InputEvent(modifiers), _key(key) {}

        const Key _key;
};

/**
@brief Mouse event

See InputEvent for more information.
@see MouseMoveEvent, mousePressEvent(), mouseReleaseEvent()
*/
class NaClApplication::MouseEvent: public NaClApplication::InputEvent {
    friend class NaClApplication;

    public:
        /**
         * @brief Button
         *
         * @see button()
         */
        enum class Button: unsigned int {
            Left = PP_INPUTEVENT_MOUSEBUTTON_LEFT,      /**< Left button */
            Middle = PP_INPUTEVENT_MOUSEBUTTON_MIDDLE,  /**< Middle button */
            Right = PP_INPUTEVENT_MOUSEBUTTON_RIGHT     /**< Right button */
        };

        /** @brief Button */
        inline Button button() const { return _button; }

        /** @brief Position */
        inline Vector2i position() const { return _position; }

    private:
        inline MouseEvent(Button button, const Vector2i& position, Modifiers modifiers): InputEvent(modifiers), _button(button), _position(position) {}

        const Button _button;
        const Vector2i _position;
};

/**
@brief Mouse move event

See InputEvent for more information.
@see MouseEvent, mouseMoveEvent()
*/
class NaClApplication::MouseMoveEvent: public NaClApplication::InputEvent {
    friend class NaClApplication;

    public:
        /** @brief Position */
        inline Vector2i position() const { return _position; }

        /**
         * @brief Relative position
         *
         * Position relative to previous event.
         */
        inline Vector2i relativePosition() const { return _relativePosition; }

    private:
        inline MouseMoveEvent(const Vector2i& position, const Vector2i& relativePosition, Modifiers modifiers): InputEvent(modifiers), _position(position), _relativePosition(relativePosition) {}

        const Vector2i _position, _relativePosition;
};

CORRADE_ENUMSET_OPERATORS(NaClApplication::Flags)

#ifndef DOXYGEN_GENERATING_OUTPUT
namespace Implementation {
    template<class Application> class NaClModule: public pp::Module {
        public:
            inline ~NaClModule() {
                glTerminatePPAPI();
            }

            inline bool Init() override {
                return glInitializePPAPI(get_browser_interface());
            }

            inline pp::Instance* CreateInstance(PP_Instance instance) {
                return new Application(instance);
            }
    };
}
#endif

/** @hideinitializer
@brief Entry point for NaCl application
@param application  Application class name

See NaClApplication and @ref portability-applications for more information.
When no other application header is included this macro is also aliased to
`MAGNUM_APPLICATION_MAIN()`.
*/
/* look at that insane placement of __attribute__. WTF. */
#define MAGNUM_NACLAPPLICATION_MAIN(application)                            \
    namespace pp {                                                          \
        Module __attribute__ ((visibility ("default"))) * CreateModule();   \
        Module __attribute__ ((visibility ("default"))) * CreateModule() {  \
            return new Magnum::Platform::Implementation::NaClModule<application>(); \
        }                                                                   \
    }

#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef MAGNUM_APPLICATION_MAIN
typedef NaClApplication Application;
#define MAGNUM_APPLICATION_MAIN(className) MAGNUM_NACLAPPLICATION_MAIN(className)
#else
#undef MAGNUM_APPLICATION_MAIN
#endif
#endif

CORRADE_ENUMSET_OPERATORS(NaClApplication::InputEvent::Modifiers)

/* Implementations for inline functions with unused parameters */
inline void NaClApplication::keyPressEvent(KeyEvent&) {}
inline void NaClApplication::keyReleaseEvent(KeyEvent&) {}
inline void NaClApplication::mousePressEvent(MouseEvent&) {}
inline void NaClApplication::mouseReleaseEvent(MouseEvent&) {}
inline void NaClApplication::mouseMoveEvent(MouseMoveEvent&) {}

}}

#endif
