#ifndef Magnum_Platform_NaClApplication_h
#define Magnum_Platform_NaClApplication_h
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

/** @file
 * @brief Class Magnum::Platform::NaClApplication
 */

#include <string>
#include <Containers/EnumSet.h>
#include <ppapi/cpp/input_event.h>
#include <ppapi/cpp/instance.h>
#include <ppapi/cpp/instance_handle.h>
#include <ppapi/cpp/module.h>
#include <ppapi/cpp/graphics_3d_client.h>
#include <ppapi/gles2/gl2ext_ppapi.h>

#include "Math/Vector2.h"
#include "Magnum.h"

#include "magnumCompatibility.h"

namespace pp {
    class Graphics3D;
}

namespace Magnum {

class Context;

namespace Platform {

/** @nosubgrouping
@brief NaCl application

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
*/
class NaClApplication: public pp::Instance, public pp::Graphics3DClient {
    public:
        /**
         * @brief Constructor
         * @param instance  Module instance
         * @param size      Rendering size
         *
         * Creates double-buffered RGBA canvas with depth and stencil buffers.
         */
        explicit NaClApplication(PP_Instance instance, const Math::Vector2<GLsizei>& size = Math::Vector2<GLsizei>(640, 480));

        ~NaClApplication();

        /** @{ @name Drawing functions */

    protected:
        /**
         * @brief Viewport event
         *
         * Called when viewport size changes. You should pass the new size to
         * Framebuffer::setViewport() or SceneGraph::Camera::setViewport(),
         * if using scene graph.
         */
        virtual void viewportEvent(const Math::Vector2<GLsizei>& size) = 0;

        /**
         * @brief Draw event
         *
         * Here implement your drawing functions, such as calling
         * SceneGraph::Camera::draw(). After drawing is finished, call
         * swapBuffers(). If you want to draw immediately again, call also
         * redraw().
         */
        virtual void drawEvent() = 0;

        /**
         * @brief Swap buffers
         *
         * Paints currently rendered framebuffer on screen.
         */
        void swapBuffers();

        /**
         * @brief Redraw immediately
         *
         * Marks the window for redrawing, resulting in call of drawEvent()
         * in the next iteration.
         */
        inline void redraw() { flags |= Flag::Redraw; }

        /*@}*/

        /** @{ @name Keyboard handling */

    public:
        /**
         * @brief %Modifier
         *
         * @todo AltGr + PP_INPUTEVENT_MODIFIER_ISKEYPAD, PP_INPUTEVENT_MODIFIER_ISAUTOREPEAT
         * @see Modifiers, keyPressEvent(), keyReleaseEvent()
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
         * @see keyPressEvent(), keyReleaseEvent()
         */
        typedef Corrade::Containers::EnumSet<Modifier, std::uint32_t> Modifiers;

        /**
         * @brief Key
         *
         * @todo Slash, percent, equal to be compatible with *XApplication
         * @see keyPressEvent(), keyReleaseEvent()
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

    protected:
        /**
         * @brief Key press event
         * @param key       Key pressed
         * @param modifiers Active modifiers
         * @param position  Cursor position (not implemented)
         *
         * Called when an key is pressed. Default implementation does nothing.
         */
        virtual void keyPressEvent(Key key, Modifiers modifiers, const Math::Vector2<int>& position);

        /**
         * @brief Key release event
         * @param key       Key released
         * @param modifiers Active modifiers
         * @param position  Cursor position (not implemented)
         */
        virtual void keyReleaseEvent(Key key, Modifiers modifiers, const Math::Vector2<int>& position);

        /*@}*/

    private:
        enum class Flag: std::uint8_t {
            ViewportUpdated = 1 << 0,
            SwapInProgress = 1 << 1,
            Redraw = 1 << 2
        };
        typedef Corrade::Containers::EnumSet<Flag, std::uint8_t> Flags;

        inline void Graphics3DContextLost() override {
            CORRADE_ASSERT(false, "NaClApplication: context unexpectedly lost", );
        }

        void DidChangeView(const pp::View& view) override;

        bool HandleInputEvent(const pp::InputEvent& event) override;

        static void swapCallback(void* applicationInstance, std::int32_t);

        pp::Graphics3D* graphics;
        Context* c;
        Math::Vector2<GLsizei> viewportSize;
        Flags flags;

        CORRADE_ENUMSET_FRIEND_OPERATORS(Flags)
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
                return glInitializePPAPI(get_browser_interface()) == GL_TRUE;
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
        Module __attribute__ ((visibility ("default"))) * CreateModule() {  \
            return new Magnum::Platform::Implementation::NaClModule<application>(); \
        }                                                                   \
    }

#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef MAGNUM_APPLICATION_MAIN
#define MAGNUM_APPLICATION_MAIN(className) MAGNUM_NACLAPPLICATION_MAIN(className)
#else
#undef MAGNUM_APPLICATION_MAIN
#endif
#endif

/* Implementations for inline functions with unused parameters */
inline void NaClApplication::keyPressEvent(Key, Modifiers, const Math::Vector2<int>&) {}
inline void NaClApplication::keyReleaseEvent(Key, Modifiers, const Math::Vector2<int>&) {}

}}

#endif
