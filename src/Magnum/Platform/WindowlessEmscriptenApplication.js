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

var Module = typeof Module !== "undefined" ? Module : {};

Module.preRun = [];
Module.postRun = [];

Module.arguments = [];

Module.doNotCaptureKeyboard = true;

Module.printErr = function(message) {
    var log = document.getElementById('log');
    log.innerHTML += Array.prototype.slice.call(arguments).join(' ')
        .replace(/[\"&<>]/g, function (a) {
            return { '"': '&quot;', '&': '&amp;', '<': '&lt;', '>': '&gt;' }[a];
        }) + '\n';
};

Module.print = function(message) {
    var log = document.getElementById('log');
    log.innerHTML += Array.prototype.slice.call(arguments).join(' ')
        .replace(/[\"&<>]/g, function (a) {
            return { '"': '&quot;', '&': '&amp;', '<': '&lt;', '>': '&gt;' }[a];
        }) + '\n';
};

/* onAbort not handled here, as the output is printed directly on the page */

Module.canvas = document.getElementById('canvas');
Module.status = document.getElementById('status');
Module.statusDescription = document.getElementById('status-description');
Module.log = document.getElementById('log');

Module.setStatus = function(message) {
    if(Module.status) Module.status.innerHTML = message;
};

Module.setStatusDescription = function(message) {
    if(Module.statusDescription)
        Module.statusDescription.innerHTML = message;
};

Module.totalDependencies = 0;

Module.monitorRunDependencies = function(left) {
    this.totalDependencies = Math.max(this.totalDependencies, left);

    if(left) {
        Module.setStatus('Downloading...');
        Module.setStatusDescription((this.totalDependencies - left) + ' / ' + this.totalDependencies);
    } else {
        Module.setStatus('Download complete');
        Module.setStatusDescription('');
        Module.log.style.display = 'block';
    }
};

/* Parse arguments, e.g. /app/?foo=bar&fizz&buzz=3 goes to the app as
   ['--foo', 'bar', '--fizz', '--buzz', '3'] */
var args = decodeURIComponent(window.location.search.substr(1)).trim().split('&');
for(var i = 0; i != args.length; ++i) {
    var j = args[i].indexOf('=');
    /* Key + value */
    if(j != -1) {
        Module.arguments.push('--' + args[i].substring(0, j));
        Module.arguments.push(args[i].substring(j + 1));

    /* Just key */
    } else Module.arguments.push('--' + args[i]);
}

Module.setStatus('Downloading...');
Module.log.style.display = 'none';
