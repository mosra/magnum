#ifndef Magnum_DebugTools_FrameProfiler_h
#define Magnum_DebugTools_FrameProfiler_h
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
 * @brief Class @ref Magnum::DebugTools::FrameProfiler, @ref Magnum::DebugTools::GLFrameProfiler
 * @m_since{2020,06}
 */

#include <string>
#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/Pointer.h>

#include "Magnum/Magnum.h"
#include "Magnum/DebugTools/visibility.h"

namespace Magnum { namespace DebugTools {

/**
@brief Frame profiler
@m_since{2020,06}

A generic implementation of a frame profiler supporting a moving average over
a set of frames as well as delayed measurements to avoid stalls when querying
the results. This class alone doesn't provide any pre-defined measurements, see
for example @ref GLFrameProfiler that provides common measurements like CPU and
GPU time.

@experimental

@section DebugTools-FrameProfiler-usage Basic usage

Measurements are performed by calling @ref beginFrame() and @ref endFrame() at
designated points in the frame:

@snippet MagnumDebugTools.cpp FrameProfiler-usage

In order to have stable profiling results, the application needs to redraw
constantly. However for applications that otherwise redraw only on change it
might be wasteful --- to account for this, it's possible to toggle the profiler
operation using @ref enable() / @ref disable() and then
@ref Platform::Sdl2Application::redraw() "redraw()" can be called only if
@ref isEnabled() returns @cpp true @ce.

Data for all measurements is then available through @ref measurementName(),
@ref measurementUnits() and @ref measurementMean(). For a convenient overview
of all measured values you can call @ref statistics() and feed its output to a
UI library or something that can render text. Alternatively, if you don't want
to bother with text rendering, call @ref printStatistics() to have the output
periodically printed to the console. If an interactive terminal is detected,
the output will be colored and refreshing itself in place. Together with the
on-demand profiling, it could look like this, refreshing the output every 10
frames:

@snippet MagnumDebugTools.cpp FrameProfiler-usage-console

And here's a sample output on the terminal --- using a fully configured
@link GLFrameProfiler @endlink:

@include debugtools-frameprofiler.ansi

@section DebugTools-FrameProfiler-setup Setting up measurements

Unless you're using this class through @ref GLFrameProfiler, measurements
have to be set up by passing @ref Measurement instances to the @ref setup()
function or to the constructor, together with specifying count of frames for
the moving average. A CPU duration measurements using the @ref std::chrono APIs
over last 50 frames can be done like this:

@snippet MagnumDebugTools.cpp FrameProfiler-setup-immediate

In the above case, the measurement result is available immediately on frame
end. That's not always the case, and for example GPU queries need a few frames
delay to avoid stalls from CPU/GPU synchronization. The following snippet sets
up sample count measurement with a delay, using three separate
@ref GL::SampleQuery instances that are cycled through each frame and retrieved
two frames later. The profiler automatically takes care of choosing one of the
three instances for each measurement via additional `current` / `previous`
parameters passed to each callback:

@snippet MagnumDebugTools-gl.cpp FrameProfiler-setup-delayed

<b></b>

@m_class{m-block m-warning}

@par Move construction and state pointers in callbacks
    The @ref FrameProfiler class is movable, which could potentially mean that
    state pointers passed to callback functions become dangling. It's not a
    problem in the above snippets because the state is always external to the
    instance, but care has to be taken when passing pointers to subclasses.
@par
    When setting up measurements from a subclass, it's recommended to always
    pass @cpp this @ce as the state pointer. The base class recognizes it
    during a move and patches the state to point to the new instance instead.
    If you don't or can't use @cpp this @ce as a state pointer, you need to
    either provide a dedicated move constructor and assignment to do the
    required patching or disable moves altogether to avoid accidents.
*/
class MAGNUM_DEBUGTOOLS_EXPORT FrameProfiler {
    public:
        /**
         * @brief Measurement units
         *
         * @see @ref Measurement
         */
        enum class Units: UnsignedByte {
            /**
             * Nanoseconds, measuring for example elapsed time. Depending on
             * the magnitude, @ref statistics() can show them as microseconds,
             * milliseconds or seconds.
             */
            Nanoseconds,

            /**
             * Bytes, measuring for example memory usage, bandwidth. Depending
             * on the magnitude, @ref statistics() can show them as kB, MB, GB
             * (with a multiplier of 1024).
             */
            Bytes,

