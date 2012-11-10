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
        inline void redraw() {
            flags |= Flag::Redraw;
        }

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

See NaClApplication for more information.
*/
/* look at that insane placement of __attribute__. WTF. */
#define MAGNUM_NACLAPPLICATION_MAIN(application)                            \
    namespace pp {                                                          \
        Module __attribute__ ((visibility ("default"))) * CreateModule() {  \
            return new Magnum::Platform::Implementation::NaClModule<application>(); \
        }                                                                   \
    }

}}

#endif
