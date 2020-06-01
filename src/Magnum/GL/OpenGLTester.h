#ifndef Magnum_GL_OpenGLTester_h
#define Magnum_GL_OpenGLTester_h
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
 * @brief Class @ref Magnum::GL::OpenGLTester, macro @ref MAGNUM_VERIFY_NO_GL_ERROR()
 */

#include "Magnum/configure.h"

#ifdef MAGNUM_TARGET_GL
#include <Corrade/TestSuite/Tester.h>

#include "Magnum/GL/Renderer.h"
#include "Magnum/GL/TimeQuery.h"

#if defined(MAGNUM_TARGET_HEADLESS) || defined(CORRADE_TARGET_EMSCRIPTEN) || defined(CORRADE_TARGET_ANDROID)
#include "Magnum/Platform/WindowlessEglApplication.h"
#elif defined(CORRADE_TARGET_IOS)
#include "Magnum/Platform/WindowlessIosApplication.h"
#elif defined(CORRADE_TARGET_APPLE)
#include "Magnum/Platform/WindowlessCglApplication.h"
#elif defined(CORRADE_TARGET_UNIX)
#if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_DESKTOP_GLES)
#include "Magnum/Platform/WindowlessEglApplication.h"
#else
#include "Magnum/Platform/WindowlessGlxApplication.h"
#endif
#elif defined(CORRADE_TARGET_WINDOWS)
#if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_DESKTOP_GLES)
#include "Magnum/Platform/WindowlessWglApplication.h"
#else
#include "Magnum/Platform/WindowlessWindowsEglApplication.h"
#endif
#else
#error cannot run OpenGL tests on this platform
#endif

namespace Magnum { namespace GL {

/**
@brief Base class for OpenGL tests and benchmarks

Extends @ref Corrade::TestSuite::Tester with features for OpenGL testing and
benchmarking. Be sure to read its documentation first to have an overview of
the base features.

This class is available on platforms with corresponding
`Platform::Windowless*Application` implementation, which currently means all
platforms. It is built into a separate static library and only if
`WITH_OPENGLTESTER` is enabled when building Magnum. To use it with CMake,
request the `OpenGLTester` component of the `Magnum` package. Derive
your test class from this class instead of @ref Corrade::TestSuite::Tester and
either link to `Magnum::OpenGLTester` target or add it to the `LIBRARIES`
section of the @ref corrade-cmake-add-test "corrade_add_test()" macro:

@code{.cmake}
find_package(Magnum REQUIRED OpenGLTester)

# ...
corrade_add_test(YourTest YourTest.cpp LIBRARIES Magnum::OpenGLTester)
@endcode

Additionally, if you're using Magnum as a CMake subproject, ensure it's enabled
as it's not built by default:

@code{.cmake}
set(WITH_OPENGLTESTER ON CACHE BOOL "" FORCE)
add_subdirectory(magnum EXCLUDE_FROM_ALL)
@endcode

See @ref building, @ref cmake and @ref testsuite for more information.

@section GL-OpenGLTester-running Running the test executables

Implicitly, running the test executables requires presence of a GPU with OpenGL
drivers. In addition, on desktop, unless Magnum is built with
`MAGNUM_TARGET_HEADLESS`, OpenGL context creation requires a graphical desktop
to be running. On embedded systems (and @ref CORRADE_TARGET_IOS "iOS",
@ref CORRADE_TARGET_ANDROID "Android" in particular) running the tests has no
special requirements. On Emscripten the tests have to be running in a browser,
as Node.js environment doesn't provide a WebGL context.

On virtualized systems and systems without a GPU (e.g. CI servers) it's
possible to link against e.g. Mesa softpipe or
[SwiftShader](https://github.com/google/swiftshader), but be prepared to expect
reduced performance, reduced feature set and possible non-comformant behavior
on such drivers.

See @ref TestSuite-Tester-running for more information about running the tests
on particular platforms.

@section GL-OpenGLTester-context OpenGL context creation

Upon construction the class creates an OpenGL context, meaning you don't have
to worry about OpenGL context being available during the tests. If the context
creation fails, the test executable exits with non-zero return code. The tester
uses a single OpenGL context for all test cases, meaning you can share
precalculated state among test cases, but on the other hand potential OpenGL
misuses will propagate to following test cases. See
@ref TestSuite-Tester-command-line "command-line option overview" for a way
to run single isolated test cases.

@section GL-OpenGLTester-debug Debug context and error checking

Because @ref DebugOutput can be quite spammy in some cases, especially when
compiling more complex shaders or doing benchmarks, it's not implicitly enabled
by default to make the test output more readable. Instead of relying on debug
output to report errors, the @ref MAGNUM_VERIFY_NO_GL_ERROR() macro should be
used to reliably check for errors regardless of platform support. For easier
debugging of OpenGL errors users are encuraged to use the
`--magnum-gpu-validation` @ref GL-Context-command-line "command line option",
which is supported here as well as in all other application implementations.

@section GL-OpenGLTester-benchmarks GPU time benchmarks

This class adds @ref BenchmarkType::GpuTime to the benchmark type enum,
allowing you to measure time spent on GPU as opposed to CPU or wall clock time.
If @gl_extension{ARB,timer_query} desktop extension (part of OpenGL 3.3),
@gl_extension{EXT,disjoint_timer_query} OpenGL ES extension,
@webgl_extension{EXT,disjoint_timer_query} WebGL 1 extension or
@gl_extension{EXT,disjoint_timer_query_webgl2} WebGL 2 extension is not
available, GPU time benchmarks will get automatically skipped, producing a
@cb{.ansi} [1;39mSKIP @ce message on the output.

@note This class is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL enabled (done by default). See @ref building-features
    for more information.
*/
class OpenGLTester: public TestSuite::Tester {
    public:
        /**
         * @brief Benchmark type
         *
         * Extends @ref Corrade::TestSuite::Tester::BenchmarkType with GPU
         * benchmark types.
         * @see @ref addBenchmarks(), @ref addInstancedBenchmarks()
         */
        enum class BenchmarkType {
            /**
             * See @ref Corrade::TestSuite::Tester::BenchmarkType::Default for
             * details.
             */
            Default = Int(Tester::BenchmarkType::Default),