            /**
             * Generic count. For discrete values that don't fit any of the
             * above. Depending on the magnitude, @ref statistics() can show
             * the value as k, M or G (with a multiplier of 1000).
             */
            Count,

            /**
             * Ratio expressed in 1/1000s. @ref statistics() divides the value
             * by 1000 and depending on the magnitude it can show it also as k,
             * M or G (with a multiplier of 1000).
             */
            RatioThousandths,

            /**
             * Percentage expressed in 1/1000s. @ref statistics() divides the
             * value by 1000 and appends a % sign.
             */
            PercentageThousandths
        };

        class Measurement;

        /**
         * @brief Default constructor
         *
         * Call @ref setup() to populate the profiler with measurements.
         */
        explicit FrameProfiler() noexcept;

        /**
         * @brief Constructor
         *
         * Equivalent to default-constructing an instance and calling
         * @ref setup() afterwards.
         */
        explicit FrameProfiler(Containers::Array<Measurement>&& measurements, UnsignedInt maxFrameCount) noexcept;

        /** @overload */
        explicit FrameProfiler(std::initializer_list<Measurement> measurements, UnsignedInt maxFrameCount);

        /** @brief Copying is not allowed */
        FrameProfiler(const FrameProfiler&) = delete;

        /** @brief Move constructor */
        FrameProfiler(FrameProfiler&&) noexcept;

        /** @brief Copying is not allowed */
        FrameProfiler& operator=(const FrameProfiler&) = delete;

        /** @brief Move assignment */
        FrameProfiler& operator=(FrameProfiler&&) noexcept;

        /**
         * @brief Setup measurements
         * @param measurements  List of measurements
         * @param maxFrameCount Max frame count over which to calculate a
         *      moving average. Expected to be at least @cpp 1 @ce.
         *
         * Calling @ref setup() on an already set up profiler will replace
         * existing measurements with @p measurements and reset
         * @ref measuredFrameCount() back to @cpp 0 @ce.
         */
        void setup(Containers::Array<Measurement>&& measurements, UnsignedInt maxFrameCount);

        /** @overload */
        void setup(std::initializer_list<Measurement> measurements, UnsignedInt maxFrameCount);

        /** @brief Whether the profiling is enabled */
        bool isEnabled() const { return _enabled; }

        /**
         * @brief Enable the profiler
         *
         * The profiler is enabled implicitly after construction. When this
         * function is called, it discards all measured data, effectively
         * making @ref measuredFrameCount() zero. If you want to reset the
         * profiler to measure different values as well, call @ref setup().
         */
        void enable();

        /**
         * @brief Disable the profiler
         *
         * Disabling the profiler will make @ref beginFrame() and
         * @ref endFrame() a no-op, effectively freezing all reported
         * measurements until the profiler is enabled again.
         */
        void disable();

        /**
         * @brief Begin a frame
         *
         * Has to be called at the beginning of a frame and be paired with a
         * corresponding @ref endFrame(). Calls @p begin functions in all
         * @ref Measurement instances passed to @ref setup(). If the profiler
         * is disabled, the function is a no-op.
         * @see @ref isEnabled()
         */
        void beginFrame();

        /**
         * @brief End a frame
         *
         * Has to be called at the end of frame, before buffer swap, and be
         * paired with a corresponding @ref beginFrame(). Calls @p end
         * functions in all @ref Measurement instances passed to @ref setup()
         * and @p query functions on all measurements that are sufficiently
         * delayed, saving their output. If the profiler is disabled, the
         * function is a no-op.
         * @see @ref isEnabled()
         */
        void endFrame();

        /**
         * @brief Max count of measured frames
         *
         * How many frames to calculate a moving average from. At the beginning
         * of a measurement when there's not enough frames, the average is
         * calculated only from @ref measuredFrameCount(). Always at least
         * @cpp 1 @ce.
         */
        UnsignedInt maxFrameCount() const { return _maxFrameCount; }

        /**
         * @brief Count of measured frames
         *
         * Count of times @ref endFrame() was called, but at most
         * @ref maxFrameCount(), after which the profiler calculates a moving
         * average over last @ref maxFrameCount() frames only. Actual data
         * availability depends on @ref measurementDelay().
         */
        UnsignedInt measuredFrameCount() const { return _measuredFrameCount; }

        /**
         * @brief Measurement count
         *
         * Count of @ref Measurement instances passed to @ref setup(). If
         * @ref setup() was not called yet, returns @cpp 0 @ce.
         */
        UnsignedInt measurementCount() const { return _measurements.size(); }

