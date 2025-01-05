/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2022 Vladislav Oleshko <vladislav.oleshko@gmail.com>
    Copyright @ 2022 Hugo Amiard <hugo.amiard@wonderlandengine.com>

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

#include "AbstractShaderProgram.h"

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/Iterable.h>
#include <Corrade/Containers/Pair.h>
#ifdef MAGNUM_BUILD_DEPRECATED
#include <Corrade/Containers/Reference.h>
#endif
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/Containers/String.h>
#include <Corrade/Containers/StringIterable.h>

#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/Mesh.h"
#include "Magnum/GL/MeshView.h"
#include "Magnum/GL/Shader.h"
#ifndef MAGNUM_TARGET_WEBGL
#include "Magnum/GL/Implementation/DebugState.h"
#endif
#ifdef MAGNUM_TARGET_GLES
#include "Magnum/GL/Implementation/MeshState.h"
#endif
#include "Magnum/GL/Implementation/ShaderProgramState.h"
#include "Magnum/GL/Implementation/State.h"
#include "Magnum/Math/RectangularMatrix.h"

namespace Magnum { namespace GL {

Int AbstractShaderProgram::maxVertexAttributes() {
    GLint& value = Context::current().state().shaderProgram.maxVertexAttributes;

    /* Get the value, if not already cached */
    if(value == 0)
        glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &value);

    return value;
}

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
Int AbstractShaderProgram::maxGeometryOutputVertices() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::geometry_shader4>())
        return 0;
    #else
    if(!Context::current().isExtensionSupported<Extensions::EXT::geometry_shader>())
        return 0;
    #endif

    GLint& value = Context::current().state().shaderProgram.maxGeometryOutputVertices;

    if(value == 0)
        glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_VERTICES, &value);

    return value;
}

Int AbstractShaderProgram::maxAtomicCounterBufferSize() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::shader_atomic_counters>())
    #else
    if(!Context::current().isVersionSupported(Version::GLES310))
    #endif
        return 0;

    GLint& value = Context::current().state().shaderProgram.maxAtomicCounterBufferSize;

    if(value == 0)
        glGetIntegerv(GL_MAX_ATOMIC_COUNTER_BUFFER_SIZE, &value);

    return value;
}

Int AbstractShaderProgram::maxComputeSharedMemorySize() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::compute_shader>())
    #else
    if(!Context::current().isVersionSupported(Version::GLES310))
    #endif
        return 0;

    GLint& value = Context::current().state().shaderProgram.maxComputeSharedMemorySize;

    if(value == 0)
        glGetIntegerv(GL_MAX_COMPUTE_SHARED_MEMORY_SIZE, &value);

    return value;
}

Int AbstractShaderProgram::maxComputeWorkGroupInvocations() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::compute_shader>())
    #else
    if(!Context::current().isVersionSupported(Version::GLES310))
    #endif
        return 0;

    GLint& value = Context::current().state().shaderProgram.maxComputeWorkGroupInvocations;

    if(value == 0)
        glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &value);

    return value;
}

Vector3i AbstractShaderProgram::maxComputeWorkGroupCount() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::compute_shader>())
    #else
    if(!Context::current().isVersionSupported(Version::GLES310))
    #endif
        return {};

    Vector3i& value = Context::current().state().shaderProgram.maxComputeWorkGroupCount;

    if(value.isZero()) {
        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &value.x());
        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &value.y());
        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &value.z());
    }

    return value;
}

Vector3i AbstractShaderProgram::maxComputeWorkGroupSize() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::compute_shader>())
    #else
    if(!Context::current().isVersionSupported(Version::GLES310))
    #endif
        return {};

    Vector3i& value = Context::current().state().shaderProgram.maxComputeWorkGroupSize;

    if(value.isZero()) {
        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &value.x());
        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &value.y());
        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &value.z());
    }

    return value;
}

Int AbstractShaderProgram::maxImageUnits() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::shader_image_load_store>())
    #else
    if(!Context::current().isVersionSupported(Version::GLES310))
    #endif
        return 0;

    GLint& value = Context::current().state().shaderProgram.maxImageUnits;

    if(value == 0)
        glGetIntegerv(GL_MAX_IMAGE_UNITS, &value);

    return value;
}
#endif

#ifndef MAGNUM_TARGET_GLES
Int AbstractShaderProgram::maxImageSamples() {
    if(!Context::current().isExtensionSupported<Extensions::ARB::shader_image_load_store>())
        return 0;

    GLint& value = Context::current().state().shaderProgram.maxImageSamples;

    if(value == 0)
        glGetIntegerv(GL_MAX_IMAGE_SAMPLES, &value);

    return value;
}
#endif

#ifndef MAGNUM_TARGET_GLES2
#ifndef MAGNUM_TARGET_WEBGL
Int AbstractShaderProgram::maxCombinedShaderOutputResources() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::shader_storage_buffer_object>() || !Context::current().isExtensionSupported<Extensions::ARB::shader_image_load_store>())
    #else
    if(!Context::current().isVersionSupported(Version::GLES310))
    #endif
        return 0;

    GLint& value = Context::current().state().shaderProgram.maxCombinedShaderOutputResources;

    if(value == 0)
        glGetIntegerv(GL_MAX_COMBINED_SHADER_OUTPUT_RESOURCES, &value);

    return value;
}

Long AbstractShaderProgram::maxShaderStorageBlockSize() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::shader_storage_buffer_object>())
    #else
    if(!Context::current().isVersionSupported(Version::GLES310))
    #endif
        return 0;

    GLint64& value = Context::current().state().shaderProgram.maxShaderStorageBlockSize;

    if(value == 0)
        glGetInteger64v(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &value);

    return value;
}
#endif

Int AbstractShaderProgram::maxUniformBlockSize() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::uniform_buffer_object>())
        return 0;
    #endif

    GLint& value = Context::current().state().shaderProgram.maxUniformBlockSize;

    if(value == 0)
        glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &value);

    return value;
}

#ifndef MAGNUM_TARGET_WEBGL
Int AbstractShaderProgram::maxUniformLocations() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::explicit_uniform_location>())
    #else
    if(!Context::current().isVersionSupported(Version::GLES310))
    #endif
        return 0;

    GLint& value = Context::current().state().shaderProgram.maxUniformLocations;

    if(value == 0)
        glGetIntegerv(GL_MAX_UNIFORM_LOCATIONS, &value);

    return value;
}
#endif

Int AbstractShaderProgram::minTexelOffset() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::EXT::gpu_shader4>())
        return 0;
    #endif

    GLint& value = Context::current().state().shaderProgram.minTexelOffset;

    if(value == 0)
        glGetIntegerv(GL_MIN_PROGRAM_TEXEL_OFFSET, &value);

    return value;
}

Int AbstractShaderProgram::maxTexelOffset() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::EXT::gpu_shader4>())
        return 0;
    #endif

    GLint& value = Context::current().state().shaderProgram.maxTexelOffset;

    if(value == 0)
        glGetIntegerv(GL_MAX_PROGRAM_TEXEL_OFFSET, &value);

    return value;
}
#endif

AbstractShaderProgram::AbstractShaderProgram(): _id(glCreateProgram()) {
    CORRADE_INTERNAL_ASSERT(_id != Implementation::State::DisengagedBinding);
}

