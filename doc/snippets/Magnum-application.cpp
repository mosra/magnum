/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
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

#include "Magnum/Platform/Sdl2Application.h"
#include "Magnum/Timeline.h"

using namespace Magnum;

#define DOXYGEN_ELLIPSIS(...) __VA_ARGS__
#define DOXYGEN_IGNORE(...) __VA_ARGS__

/* Wrapping in a namespace to not conflict with MyApplication defined in
   Platform.cpp */
namespace A {

class MyApplication: public Platform::Application {
    public:
        explicit MyApplication(const Arguments& arguments);

        void drawEvent();

    private:
        Timeline _timeline;
};

/* [Timeline-usage] */
MyApplication::MyApplication(const Arguments& arguments):
    Platform::Application{arguments, NoCreate}
{
    DOXYGEN_ELLIPSIS()

    // Enable VSync or set minimal loop period for the application, if
    // needed/applicable ...

    _timeline.start();
}

void MyApplication::drawEvent() {
    DOXYGEN_ELLIPSIS()

    // Distance of object traveling at speed of 15 units per second
    Float distance = 15.0f*_timeline.previousFrameDuration(); DOXYGEN_IGNORE(static_cast<void>(distance);)

    // Move an object, draw it ...

    swapBuffers();
    redraw();
    _timeline.nextFrame();
}
/* [Timeline-usage] */

}

/* To prevent macOS ranlib from complaining that there are no symbols. OTOH
   also make sure the name doesn't conflict with any other snippets to avoid
   linker warnings, AND unlike with `int main()` there now has to be a
   declaration to avoid -Wmisssing-prototypes */
void mainMagnum();
void mainMagnum() {}