        /**
         * @brief Measurement name
         *
         * The @p id corresponds to the index of the measurement in the list
         * passed to @ref setup(). Expects that @p id is less than
         * @ref measurementCount().
         */
        std::string measurementName(UnsignedInt id) const;

        /**
         * @brief Measurement units
         *
         * The @p id corresponds to the index of the measurement in the list
         * passed to @ref setup(). Expects that @p id is less than
         * @ref measurementCount().
         */
        Units measurementUnits(UnsignedInt id) const;

        /**
         * @brief Measurement delay
         *
         * How many @ref beginFrame() / @ref endFrame() call pairs needs to be
         * performed before a value for given measurement is available. Always
         * at least @cpp 1 @ce. The @p id corresponds to the index of the
         * measurement in the list passed to @ref setup(). Expects that @p id
         * is less than @ref measurementCount().
         */
        UnsignedInt measurementDelay(UnsignedInt id) const;

        /**
         * @brief Whether given measurement is available
         *
         * Returns @cpp true @ce if @ref measuredFrameCount() is at least
         * @ref measurementDelay() for given @p id, @cpp false @ce otherwise.
         * The @p id corresponds to the index of the measurement in the list
         * passed to @ref setup(). Expects that @p id is less than
         * @ref measurementCount().
         */
        bool isMeasurementAvailable(UnsignedInt id) const;

        /**
         * @brief Measurement data at given frame
         *
         * A @p frame value of @cpp 0 @ce is the oldest recorded frame and can
         * go up to @ref measurementCount() lessened by @ref measurementDelay()
         * or @ref maxFrameCount() minus one, whichever is smaller. Expects that
         * @p id is less than @ref measurementCount() and at least
         * one measurement is available.
         * @see @ref isMeasurementAvailable()
         */
        UnsignedLong measurementData(UnsignedInt id, UnsignedInt frame) const;

        /**
         * @brief Measurement mean
         *
         * Returns a moving average of @f$ n @f$ previous measurements out of
         * the total @f$ M @f$ values: @f[
         *      \bar{x}_\text{SM} = \dfrac{1}{n} \sum\limits_{i=0}^{n-1} x_{M -i}
         * @f]
         *
         * The @p id corresponds to the index of the measurement in the list
         * passed to @ref setup(). Expects that @p id is less than
         * @ref measurementCount() and that the measurement is available.
         * @see @ref isMeasurementAvailable()
         */
        Double measurementMean(UnsignedInt id) const;

        /**
         * @brief Overview of all measurements
         *
         * Returns a formatted string with names, means and units of all
         * measurements in the order they were added. If some measurement data
         * is available yet, prints placeholder values for these; if the
         * @see @ref isMeasurementAvailable(), @ref isEnabled()
         */
        std::string statistics() const;

        /**
         * @brief Print an overview of all measurements to a console at given rate
         *
         * Expected to be called every frame. On every `frequency`th frame
         * prints the same information as @ref statistics(), but in addition,
         * if the output is a TTY, it's colored and overwrites itself instead
         * of filling up the terminal history.
         * @see @ref isMeasurementAvailable(), @ref isEnabled()
         *      @ref Corrade::Utility::Debug::isTty()
         */
        void printStatistics(UnsignedInt frequency) const;

        /**
         * @brief Print an overview of all measurements to given output at given rate
         *
         * Compared to @ref printStatistics(UnsignedInt) const prints to given
         * @p out (which can be also @ref Corrade::Utility::Warning or
         * @ref Corrade::Utility::Error) and uses it to decide whether the
         * output is a TTY and whether to print colors.
         * @see @ref Corrade::Utility::Debug::isTty(),
         *      @ref Corrade::Utility::Debug::Flag::DisableColors
         */
        void printStatistics(Debug& out, UnsignedInt frequency) const;

        /** @overload */
        void printStatistics(Debug&& out, UnsignedInt frequency) const {
            printStatistics(out, frequency);
        }

    private:
        UnsignedInt delayedCurrentData(UnsignedInt delay) const;
        Double measurementMeanInternal(const Measurement& measurement) const;
        void printStatisticsInternal(Debug& out) const;