AbstractShaderProgram::AbstractShaderProgram(NoCreateT) noexcept: _id{0} {}

AbstractShaderProgram::AbstractShaderProgram(AbstractShaderProgram&& other) noexcept: _id(other._id) {
    other._id = 0;
}

AbstractShaderProgram::~AbstractShaderProgram() {
    if(!_id) return;

    /* Remove current usage from the state */
    GLuint& current = Context::current().state().shaderProgram.current;
    if(current == _id) current = 0;

    glDeleteProgram(_id);
}

AbstractShaderProgram& AbstractShaderProgram::operator=(AbstractShaderProgram&& other) noexcept {
    using Utility::swap;
    swap(_id, other._id);
    return *this;
}

#ifndef MAGNUM_TARGET_WEBGL
Containers::String AbstractShaderProgram::label() const {
    #ifndef MAGNUM_TARGET_GLES2
    return Context::current().state().debug.getLabelImplementation(GL_PROGRAM, _id);
    #else
    return Context::current().state().debug.getLabelImplementation(GL_PROGRAM_KHR, _id);
    #endif
}

AbstractShaderProgram& AbstractShaderProgram::setLabel(const Containers::StringView label) {
    #ifndef MAGNUM_TARGET_GLES2
    Context::current().state().debug.labelImplementation(GL_PROGRAM, _id, label);
    #else
    Context::current().state().debug.labelImplementation(GL_PROGRAM_KHR, _id, label);
    #endif
    return *this;
}
#endif

Containers::Pair<bool, Containers::String> AbstractShaderProgram::validate() {
    glValidateProgram(_id);

    /* Check validation status */
    GLint success, logLength;
    glGetProgramiv(_id, GL_VALIDATE_STATUS, &success);
    glGetProgramiv(_id, GL_INFO_LOG_LENGTH, &logLength);

    /* Error or warning message. The length is reported including the null
       terminator and the string implicitly has a storage for that, thus
       specify one byte less. */
    Containers::String message{NoInit, std::size_t(Math::max(logLength, 1) - 1)};
    if(logLength > 1)
        glGetProgramInfoLog(_id, logLength, nullptr, message.data());

    /* On some drivers (such as SwiftShader) the message contains a newline at
       the end, on some (such as Mesa) it doesn't. Same as with link() or
       compile() message trimming it doesn't make sense to add driver-specific
       workarounds for this, so just trim it always. */
    /** @todo this allocates a new string, revisit once String is capable of
        trimming in-place, e.g. `Utility::move(message).trimmed()` would just
        shift the data around */
    return {bool(success), message.trimmed()};
}

AbstractShaderProgram& AbstractShaderProgram::draw(Mesh& mesh) {
    CORRADE_ASSERT(mesh._countSet, "GL::AbstractShaderProgram::draw(): Mesh::setCount() was never called, probably a mistake?", *this);

    /* Nothing to draw, exit without touching any state */
    if(!mesh._count || !mesh._instanceCount) return *this;

    use();

    #ifndef MAGNUM_TARGET_GLES2
    mesh.drawInternal(mesh._count, mesh._baseVertex, mesh._instanceCount, mesh._baseInstance, mesh._indexOffset, mesh._indexStart, mesh._indexEnd);
    #else
    mesh.drawInternal(mesh._count, mesh._baseVertex, mesh._instanceCount, mesh._indexOffset);
    #endif
    return *this;
}

AbstractShaderProgram& AbstractShaderProgram::draw(MeshView& mesh) {
    CORRADE_ASSERT(mesh._countSet, "GL::AbstractShaderProgram::draw(): MeshView::setCount() was never called, probably a mistake?", *this);

    /* Nothing to draw, exit without touching any state */
    if(!mesh._count || !mesh._instanceCount) return *this;

    use();

    #ifndef MAGNUM_TARGET_GLES2
    mesh._original->drawInternal(mesh._count, mesh._baseVertex, mesh._instanceCount, mesh._baseInstance, mesh._indexOffset, mesh._indexStart, mesh._indexEnd);
    #else
    mesh._original->drawInternal(mesh._count, mesh._baseVertex, mesh._instanceCount, mesh._indexOffset);
    #endif
    return *this;
}

AbstractShaderProgram& AbstractShaderProgram::draw(Mesh& mesh, const Containers::StridedArrayView1D<const UnsignedInt>& counts, const Containers::StridedArrayView1D<const UnsignedInt>& vertexOffsets, const Containers::StridedArrayView1D<const UnsignedInt>& indexOffsets) {
    if(!counts.size()) return *this;

    use();

    mesh.drawInternalStrided(counts, vertexOffsets, indexOffsets);
    return *this;
}

#ifndef CORRADE_TARGET_32BIT
AbstractShaderProgram& AbstractShaderProgram::draw(Mesh& mesh, const Containers::StridedArrayView1D<const UnsignedInt>& counts, const Containers::StridedArrayView1D<const UnsignedInt>& vertexOffsets, const Containers::StridedArrayView1D<const UnsignedLong>& indexOffsets) {
    if(!counts.size()) return *this;

    use();

    mesh.drawInternalStrided(counts, vertexOffsets, indexOffsets);
    return *this;
}

AbstractShaderProgram& AbstractShaderProgram::draw(Mesh& mesh, const Containers::StridedArrayView1D<const UnsignedInt>& counts, const Containers::StridedArrayView1D<const UnsignedInt>& vertexOffsets, std::nullptr_t) {
    return draw(mesh, counts, vertexOffsets, Containers::StridedArrayView1D<const UnsignedLong>{});
}
#endif

#ifdef MAGNUM_TARGET_GLES
#ifndef MAGNUM_TARGET_GLES2
AbstractShaderProgram& AbstractShaderProgram::draw(Mesh& mesh, const Containers::StridedArrayView1D<const UnsignedInt>& counts, const Containers::StridedArrayView1D<const UnsignedInt>& instanceCounts, const Containers::StridedArrayView1D<const UnsignedInt>& vertexOffsets, const Containers::StridedArrayView1D<const UnsignedInt>& indexOffsets, const Containers::StridedArrayView1D<const UnsignedInt>& instanceOffsets) {
    if(!counts.size()) return *this;

    use();

    mesh.drawInternalStrided(counts, instanceCounts, vertexOffsets, indexOffsets, instanceOffsets);
    return *this;
}

#ifndef CORRADE_TARGET_32BIT
AbstractShaderProgram& AbstractShaderProgram::draw(Mesh& mesh, const Containers::StridedArrayView1D<const UnsignedInt>& counts, const Containers::StridedArrayView1D<const UnsignedInt>& instanceCounts, const Containers::StridedArrayView1D<const UnsignedInt>& vertexOffsets, const Containers::StridedArrayView1D<const UnsignedLong>& indexOffsets, const Containers::StridedArrayView1D<const UnsignedInt>& instanceOffsets) {
    if(!counts.size()) return *this;

    use();

    mesh.drawInternalStrided(counts, instanceCounts, vertexOffsets, indexOffsets, instanceOffsets);
    return *this;
}

