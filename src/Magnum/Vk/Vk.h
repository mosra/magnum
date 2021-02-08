#ifndef Magnum_Vk_Vk_h
#define Magnum_Vk_Vk_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021 Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Forward declarations for the @ref Magnum::Vk namespace
 */

#include <Corrade/Containers/Containers.h>

#include "Magnum/Magnum.h"

namespace Magnum { namespace Vk {

#ifndef DOXYGEN_GENERATING_OUTPUT
enum class Access: UnsignedInt;
typedef Containers::EnumSet<Access> Accesses;
class Buffer;
class BufferCreateInfo;
class BufferMemoryBarrier;
class CommandBuffer;
/* CommandBufferBeginInfo is useful only in combination with CommandBuffer */
class CommandPool;
class CommandPoolCreateInfo;
class ComputePipelineCreateInfo;
/* BufferCopy used only directly inside CopyBufferInfo */
class CopyBufferInfo;
/* ImageCopy used only directly inside CopyImageInfo */
class CopyImageInfo;
/* BufferImageCopy used only directly inside CopyBufferToImageInfo /
   CopyImageToBufferInfo */
class CopyBufferToImageInfo;
class CopyImageToBufferInfo;
/* Not forward-declaring CopyBufferToImageInfo1D etc right now, I see no need */
enum class DependencyFlag: UnsignedInt;
typedef Containers::EnumSet<DependencyFlag> DependencyFlags;
class Device;
class DeviceCreateInfo;
enum class DeviceFeature: UnsignedShort;
typedef Containers::BigEnumSet<DeviceFeature, 4> DeviceFeatures;
class DeviceProperties;
enum class DeviceType: Int;
enum class DynamicRasterizationState: UnsignedByte;
typedef Containers::BigEnumSet<DynamicRasterizationState, 1> DynamicRasterizationStates;
class Extension;
class ExtensionProperties;
class Fence;
class FenceCreateInfo;
class Framebuffer;
class FramebufferCreateInfo;
enum class HandleFlag: UnsignedByte;
typedef Containers::EnumSet<HandleFlag> HandleFlags;
class Image;
enum class ImageAspect: UnsignedInt;
typedef Containers::EnumSet<ImageAspect> ImageAspects;
enum class ImageLayout: Int;
class ImageCreateInfo;
/* Not forward-declaring ImageCreateInfo1D etc right now, I see no need */
class ImageMemoryBarrier;
class ImageView;
class ImageViewCreateInfo;
/* Not forward-declaring ImageViewCreateInfo1D etc right now, I see no need */
class Instance;
class InstanceCreateInfo;
class InstanceExtension;
class InstanceExtensionProperties;
class LayerProperties;
class Memory;
class MemoryAllocateInfo;
class MemoryBarrier;
class MemoryMapDeleter;
class MemoryRequirements;
enum class MemoryFlag: UnsignedInt;
typedef Containers::EnumSet<MemoryFlag> MemoryFlags;
enum class MemoryHeapFlag: UnsignedInt;
typedef Containers::EnumSet<MemoryHeapFlag> MemoryHeapFlags;
class Mesh;
enum class MeshIndexType: Int;
class MeshLayout;
enum class MeshPrimitive: Int;
class Pipeline;
enum class PipelineBindPoint: Int;
class PipelineLayout;
class PipelineLayoutCreateInfo;
enum class PipelineStage: UnsignedInt;
typedef Containers::EnumSet<PipelineStage> PipelineStages;
enum class PixelFormat: Int;
class Queue;
enum class QueueFlag: UnsignedInt;
typedef Containers::EnumSet<QueueFlag> QueueFlags;
class RasterizationPipelineCreateInfo;
class RenderPass;
class RenderPassBeginInfo;
class RenderPassCreateInfo;
/* AttachmentDescription, AttachmentReference, SubpassDescription,
   SubpassDependency are useful only to be passed directly to
   RenderPassCreateInfo */
enum class Result: Int;
class Shader;
class ShaderCreateInfo;
class ShaderSet;
/* ShaderSpecialization used only directly with ShaderSet */
enum class ShaderStage: UnsignedInt;
class SubmitInfo;
class SubpassBeginInfo;
class SubpassEndInfo;
enum class Version: UnsignedInt;
enum class VertexFormat: Int;
#endif

}}

#endif
