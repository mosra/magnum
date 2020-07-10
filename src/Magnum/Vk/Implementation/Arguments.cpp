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

#include "Arguments.h"

#include <Corrade/Utility/Arguments.h>

namespace Magnum { namespace Vk { namespace Implementation {

Utility::Arguments arguments() {
    Utility::Arguments args{"magnum"};
    args.addOption("disable-layers").setHelp("disable-layers", "Vulkan layers to disable", "LIST")
        .addOption("disable-extensions").setHelp("disable-extensions", "Vulkan instance or device extensions to disable", "LIST")
        .addOption("enable-layers").setHelp("enable-layers", "Vulkan layers to enable in addition to the defaults and what the application requests", "LIST")
        .addOption("enable-instance-extensions").setHelp("enable-instance-extensions", "Vulkan instance extensions to enable in addition to the defaults and what the application requests", "LIST")
        .addOption("enable-extensions").setHelp("enable-extensions", "Vulkan device extensions to enable in addition to the defaults and what the application requests", "LIST")
        .addOption("vulkan-version").setHelp("vulkan-version", "force Vulkan version", "X.Y")
        .addOption("log", "default").setHelp("log", "console logging", "default|quiet|verbose")
        .addOption("device").setHelp("device", "device to use", "ID|integrated|discrete|virtual|cpu")
        .setFromEnvironment("disable-layers")
        .setFromEnvironment("disable-extensions")
        .setFromEnvironment("enable-layers")
        .setFromEnvironment("enable-instance-extensions")
        .setFromEnvironment("enable-extensions")
        .setFromEnvironment("vulkan-version")
        .setFromEnvironment("log")
        .setFromEnvironment("device");
    return args;
}

}}}