        bool _enabled = true;
        #ifndef CORRADE_NO_ASSERT
        /* Here it shouldn't cause the class to have a different size when
           asserts get disabled */
        bool _beginFrameCalled{};
        #endif
        UnsignedInt _maxFrameCount{1}, _measuredFrameCount{};
        Containers::Array<Measurement> _measurements;
        Containers::Array<UnsignedLong> _data;
};

/**
@brief Measurement

Describes a single measurement passed to @ref FrameProfiler::setup(). See
@ref DebugTools-FrameProfiler-setup for introduction and examples.
*/
class MAGNUM_DEBUGTOOLS_EXPORT FrameProfiler::Measurement {
    public:
        /**
         * @brief Construct an immediate measurement
         * @param name      Measurement name, used in
         *      @ref FrameProfiler::measurementName() and
         *      @ref FrameProfiler::statistics()
         * @param units     Measurement units, used in
         *      @ref FrameProfiler::measurementUnits() and
         *      @ref FrameProfiler::statistics()
         * @param begin     Function to call at the beginning of a frame
         * @param end       Function to call at the end of a frame, returning
         *      the measured value
         * @param state     State pointer passed to both @p begin and @p end
         *      as a first argument
         */
        explicit Measurement(const std::string& name, Units units, void(*begin)(void*), UnsignedLong(*end)(void*), void* state);

        /**
         * @brief Construct a delayed measurement
         * @param name      Measurement name, used in
         *      @ref FrameProfiler::measurementName() and
         *      @ref FrameProfiler::statistics()
         * @param units     Measurement units, used in
         *      @ref FrameProfiler::measurementUnits() and
         *      @ref FrameProfiler::statistics()
         * @param delay     How many @ref FrameProfiler::endFrame() calls has
         *      to happen before a measured value can be retrieved using
         *      @p query. Has to be at least @cpp 1 @ce, delay of @cpp 1 @ce is
         *       equal in behavior to immediate measurements.
         * @param begin     Function to call at the beginning of a frame.
         *      Second argument is a `current` index that's guaranteed to be
         *      less than @p delay and always different in each consecutive
         *      call.
         * @param end       Function to call at the end of a frame.
         *      Second argument is a `current` index that's guaranteed to be
         *      less than @p delay and always different in each consecutive
         *      call.
         * @param query     Function to call to get the measured value. Called
         *      after @p delay frames. First argument is a `previous` index
         *      that is the same as the `current` argument passed to a
         *      corresponding @p begin / @p end function of the measurement to
         *      query the value of. Second argument is a `current` index that
         *      corresponds to current frame.
         * @param state     State pointer passed to both @p begin and @p end
         *      as a first argument
         */
        explicit Measurement(const std::string& name, Units units, UnsignedInt delay, void(*begin)(void*, UnsignedInt), void(*end)(void*, UnsignedInt), UnsignedLong(*query)(void*, UnsignedInt, UnsignedInt), void* state);

    private:
        friend FrameProfiler;

        std::string _name;
        union {
            void(*immediate)(void*);
            void(*delayed)(void*, UnsignedInt);
        } _begin;
        void(*_end)(void*, UnsignedInt);
        union {
            UnsignedLong(*immediate)(void*);
            UnsignedLong(*delayed)(void*, UnsignedInt, UnsignedInt);
        } _query;
        void* _state;
        Units _units;
        /* Set to 0 to distinguish immediate measurements (first
           constructor), however always used as max(_delay, 1) */
        UnsignedInt _delay;

        UnsignedInt _current{};
        UnsignedLong _movingSum{};
};

/**
@debugoperatorclassenum{FrameProfiler,FrameProfiler::Units}
@m_since{2020,06}
*/
MAGNUM_DEBUGTOOLS_EXPORT Debug& operator<<(Debug& debug, FrameProfiler::Units value);

#ifdef MAGNUM_TARGET_GL
/**
@brief OpenGL frame profiler
@m_since{2020,06}

A @ref FrameProfiler with OpenGL-specific measurements. Instantiate with a
desired subset of measured values and then continue the same way as described
in the @ref DebugTools-FrameProfiler-usage "FrameProfiler usage documentation":

@snippet MagnumDebugTools-gl.cpp GLFrameProfiler-usage

If none if @ref Value::GpuDuration, @ref Value::VertexFetchRatio and
@ref Value::PrimitiveClipRatio is not enabled, the class can operate without an
active OpenGL context.

@experimental
*/
class MAGNUM_DEBUGTOOLS_EXPORT GLFrameProfiler: public FrameProfiler {
    public:
        /**
         * @brief Measured value
         *
         * @see @ref Values, @ref GLFrameProfiler(Values, UnsignedInt),
         *      @ref setup()
         */
        enum class Value: UnsignedShort {
            /**
             * Measure total frame time (i.e., time between consecutive
             * @ref beginFrame() calls). Reported in @ref Units::Nanoseconds
             * with a delay of 2 frames. When converted to seconds, the value
             * is an inverse of FPS.
             */
            FrameTime = 1 << 0,