            /**
             * See @ref Corrade::TestSuite::Tester::BenchmarkType::WallTime for
             * details.
             */
            WallTime = Int(Tester::BenchmarkType::WallTime),

            /**
             * See @ref Corrade::TestSuite::Tester::BenchmarkType::CpuTime for
             * details.
             */
            CpuTime = Int(Tester::BenchmarkType::CpuTime),

            /**
             * See @ref Corrade::TestSuite::Tester::BenchmarkType::CpuCycles
             * for details.
             */
            CpuCycles = Int(Tester::BenchmarkType::CpuCycles),

            /**
             * GPU time, measured using @ref TimeQuery::Target::TimeElapsed.
             * Note that the result of the query is retrieved synchronously and
             * thus may cause a pipeline bubble. Increase number of iterations
             * passed to @ref CORRADE_BENCHMARK() to amortize the measurement
             * error.
             *
             * If @gl_extension{ARB,timer_query} desktop extension (part of
             * OpenGL 3.3), @gl_extension{EXT,disjoint_timer_query} OpenGL ES
             * extension, @webgl_extension{EXT,disjoint_timer_query} WebGL 1
             * extension or @gl_extension{EXT,disjoint_timer_query_webgl2}
             * WebGL 2 extension is not available, GPU time benchmarks will get
             * automatically skipped.
             */
            GpuTime = 32
        };

        /**
         * @brief Constructor
         *
         * Creates an OpenGL context.
         */
        explicit OpenGLTester();

        ~OpenGLTester();

        /**
         * @brief Add benchmarks
         *
         * Extends @ref Corrade::TestSuite::Tester::addBenchmarks(std::initializer_list<void(Derived::*)()>, std::size_t, BenchmarkType) with support
         * for GPU benchmark types.
         *
         * If @gl_extension{ARB,timer_query} desktop extension (part of OpenGL
         * 3.3), @gl_extension{EXT,disjoint_timer_query} OpenGL ES extension,
         * @webgl_extension{EXT,disjoint_timer_query} WebGL 1 extension or
         * @gl_extension{EXT,disjoint_timer_query_webgl2} WebGL 2 extension is
         * not available, @ref BenchmarkType::GpuTime benchmarks will get
         * automatically skipped.
         */
        template<class Derived> void addBenchmarks(std::initializer_list<void(Derived::*)()> benchmarks, std::size_t batchCount, BenchmarkType benchmarkType = BenchmarkType::Default) {
            if(benchmarkType == BenchmarkType::GpuTime)
                addCustomBenchmarks<Derived>(benchmarks, batchCount, &OpenGLTester::gpuTimeBenchmarkBegin, &OpenGLTester::gpuTimeBenchmarkEnd, BenchmarkUnits::Nanoseconds);
            else
                Tester::addBenchmarks(benchmarks, batchCount, Tester::BenchmarkType(Int(benchmarkType)));
        }