AbstractShaderProgram& AbstractShaderProgram::draw(Mesh& mesh, const Containers::StridedArrayView1D<const UnsignedInt>& counts, const Containers::StridedArrayView1D<const UnsignedInt>& instanceCounts, const Containers::StridedArrayView1D<const UnsignedInt>& vertexOffsets, std::nullptr_t, const Containers::StridedArrayView1D<const UnsignedInt>& instanceOffsets) {
    return draw(mesh, counts, instanceCounts, vertexOffsets, Containers::StridedArrayView1D<const UnsignedLong>{}, instanceOffsets);
}
#endif
#endif

AbstractShaderProgram& AbstractShaderProgram::draw(Mesh& mesh, const Containers::StridedArrayView1D<const UnsignedInt>& counts, const Containers::StridedArrayView1D<const UnsignedInt>& instanceCounts, const Containers::StridedArrayView1D<const UnsignedInt>& vertexOffsets, const Containers::StridedArrayView1D<const UnsignedInt>& indexOffsets) {
    if(!counts.size()) return *this;

    use();

    mesh.drawInternalStrided(counts, instanceCounts, vertexOffsets, indexOffsets
        #ifndef MAGNUM_TARGET_GLES2
        , Containers::StridedArrayView1D<const UnsignedInt>{}
        #endif
    );
    return *this;
}

#ifndef CORRADE_TARGET_32BIT
AbstractShaderProgram& AbstractShaderProgram::draw(Mesh& mesh, const Containers::StridedArrayView1D<const UnsignedInt>& counts, const Containers::StridedArrayView1D<const UnsignedInt>& instanceCounts, const Containers::StridedArrayView1D<const UnsignedInt>& vertexOffsets, const Containers::StridedArrayView1D<const UnsignedLong>& indexOffsets) {
    if(!counts.size()) return *this;

    use();

    mesh.drawInternalStrided(counts, instanceCounts, vertexOffsets, indexOffsets
        #ifndef MAGNUM_TARGET_GLES2
        , Containers::StridedArrayView1D<const UnsignedInt>{}
        #endif
    );
    return *this;
}

AbstractShaderProgram& AbstractShaderProgram::draw(Mesh& mesh, const Containers::StridedArrayView1D<const UnsignedInt>& counts, const Containers::StridedArrayView1D<const UnsignedInt>& instanceCounts, const Containers::StridedArrayView1D<const UnsignedInt>& vertexOffsets, std::nullptr_t) {
    return draw(mesh, counts, instanceCounts, vertexOffsets, Containers::StridedArrayView1D<const UnsignedLong>{});
}
#endif
#endif

AbstractShaderProgram& AbstractShaderProgram::draw(const Containers::Iterable<MeshView>& meshes) {
    if(meshes.isEmpty()) return *this;

    use();

    #ifndef CORRADE_NO_ASSERT
    const Mesh* original = &*meshes.front()._original;
    for(std::size_t i = 0; i != meshes.size(); ++i)
        CORRADE_ASSERT(&*meshes[i]._original == original, "GL::AbstractShaderProgram::draw(): all meshes must be views of the same original mesh, expected" << original << "but got" << &*meshes[i]._original << "at index" << i, *this);
    #endif

    #ifndef MAGNUM_TARGET_GLES
    MeshView::multiDrawImplementationDefault(meshes);
    #else
    Context::current().state().mesh.multiDrawViewImplementation(meshes);
    #endif
    return *this;
}

#ifndef MAGNUM_TARGET_GLES
AbstractShaderProgram& AbstractShaderProgram::drawTransformFeedback(Mesh& mesh, TransformFeedback& xfb, UnsignedInt stream) {
    /* Nothing to draw, exit without touching any state */
    if(!mesh._instanceCount) return *this;

    use();
    mesh.drawInternal(xfb, stream, mesh._instanceCount);
    return *this;
}

AbstractShaderProgram& AbstractShaderProgram::drawTransformFeedback(MeshView& mesh, TransformFeedback& xfb, UnsignedInt stream) {
    /* If nothing to draw, exit without touching any state */
    if(mesh._instanceCount) return *this;

    use();
    mesh._original->drawInternal(xfb, stream, mesh._instanceCount);
    return *this;
}
#endif

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
AbstractShaderProgram& AbstractShaderProgram::dispatchCompute(const Vector3ui& workgroupCount) {
    /* Nothing to dispatch, exit without touching any state */
    if(!workgroupCount.product()) return *this;

    use();
    glDispatchCompute(workgroupCount.x(), workgroupCount.y(), workgroupCount.z());
    return *this;
}
#endif

void AbstractShaderProgram::use(const GLuint id) {
    /* Use only if the program isn't already in use */
    GLuint& current = Context::current().state().shaderProgram.current;
    if(current != id) glUseProgram(current = id);
}

void AbstractShaderProgram::use() { use(_id); }

void AbstractShaderProgram::attachShader(Shader& shader) {
    glAttachShader(_id, shader.id());
}

void AbstractShaderProgram::attachShaders(const Containers::Iterable<Shader>& shaders) {
    for(Shader& s: shaders) attachShader(s);
}

void AbstractShaderProgram::bindAttributeLocation(const UnsignedInt location, const Containers::StringView name) {
    glBindAttribLocation(_id, location, Containers::String::nullTerminatedView(name).data());
}

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
void AbstractShaderProgram::bindFragmentDataLocation(const UnsignedInt location, const Containers::StringView name) {
    #ifndef MAGNUM_TARGET_GLES
    glBindFragDataLocation
    #else
    glBindFragDataLocationEXT
    #endif
        (_id, location, Containers::String::nullTerminatedView(name).data());
}