            /**
             * Measure CPU frame duration (i.e., CPU time spent between
             * @ref beginFrame() and @ref endFrame()). Reported in
             * @ref Units::Nanoseconds with a delay of 1 frame.
             */
            CpuDuration = 1 << 1,

            /**
             * Measure GPU frame duration (i.e., time between @ref beginFrame()
             * and @ref endFrame()). Reported in @ref Units::Nanoseconds
             * with a delay of 3 frames. This value requires an active OpenGL
             * context.
             * @requires_gl33 Extension @gl_extension{ARB,timer_query}
             * @requires_es_extension Extension @gl_extension{EXT,disjoint_timer_query}
             * @requires_webgl_extension Extension @webgl_extension{EXT,disjoint_timer_query}
             *      on WebGL 1, @gl_extension{EXT,disjoint_timer_query_webgl2}
             *      on WebGL 2
             */
            GpuDuration = 1 << 2,

            #ifndef MAGNUM_TARGET_GLES
            /**
             * Ratio of vertex shader invocations to count of vertices
             * submitted. For a non-indexed draw the ratio will be 1, for
             * indexed draws ratio is less than 1. The lower the value is, the
             * better a mesh is optimized for post-transform vertex cache.
             * Reported in @ref Units::RatioThousandths with a delay of 3
             * frames. This value requires an active OpenGL context.
             * @requires_gl46 Extension @gl_extension{ARB,pipeline_statistics_query}
             */
            VertexFetchRatio = 1 << 3,

            /**
             * Ratio of primitives discarded by the clipping stage to count of
             * primitives submitted. The ratio is 0 when all primitives pass
             * the clipping stage and 1 when all are discarded. Can be used to
             * measure efficiency of a frustum culling algorithm. Reported in
             * @ref Units::PercentageThousandths with a delay of 3 frames. This
             * value requires an active OpenGL context.
             * @requires_gl46 Extension @gl_extension{ARB,pipeline_statistics_query}
             */
            PrimitiveClipRatio = 1 << 4
            #endif
        };

        /**
         * @brief Measured values
         *
         * @see @ref GLFrameProfiler(Values, UnsignedInt), @ref setup()
         */
        typedef Containers::EnumSet<Value> Values;

        /**
         * @brief Default constructor
         *
         * Call @ref setup() to populate the profiler with measurements.
         */
        explicit GLFrameProfiler();

        /**
         * @brief Constructor
         *
         * Equivalent to default-constructing an instance and calling
         * @ref setup() afterwards.
         */
        explicit GLFrameProfiler(Values values, UnsignedInt maxFrameCount);

        /** @brief Copying is not allowed */
        GLFrameProfiler(const GLFrameProfiler&) = delete;

        /** @brief Move constructor */
        GLFrameProfiler(GLFrameProfiler&&) noexcept;

        /** @brief Copying is not allowed */
        GLFrameProfiler& operator=(const GLFrameProfiler&) = delete;

        /** @brief Move assignment */
        GLFrameProfiler& operator=(GLFrameProfiler&&) noexcept;

        ~GLFrameProfiler();

        /**
         * @brief Setup measured values
         * @param values        List of measuremed values
         * @param maxFrameCount Max frame count over which to calculate a
         *      moving average. Expected to be at least @cpp 1 @ce.
         *
         * Calling @ref setup() on an already set up profiler will replace
         * existing measurements with @p measurements and reset
         * @ref measuredFrameCount() back to @cpp 0 @ce.
         */
        void setup(Values values, UnsignedInt maxFrameCount);

        /**
         * @brief Measured values
         *
         * Corresponds to the @p values parameter passed to
         * @ref GLFrameProfiler(Values, UnsignedInt) or @ref setup().
         */
        Values values() const;

        /**
         * @brief Whether given measurement is available
         *
         * Returns @cpp true @ce if enough frames was captured to calculate
         * given @p value, @cpp false @ce otherwise. Expects that @p value was
         * enabled.
         */
        bool isMeasurementAvailable(Value value) const;

        using FrameProfiler::isMeasurementAvailable;

