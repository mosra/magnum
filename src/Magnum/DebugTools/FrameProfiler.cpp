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

#include "FrameProfiler.h"

#include <chrono>
#include <sstream>
#include <Corrade/Containers/EnumSet.hpp>
#include <Corrade/Containers/GrowableArray.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/FormatStl.h>
#include <Corrade/Utility/String.h>

#include "Magnum/Math/Functions.h"
#ifdef MAGNUM_TARGET_GL
#include "Magnum/GL/TimeQuery.h"
#ifndef MAGNUM_TARGET_GLES
#include "Magnum/GL/PipelineStatisticsQuery.h"
#endif
#endif

namespace Magnum { namespace DebugTools {

FrameProfiler::Measurement::Measurement(const std::string& name, const Units units, void(*const begin)(void*), UnsignedLong(*const end)(void*), void* const state): _name{name}, _end{nullptr}, _state{state}, _units{units}, _delay{0} {
    _begin.immediate = begin;
    _query.immediate = end;
}

FrameProfiler::Measurement::Measurement(const std::string& name, const Units units, const UnsignedInt delay, void(*const begin)(void*, UnsignedInt), void(*const end)(void*, UnsignedInt), UnsignedLong(*const query)(void*, UnsignedInt, UnsignedInt), void* const state): _name{name}, _state{state}, _units{units}, _delay{delay} {
    CORRADE_ASSERT(delay >= 1, "DebugTools::FrameProfiler::Measurement: delay can't be zero", );
    _begin.delayed = begin;
    _end = end;
    _query.delayed = query;
}

FrameProfiler::FrameProfiler() noexcept = default;

FrameProfiler::FrameProfiler(Containers::Array<Measurement>&& measurements, UnsignedInt maxFrameCount) noexcept {
    setup(std::move(measurements), maxFrameCount);
}

FrameProfiler::FrameProfiler(const std::initializer_list<Measurement> measurements, const UnsignedInt maxFrameCount): FrameProfiler{Containers::array(measurements), maxFrameCount} {}

FrameProfiler::FrameProfiler(FrameProfiler&& other) noexcept:
    _enabled{other._enabled},
    #ifndef CORRADE_NO_ASSERT
    _beginFrameCalled{other._beginFrameCalled},
    #endif
    _maxFrameCount{other._maxFrameCount},
    _measuredFrameCount{other._measuredFrameCount},
    _measurements{std::move(other._measurements)},
    _data{std::move(other._data)}
{
    /* For all state pointers that point to &other patch them to point to this
       instead, to account for 90% of use cases of derived classes */
    for(Measurement& measurement: _measurements)
        if(measurement._state == &other) measurement._state = this;
}

FrameProfiler& FrameProfiler::operator=(FrameProfiler&& other) noexcept {
    using std::swap;
    swap(_enabled, other._enabled);
    #ifndef CORRADE_NO_ASSERT
    swap(_beginFrameCalled, other._beginFrameCalled);
    #endif
    swap(_maxFrameCount, other._maxFrameCount);
    swap(_measuredFrameCount, other._measuredFrameCount);
    swap(_measurements, other._measurements);
    swap(_data, other._data);

    /* For all state pointers that point to &other patch them to point to this
       instead, to account for 90% of use cases of derived classes */
    for(Measurement& measurement: _measurements)
        if(measurement._state == &other) measurement._state = this;

    /* And the same the other way to avoid the other instance accidentally
       affecting out measurements */
    for(Measurement& measurement: other._measurements)
        if(measurement._state == this) measurement._state = &other;

    return *this;
}

void FrameProfiler::setup(Containers::Array<Measurement>&& measurements, const UnsignedInt maxFrameCount) {
    CORRADE_ASSERT(maxFrameCount >= 1, "DebugTools::FrameProfiler::setup(): max frame count can't be zero", );

    _maxFrameCount = maxFrameCount;
    _measurements = std::move(measurements);
    arrayReserve(_data, maxFrameCount*_measurements.size());

    /* Calculate the max delay, which signalizes when data will be available.
       Non-delayed measurements are distinguished by _delay set to 0, so start
       with 1 to exclude these. */
    for(const Measurement& measurement: _measurements) {
        /* Max frame count is always >= 1, so even if _delay is 0 the condition
           makes sense and we don't need to do a max() */
        CORRADE_ASSERT(maxFrameCount >= measurement._delay,
            "DebugTools::FrameProfiler::setup(): max delay" << measurement._delay << "is larger than max frame count" << maxFrameCount, );
    }

    /* Reset to have a clean slate in case we did some other measurements
       before */
    enable();
}

void FrameProfiler::setup(const std::initializer_list<Measurement> measurements, const UnsignedInt maxFrameCount) {
    setup(Containers::array(measurements), maxFrameCount);
}

void FrameProfiler::enable() {
    _enabled = true;
    #ifndef CORRADE_NO_ASSERT
    _beginFrameCalled = false;
    #endif
    _measuredFrameCount = 0;
    arrayResize(_data, 0);

    /* Wipe out no longer relevant moving sums from all measurements, and
       delayed measurement indices as well (tho for these it's not so
       important) */
    for(Measurement& measurement: _measurements) {
        measurement._movingSum = 0;
        measurement._current = 0;
    }
}

void FrameProfiler::disable() {
    _enabled = false;
}

void FrameProfiler::beginFrame() {
    if(!_enabled) return;

    CORRADE_ASSERT(!_beginFrameCalled, "DebugTools::FrameProfiler::beginFrame(): expected end of frame", );
    #ifndef CORRADE_NO_ASSERT
    _beginFrameCalled = true;
    #endif

    /* For all measurements call the begin function */
    for(const Measurement& measurement: _measurements) {
        if(!measurement._delay)
            measurement._begin.immediate(measurement._state);
        else
            measurement._begin.delayed(measurement._state, measurement._current);
    }
}

UnsignedInt FrameProfiler::delayedCurrentData(UnsignedInt delay) const {
    CORRADE_INTERNAL_ASSERT(delay >= 1);
    return (_measuredFrameCount - delay) % _maxFrameCount;
}

void FrameProfiler::endFrame() {
    if(!_enabled) return;

    CORRADE_ASSERT(_beginFrameCalled, "DebugTools::FrameProfiler::endFrame(): expected begin of frame", );
    #ifndef CORRADE_NO_ASSERT
    _beginFrameCalled = false;
    #endif

    /* If we don't have all frames yet, enlarge the array */
    if(++_measuredFrameCount <= _maxFrameCount)
        arrayAppend(_data, Containers::NoInit, _measurements.size());

    /* Wrap up measurements for this frame  */
    for(std::size_t i = 0; i != _measurements.size(); ++i) {
        Measurement& measurement = _measurements[i];
        const UnsignedInt measurementDelay = Math::max(1u, measurement._delay);

        /* Where to save currently queried data. For _delay of 0 or 1,
           delayedCurrentData(Math::max(1u, measurement._delay)) is equal to
           _currentData. */
        UnsignedLong& currentMeasurementData = _data[delayedCurrentData(measurementDelay)*_measurements.size() + i];

        /* If we're wrapping around, subtract the oldest data from the moving
           average so we can reuse the memory for currently queried data */
        if(_measuredFrameCount > _maxFrameCount + measurementDelay - 1) {
            CORRADE_INTERNAL_ASSERT(measurement._movingSum >= currentMeasurementData);
            measurement._movingSum -= currentMeasurementData;
        }

        /* Simply save the data if not delayed */
        if(!measurement._delay)
            currentMeasurementData = measurement._query.immediate(measurement._state);

        /* For delayed measurements call the end function for current frame and
           then save the data for the delayed frame */
        else {
            measurement._end(measurement._state, measurement._current);

            /* The slot from which we just retrieved a delayed value will be
               reused for a a new value next frame */
            const UnsignedInt previous = (measurement._current + 1) % measurement._delay;
            if(_measuredFrameCount >= measurement._delay) {
                currentMeasurementData =
                    measurement._query.delayed(measurement._state, previous, measurement._current);
            }
            measurement._current = previous;
        }
    }

    /* Process the new data if we have enough frames even for the largest
       delay */
    for(std::size_t i = 0; i != _measurements.size(); ++i) {
        Measurement& measurement = _measurements[i];
        const UnsignedInt measurementDelay = Math::max(1u, measurement._delay);

        /* If we have enough frames, add the new measurement to the moving sum.
           For _delay of 0 or 1, delayedCurrentData(Math::max(1u, measurement._delay))
           is equal to _currentData. */
        if(_measuredFrameCount >= measurementDelay)
            _measurements[i]._movingSum += _data[delayedCurrentData(measurementDelay)*_measurements.size() + i];
    }
}

std::string FrameProfiler::measurementName(const UnsignedInt id) const {
    CORRADE_ASSERT(id < _measurements.size(),
        "DebugTools::FrameProfiler::measurementName(): index" << id << "out of range for" << _measurements.size() << "measurements", {});
    return _measurements[id]._name;
}

FrameProfiler::Units FrameProfiler::measurementUnits(const UnsignedInt id) const {
    CORRADE_ASSERT(id < _measurements.size(),
        "DebugTools::FrameProfiler::measurementUnits(): index" << id << "out of range for" << _measurements.size() << "measurements", {});
    return _measurements[id]._units;
}

UnsignedInt FrameProfiler::measurementDelay(const UnsignedInt id) const {
    CORRADE_ASSERT(id < _measurements.size(),
        "DebugTools::FrameProfiler::measurementDelay(): index" << id << "out of range for" << _measurements.size() << "measurements", {});
    return Math::max(_measurements[id]._delay, 1u);
}

bool FrameProfiler::isMeasurementAvailable(const UnsignedInt id) const {
    CORRADE_ASSERT(id < _measurements.size(),
        "DebugTools::FrameProfiler::measurementDelay(): index" << id << "out of range for" << _measurements.size() << "measurements", {});
    return _measuredFrameCount >= Math::max(_measurements[id]._delay, 1u);
}

UnsignedLong FrameProfiler::measurementData(const UnsignedInt id, const UnsignedInt frame) const {
    CORRADE_ASSERT(id < _measurements.size(),
        "DebugTools::FrameProfiler::measurementData(): index" << id << "out of range for" << _measurements.size() << "measurements", {});
    CORRADE_ASSERT(frame < _maxFrameCount,
        "DebugTools::FrameProfiler::measurementData(): frame" << frame << "out of bounds for max" << _maxFrameCount << "frames", {});
    CORRADE_ASSERT(_measuredFrameCount >= Math::max(_measurements[id]._delay, 1u) && frame <= _measuredFrameCount - Math::max(_measurements[id]._delay, 1u),
        "DebugTools::FrameProfiler::measurementData(): frame" << frame << "of measurement" << id << "not available yet (delay" << Math::max(_measurements[id]._delay, 1u) << Debug::nospace << "," << _measuredFrameCount << "frames measured so far)", {});

    /* We're returning data from the previous maxFrameCount. If the full range
       is not available, cap that only to the count of actually measured frames
       minus the delay. */
    return _data[((_measuredFrameCount - Math::min(_maxFrameCount + Math::max(_measurements[id]._delay, 1u) - 1, _measuredFrameCount) + frame) % _maxFrameCount)*_measurements.size() + id];
}

Double FrameProfiler::measurementMeanInternal(const Measurement& measurement) const {
    return Double(measurement._movingSum)/
        Math::min(_measuredFrameCount - Math::max(measurement._delay, 1u) + 1, _maxFrameCount);
}

Double FrameProfiler::measurementMean(const UnsignedInt id) const {
    CORRADE_ASSERT(id < _measurements.size(),
        "DebugTools::FrameProfiler::measurementMean(): index" << id << "out of range for" << _measurements.size() << "measurements", {});
    CORRADE_ASSERT(_measuredFrameCount >= Math::max(_measurements[id]._delay, 1u), "DebugTools::FrameProfiler::measurementMean(): measurement data available after" << Math::max(_measurements[id]._delay, 1u) - _measuredFrameCount << "more frames", {});

    return measurementMeanInternal(_measurements[id]);
}

namespace {

/* Based on Corrade/TestSuite/Implementation/BenchmarkStats.h */

void printValue(Utility::Debug& out, const Double mean, const Double divisor, const char* const unitPrefix, const char* const units) {
    out << Debug::boldColor(Debug::Color::Green)
        << Utility::formatString("{:.2f}", mean/divisor) << Debug::resetColor
        << Debug::nospace << unitPrefix << Debug::nospace << units;
}

void printTime(Utility::Debug& out, const Double mean) {
    if(mean >= 1000000000.0)
        printValue(out, mean, 1000000000.0, " ", "s");
    else if(mean >= 1000000.0)
        printValue(out, mean, 1000000.0, " m", "s");
    else if(mean >= 1000.0)
        printValue(out, mean, 1000.0, " µ", "s");
    else
        printValue(out, mean, 1.0, " n", "s");
}

void printCount(Utility::Debug& out, const Double mean, Double multiplier, const char* const units) {
    if(mean >= multiplier*multiplier*multiplier)
        printValue(out, mean, multiplier*multiplier*multiplier, " G", units);
    else if(mean >= multiplier*multiplier)
        printValue(out, mean, multiplier*multiplier, " M", units);
    else if(mean >= multiplier)
        printValue(out, mean, multiplier, " k", units);
    else
        printValue(out, mean, 1.0, std::strlen(units) ? " " : "", units);
}

}

void FrameProfiler::printStatisticsInternal(Debug& out) const {
    out << Debug::boldColor(Debug::Color::Default) << "Last"
        << Debug::boldColor(Debug::Color::Cyan)
        << Math::min(_measuredFrameCount, _maxFrameCount)
        << Debug::boldColor(Debug::Color::Default) << "frames:";

    for(const Measurement& measurement: _measurements) {
        out << Debug::newline << " " << Debug::boldColor(Debug::Color::Default)
            << measurement._name << Debug::nospace << ":" << Debug::resetColor;

        /* If this measurement is not available yet, print a placeholder */
        if(_measuredFrameCount < Math::max(measurement._delay, 1u)) {
            const char* units = nullptr;
            switch(measurement._units) {
                case Units::Count:
                case Units::RatioThousandths:
                    units = "";
                    break;
                case Units::Nanoseconds:
                    units = "s";
                    break;
                case Units::Bytes:
                    units = "B";
                    break;
                case Units::PercentageThousandths:
                    units = "%";
                    break;
            }
            CORRADE_INTERNAL_ASSERT(units);

            out << Debug::color(Debug::Color::Blue) << "-.--"
                << Debug::resetColor;
            if(units[0] != '\0') out << units;

        /* Otherwise format the value */
        } else {
            const Double mean = measurementMeanInternal(measurement);
            switch(measurement._units) {
                case Units::Nanoseconds:
                    printTime(out, mean);
                    continue;
                case Units::Bytes:
                    printCount(out, mean, 1024.0, "B");
                    continue;
                case Units::Count:
                    printCount(out, mean, 1000.0, "");
                    continue;
                case Units::RatioThousandths:
                    printCount(out, mean/1000.0, 1000.0, "");
                    continue;
                case Units::PercentageThousandths:
                    printValue(out, mean, 1000.0, " ", "%");
                    continue;
            }

            CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
        }
    }
}

std::string FrameProfiler::statistics() const {
    std::ostringstream out;
    Debug d{&out, Debug::Flag::NoNewlineAtTheEnd|Debug::Flag::DisableColors};
    printStatisticsInternal(d);
    return out.str();
}

void FrameProfiler::printStatistics(const UnsignedInt frequency) const {
    Debug::Flags flags;
    if(!Debug::isTty()) flags |= Debug::Flag::DisableColors;
    printStatistics(Debug{flags}, frequency);
}

void FrameProfiler::printStatistics(Debug& out, const UnsignedInt frequency) const {
    if(!isEnabled() || _measuredFrameCount % frequency != 0) return;

    /* If on a TTY and we printed at least something already, scroll back up to
       overwrite previous output */
    if(out.isTty() && _measuredFrameCount > frequency)
        out << Debug::nospace << "\033[" << Debug::nospace
            << _measurements.size() + 1 << Debug::nospace << "A\033[J"
            << Debug::nospace;

    printStatisticsInternal(out);

    /* Unconditionally finish with a newline so the TTY scrollback works
       correctly */
    if(out.flags() & Debug::Flag::NoNewlineAtTheEnd)
        out << Debug::newline;
}

Debug& operator<<(Debug& debug, const FrameProfiler::Units value) {
    debug << "DebugTools::FrameProfiler::Units" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(v) case FrameProfiler::Units::v: return debug << "::" #v;
        _c(Nanoseconds)
        _c(Bytes)
        _c(Count)
        _c(RatioThousandths)
        _c(PercentageThousandths)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(UnsignedByte(value)) << Debug::nospace << ")";
}

#ifdef MAGNUM_TARGET_GL
struct GLFrameProfiler::State {
    UnsignedShort cpuDurationIndex = 0xffff,
        gpuDurationIndex = 0xffff,
        frameTimeIndex = 0xffff;
    #ifndef MAGNUM_TARGET_GLES
    UnsignedShort vertexFetchRatioIndex = 0xffff,
        primitiveClipRatioIndex = 0xffff;
    #endif
    UnsignedLong frameTimeStartFrame[2];
    UnsignedLong cpuDurationStartFrame;
    GL::TimeQuery timeQueries[3]{GL::TimeQuery{NoCreate}, GL::TimeQuery{NoCreate}, GL::TimeQuery{NoCreate}};
    #ifndef MAGNUM_TARGET_GLES
    GL::PipelineStatisticsQuery verticesSubmittedQueries[3]{GL::PipelineStatisticsQuery{NoCreate}, GL::PipelineStatisticsQuery{NoCreate}, GL::PipelineStatisticsQuery{NoCreate}};
    GL::PipelineStatisticsQuery vertexShaderInvocationsQueries[3]{GL::PipelineStatisticsQuery{NoCreate}, GL::PipelineStatisticsQuery{NoCreate}, GL::PipelineStatisticsQuery{NoCreate}};
    GL::PipelineStatisticsQuery clippingInputPrimitivesQueries[3]{GL::PipelineStatisticsQuery{NoCreate}, GL::PipelineStatisticsQuery{NoCreate}, GL::PipelineStatisticsQuery{NoCreate}};
    GL::PipelineStatisticsQuery clippingOutputPrimitivesQueries[3]{GL::PipelineStatisticsQuery{NoCreate}, GL::PipelineStatisticsQuery{NoCreate}, GL::PipelineStatisticsQuery{NoCreate}};
    #endif
};

GLFrameProfiler::GLFrameProfiler(): _state{Containers::InPlaceInit} {}

GLFrameProfiler::GLFrameProfiler(const Values values, const UnsignedInt maxFrameCount): GLFrameProfiler{}
{
    setup(values, maxFrameCount);
}

GLFrameProfiler::GLFrameProfiler(GLFrameProfiler&&) noexcept = default;

GLFrameProfiler& GLFrameProfiler::operator=(GLFrameProfiler&&) noexcept = default;

GLFrameProfiler::~GLFrameProfiler() = default;

void GLFrameProfiler::setup(const Values values, const UnsignedInt maxFrameCount) {
    UnsignedShort index = 0;
    Containers::Array<Measurement> measurements;
    if(values & Value::FrameTime) {
        /* Fucking hell, STL. When I first saw std::chrono back in 2010 I
           should have flipped the table and learn carpentry instead. BUT NO,
           I'm still suffering this abomination a decade later! */
        arrayAppend(measurements, Containers::InPlaceInit,
            "Frame time", Units::Nanoseconds, UnsignedInt(Containers::arraySize(_state->frameTimeStartFrame)),
            [](void* state, UnsignedInt current) {
                static_cast<State*>(state)->frameTimeStartFrame[current] = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
            },
            [](void*, UnsignedInt) {},
            [](void* state, UnsignedInt previous, UnsignedInt current) {
                auto& self = *static_cast<State*>(state);
                return self.frameTimeStartFrame[current] -
                    self.frameTimeStartFrame[previous];
            }, _state.get());
        _state->frameTimeIndex = index++;
    }
    if(values & Value::CpuDuration) {
        arrayAppend(measurements, Containers::InPlaceInit,
            "CPU duration", Units::Nanoseconds,
            [](void* state) {
                static_cast<State*>(state)->cpuDurationStartFrame = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
            },
            [](void* state) {
                /* libc++ 10 needs an explicit cast to UnsignedLong */
                return UnsignedLong(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count() - static_cast<State*>(state)->cpuDurationStartFrame);
            }, _state.get());
        _state->cpuDurationIndex = index++;
    }
    if(values & Value::GpuDuration) {
        for(GL::TimeQuery& q: _state->timeQueries)
            q = GL::TimeQuery{GL::TimeQuery::Target::TimeElapsed};
        arrayAppend(measurements, Containers::InPlaceInit,
            "GPU duration", Units::Nanoseconds,
            UnsignedInt(Containers::arraySize(_state->timeQueries)),
            [](void* state, UnsignedInt current) {
                static_cast<State*>(state)->timeQueries[current].begin();
            },
            [](void* state, UnsignedInt current) {
                static_cast<State*>(state)->timeQueries[current].end();
            },
            [](void* state, UnsignedInt previous, UnsignedInt) {
                return static_cast<State*>(state)->timeQueries[previous].result<UnsignedLong>();
            }, _state.get());
        _state->gpuDurationIndex = index++;
    }
    #ifndef MAGNUM_TARGET_GLES
    if(values & Value::VertexFetchRatio) {
        for(GL::PipelineStatisticsQuery& q: _state->verticesSubmittedQueries)
            q = GL::PipelineStatisticsQuery{GL::PipelineStatisticsQuery::Target::VerticesSubmitted};
        for(GL::PipelineStatisticsQuery& q: _state->vertexShaderInvocationsQueries)
            q = GL::PipelineStatisticsQuery{GL::PipelineStatisticsQuery::Target::VertexShaderInvocations};
        arrayAppend(measurements, Containers::InPlaceInit,
            "Vertex fetch ratio", Units::RatioThousandths,
            UnsignedInt(Containers::arraySize(_state->verticesSubmittedQueries)),
            [](void* state, UnsignedInt current) {
                static_cast<State*>(state)->verticesSubmittedQueries[current].begin();
                static_cast<State*>(state)->vertexShaderInvocationsQueries[current].begin();
            },
            [](void* state, UnsignedInt current) {
                static_cast<State*>(state)->verticesSubmittedQueries[current].end();
                static_cast<State*>(state)->vertexShaderInvocationsQueries[current].end();
            },
            [](void* state, UnsignedInt previous, UnsignedInt) {
                /* Avoid division by zero if a frame doesn't have any draws */
                const auto submitted = static_cast<State*>(state)->verticesSubmittedQueries[previous].result<UnsignedLong>();
                if(!submitted) return UnsignedLong{};

                return static_cast<State*>(state)->vertexShaderInvocationsQueries[previous].result<UnsignedLong>()*1000/submitted;
            }, _state.get());
        _state->vertexFetchRatioIndex = index++;
    }
    if(values & Value::PrimitiveClipRatio) {
        for(GL::PipelineStatisticsQuery& q: _state->clippingInputPrimitivesQueries)
            q = GL::PipelineStatisticsQuery{GL::PipelineStatisticsQuery::Target::ClippingInputPrimitives};
        for(GL::PipelineStatisticsQuery& q: _state->clippingOutputPrimitivesQueries)
            q = GL::PipelineStatisticsQuery{GL::PipelineStatisticsQuery::Target::ClippingOutputPrimitives};
        arrayAppend(measurements, Containers::InPlaceInit,
            "Primitives clipped", Units::PercentageThousandths,
            UnsignedInt(Containers::arraySize(_state->clippingInputPrimitivesQueries)),
            [](void* state, UnsignedInt current) {
                static_cast<State*>(state)->clippingInputPrimitivesQueries[current].begin();
                static_cast<State*>(state)->clippingOutputPrimitivesQueries[current].begin();
            },
            [](void* state, UnsignedInt current) {
                static_cast<State*>(state)->clippingInputPrimitivesQueries[current].end();
                static_cast<State*>(state)->clippingOutputPrimitivesQueries[current].end();
            },
            [](void* state, UnsignedInt previous, UnsignedInt) {
                /* Avoid division by zero if a frame doesn't have any draws */
                const auto input = static_cast<State*>(state)->clippingInputPrimitivesQueries[previous].result<UnsignedLong>();
                if(!input) return UnsignedLong{};

                return 100000 - static_cast<State*>(state)->clippingOutputPrimitivesQueries[previous].result<UnsignedLong>()*100000/input;
            }, _state.get());
        _state->primitiveClipRatioIndex = index++;
    }
    #endif
    setup(std::move(measurements), maxFrameCount);
}

auto GLFrameProfiler::values() const -> Values {
    Values values;
    if(_state->frameTimeIndex != 0xffff) values |= Value::FrameTime;
    if(_state->cpuDurationIndex != 0xffff) values |= Value::CpuDuration;
    if(_state->gpuDurationIndex != 0xffff) values |= Value::GpuDuration;
    #ifndef MAGNUM_TARGET_GLES
    if(_state->vertexFetchRatioIndex != 0xffff) values |= Value::VertexFetchRatio;
    if(_state->primitiveClipRatioIndex != 0xffff) values |= Value::PrimitiveClipRatio;
    #endif
    return values;
}

bool GLFrameProfiler::isMeasurementAvailable(const Value value) const {
    const UnsignedShort* index = nullptr;
    switch(value) {
        case Value::FrameTime: index = &_state->frameTimeIndex; break;
        case Value::CpuDuration: index = &_state->cpuDurationIndex; break;
        case Value::GpuDuration: index = &_state->gpuDurationIndex; break;
        #ifndef MAGNUM_TARGET_GLES
        case Value::VertexFetchRatio: index = &_state->vertexFetchRatioIndex; break;
        case Value::PrimitiveClipRatio: index = &_state->primitiveClipRatioIndex; break;
        #endif
    }
    CORRADE_INTERNAL_ASSERT(index);
    CORRADE_ASSERT(*index < measurementCount(),
        "DebugTools::GLFrameProfiler::isMeasurementAvailable():" << value << "not enabled", {});
    return isMeasurementAvailable(*index);
}

Double GLFrameProfiler::frameTimeMean() const {
    CORRADE_ASSERT(_state->frameTimeIndex < measurementCount(),
        "DebugTools::GLFrameProfiler::frameTimeMean(): not enabled", {});
    return measurementMean(_state->frameTimeIndex);
}

Double GLFrameProfiler::cpuDurationMean() const {
    CORRADE_ASSERT(_state->cpuDurationIndex < measurementCount(),
        "DebugTools::GLFrameProfiler::cpuDurationMean(): not enabled", {});
    return measurementMean(_state->cpuDurationIndex);
}

Double GLFrameProfiler::gpuDurationMean() const {
    CORRADE_ASSERT(_state->gpuDurationIndex < measurementCount(),
        "DebugTools::GLFrameProfiler::gpuDurationMean(): not enabled", {});
    return measurementMean(_state->gpuDurationIndex);
}

#ifndef MAGNUM_TARGET_GLES
Double GLFrameProfiler::vertexFetchRatioMean() const {
    CORRADE_ASSERT(_state->vertexFetchRatioIndex < measurementCount(),
        "DebugTools::GLFrameProfiler::vertexFetchRatioMean(): not enabled", {});
    return measurementMean(_state->vertexFetchRatioIndex);
}

Double GLFrameProfiler::primitiveClipRatioMean() const {
    CORRADE_ASSERT(_state->primitiveClipRatioIndex < measurementCount(),
        "DebugTools::GLFrameProfiler::primitiveClipRatioMean(): not enabled", {});
    return measurementMean(_state->primitiveClipRatioIndex);
}
#endif

namespace {

constexpr const char* GLFrameProfilerValueNames[] {
    "FrameTime",
    "CpuDuration",
    "GpuDuration",
    "VertexFetchRatio",
    "PrimitiveClipRatio"
};

}

Debug& operator<<(Debug& debug, const GLFrameProfiler::Value value) {
    debug << "DebugTools::GLFrameProfiler::Value" << Debug::nospace;

    const UnsignedInt bit = Math::log2(UnsignedShort(value));
    if(1 << bit == UnsignedShort(value))
        return debug << "::" << Debug::nospace << GLFrameProfilerValueNames[bit];

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(UnsignedShort(value)) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const GLFrameProfiler::Values value) {
    return Containers::enumSetDebugOutput(debug, value, "DebugTools::GLFrameProfiler::Values{}", {
        GLFrameProfiler::Value::FrameTime,
        GLFrameProfiler::Value::CpuDuration,
        GLFrameProfiler::Value::GpuDuration,
        #ifndef MAGNUM_TARGET_GLES
        GLFrameProfiler::Value::VertexFetchRatio,
        GLFrameProfiler::Value::PrimitiveClipRatio
        #endif
        });
}
#endif

}}

