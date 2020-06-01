#ifndef Magnum_Platform_Implementation_AbstractContextHandler_h
#define Magnum_Platform_Implementation_AbstractContextHandler_h
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

namespace Magnum { namespace Platform { namespace Implementation {

/**
@brief Base for OpenGL context handlers

@todo GLX_MESA_query_renderer, EGL_MESA_query_renderer
*/
template<class Configuration, class Display, class VisualId, class Window> class AbstractContextHandler {
    public:
        /**
         * @brief Get visual ID
         *
         * Initializes the handler on given display and returns visual ID.
         */
        virtual VisualId getVisualId(Display nativeDisplay) = 0;

        explicit AbstractContextHandler() = default;

        /**
         * @brief Destructor
         *
         * Finalizes and closes the handler.
         */
        virtual ~AbstractContextHandler() {}

        /** @brief Create context */
        virtual void createContext(const Configuration& configuration, Window nativeWindow) = 0;

        /** @brief Make the context current */
        virtual void makeCurrent() = 0;

        /** @brief Swap buffers */
        virtual void swapBuffers() = 0;
};

}}}

#endif