        /**
         * @brief Mean frame time in nanoseconds
         *
         * Expects that @ref Value::FrameTime was enabled, and that measurement
         * data is available. See the flag documentation for more information.
         * @see @ref isMeasurementAvailable(), @ref measurementMean()
         */
        Double frameTimeMean() const;

        /**
         * @brief Mean CPU frame duration in nanoseconds
         *
         * Expects that @ref Value::CpuDuration was enabled, and that
         * measurement data is available. See the flag documentation for more
         * information.
         * @see @ref isMeasurementAvailable(), @ref measurementMean()
         */
        Double cpuDurationMean() const;

        /**
         * @brief Mean GPU frame duration in nanoseconds
         *
         * Expects that @ref Value::GpuDuration was enabled, and that
         * measurement data is available. See the flag documentation for more
         * information.
         * @see @ref isMeasurementAvailable(), @ref measurementMean()
         */
        Double gpuDurationMean() const;

        #ifndef MAGNUM_TARGET_GLES
        /**
         * @brief Mean vertex fetch ratio in thousandths
         *
         * Expects that @ref Value::VertexFetchRatio was enabled, and that
         * measurement data is available. See the flag documentation for more
         * information.
         * @requires_gl46 Extension @gl_extension{ARB,pipeline_statistics_query}
         * @see @ref isMeasurementAvailable(), @ref measurementMean()
         */
        Double vertexFetchRatioMean() const;

        /**
         * @brief Mean primitive clip ratio in percentage thousandths
         *
         * Expects that @ref Value::PrimitiveClipRatio was enabled, and that
         * measurement data is available. See the flag documentation for more
         * information.
         * @requires_gl46 Extension @gl_extension{ARB,pipeline_statistics_query}
         * @see @ref isMeasurementAvailable(), @ref measurementMean()
         */
        Double primitiveClipRatioMean() const;
        #endif

    private:
        using FrameProfiler::setup;

        struct State;
        Containers::Pointer<State> _state;
};

CORRADE_ENUMSET_OPERATORS(GLFrameProfiler::Values)

/**
@debugoperatorclassenum{GLFrameProfiler,GLFrameProfiler::Value}
@m_since{2020,06}
*/
MAGNUM_DEBUGTOOLS_EXPORT Debug& operator<<(Debug& debug, GLFrameProfiler::Value value);

/**
@debugoperatorclassenum{GLFrameProfiler,GLFrameProfiler::Values}
@m_since{2020,06}
*/
MAGNUM_DEBUGTOOLS_EXPORT Debug& operator<<(Debug& debug, GLFrameProfiler::Values value);
#endif

}}

namespace Corrade { namespace Utility {

#ifdef MAGNUM_TARGET_GL
/**
@configurationvalue{Magnum::DebugTools::GLFrameProfiler::Value}
@m_since{2020,06}
*/
template<> struct MAGNUM_DEBUGTOOLS_EXPORT ConfigurationValue<Magnum::DebugTools::GLFrameProfiler::Value> {
    ConfigurationValue() = delete;

    /**
     * @brief Writes enum value as a string
     *
     * If the value is invalid, returns an empty string.
     */
    static std::string toString(Magnum::DebugTools::GLFrameProfiler::Value value, ConfigurationValueFlags);

    /**
     * @brief Reads enum value as a string
     *
     * If the string is invalid, returns a zero (invalid) value.
     */
    static Magnum::DebugTools::GLFrameProfiler::Value fromString(const std::string& stringValue, ConfigurationValueFlags);
};

/**
@configurationvalue{Magnum::DebugTools::GLFrameProfiler::Values}
@m_since{2020,06}
*/
template<> struct MAGNUM_DEBUGTOOLS_EXPORT ConfigurationValue<Magnum::DebugTools::GLFrameProfiler::Values> {
    ConfigurationValue() = delete;

    /**
     * @brief Writes enum set value as a string
     *
     * Writes the enum set as a sequence of flag names separated by spaces. If
     * the value is invalid, returns an empty string.
     */
    static std::string toString(Magnum::DebugTools::GLFrameProfiler::Values value, ConfigurationValueFlags);

    /**
     * @brief Reads enum set value as a string
     *
     * Assumes the string is a sequence of flag names separated by spaces. If
     * the value is invalid, returns an empty set.
     */
    static Magnum::DebugTools::GLFrameProfiler::Values fromString(const std::string& stringValue, ConfigurationValueFlags);
};
#endif

}}

#endif