void AbstractShaderProgram::bindFragmentDataLocationIndexed(const UnsignedInt location, UnsignedInt index, const Containers::StringView name) {
    #ifndef MAGNUM_TARGET_GLES
    glBindFragDataLocationIndexed
    #else
    glBindFragDataLocationIndexedEXT
    #endif
        (_id, location, index, Containers::String::nullTerminatedView(name).data());
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void AbstractShaderProgram::setTransformFeedbackOutputs(const Containers::StringIterable& outputs, const TransformFeedbackBufferMode bufferMode) {
    Context::current().state().shaderProgram.transformFeedbackVaryingsImplementation(*this, outputs, bufferMode);
}

void AbstractShaderProgram::transformFeedbackVaryingsImplementationDefault(AbstractShaderProgram& self, const Containers::StringIterable& outputs, const TransformFeedbackBufferMode bufferMode) {
    Containers::ArrayView<Containers::String> nameData;
    Containers::ArrayView<const char*> names;
    Containers::ArrayTuple data{
        {NoInit, outputs.size(), nameData},
        {NoInit, outputs.size(), names}
    };
    for(std::size_t i = 0; i != outputs.size(); ++i) {
        new(&nameData[i]) Containers::String{Containers::String::nullTerminatedView(outputs[i])};
        names[i] = nameData[i].data();
    }

    glTransformFeedbackVaryings(self._id, outputs.size(), names, GLenum(bufferMode));
}

#ifdef CORRADE_TARGET_WINDOWS
void AbstractShaderProgram::transformFeedbackVaryingsImplementationDanglingWorkaround(AbstractShaderProgram& self, const Containers::StringIterable& outputs, const TransformFeedbackBufferMode bufferMode) {
    /* NVidia on Windows doesn't copy the names when calling
       glTransformFeedbackVaryings() so it then might fail at link time because
       the char* get dangling. We have to do the copy on the engine side and
       keep the values until link time (which can happen any time and multiple
       times, so basically for the remaining lifetime of the shader program).

       Compared to the above case we thus do a copy also if the view is not
       global in addition to not null-terminated, and we keep the string memory
       in a member variable. */
    Containers::ArrayView<Containers::String> nameData;
    Containers::ArrayView<const char*> names;
    self._transformFeedbackVaryingNames = Containers::ArrayTuple{
        {NoInit, outputs.size(), nameData},
        {NoInit, outputs.size(), names}
    };
    for(std::size_t i = 0; i != outputs.size(); ++i) {
        new(&nameData[i]) Containers::String{Containers::String::nullTerminatedGlobalView(outputs[i])};
        names[i] = nameData[i].data();
    }

    glTransformFeedbackVaryings(self._id, outputs.size(), names, GLenum(bufferMode));
}
#endif
#endif

bool AbstractShaderProgram::link() {
    submitLink();
    return checkLink({});
}

void AbstractShaderProgram::submitLink() {
    glLinkProgram(_id);
}

bool AbstractShaderProgram::checkLink(const Containers::Iterable<Shader>& shaders) {
    /* If any compilation failed, abort without even checking the link status.
       The checkCompile() API is called always, to print also compilation
       warnings even in case everything still manages to link well. */
    for(Shader& shader: shaders)
        if(!shader.checkCompile()) return false;

    GLint success, logLength;
    glGetProgramiv(_id, GL_LINK_STATUS, &success);
    glGetProgramiv(_id, GL_INFO_LOG_LENGTH, &logLength);

    /* Error or warning message. The length is reported including the null
       terminator and the string implicitly has a storage for that, thus
       specify one byte less. */
    Containers::String message{NoInit, std::size_t(Math::max(logLength, 1)) - 1};
    if(logLength > 1)
        glGetProgramInfoLog(_id, logLength, nullptr, message.data());

    /* Some drivers are chatty and can't keep shut when there's nothing to
       be said, handle that as well. */
    Context::current().state().shaderProgram.cleanLogImplementation(message);

    /* Usually the driver messages contain a newline at the end. But sometimes
       not, such as in case of a program link error due to shaders not being
       compiled yet on Mesa; sometimes there's two newlines, sometimes just a
       newline and nothing else etc. Because trying do this in driver-specific
       workarounds would involve an impossible task of checking all possible
       error messages on every possible driver, just trim all whitespace around
       the message always and let Debug add its own newline. */
    const Containers::StringView messageTrimmed = Containers::StringView{message}.trimmed();

    /* Show error log */
    if(!success) {
        Error{} << "GL::AbstractShaderProgram::link(): linking failed with the following message:"
            << Debug::newline << messageTrimmed;

    /* Or just warnings, if any */
    } else if(messageTrimmed) {
        Warning{} << "GL::AbstractShaderProgram::link(): linking succeeded with the following message:"
            << Debug::newline << messageTrimmed;
    }

    return success;
}

#ifdef MAGNUM_BUILD_DEPRECATED
bool AbstractShaderProgram::link(std::initializer_list<Containers::Reference<AbstractShaderProgram>> shaders) {
    for(AbstractShaderProgram& shader: shaders) shader.submitLink();
    bool allSuccess = true;
    for(AbstractShaderProgram& shader: shaders) allSuccess = allSuccess && shader.checkLink({});
    return allSuccess;
}
#endif

bool AbstractShaderProgram::isLinkFinished() {
    GLint success;
    Context::current().state().shaderProgram.completionStatusImplementation(_id, GL_COMPLETION_STATUS_KHR, &success);
    return success == GL_TRUE;
}

void AbstractShaderProgram::cleanLogImplementationNoOp(Containers::String&) {}

#if defined(CORRADE_TARGET_WINDOWS) && !defined(MAGNUM_TARGET_GLES)
void AbstractShaderProgram::cleanLogImplementationIntelWindows(Containers::String& message) {
    if(message == "No errors.\n") message = {};
}
#endif

#if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL)
void AbstractShaderProgram::cleanLogImplementationAngle(Containers::String& message) {
    if(message == "\n") message = {};
}
#endif

void AbstractShaderProgram::completionStatusImplementationFallback(GLuint, GLenum, GLint* value) {
    *value = GL_TRUE;
}

Int AbstractShaderProgram::uniformLocation(const Containers::StringView name) {
    const GLint location = glGetUniformLocation(_id, Containers::String::nullTerminatedView(name).data());
    if(location == -1)
        Warning{} << "GL::AbstractShaderProgram: location of uniform \'" << Debug::nospace << name << Debug::nospace << "\' cannot be retrieved";
    return location;
}

#ifndef MAGNUM_TARGET_GLES2
UnsignedInt AbstractShaderProgram::uniformBlockIndex(const Containers::StringView name) {
    const GLuint index = glGetUniformBlockIndex(_id, Containers::String::nullTerminatedView(name).data());
    if(index == GL_INVALID_INDEX)
        Warning{} << "GL::AbstractShaderProgram: index of uniform block \'" << Debug::nospace << name << Debug::nospace << "\' cannot be retrieved";
    return index;
}
#endif

void AbstractShaderProgram::setUniform(const Int location, Float value) {
    #ifndef MAGNUM_TARGET_WEBGL
    Context::current().state().shaderProgram.uniform1fImplementation
    #else
    uniform1fImplementationDefault
    #endif
        (_id, location, value);
}

#ifdef MAGNUM_TARGET_WEBGL
inline
#endif
void AbstractShaderProgram::uniform1fImplementationDefault(const GLuint id, const GLint location, GLfloat v0) {
    use(id);
    glUniform1f(location, v0);
}

void AbstractShaderProgram::setUniform(const Int location, const Math::Vector<2, Float>& value) {
    #ifndef MAGNUM_TARGET_WEBGL
    Context::current().state().shaderProgram.uniform2fImplementation
    #else
    uniform2fImplementationDefault
    #endif
        (_id, location, value[0], value[1]);
}

#ifdef MAGNUM_TARGET_WEBGL
inline
#endif
void AbstractShaderProgram::uniform2fImplementationDefault(const GLuint id, const GLint location, GLfloat v0, GLfloat v1) {
    use(id);
    glUniform2f(location, v0, v1);
}

void AbstractShaderProgram::setUniform(const Int location, const Math::Vector<3, Float>& value) {
    #ifndef MAGNUM_TARGET_WEBGL
    Context::current().state().shaderProgram.uniform3fImplementation
    #else
    uniform3fImplementationDefault
    #endif
        (_id, location, value[0], value[1], value[2]);
}

#ifdef MAGNUM_TARGET_WEBGL
inline
#endif
void AbstractShaderProgram::uniform3fImplementationDefault(const GLuint id, const GLint location, GLfloat v0, GLfloat v1, GLfloat v2) {
    use(id);
    glUniform3f(location, v0, v1, v2);
}

void AbstractShaderProgram::setUniform(const Int location, const Math::Vector<4, Float>& value) {
    #ifndef MAGNUM_TARGET_WEBGL
    Context::current().state().shaderProgram.uniform4fImplementation
    #else
    uniform4fImplementationDefault
    #endif
        (_id, location, value[0], value[1], value[2], value[3]);
}

#ifdef MAGNUM_TARGET_WEBGL
inline
#endif
void AbstractShaderProgram::uniform4fImplementationDefault(const GLuint id, const GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) {
    use(id);
    glUniform4f(location, v0, v1, v2, v3);
}

void AbstractShaderProgram::setUniform(const Int location, Int value) {
    #ifndef MAGNUM_TARGET_WEBGL
    Context::current().state().shaderProgram.uniform1iImplementation
    #else
    uniform1iImplementationDefault
    #endif
        (_id, location, value);
}

#ifdef MAGNUM_TARGET_WEBGL
inline
#endif
void AbstractShaderProgram::uniform1iImplementationDefault(const GLuint id, const GLint location, GLint v0) {
    use(id);
    glUniform1i(location, v0);
}

void AbstractShaderProgram::setUniform(const Int location, const Math::Vector<2, Int>& value) {
    #ifndef MAGNUM_TARGET_WEBGL
    Context::current().state().shaderProgram.uniform2iImplementation
    #else
    uniform2iImplementationDefault
    #endif
        (_id, location, value[0], value[1]);
}

#ifdef MAGNUM_TARGET_WEBGL
inline
#endif
void AbstractShaderProgram::uniform2iImplementationDefault(const GLuint id, const GLint location, GLint v0, GLint v1) {
    use(id);
    glUniform2i(location, v0, v1);
}

void AbstractShaderProgram::setUniform(const Int location, const Math::Vector<3, Int>& value) {
    #ifndef MAGNUM_TARGET_WEBGL
    Context::current().state().shaderProgram.uniform3iImplementation
    #else
    uniform3iImplementationDefault
    #endif
        (_id, location, value[0], value[1], value[2]);
}

#ifdef MAGNUM_TARGET_WEBGL
inline
#endif
void AbstractShaderProgram::uniform3iImplementationDefault(const GLuint id, const GLint location, GLint v0, GLint v1, GLint v2) {
    use(id);
    glUniform3i(location, v0, v1, v2);
}

void AbstractShaderProgram::setUniform(const Int location, const Math::Vector<4, Int>& value) {
    #ifndef MAGNUM_TARGET_WEBGL
    Context::current().state().shaderProgram.uniform4iImplementation
    #else
    uniform4iImplementationDefault
    #endif
        (_id, location, value[0], value[1], value[2], value[3]);
}

#ifdef MAGNUM_TARGET_WEBGL
inline
#endif
void AbstractShaderProgram::uniform4iImplementationDefault(const GLuint id, const GLint location, GLint v0, GLint v1, GLint v2, GLint v3) {
    use(id);
    glUniform4i(location, v0, v1, v2, v3);
}

#ifndef MAGNUM_TARGET_GLES2
void AbstractShaderProgram::setUniform(const Int location, UnsignedInt value) {
    #ifndef MAGNUM_TARGET_WEBGL
    Context::current().state().shaderProgram.uniform1uiImplementation
    #else
    uniform1uiImplementationDefault
    #endif
        (_id, location, value);
}

#ifdef MAGNUM_TARGET_WEBGL
inline
#endif
void AbstractShaderProgram::uniform1uiImplementationDefault(const GLuint id, const GLint location, GLuint v0) {
    use(id);
    glUniform1ui(location, v0);
}

void AbstractShaderProgram::setUniform(const Int location, const Math::Vector<2, UnsignedInt>& value) {
    #ifndef MAGNUM_TARGET_WEBGL
    Context::current().state().shaderProgram.uniform2uiImplementation
    #else
    uniform2uiImplementationDefault
    #endif
        (_id, location, value[0], value[1]);
}

#ifdef MAGNUM_TARGET_WEBGL
inline
#endif
void AbstractShaderProgram::uniform2uiImplementationDefault(const GLuint id, const GLint location, GLuint v0, GLuint v1) {
    use(id);
    glUniform2ui(location, v0, v1);
}

void AbstractShaderProgram::setUniform(const Int location, const Math::Vector<3, UnsignedInt>& value) {
    #ifndef MAGNUM_TARGET_WEBGL
    Context::current().state().shaderProgram.uniform3uiImplementation
    #else
    uniform3uiImplementationDefault
    #endif
        (_id, location, value[0], value[1], value[2]);
}

#ifdef MAGNUM_TARGET_WEBGL
inline
#endif
void AbstractShaderProgram::uniform3uiImplementationDefault(const GLuint id, const GLint location, GLuint v0, GLuint v1, GLuint v2) {
    use(id);
    glUniform3ui(location, v0, v1, v2);
}

void AbstractShaderProgram::setUniform(const Int location, const Math::Vector<4, UnsignedInt>& value) {
    #ifndef MAGNUM_TARGET_WEBGL
    Context::current().state().shaderProgram.uniform4uiImplementation
    #else
    uniform4uiImplementationDefault
    #endif
        (_id, location, value[0], value[1], value[2], value[3]);
}

#ifdef MAGNUM_TARGET_WEBGL
inline
#endif
void AbstractShaderProgram::uniform4uiImplementationDefault(const GLuint id, const GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3) {
    use(id);
    glUniform4ui(location, v0, v1, v2, v3);
}
#endif

#ifndef MAGNUM_TARGET_GLES
void AbstractShaderProgram::setUniform(const Int location, Double value) {
    #ifndef MAGNUM_TARGET_WEBGL
    Context::current().state().shaderProgram.uniform1dImplementation
    #else
    uniform1dImplementationDefault
    #endif
        (_id, location, value);
}

void AbstractShaderProgram::uniform1dImplementationDefault(const GLuint id, const GLint location, GLdouble v0) {
    use(id);
    glUniform1d(location, v0);
}

void AbstractShaderProgram::setUniform(const Int location, const Math::Vector<2, Double>& value) {
    Context::current().state().shaderProgram.uniform2dImplementation(_id, location, value[0], value[1]);
}

void AbstractShaderProgram::uniform2dImplementationDefault(const GLuint id, const GLint location, GLdouble v0, GLdouble v1) {
    use(id);
    glUniform2d(location, v0, v1);
}

void AbstractShaderProgram::setUniform(const Int location, const Math::Vector<3, Double>& value) {
    Context::current().state().shaderProgram.uniform3dImplementation(_id, location, value[0], value[1], value[2]);
}

void AbstractShaderProgram::uniform3dImplementationDefault(const GLuint id, const GLint location, GLdouble v0, GLdouble v1, GLdouble v2) {
    use(id);
    glUniform3d(location, v0, v1, v2);
}

void AbstractShaderProgram::setUniform(const Int location, const Math::Vector<4, Double>& value) {
    Context::current().state().shaderProgram.uniform4dImplementation(_id, location, value[0], value[1], value[2], value[3]);
}

void AbstractShaderProgram::uniform4dImplementationDefault(const GLuint id, const GLint location, GLdouble v0, GLdouble v1, GLdouble v2, GLdouble v3) {
    use(id);
    glUniform4d(location, v0, v1, v2, v3);
}
#endif

void AbstractShaderProgram::setUniform(const Int location, const Containers::ArrayView<const Float> values) {
    #ifndef MAGNUM_TARGET_WEBGL
    Context::current().state().shaderProgram.uniform1fvImplementation
    #else
    uniform1fvImplementationDefault
    #endif
        (_id, location, values.size(), values.data());
}

#ifdef MAGNUM_TARGET_WEBGL
inline
#endif
void AbstractShaderProgram::uniform1fvImplementationDefault(const GLuint id, const GLint location, const GLsizei count, const GLfloat* const values) {
    use(id);
    glUniform1fv(location, count, values);
}

void AbstractShaderProgram::setUniform(const Int location,  const Containers::ArrayView<const Math::Vector<2, Float>> values) {
    #ifndef MAGNUM_TARGET_WEBGL
    Context::current().state().shaderProgram.uniform2fvImplementation
    #else
    uniform2fvImplementationDefault
    #endif
        (_id, location, values.size(), values.data()->data());
}

#ifdef MAGNUM_TARGET_WEBGL
inline
#endif
void AbstractShaderProgram::uniform2fvImplementationDefault(const GLuint id, const GLint location, const GLsizei count, const GLfloat* const values) {
    use(id);
    glUniform2fv(location, count, values);
}

void AbstractShaderProgram::setUniform(const Int location, const Containers::ArrayView<const Math::Vector<3, Float>> values) {
    #ifndef MAGNUM_TARGET_WEBGL
    Context::current().state().shaderProgram.uniform3fvImplementation
    #else
    uniform3fvImplementationDefault
    #endif
        (_id, location, values.size(), values.data()->data());
}

#ifdef MAGNUM_TARGET_WEBGL
inline
#endif
void AbstractShaderProgram::uniform3fvImplementationDefault(const GLuint id, const GLint location, const GLsizei count, const GLfloat* const values) {
    use(id);
    glUniform3fv(location, count, values);
}

void AbstractShaderProgram::setUniform(const Int location, const Containers::ArrayView<const Math::Vector<4, Float>> values) {
    #ifndef MAGNUM_TARGET_WEBGL
    Context::current().state().shaderProgram.uniform4fvImplementation
    #else
    uniform4fvImplementationDefault
    #endif
        (_id, location, values.size(), values.data()->data());
}

#ifdef MAGNUM_TARGET_WEBGL
inline
#endif
void AbstractShaderProgram::uniform4fvImplementationDefault(const GLuint id, const GLint location, const GLsizei count, const GLfloat* const values) {
    use(id);
    glUniform4fv(location, count, values);
}

void AbstractShaderProgram::setUniform(const Int location, const Containers::ArrayView<const Int> values) {
    #ifndef MAGNUM_TARGET_WEBGL
    Context::current().state().shaderProgram.uniform1ivImplementation
    #else
    uniform1ivImplementationDefault
    #endif
        (_id, location, values.size(), values.data());
}

#ifdef MAGNUM_TARGET_WEBGL
inline
#endif
void AbstractShaderProgram::uniform1ivImplementationDefault(const GLuint id, const GLint location, const GLsizei count, const GLint* values) {
    use(id);
    glUniform1iv(location, count, values);
}

void AbstractShaderProgram::setUniform(const Int location, const Containers::ArrayView<const Math::Vector<2, Int>> values) {
    #ifndef MAGNUM_TARGET_WEBGL
    Context::current().state().shaderProgram.uniform2ivImplementation
    #else
    uniform2ivImplementationDefault
    #endif
        (_id, location, values.size(), values.data()->data());
}

#ifdef MAGNUM_TARGET_WEBGL
inline
#endif
void AbstractShaderProgram::uniform2ivImplementationDefault(const GLuint id, const GLint location, const GLsizei count, const GLint* const values) {
    use(id);
    glUniform2iv(location, count, values);
}

void AbstractShaderProgram::setUniform(const Int location, const Containers::ArrayView<const Math::Vector<3, Int>> values) {
    #ifndef MAGNUM_TARGET_WEBGL
    Context::current().state().shaderProgram.uniform3ivImplementation
    #else
    uniform3ivImplementationDefault
    #endif
        (_id, location, values.size(), values.data()->data());
}

#ifdef MAGNUM_TARGET_WEBGL
inline
#endif
void AbstractShaderProgram::uniform3ivImplementationDefault(const GLuint id, const GLint location, const GLsizei count, const GLint* const values) {
    use(id);
    glUniform3iv(location, count, values);
}

void AbstractShaderProgram::setUniform(const Int location, const Containers::ArrayView<const Math::Vector<4, Int>> values) {
    #ifndef MAGNUM_TARGET_WEBGL
    Context::current().state().shaderProgram.uniform4ivImplementation
    #else
    uniform4ivImplementationDefault
    #endif
        (_id, location, values.size(), values.data()->data());
}

#ifdef MAGNUM_TARGET_WEBGL
inline
#endif
void AbstractShaderProgram::uniform4ivImplementationDefault(const GLuint id, const GLint location, const GLsizei count, const GLint* const values) {
    use(id);
    glUniform4iv(location, count, values);
}

#ifndef MAGNUM_TARGET_GLES2
void AbstractShaderProgram::setUniform(const Int location, const Containers::ArrayView<const UnsignedInt> values) {
    #ifndef MAGNUM_TARGET_WEBGL
    Context::current().state().shaderProgram.uniform1uivImplementation
    #else
    uniform1uivImplementationDefault
    #endif
        (_id, location, values.size(), values.data());
}

#ifdef MAGNUM_TARGET_WEBGL
inline
#endif
void AbstractShaderProgram::uniform1uivImplementationDefault(const GLuint id, const GLint location, const GLsizei count, const GLuint* const values) {
    use(id);
    glUniform1uiv(location, count, values);
}

void AbstractShaderProgram::setUniform(const Int location, const Containers::ArrayView<const Math::Vector<2, UnsignedInt>> values) {
    #ifndef MAGNUM_TARGET_WEBGL
    Context::current().state().shaderProgram.uniform2uivImplementation
    #else
    uniform2uivImplementationDefault
    #endif
        (_id, location, values.size(), values.data()->data());
}

#ifdef MAGNUM_TARGET_WEBGL
inline
#endif
void AbstractShaderProgram::uniform2uivImplementationDefault(const GLuint id, const GLint location, const GLsizei count, const GLuint* const values) {
    use(id);
    glUniform2uiv(location, count, values);
}

void AbstractShaderProgram::setUniform(const Int location, const Containers::ArrayView<const Math::Vector<3, UnsignedInt>> values) {
    #ifndef MAGNUM_TARGET_WEBGL
    Context::current().state().shaderProgram.uniform3uivImplementation
    #else
    uniform3uivImplementationDefault
    #endif
        (_id, location, values.size(), values.data()->data());
}

#ifdef MAGNUM_TARGET_WEBGL
inline
#endif
void AbstractShaderProgram::uniform3uivImplementationDefault(const GLuint id, const GLint location, const GLsizei count, const GLuint* const values) {
    use(id);
    glUniform3uiv(location, count, values);
}

void AbstractShaderProgram::setUniform(const Int location, const Containers::ArrayView<const Math::Vector<4, UnsignedInt>> values) {
    #ifndef MAGNUM_TARGET_WEBGL
    Context::current().state().shaderProgram.uniform4uivImplementation
    #else
    uniform4uivImplementationDefault
    #endif
        (_id, location, values.size(), values.data()->data());
}

#ifdef MAGNUM_TARGET_WEBGL
inline
#endif
void AbstractShaderProgram::uniform4uivImplementationDefault(const GLuint id, const GLint location, const GLsizei count, const GLuint* const values) {
    use(id);
    glUniform4uiv(location, count, values);
}
#endif

#ifndef MAGNUM_TARGET_GLES
void AbstractShaderProgram::setUniform(const Int location, const Containers::ArrayView<const Double> values) {
    Context::current().state().shaderProgram.uniform1dvImplementation(_id, location, values.size(), values.data());
}

void AbstractShaderProgram::uniform1dvImplementationDefault(const GLuint id, const GLint location, const GLsizei count, const GLdouble* const values) {
    use(id);
    glUniform1dv(location, count, values);
}

void AbstractShaderProgram::setUniform(const Int location, const Containers::ArrayView<const Math::Vector<2, Double>> values) {
    Context::current().state().shaderProgram.uniform2dvImplementation(_id, location, values.size(), values.data()->data());
}

void AbstractShaderProgram::uniform2dvImplementationDefault(const GLuint id, const GLint location, const GLsizei count, const GLdouble* const values) {
    use(id);
    glUniform2dv(location, count, values);
}

void AbstractShaderProgram::setUniform(const Int location, const Containers::ArrayView<const Math::Vector<3, Double>> values) {
    Context::current().state().shaderProgram.uniform3dvImplementation(_id, location, values.size(), values.data()->data());
}

void AbstractShaderProgram::uniform3dvImplementationDefault(const GLuint id, const GLint location, const GLsizei count, const GLdouble* const values) {
    use(id);
    glUniform3dv(location, count, values);
}

void AbstractShaderProgram::setUniform(const Int location, const Containers::ArrayView<const Math::Vector<4, Double>> values) {
    Context::current().state().shaderProgram.uniform4dvImplementation(_id, location, values.size(), values.data()->data());
}

void AbstractShaderProgram::uniform4dvImplementationDefault(const GLuint id, const GLint location, const GLsizei count, const GLdouble* const values) {
    use(id);
    glUniform4dv(location, count, values);
}
#endif

void AbstractShaderProgram::setUniform(const Int location, const Containers::ArrayView<const Math::RectangularMatrix<2, 2, Float>> values) {
    #ifndef MAGNUM_TARGET_WEBGL
    Context::current().state().shaderProgram.uniformMatrix2fvImplementation
    #else
    uniformMatrix2fvImplementationDefault
    #endif
        (_id, location, values.size(), GL_FALSE, values.data()->data());
}

#ifdef MAGNUM_TARGET_WEBGL
inline
#endif
void AbstractShaderProgram::uniformMatrix2fvImplementationDefault(const GLuint id, const GLint location, const GLsizei count, const GLboolean transpose, const GLfloat* const values) {
    use(id);
    glUniformMatrix2fv(location, count, transpose, values);
}

void AbstractShaderProgram::setUniform(const Int location, const Containers::ArrayView<const Math::RectangularMatrix<3, 3, Float>> values) {
    #ifndef MAGNUM_TARGET_WEBGL
    Context::current().state().shaderProgram.uniformMatrix3fvImplementation
    #else
    uniformMatrix3fvImplementationDefault
    #endif
        (_id, location, values.size(), GL_FALSE, values.data()->data());
}

#ifdef MAGNUM_TARGET_WEBGL
inline
#endif
void AbstractShaderProgram::uniformMatrix3fvImplementationDefault(const GLuint id, const GLint location, const GLsizei count, const GLboolean transpose, const GLfloat* const values) {
    use(id);
    glUniformMatrix3fv(location, count, transpose, values);
}

void AbstractShaderProgram::setUniform(const Int location, const Containers::ArrayView<const Math::RectangularMatrix<4, 4, Float>> values) {
    #ifndef MAGNUM_TARGET_WEBGL
    Context::current().state().shaderProgram.uniformMatrix4fvImplementation
    #else
    uniformMatrix4fvImplementationDefault
    #endif
        (_id, location, values.size(), GL_FALSE, values.data()->data());
}

#ifdef MAGNUM_TARGET_WEBGL
inline
#endif
void AbstractShaderProgram::uniformMatrix4fvImplementationDefault(const GLuint id, const GLint location, const GLsizei count, const GLboolean transpose, const GLfloat* const values) {
    use(id);
    glUniformMatrix4fv(location, count, transpose, values);
}

#ifndef MAGNUM_TARGET_GLES2
void AbstractShaderProgram::setUniform(const Int location, const Containers::ArrayView<const Math::RectangularMatrix<2, 3, Float>> values) {
    #ifndef MAGNUM_TARGET_WEBGL
    Context::current().state().shaderProgram.uniformMatrix2x3fvImplementation
    #else
    uniformMatrix2x3fvImplementationDefault
    #endif
        (_id, location, values.size(), GL_FALSE, values.data()->data());
}

#ifdef MAGNUM_TARGET_WEBGL
inline
#endif
void AbstractShaderProgram::uniformMatrix2x3fvImplementationDefault(const GLuint id, const GLint location, const GLsizei count, const GLboolean transpose, const GLfloat* const values) {
    use(id);
    glUniformMatrix2x3fv(location, count, transpose, values);
}

void AbstractShaderProgram::setUniform(const Int location, const Containers::ArrayView<const Math::RectangularMatrix<3, 2, Float>> values) {
    #ifndef MAGNUM_TARGET_WEBGL
    Context::current().state().shaderProgram.uniformMatrix3x2fvImplementation
    #else
    uniformMatrix3x2fvImplementationDefault
    #endif
        (_id, location, values.size(), GL_FALSE, values.data()->data());
}

#ifdef MAGNUM_TARGET_WEBGL
inline
#endif
void AbstractShaderProgram::uniformMatrix3x2fvImplementationDefault(const GLuint id, const GLint location, const GLsizei count, const GLboolean transpose, const GLfloat* const values) {
    use(id);
    glUniformMatrix3x2fv(location, count, transpose, values);
}

void AbstractShaderProgram::setUniform(const Int location, const Containers::ArrayView<const Math::RectangularMatrix<2, 4, Float>> values) {
    #ifndef MAGNUM_TARGET_WEBGL
    Context::current().state().shaderProgram.uniformMatrix2x4fvImplementation
    #else
    uniformMatrix2x4fvImplementationDefault
    #endif
        (_id, location, values.size(), GL_FALSE, values.data()->data());
}

#ifdef MAGNUM_TARGET_WEBGL
inline
#endif
void AbstractShaderProgram::uniformMatrix2x4fvImplementationDefault(const GLuint id, const GLint location, const GLsizei count, const GLboolean transpose, const GLfloat* const values) {
    use(id);
    glUniformMatrix2x4fv(location, count, transpose, values);
}

void AbstractShaderProgram::setUniform(const Int location, const Containers::ArrayView<const Math::RectangularMatrix<4, 2, Float>> values) {
    #ifndef MAGNUM_TARGET_WEBGL
    Context::current().state().shaderProgram.uniformMatrix4x2fvImplementation
    #else
    uniformMatrix4x2fvImplementationDefault
    #endif
        (_id, location, values.size(), GL_FALSE, values.data()->data());
}

#ifdef MAGNUM_TARGET_WEBGL
inline
#endif
void AbstractShaderProgram::uniformMatrix4x2fvImplementationDefault(const GLuint id, const GLint location, const GLsizei count, const GLboolean transpose, const GLfloat* const values) {
    use(id);
    glUniformMatrix4x2fv(location, count, transpose, values);
}

void AbstractShaderProgram::setUniform(const Int location, const Containers::ArrayView<const Math::RectangularMatrix<3, 4, Float>> values) {
    #ifndef MAGNUM_TARGET_WEBGL
    Context::current().state().shaderProgram.uniformMatrix3x4fvImplementation
    #else
    uniformMatrix3x4fvImplementationDefault
    #endif
        (_id, location, values.size(), GL_FALSE, values.data()->data());
}

#ifdef MAGNUM_TARGET_WEBGL
inline
#endif
void AbstractShaderProgram::uniformMatrix3x4fvImplementationDefault(const GLuint id, const GLint location, const GLsizei count, const GLboolean transpose, const GLfloat* const values) {
    use(id);
    glUniformMatrix3x4fv(location, count, transpose, values);
}

void AbstractShaderProgram::setUniform(const Int location, const Containers::ArrayView<const Math::RectangularMatrix<4, 3, Float>> values) {
    #ifndef MAGNUM_TARGET_WEBGL
    Context::current().state().shaderProgram.uniformMatrix4x3fvImplementation
    #else
    uniformMatrix4x3fvImplementationDefault
    #endif
        (_id, location, values.size(), GL_FALSE, values.data()->data());
}

#ifdef MAGNUM_TARGET_WEBGL
inline
#endif
void AbstractShaderProgram::uniformMatrix4x3fvImplementationDefault(const GLuint id, const GLint location, const GLsizei count, const GLboolean transpose, const GLfloat* const values) {
    use(id);
    glUniformMatrix4x3fv(location, count, transpose, values);
}
#endif

#ifndef MAGNUM_TARGET_GLES
void AbstractShaderProgram::setUniform(const Int location, const Containers::ArrayView<const Math::RectangularMatrix<2, 2, Double>> values) {
    Context::current().state().shaderProgram.uniformMatrix2dvImplementation(_id, location, values.size(), GL_FALSE, values.data()->data());
}

void AbstractShaderProgram::uniformMatrix2dvImplementationDefault(const GLuint id, const GLint location, const GLsizei count, const GLboolean transpose, const GLdouble* const values) {
    use(id);
    glUniformMatrix2dv(location, count, transpose, values);
}

void AbstractShaderProgram::setUniform(const Int location, const Containers::ArrayView<const Math::RectangularMatrix<3, 3, Double>> values) {
    Context::current().state().shaderProgram.uniformMatrix3dvImplementation(_id, location, values.size(), GL_FALSE, values.data()->data());
}

void AbstractShaderProgram::uniformMatrix3dvImplementationDefault(const GLuint id, const GLint location, const GLsizei count, const GLboolean transpose, const GLdouble* const values) {
    use(id);
    glUniformMatrix3dv(location, count, transpose, values);
}

void AbstractShaderProgram::setUniform(const Int location, const Containers::ArrayView<const Math::RectangularMatrix<4, 4, Double>> values) {
    Context::current().state().shaderProgram.uniformMatrix4dvImplementation(_id, location, values.size(), GL_FALSE, values.data()->data());
}

void AbstractShaderProgram::uniformMatrix4dvImplementationDefault(const GLuint id, const GLint location, const GLsizei count, const GLboolean transpose, const GLdouble* const values) {
    use(id);
    glUniformMatrix4dv(location, count, transpose, values);
}

void AbstractShaderProgram::setUniform(const Int location, const Containers::ArrayView<const Math::RectangularMatrix<2, 3, Double>> values) {
    Context::current().state().shaderProgram.uniformMatrix2x3dvImplementation(_id, location, values.size(), GL_FALSE, values.data()->data());
}

void AbstractShaderProgram::uniformMatrix2x3dvImplementationDefault(const GLuint id, const GLint location, const GLsizei count, const GLboolean transpose, const GLdouble* const values) {
    use(id);
    glUniformMatrix2x3dv(location, count, transpose, values);
}

void AbstractShaderProgram::setUniform(const Int location, const Containers::ArrayView<const Math::RectangularMatrix<3, 2, Double>> values) {
    Context::current().state().shaderProgram.uniformMatrix3x2dvImplementation(_id, location, values.size(), GL_FALSE, values.data()->data());
}

void AbstractShaderProgram::uniformMatrix3x2dvImplementationDefault(const GLuint id, const GLint location, const GLsizei count, const GLboolean transpose, const GLdouble* const values) {
    use(id);
    glUniformMatrix3x2dv(location, count, transpose, values);
}

void AbstractShaderProgram::setUniform(const Int location, const Containers::ArrayView<const Math::RectangularMatrix<2, 4, Double>> values) {
    Context::current().state().shaderProgram.uniformMatrix2x4dvImplementation(_id, location, values.size(), GL_FALSE, values.data()->data());
}

void AbstractShaderProgram::uniformMatrix2x4dvImplementationDefault(const GLuint id, const GLint location, const GLsizei count, const GLboolean transpose, const GLdouble* const values) {
    use(id);
    glUniformMatrix2x4dv(location, count, transpose, values);
}

void AbstractShaderProgram::setUniform(const Int location, const Containers::ArrayView<const Math::RectangularMatrix<4, 2, Double>> values) {
    Context::current().state().shaderProgram.uniformMatrix4x2dvImplementation(_id, location, values.size(), GL_FALSE, values.data()->data());
}

void AbstractShaderProgram::uniformMatrix4x2dvImplementationDefault(const GLuint id, const GLint location, const GLsizei count, const GLboolean transpose, const GLdouble* const values) {
    use(id);
    glUniformMatrix4x2dv(location, count, transpose, values);
}

void AbstractShaderProgram::setUniform(const Int location, const Containers::ArrayView<const Math::RectangularMatrix<3, 4, Double>> values) {
    Context::current().state().shaderProgram.uniformMatrix3x4dvImplementation(_id, location, values.size(), GL_FALSE, values.data()->data());
}

void AbstractShaderProgram::uniformMatrix3x4dvImplementationDefault(const GLuint id, const GLint location, const GLsizei count, const GLboolean transpose, const GLdouble* const values) {
    use(id);
    glUniformMatrix3x4dv(location, count, transpose, values);
}

void AbstractShaderProgram::setUniform(const Int location, const Containers::ArrayView<const Math::RectangularMatrix<4, 3, Double>> values) {
    Context::current().state().shaderProgram.uniformMatrix4x3dvImplementation(_id, location, values.size(), GL_FALSE, values.data()->data());
}

void AbstractShaderProgram::uniformMatrix4x3dvImplementationDefault(const GLuint id, const GLint location, const GLsizei count, const GLboolean transpose, const GLdouble* const values) {
    use(id);
    glUniformMatrix4x3dv(location, count, transpose, values);
}
#endif

}}