        /**
         * @brief Add benchmarks with explicit setup and teardown functions
         *
         * Extends @ref Corrade::TestSuite::Tester::addBenchmarks(std::initializer_list<void(Derived::*)()>, std::size_t, void(Derived::*)(), void(Derived::*)(), BenchmarkType) with support
         * for GPU benchmark types.
         *
         * If @gl_extension{ARB,timer_query} desktop extension (part of OpenGL
         * 3.3), @gl_extension{EXT,disjoint_timer_query} OpenGL ES extension,
         * @webgl_extension{EXT,disjoint_timer_query} WebGL 1 extension or
         * @gl_extension{EXT,disjoint_timer_query_webgl2} WebGL 2 extension is
         * not available, @ref BenchmarkType::GpuTime benchmarks will get
         * automatically skipped.
         */
        template<class Derived> void addBenchmarks(std::initializer_list<void(Derived::*)()> benchmarks, std::size_t batchCount, void(Derived::*setup)(), void(Derived::*teardown)(), BenchmarkType benchmarkType = BenchmarkType::Default) {
            if(benchmarkType == BenchmarkType::GpuTime)
                addCustomBenchmarks<Derived>(benchmarks, batchCount, &OpenGLTester::gpuTimeBenchmarkBegin, &OpenGLTester::gpuTimeBenchmarkEnd, setup, teardown, BenchmarkUnits::Nanoseconds);
            else
                Tester::addBenchmarks(benchmarks, batchCount, setup, teardown, Tester::BenchmarkType(Int(benchmarkType)));
        }

        /**
         * @brief Add instanced benchmarks
         *
         * Extends @ref Corrade::TestSuite::Tester::addInstancedBenchmarks(std::initializer_list<void(Derived::*)()>, std::size_t, std::size_t, BenchmarkType) with support for GPU
         * benchmark types.
         *
         * If @gl_extension{ARB,timer_query} desktop extension (part of OpenGL
         * 3.3), @gl_extension{EXT,disjoint_timer_query} OpenGL ES extension,
         * @webgl_extension{EXT,disjoint_timer_query} WebGL 1 extension or
         * @gl_extension{EXT,disjoint_timer_query_webgl2} WebGL 2 extension is
         * not available, @ref BenchmarkType::GpuTime benchmarks will get
         * automatically skipped.
         */
        template<class Derived> void addInstancedBenchmarks(std::initializer_list<void(Derived::*)()> benchmarks, std::size_t batchCount, std::size_t instanceCount, BenchmarkType benchmarkType = BenchmarkType::Default) {
            if(benchmarkType == BenchmarkType::GpuTime)
                addCustomInstancedBenchmarks<Derived>(benchmarks, batchCount, instanceCount, &OpenGLTester::gpuTimeBenchmarkBegin, &OpenGLTester::gpuTimeBenchmarkEnd, BenchmarkUnits::Nanoseconds);
            else
                Tester::addInstancedBenchmarks(benchmarks, batchCount, instanceCount, Tester::BenchmarkType(Int(benchmarkType)));
        }

        /**
         * @brief Add instanced benchmarks with explicit setup and teardown functions
         *
         * Extends @ref Corrade::TestSuite::Tester::addInstancedBenchmarks(std::initializer_list<void(Derived::*)()>, std::size_t, std::size_t, void(Derived::*)(), void(Derived::*)(), BenchmarkType)
         * with support for GPU benchmark types.
         *
         * If @gl_extension{ARB,timer_query} desktop extension (part of OpenGL
         * 3.3), @gl_extension{EXT,disjoint_timer_query} OpenGL ES extension,
         * @webgl_extension{EXT,disjoint_timer_query} WebGL 1 extension or
         * @gl_extension{EXT,disjoint_timer_query_webgl2} WebGL 2 extension is
         * not available, @ref BenchmarkType::GpuTime benchmarks will get
         * automatically skipped.
         */
        template<class Derived> void addInstancedBenchmarks(std::initializer_list<void(Derived::*)()> benchmarks, std::size_t batchCount, std::size_t instanceCount, void(Derived::*setup)(), void(Derived::*teardown)(), BenchmarkType benchmarkType = BenchmarkType::Default) {
            if(benchmarkType == BenchmarkType::GpuTime)
                addCustomInstancedBenchmarks<Derived>(benchmarks, batchCount, instanceCount, setup, teardown, &OpenGLTester::gpuTimeBenchmarkBegin, &OpenGLTester::gpuTimeBenchmarkEnd, BenchmarkUnits::Nanoseconds);
            else
                Tester::addInstancedBenchmarks(benchmarks, batchCount, instanceCount, setup, teardown, Tester::BenchmarkType(Int(benchmarkType)));
        }

    private:
        void gpuTimeBenchmarkBegin();
        std::uint64_t gpuTimeBenchmarkEnd();

        struct WindowlessApplication: Platform::WindowlessApplication {
            explicit WindowlessApplication(const Arguments& arguments): Platform::WindowlessApplication{arguments} {}
            int exec() override final { return 0; }
        } _windowlessApplication;

        TimeQuery _gpuTimeQuery{NoCreate};
};

/** @hideinitializer
@brief Verify that no OpenGL error occurred

Equivalent to

@snippet MagnumGL.cpp OpenGLTester-MAGNUM_VERIFY_NO_GL_ERROR
*/
#define MAGNUM_VERIFY_NO_GL_ERROR() CORRADE_COMPARE(Magnum::GL::Renderer::error(), Magnum::GL::Renderer::Error::NoError)

}}
#else
#error this header is available only in the OpenGL build
#endif

#endif