namespace Corrade { namespace Utility {

using namespace Magnum;

#ifdef MAGNUM_TARGET_GL
std::string ConfigurationValue<DebugTools::GLFrameProfiler::Value>::toString(const DebugTools::GLFrameProfiler::Value value, ConfigurationValueFlags) {
    const UnsignedInt bit = Math::log2(UnsignedShort(value));
    if(1 << bit == UnsignedShort(value))
        return DebugTools::GLFrameProfilerValueNames[bit];
    return "";
}

DebugTools::GLFrameProfiler::Value ConfigurationValue<DebugTools::GLFrameProfiler::Value>::fromString(const std::string& value, ConfigurationValueFlags) {
    for(std::size_t i = 0; i != Containers::arraySize(DebugTools::GLFrameProfilerValueNames); ++i)
        if(DebugTools::GLFrameProfilerValueNames[i] == value)
            return DebugTools::GLFrameProfiler::Value(1 << i);

    return DebugTools::GLFrameProfiler::Value{};
}

std::string ConfigurationValue<DebugTools::GLFrameProfiler::Values>::toString(const DebugTools::GLFrameProfiler::Values value, ConfigurationValueFlags) {
    std::string out;

    for(std::size_t i = 0; i != Containers::arraySize(DebugTools::GLFrameProfilerValueNames); ++i) {
        const auto bit = DebugTools::GLFrameProfiler::Value(1 << i);
        if(value & bit) {
            if(!out.empty()) out += ' ';
            out += DebugTools::GLFrameProfilerValueNames[i];
        }
    }

    return out;
}

DebugTools::GLFrameProfiler::Values ConfigurationValue<DebugTools::GLFrameProfiler::Values>::fromString(const std::string& value, ConfigurationValueFlags) {
    const std::vector<std::string> bits = Utility::String::splitWithoutEmptyParts(value);

    DebugTools::GLFrameProfiler::Values values;
    for(const std::string& bit: bits)
        for(std::size_t i = 0; i != Containers::arraySize(DebugTools::GLFrameProfilerValueNames); ++i)
            if(DebugTools::GLFrameProfilerValueNames[i] == bit)
                values |= DebugTools::GLFrameProfiler::Value(1 << i);

    return values;
}
#endif

}}
