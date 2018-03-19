//
// Copyright (c) 2017-2018 Advanced Micro Devices, Inc. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#ifndef AMD_VULKAN_MEMORY_ALLOCATOR_H
#define AMD_VULKAN_MEMORY_ALLOCATOR_H

#ifdef __cplusplus
extern "C" {
#endif

/** \mainpage Vulkan Memory Allocator

<b>Version 2.0.0-alpha.6</b> (2017-11-13)

Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved. \n
License: MIT

Documentation of all members: vk_mem_alloc.h

Table of contents:

- User guide
  - \subpage quick_start
  - \subpage choosing_memory_type
  - \subpage memory_mapping
  - \subpage custom_memory_pools
  - \subpage defragmentation
  - \subpage lost_allocations
  - \subpage allocation_annotation
- \subpage configuration
  - \subpage vk_khr_dedicated_allocation
- \subpage thread_safety

See also:

- [Source repository on GitHub](https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator)
- [Product page on GPUOpen](https://gpuopen.com/gaming-product/vulkan-memory-allocator/)




\page quick_start Quick start

\section project_setup Project setup

In your project code:

-# Include "vk_mem_alloc.h" file wherever you want to use the library.
-# In exacly one C++ file define following macro before include to build library
   implementation.

\code
#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"
\endcode

\section initialization Initialization

At program startup:

-# Initialize Vulkan to have `VkPhysicalDevice` and `VkDevice` object.
-# Fill VmaAllocatorCreateInfo structure and create `VmaAllocator` object by
   calling vmaCreateAllocator().

\code
VmaAllocatorCreateInfo allocatorInfo = {};
allocatorInfo.physicalDevice = physicalDevice;
allocatorInfo.device = device;

VmaAllocator allocator;
vmaCreateAllocator(&allocatorInfo, &allocator);
\endcode

\section resource_allocation Resource allocation

When you want to create a buffer or image:

-# Fill `VkBufferCreateInfo` / `VkImageCreateInfo` structure.
-# Fill VmaAllocationCreateInfo structure.
-# Call vmaCreateBuffer() / vmaCreateImage() to get `VkBuffer`/`VkImage` with memory
   already allocated and bound to it.

\code
VkBufferCreateInfo bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
bufferInfo.size = 65536;
bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

VmaAllocationCreateInfo allocInfo = {};
allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

VkBuffer buffer;
VmaAllocation allocation;
vmaCreateBuffer(allocator, &bufferInfo, &allocInfo, &buffer, &allocation, nullptr);
\endcode

Don't forget to destroy your objects when no longer needed:

\code
vmaDestroyBuffer(allocator, buffer, allocation);
vmaDestroyAllocator(allocator);
\endcode


\page choosing_memory_type Choosing memory type

Physical devices in Vulkan support various combinations of memory heaps and
types. Help with choosing correct and optimal memory type for your specific
resource is one of the key features of this library. You can use it by filling
appropriate members of VmaAllocationCreateInfo structure, as described below.
You can also combine multiple methods.

-# If you just want to find memory type index that meets your requirements, you
   can use function vmaFindMemoryTypeIndex().
-# If you want to allocate a region of device memory without association with any
   specific image or buffer, you can use function vmaAllocateMemory(). Usage of
   this function is not recommended and usually not needed.
-# If you already have a buffer or an image created, you want to allocate memory
   for it and then you will bind it yourself, you can use function
   vmaAllocateMemoryForBuffer(), vmaAllocateMemoryForImage().
-# If you want to create a buffer or an image, allocate memory for it and bind
   them together, all in one call, you can use function vmaCreateBuffer(),
   vmaCreateImage(). This is the recommended way to use this library.

When using 3. or 4., the library internally queries Vulkan for memory types
supported for that buffer or image (function `vkGetBufferMemoryRequirements()`)
and uses only one of these types.

If no memory type can be found that meets all the requirements, these functions
return `VK_ERROR_FEATURE_NOT_PRESENT`.

You can leave VmaAllocationCreateInfo structure completely filled with zeros.
It means no requirements are specified for memory type.
It is valid, although not very useful.

\section choosing_memory_type_usage Usage

The easiest way to specify memory requirements is to fill member
VmaAllocationCreateInfo::usage using one of the values of enum `VmaMemoryUsage`.
It defines high level, common usage types.

For example, if you want to create a uniform buffer that will be filled using
transfer only once or infrequently and used for rendering every frame, you can
do it using following code:

\code
VkBufferCreateInfo bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
bufferInfo.size = 65536;
bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

VmaAllocationCreateInfo allocInfo = {};
allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

VkBuffer buffer;
VmaAllocation allocation;
vmaCreateBuffer(allocator, &bufferInfo, &allocInfo, &buffer, &allocation, nullptr);
\endcode

\section choosing_memory_type_required_preferred_flags Required and preferred flags

You can specify more detailed requirements by filling members
VmaAllocationCreateInfo::requiredFlags and VmaAllocationCreateInfo::preferredFlags
with a combination of bits from enum `VkMemoryPropertyFlags`. For example,
if you want to create a buffer that will be persistently mapped on host (so it
must be `HOST_VISIBLE`) and preferably will also be `HOST_COHERENT` and `HOST_CACHED`,
use following code:

\code
VmaAllocationCreateInfo allocInfo = {};
allocInfo.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
allocInfo.preferredFlags = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
allocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

VkBuffer buffer;
VmaAllocation allocation;
vmaCreateBuffer(allocator, &bufferInfo, &allocInfo, &buffer, &allocation, nullptr);
\endcode

A memory type is chosen that has all the required flags and as many preferred
flags set as possible.

If you use VmaAllocationCreateInfo::usage, it is just internally converted to
a set of required and preferred flags.

\section choosing_memory_type_explicit_memory_types Explicit memory types

If you inspected memory types available on the physical device and you have
a preference for memory types that you want to use, you can fill member
VmaAllocationCreateInfo::memoryTypeBits. It is a bit mask, where each bit set
means that a memory type with that index is allowed to be used for the
allocation. Special value 0, just like UINT32_MAX, means there are no
restrictions to memory type index.

Please note that this member is NOT just a memory type index.
Still you can use it to choose just one, specific memory type.
For example, if you already determined that your buffer should be created in
memory type 2, use following code:

\code
uint32_t memoryTypeIndex = 2;

VmaAllocationCreateInfo allocInfo = {};
allocInfo.memoryTypeBits = 1u << memoryTypeIndex;

VkBuffer buffer;
VmaAllocation allocation;
vmaCreateBuffer(allocator, &bufferInfo, &allocInfo, &buffer, &allocation, nullptr);
\endcode

\section choosing_memory_type_custom_memory_pools Custom memory pools

If you allocate from custom memory pool, all the ways of specifying memory
requirements described above are not applicable and the aforementioned members
of VmaAllocationCreateInfo structure are ignored. Memory type is selected
explicitly when creating the pool and then used to make all the allocations from
that pool. For further details, see \ref custom_memory_pools.


\page memory_mapping Memory mapping

\section persistently_mapped_memory Persistently mapped memory

If you need to map memory on host, it may happen that two allocations are
assigned to the same `VkDeviceMemory` block, so if you map them both at the same
time, it will cause error because mapping single memory block multiple times is
illegal in Vulkan.

TODO update this...

It is safer, more convenient and more efficient to use special feature designed
for that: persistently mapped memory. Allocations made with
`VMA_ALLOCATION_CREATE_MAPPED_BIT` flag set in
VmaAllocationCreateInfo::flags are returned from device memory
blocks that stay mapped all the time, so you can just access CPU pointer to it.
VmaAllocationInfo::pMappedData pointer is already offseted to the beginning of
particular allocation. Example:

\code
VkBufferCreateInfo bufCreateInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
bufCreateInfo.size = 1024;
bufCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

VmaAllocationCreateInfo allocCreateInfo = {};
allocCreateInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
allocCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

VkBuffer buf;
VmaAllocation alloc;
VmaAllocationInfo allocInfo;
vmaCreateBuffer(allocator, &bufCreateInfo, &allocCreateInfo, &buf, &alloc, &allocInfo);

// Buffer is immediately mapped. You can access its memory.
memcpy(allocInfo.pMappedData, myData, 1024);
\endcode

Memory in Vulkan doesn't need to be unmapped before using it e.g. for transfers,
but if you are not sure whether it's `HOST_COHERENT` (here is surely is because
it's created with `VMA_MEMORY_USAGE_CPU_ONLY`), you should check it. If it's
not, you should call `vkInvalidateMappedMemoryRanges()` before reading and
`vkFlushMappedMemoryRanges()` after writing to mapped memory on CPU. Example:

\code
VkMemoryPropertyFlags memFlags;
vmaGetMemoryTypeProperties(allocator, allocInfo.memoryType, &memFlags);
if((memFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0)
{
    VkMappedMemoryRange memRange = { VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE };
    memRange.memory = allocInfo.deviceMemory;
    memRange.offset = allocInfo.offset;
    memRange.size   = allocInfo.size;
    vkFlushMappedMemoryRanges(device, 1, &memRange);
}
\endcode

\section amd_perf_note Note on performance

There is a situation that you should be careful about. It happens only if all of
following conditions are met:

-# You use AMD GPU.
-# You use the memory type that is both `DEVICE_LOCAL` and `HOST_VISIBLE`
   (used when you specify `VMA_MEMORY_USAGE_CPU_TO_GPU`).
-# Operating system is Windows 7 or 8.x (Windows 10 is not affected because it
   uses WDDM2).

Then whenever a `VkDeviceMemory` block allocated from this memory type is mapped
for the time of any call to `vkQueueSubmit()` or `vkQueuePresentKHR()`, this
block is migrated by WDDM to system RAM, which degrades performance. It doesn't
matter if that particular memory block is actually used by the command buffer
being submitted.

To avoid this problem, either make sure to unmap all allocations made from this
memory type before your Submit and Present, or use `VMA_MEMORY_USAGE_GPU_ONLY`
and transfer from a staging buffer in `VMA_MEMORY_USAGE_CPU_ONLY`, which can
safely stay mapped all the time.

\page custom_memory_pools Custom memory pools

The library automatically creates and manages default memory pool for each
memory type available on the device. A pool contains a number of
`VkDeviceMemory` blocks. You can create custom pool and allocate memory out of
it. It can be useful if you want to:

- Keep certain kind of allocations separate from others.
- Enforce particular size of Vulkan memory blocks.
- Limit maximum amount of Vulkan memory allocated for that pool.

To use custom memory pools:

-# Fill VmaPoolCreateInfo structure.
-# Call vmaCreatePool() to obtain `VmaPool` handle.
-# When making an allocation, set VmaAllocationCreateInfo::pool to this handle.
   You don't need to specify any other parameters of this structure, like usage.

Example:

\code
// Create a pool that could have at most 2 blocks, 128 MiB each.
VmaPoolCreateInfo poolCreateInfo = {};
poolCreateInfo.memoryTypeIndex = ...
poolCreateInfo.blockSize = 128ull * 1024 * 1024;
poolCreateInfo.maxBlockCount = 2;

VmaPool pool;
vmaCreatePool(allocator, &poolCreateInfo, &pool);

// Allocate a buffer out of it.
VkBufferCreateInfo bufCreateInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
bufCreateInfo.size = 1024;
bufCreateInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

VmaAllocationCreateInfo allocCreateInfo = {};
allocCreateInfo.pool = pool;

VkBuffer buf;
VmaAllocation alloc;
VmaAllocationInfo allocInfo;
vmaCreateBuffer(allocator, &bufCreateInfo, &allocCreateInfo, &buf, &alloc, &allocInfo);
\endcode

You have to free all allocations made from this pool before destroying it.

\code
vmaDestroyBuffer(allocator, buf, alloc);
vmaDestroyPool(allocator, pool);
\endcode

\page defragmentation Defragmentation

Interleaved allocations and deallocations of many objects of varying size can
cause fragmentation, which can lead to a situation where the library is unable
to find a continuous range of free memory for a new allocation despite there is
enough free space, just scattered across many small free ranges between existing
allocations.

To mitigate this problem, you can use vmaDefragment(). Given set of allocations,
this function can move them to compact used memory, ensure more continuous free
space and possibly also free some `VkDeviceMemory`. It can work only on
allocations made from memory type that is `HOST_VISIBLE`. Allocations are
modified to point to the new `VkDeviceMemory` and offset. Data in this memory is
also `memmove`-ed to the new place. However, if you have images or buffers bound
to these allocations (and you certainly do), you need to destroy, recreate, and
bind them to the new place in memory.

For further details and example code, see documentation of function
vmaDefragment().

\page lost_allocations Lost allocations

If your game oversubscribes video memory, if may work OK in previous-generation
graphics APIs (DirectX 9, 10, 11, OpenGL) because resources are automatically
paged to system RAM. In Vulkan you can't do it because when you run out of
memory, an allocation just fails. If you have more data (e.g. textures) that can
fit into VRAM and you don't need it all at once, you may want to upload them to
GPU on demand and "push out" ones that are not used for a long time to make room
for the new ones, effectively using VRAM (or a cartain memory pool) as a form of
cache. Vulkan Memory Allocator can help you with that by supporting a concept of
"lost allocations".

To create an allocation that can become lost, include `VMA_ALLOCATION_CREATE_CAN_BECOME_LOST_BIT`
flag in VmaAllocationCreateInfo::flags. Before using a buffer or image bound to
such allocation in every new frame, you need to query it if it's not lost. To
check it: call vmaGetAllocationInfo() and see if VmaAllocationInfo::deviceMemory
is not `VK_NULL_HANDLE`. If the allocation is lost, you should not use it or
buffer/image bound to it. You mustn't forget to destroy this allocation and this
buffer/image.

To create an allocation that can make some other allocations lost to make room
for it, use `VMA_ALLOCATION_CREATE_CAN_MAKE_OTHER_LOST_BIT` flag. You will
usually use both flags `VMA_ALLOCATION_CREATE_CAN_MAKE_OTHER_LOST_BIT` and
`VMA_ALLOCATION_CREATE_CAN_BECOME_LOST_BIT` at the same time.

Warning! Current implementation uses quite naive, brute force algorithm,
which can make allocation calls that use `VMA_ALLOCATION_CREATE_CAN_MAKE_OTHER_LOST_BIT`
flag quite slow. A new, more optimal algorithm and data structure to speed this
up is planned for the future.

<b>When interleaving creation of new allocations with usage of existing ones,
how do you make sure that an allocation won't become lost while it's used in the
current frame?</b>

It is ensured because vmaGetAllocationInfo() not only returns allocation
parameters and checks whether it's not lost, but when it's not, it also
atomically marks it as used in the current frame, which makes it impossible to
become lost in that frame. It uses lockless algorithm, so it works fast and
doesn't involve locking any internal mutex.

<b>What if my allocation may still be in use by the GPU when it's rendering a
previous frame while I already submit new frame on the CPU?</b>

You can make sure that allocations "touched" by vmaGetAllocationInfo() will not
become lost for a number of additional frames back from the current one by
specifying this number as VmaAllocatorCreateInfo::frameInUseCount (for default
memory pool) and VmaPoolCreateInfo::frameInUseCount (for custom pool).

<b>How do you inform the library when new frame starts?</b>

You need to call function vmaSetCurrentFrameIndex().

Example code:

\code
struct MyBuffer
{
    VkBuffer m_Buf = nullptr;
    VmaAllocation m_Alloc = nullptr;

    // Called when the buffer is really needed in the current frame.
    void EnsureBuffer();
};

void MyBuffer::EnsureBuffer()
{
    // Buffer has been created.
    if(m_Buf != VK_NULL_HANDLE)
    {
        // Check if its allocation is not lost + mark it as used in current frame.
        VmaAllocationInfo allocInfo;
        vmaGetAllocationInfo(allocator, m_Alloc, &allocInfo);
        if(allocInfo.deviceMemory != VK_NULL_HANDLE)
        {
            // It's all OK - safe to use m_Buf.
            return;
        }
    }

    // Buffer not yet exists or lost - destroy and recreate it.

    vmaDestroyBuffer(allocator, m_Buf, m_Alloc);

    VkBufferCreateInfo bufCreateInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
    bufCreateInfo.size = 1024;
    bufCreateInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

    VmaAllocationCreateInfo allocCreateInfo = {};
    allocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    allocCreateInfo.flags = VMA_ALLOCATION_CREATE_CAN_BECOME_LOST_BIT |
        VMA_ALLOCATION_CREATE_CAN_MAKE_OTHER_LOST_BIT;

    vmaCreateBuffer(allocator, &bufCreateInfo, &allocCreateInfo, &m_Buf, &m_Alloc, nullptr);
}
\endcode

When using lost allocations, you may see some Vulkan validation layer warnings
about overlapping regions of memory bound to different kinds of buffers and
images. This is still valid as long as you implement proper handling of lost
allocations (like in the example above) and don't use them.

The library uses following algorithm for allocation, in order:

-# Try to find free range of memory in existing blocks.
-# If failed, try to create a new block of `VkDeviceMemory`, with preferred block size.
-# If failed, try to create such block with size/2 and size/4.
-# If failed and `VMA_ALLOCATION_CREATE_CAN_MAKE_OTHER_LOST_BIT` flag was
   specified, try to find space in existing blocks, possilby making some other
   allocations lost.
-# If failed, try to allocate separate `VkDeviceMemory` for this allocation,
   just like when you use `VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT`.
-# If failed, choose other memory type that meets the requirements specified in
   VmaAllocationCreateInfo and go to point 1.
-# If failed, return `VK_ERROR_OUT_OF_DEVICE_MEMORY`.


\page allocation_annotation Allocation names and user data

\section allocation_user_data Allocation user data

You can annotate allocations with your own information, e.g. for debugging purposes.
To do that, fill VmaAllocationCreateInfo::pUserData field when creating
an allocation. It's an opaque `void*` pointer. You can use it e.g. as a pointer,
some handle, index, key, ordinal number or any other value that would associate
the allocation with your custom metadata.

\code
VkBufferCreateInfo bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
// Fill bufferInfo...

MyBufferMetadata* pMetadata = CreateBufferMetadata();

VmaAllocationCreateInfo allocCreateInfo = {};
allocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
allocCreateInfo.pUserData = pMetadata;

VkBuffer buffer;
VmaAllocation allocation;
vmaCreateBuffer(allocator, &bufferInfo, &allocCreateInfo, &buffer, &allocation, nullptr);
\endcode

The pointer may be later retrieved as VmaAllocationInfo::pUserData:

\code
VmaAllocationInfo allocInfo;
vmaGetAllocationInfo(allocator, allocation, &allocInfo);
MyBufferMetadata* pMetadata = (MyBufferMetadata*)allocInfo.pUserData;
\endcode

It can also be changed using function vmaSetAllocationUserData().

Values of (non-zero) allocations' `pUserData` are printed in JSON report created by
vmaBuildStatsString(), in hexadecimal form.

\section allocation_names Allocation names

There is alternative mode available where `pUserData` pointer is used to point to
a null-terminated string, giving a name to the allocation. To use this mode,
set `VMA_ALLOCATION_CREATE_USER_DATA_COPY_STRING_BIT` flag in VmaAllocationCreateInfo::flags.
Then `pUserData` passed as VmaAllocationCreateInfo::pUserData or argument to
vmaSetAllocationUserData() must be either null or pointer to a null-terminated string.
The library creates internal copy of the string, so the pointer you pass doesn't need
to be valid for whole lifetime of the allocation. You can free it after the call.

\code
VkImageCreateInfo imageInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
// Fill imageInfo...

std::string imageName = "Texture: ";
imageName += fileName;

VmaAllocationCreateInfo allocCreateInfo = {};
allocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
allocCreateInfo.flags = VMA_ALLOCATION_CREATE_USER_DATA_COPY_STRING_BIT;
allocCreateInfo.pUserData = imageName.c_str();

VkImage image;
VmaAllocation allocation;
vmaCreateImage(allocator, &imageInfo, &allocCreateInfo, &image, &allocation, nullptr);
\endcode

The value of `pUserData` pointer of the allocation will be different than the one
you passed when setting allocation's name - pointing to a buffer managed
internally that holds copy of the string.

\code
VmaAllocationInfo allocInfo;
vmaGetAllocationInfo(allocator, allocation, &allocInfo);
const char* imageName = (const char*)allocInfo.pUserData;
printf("Image name: %s\n", imageName);
\endcode

That string is also printed in JSON report created by vmaBuildStatsString().

\page configuration Configuration

Please check "CONFIGURATION SECTION" in the code to find macros that you can define
before each include of this file or change directly in this file to provide
your own implementation of basic facilities like assert, `min()` and `max()` functions,
mutex etc. C++ STL is used by default, but changing these allows you to get rid
of any STL usage if you want, as many game developers tend to do.

\section config_Vulkan_functions Pointers to Vulkan functions

The library uses Vulkan functions straight from the `vulkan.h` header by default.
If you want to provide your own pointers to these functions, e.g. fetched using
`vkGetInstanceProcAddr()` and `vkGetDeviceProcAddr()`:

-# Define `VMA_STATIC_VULKAN_FUNCTIONS 0`.
-# Provide valid pointers through VmaAllocatorCreateInfo::pVulkanFunctions.

\section custom_memory_allocator Custom host memory allocator

If you use custom allocator for CPU memory rather than default operator `new`
and `delete` from C++, you can make this library using your allocator as well
by filling optional member VmaAllocatorCreateInfo::pAllocationCallbacks. These
functions will be passed to Vulkan, as well as used by the library itself to
make any CPU-side allocations.

\section allocation_callbacks Device memory allocation callbacks

The library makes calls to `vkAllocateMemory()` and `vkFreeMemory()` internally.
You can setup callbacks to be informed about these calls, e.g. for the purpose
of gathering some statistics. To do it, fill optional member
VmaAllocatorCreateInfo::pDeviceMemoryCallbacks.

\section heap_memory_limit Device heap memory limit

If you want to test how your program behaves with limited amount of Vulkan device
memory available without switching your graphics card to one that really has
smaller VRAM, you can use a feature of this library intended for this purpose.
To do it, fill optional member VmaAllocatorCreateInfo::pHeapSizeLimit.



\page vk_khr_dedicated_allocation VK_KHR_dedicated_allocation

VK_KHR_dedicated_allocation is a Vulkan extension which can be used to improve
performance on some GPUs. It augments Vulkan API with possibility to query
driver whether it prefers particular buffer or image to have its own, dedicated
allocation (separate `VkDeviceMemory` block) for better efficiency - to be able
to do some internal optimizations.

The extension is supported by this library. It will be used automatically when
enabled. To enable it:

1 . When creating Vulkan device, check if following 2 device extensions are
supported (call `vkEnumerateDeviceExtensionProperties()`).
If yes, enable them (fill `VkDeviceCreateInfo::ppEnabledExtensionNames`).

- VK_KHR_get_memory_requirements2
- VK_KHR_dedicated_allocation

If you enabled these extensions:

2 . Use `VMA_ALLOCATOR_CREATE_KHR_DEDICATED_ALLOCATION_BIT` flag when creating
your `VmaAllocator` to inform the library that you enabled required extensions
and you want the library to use them.

\code
allocatorInfo.flags |= VMA_ALLOCATOR_CREATE_KHR_DEDICATED_ALLOCATION_BIT;

vmaCreateAllocator(&allocatorInfo, &allocator);
\endcode

That's all. The extension will be automatically used whenever you create a
buffer using vmaCreateBuffer() or image using vmaCreateImage().

When using the extension together with Vulkan Validation Layer, you will receive
warnings like this:

    vkBindBufferMemory(): Binding memory to buffer 0x33 but vkGetBufferMemoryRequirements() has not been called on that buffer.

It is OK, you should just ignore it. It happens because you use function
`vkGetBufferMemoryRequirements2KHR()` instead of standard
`vkGetBufferMemoryRequirements()`, while the validation layer seems to be
unaware of it.

To learn more about this extension, see:

- [VK_KHR_dedicated_allocation in Vulkan specification](https://www.khronos.org/registry/vulkan/specs/1.0-extensions/html/vkspec.html#VK_KHR_dedicated_allocation)
- [VK_KHR_dedicated_allocation unofficial manual](http://asawicki.info/articles/VK_KHR_dedicated_allocation.php5)



\page thread_safety Thread safety

- The library has no global state, so separate `VmaAllocator` objects can be used
  independently. 
- By default, all calls to functions that take `VmaAllocator` as first parameter
  are safe to call from multiple threads simultaneously because they are
  synchronized internally when needed.
- When the allocator is created with `VMA_ALLOCATOR_CREATE_EXTERNALLY_SYNCHRONIZED_BIT`
  flag, calls to functions that take such `VmaAllocator` object must be
  synchronized externally.
- Access to a `VmaAllocation` object must be externally synchronized. For example,
  you must not call vmaGetAllocationInfo() and vmaMapMemory() from different
  threads at the same time if you pass the same `VmaAllocation` object to these
  functions.

*/

#include <vulkan/vulkan.h>

VK_DEFINE_HANDLE(VmaAllocator)

/// Callback function called after successful vkAllocateMemory.
typedef void (VKAPI_PTR *PFN_vmaAllocateDeviceMemoryFunction)(
    VmaAllocator      allocator,
    uint32_t          memoryType,
    VkDeviceMemory    memory,
    VkDeviceSize      size);
/// Callback function called before vkFreeMemory.
typedef void (VKAPI_PTR *PFN_vmaFreeDeviceMemoryFunction)(
    VmaAllocator      allocator,
    uint32_t          memoryType,
    VkDeviceMemory    memory,
    VkDeviceSize      size);

/** \brief Set of callbacks that the library will call for `vkAllocateMemory` and `vkFreeMemory`.

Provided for informative purpose, e.g. to gather statistics about number of
allocations or total amount of memory allocated in Vulkan.

Used in VmaAllocatorCreateInfo::pDeviceMemoryCallbacks.
*/
typedef struct VmaDeviceMemoryCallbacks {
    /// Optional, can be null.
    PFN_vmaAllocateDeviceMemoryFunction pfnAllocate;
    /// Optional, can be null.
    PFN_vmaFreeDeviceMemoryFunction pfnFree;
} VmaDeviceMemoryCallbacks;

/// Flags for created VmaAllocator.
typedef enum VmaAllocatorCreateFlagBits {
    /** \brief Allocator and all objects created from it will not be synchronized internally, so you must guarantee they are used from only one thread at a time or synchronized externally by you.

    Using this flag may increase performance because internal mutexes are not used.
    */
    VMA_ALLOCATOR_CREATE_EXTERNALLY_SYNCHRONIZED_BIT = 0x00000001,
    /** \brief Enables usage of VK_KHR_dedicated_allocation extension.

    Using this extenion will automatically allocate dedicated blocks of memory for
    some buffers and images instead of suballocating place for them out of bigger
    memory blocks (as if you explicitly used VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT
    flag) when it is recommended by the driver. It may improve performance on some
    GPUs.

    You may set this flag only if you found out that following device extensions are
    supported, you enabled them while creating Vulkan device passed as
    VmaAllocatorCreateInfo::device, and you want them to be used internally by this
    library:

    - VK_KHR_get_memory_requirements2
    - VK_KHR_dedicated_allocation

When this flag is set, you can experience following warnings reported by Vulkan
validation layer. You can ignore them.

> vkBindBufferMemory(): Binding memory to buffer 0x2d but vkGetBufferMemoryRequirements() has not been called on that buffer.
    */
    VMA_ALLOCATOR_CREATE_KHR_DEDICATED_ALLOCATION_BIT = 0x00000002,

    VMA_ALLOCATOR_CREATE_FLAG_BITS_MAX_ENUM = 0x7FFFFFFF
} VmaAllocatorCreateFlagBits;
typedef VkFlags VmaAllocatorCreateFlags;

/** \brief Pointers to some Vulkan functions - a subset used by the library.

Used in VmaAllocatorCreateInfo::pVulkanFunctions.
*/
typedef struct VmaVulkanFunctions {
    PFN_vkGetPhysicalDeviceProperties vkGetPhysicalDeviceProperties;
    PFN_vkGetPhysicalDeviceMemoryProperties vkGetPhysicalDeviceMemoryProperties;
    PFN_vkAllocateMemory vkAllocateMemory;
    PFN_vkFreeMemory vkFreeMemory;
    PFN_vkMapMemory vkMapMemory;
    PFN_vkUnmapMemory vkUnmapMemory;
    PFN_vkBindBufferMemory vkBindBufferMemory;
    PFN_vkBindImageMemory vkBindImageMemory;
    PFN_vkGetBufferMemoryRequirements vkGetBufferMemoryRequirements;
    PFN_vkGetImageMemoryRequirements vkGetImageMemoryRequirements;
    PFN_vkCreateBuffer vkCreateBuffer;
    PFN_vkDestroyBuffer vkDestroyBuffer;
    PFN_vkCreateImage vkCreateImage;
    PFN_vkDestroyImage vkDestroyImage;
    PFN_vkGetBufferMemoryRequirements2KHR vkGetBufferMemoryRequirements2KHR;
    PFN_vkGetImageMemoryRequirements2KHR vkGetImageMemoryRequirements2KHR;
} VmaVulkanFunctions;

/// Description of a Allocator to be created.
typedef struct VmaAllocatorCreateInfo
{
    /// Flags for created allocator. Use VmaAllocatorCreateFlagBits enum.
    VmaAllocatorCreateFlags flags;
    /// Vulkan physical device.
    /** It must be valid throughout whole lifetime of created allocator. */
    VkPhysicalDevice physicalDevice;
    /// Vulkan device.
    /** It must be valid throughout whole lifetime of created allocator. */
    VkDevice device;
    /// Preferred size of a single `VkDeviceMemory` block to be allocated from large heaps.
    /** Set to 0 to use default, which is currently 256 MiB. */
    VkDeviceSize preferredLargeHeapBlockSize;
    /// Preferred size of a single `VkDeviceMemory` block to be allocated from small heaps <= 512 MiB.
    /** Set to 0 to use default, which is currently 64 MiB. */
    VkDeviceSize preferredSmallHeapBlockSize;
    /// Custom CPU memory allocation callbacks.
    /** Optional, can be null. When specified, will also be used for all CPU-side memory allocations. */
    const VkAllocationCallbacks* pAllocationCallbacks;
    /// Informative callbacks for vkAllocateMemory, vkFreeMemory.
    /** Optional, can be null. */
    const VmaDeviceMemoryCallbacks* pDeviceMemoryCallbacks;
    /** \brief Maximum number of additional frames that are in use at the same time as current frame.

    This value is used only when you make allocations with
    VMA_ALLOCATION_CREATE_CAN_BECOME_LOST_BIT flag. Such allocation cannot become
    lost if allocation.lastUseFrameIndex >= allocator.currentFrameIndex - frameInUseCount.

    For example, if you double-buffer your command buffers, so resources used for
    rendering in previous frame may still be in use by the GPU at the moment you
    allocate resources needed for the current frame, set this value to 1.

    If you want to allow any allocations other than used in the current frame to
    become lost, set this value to 0.
    */
    uint32_t frameInUseCount;
    /** \brief Either NULL or a pointer to an array of limits on maximum number of bytes that can be allocated out of particular Vulkan memory heap.

    If not NULL, it must be a pointer to an array of
    `VkPhysicalDeviceMemoryProperties::memoryHeapCount` elements, defining limit on
    maximum number of bytes that can be allocated out of particular Vulkan memory
    heap.

    Any of the elements may be equal to `VK_WHOLE_SIZE`, which means no limit on that
    heap. This is also the default in case of `pHeapSizeLimit` = NULL.

    If there is a limit defined for a heap:

    - If user tries to allocate more memory from that heap using this allocator,
      the allocation fails with `VK_ERROR_OUT_OF_DEVICE_MEMORY`.
    - If the limit is smaller than heap size reported in `VkMemoryHeap::size`, the
      value of this limit will be reported instead when using vmaGetMemoryProperties().

    Warning! Using this feature may not be equivalent to installing a GPU with
    smaller amount of memory, because graphics driver doesn't necessary fail new
    allocations with `VK_ERROR_OUT_OF_DEVICE_MEMORY` result when memory capacity is
    exceeded. It may return success and just silently migrate some device memory
    blocks to system RAM.
    */
    const VkDeviceSize* pHeapSizeLimit;
    /** \brief Pointers to Vulkan functions. Can be null if you leave define `VMA_STATIC_VULKAN_FUNCTIONS 1`.

    If you leave define `VMA_STATIC_VULKAN_FUNCTIONS 1` in configuration section,
    you can pass null as this member, because the library will fetch pointers to
    Vulkan functions internally in a static way, like:

        vulkanFunctions.vkAllocateMemory = &vkAllocateMemory;

    Fill this member if you want to provide your own pointers to Vulkan functions,
    e.g. fetched using `vkGetInstanceProcAddr()` and `vkGetDeviceProcAddr()`.
    */
    const VmaVulkanFunctions* pVulkanFunctions;
} VmaAllocatorCreateInfo;

/// Creates Allocator object.
VkResult vmaCreateAllocator(
    const VmaAllocatorCreateInfo* pCreateInfo,
    VmaAllocator* pAllocator);

/// Destroys allocator object.
void vmaDestroyAllocator(
    VmaAllocator allocator);

/**
PhysicalDeviceProperties are fetched from physicalDevice by the allocator.
You can access it here, without fetching it again on your own.
*/
void vmaGetPhysicalDeviceProperties(
    VmaAllocator allocator,
    const VkPhysicalDeviceProperties** ppPhysicalDeviceProperties);

/**
PhysicalDeviceMemoryProperties are fetched from physicalDevice by the allocator.
You can access it here, without fetching it again on your own.
*/
void vmaGetMemoryProperties(
    VmaAllocator allocator,
    const VkPhysicalDeviceMemoryProperties** ppPhysicalDeviceMemoryProperties);

/**
\brief Given Memory Type Index, returns Property Flags of this memory type.

This is just a convenience function. Same information can be obtained using
vmaGetMemoryProperties().
*/
void vmaGetMemoryTypeProperties(
    VmaAllocator allocator,
    uint32_t memoryTypeIndex,
    VkMemoryPropertyFlags* pFlags);

/** \brief Sets index of the current frame.

This function must be used if you make allocations with
`VMA_ALLOCATION_CREATE_CAN_BECOME_LOST_BIT` and
`VMA_ALLOCATION_CREATE_CAN_MAKE_OTHER_LOST_BIT` flags to inform the allocator
when a new frame begins. Allocations queried using vmaGetAllocationInfo() cannot
become lost in the current frame.
*/
void vmaSetCurrentFrameIndex(
    VmaAllocator allocator,
    uint32_t frameIndex);

/** \brief Calculated statistics of memory usage in entire allocator.
*/
typedef struct VmaStatInfo
{
    /// Number of `VkDeviceMemory` Vulkan memory blocks allocated.
    uint32_t blockCount;
    /// Number of `VmaAllocation` allocation objects allocated.
    uint32_t allocationCount;
    /// Number of free ranges of memory between allocations.
    uint32_t unusedRangeCount;
    /// Total number of bytes occupied by all allocations.
    VkDeviceSize usedBytes;
    /// Total number of bytes occupied by unused ranges.
    VkDeviceSize unusedBytes;
    VkDeviceSize allocationSizeMin, allocationSizeAvg, allocationSizeMax;
    VkDeviceSize unusedRangeSizeMin, unusedRangeSizeAvg, unusedRangeSizeMax;
} VmaStatInfo;

/// General statistics from current state of Allocator.
typedef struct VmaStats
{
    VmaStatInfo memoryType[VK_MAX_MEMORY_TYPES];
    VmaStatInfo memoryHeap[VK_MAX_MEMORY_HEAPS];
    VmaStatInfo total;
} VmaStats;

/// Retrieves statistics from current state of the Allocator.
void vmaCalculateStats(
    VmaAllocator allocator,
    VmaStats* pStats);

#define VMA_STATS_STRING_ENABLED 1

#if VMA_STATS_STRING_ENABLED

/// Builds and returns statistics as string in JSON format.
/** @param[out] ppStatsString Must be freed using vmaFreeStatsString() function.
*/
void vmaBuildStatsString(
    VmaAllocator allocator,
    char** ppStatsString,
    VkBool32 detailedMap);

void vmaFreeStatsString(
    VmaAllocator allocator,
    char* pStatsString);

#endif // #if VMA_STATS_STRING_ENABLED

VK_DEFINE_HANDLE(VmaPool)

typedef enum VmaMemoryUsage
{
    /** No intended memory usage specified.
    Use other members of VmaAllocationCreateInfo to specify your requirements.
    */
    VMA_MEMORY_USAGE_UNKNOWN = 0,
    /** Memory will be used on device only, so faster access from the device is preferred.
    It usually means device-local GPU memory.
    No need to be mappable on host.
    Good e.g. for images to be used as attachments, images containing textures to be sampled,
    buffers used as vertex buffer, index buffer, uniform buffer and majority of
    other types of resources used by device.
    You can still do transfers from/to such resource to/from host memory.

    The allocation may still end up in `HOST_VISIBLE` memory on some implementations.
    In such case, you are free to map it.
    You can also use `VMA_ALLOCATION_CREATE_MAPPED_BIT` with this usage type.
    */
    VMA_MEMORY_USAGE_GPU_ONLY = 1,
    /** Memory will be mapped and used on host.
    It usually means CPU system memory.
    Could be used for transfer to/from device.
    Good e.g. for "staging" copy of buffers and images, used as transfer source or destination.
    Resources created in this pool may still be accessible to the device, but access to them can be slower.
    
    Guarantees to be `HOST_VISIBLE` and `HOST_COHERENT`.
    */
    VMA_MEMORY_USAGE_CPU_ONLY = 2,
    /** Memory will be used for frequent (dynamic) updates from host and reads on device (upload).
    Good e.g. for vertex buffers or uniform buffers updated every frame.

    Guarantees to be `HOST_VISIBLE`.
    */
    VMA_MEMORY_USAGE_CPU_TO_GPU = 3,
    /** Memory will be used for frequent writing on device and readback on host (download).

    Guarantees to be `HOST_VISIBLE`.
    */
    VMA_MEMORY_USAGE_GPU_TO_CPU = 4,
    VMA_MEMORY_USAGE_MAX_ENUM = 0x7FFFFFFF
} VmaMemoryUsage;

/// Flags to be passed as VmaAllocationCreateInfo::flags.
typedef enum VmaAllocationCreateFlagBits {
    /** \brief Set this flag if the allocation should have its own memory block.
    
    Use it for special, big resources, like fullscreen images used as attachments.
   
    This flag must also be used for host visible resources that you want to map
    simultaneously because otherwise they might end up as regions of the same
    `VkDeviceMemory`, while mapping same `VkDeviceMemory` multiple times
    simultaneously is illegal.

    You should not use this flag if VmaAllocationCreateInfo::pool is not null.
    */
    VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT = 0x00000001,

    /** \brief Set this flag to only try to allocate from existing `VkDeviceMemory` blocks and never create new such block.
    
    If new allocation cannot be placed in any of the existing blocks, allocation
    fails with `VK_ERROR_OUT_OF_DEVICE_MEMORY` error.
    
    You should not use `VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT` and
    `VMA_ALLOCATION_CREATE_NEVER_ALLOCATE_BIT` at the same time. It makes no sense.
    
    If VmaAllocationCreateInfo::pool is not null, this flag is implied and ignored. */
    VMA_ALLOCATION_CREATE_NEVER_ALLOCATE_BIT = 0x00000002,
    /** \brief Set this flag to use a memory that will be persistently mapped and retrieve pointer to it.
    
    Pointer to mapped memory will be returned through VmaAllocationInfo::pMappedData.

    Is it valid to use this flag for allocation made from memory type that is not
    `HOST_VISIBLE`. This flag is then ignored and memory is not mapped. This is
    useful if you need an allocation that is efficient to use on GPU
    (`DEVICE_LOCAL`) and still want to map it directly if possible on platforms that
    support it (e.g. Intel GPU).

    You should not use this flag together with `VMA_ALLOCATION_CREATE_CAN_BECOME_LOST_BIT`.
    */
    VMA_ALLOCATION_CREATE_MAPPED_BIT = 0x00000004,
    /** Allocation created with this flag can become lost as a result of another
    allocation with `VMA_ALLOCATION_CREATE_CAN_MAKE_OTHER_LOST_BIT` flag, so you
    must check it before use.

    To check if allocation is not lost, call vmaGetAllocationInfo() and check if
    VmaAllocationInfo::deviceMemory is not `VK_NULL_HANDLE`.

    For details about supporting lost allocations, see Lost Allocations
    chapter of User Guide on Main Page.

    You should not use this flag together with `VMA_ALLOCATION_CREATE_MAPPED_BIT`.
    */
    VMA_ALLOCATION_CREATE_CAN_BECOME_LOST_BIT = 0x00000008,
    /** While creating allocation using this flag, other allocations that were
    created with flag `VMA_ALLOCATION_CREATE_CAN_BECOME_LOST_BIT` can become lost.

    For details about supporting lost allocations, see Lost Allocations
    chapter of User Guide on Main Page.
    */
    VMA_ALLOCATION_CREATE_CAN_MAKE_OTHER_LOST_BIT = 0x00000010,
    /** Set this flag to treat VmaAllocationCreateInfo::pUserData as pointer to a
    null-terminated string. Instead of copying pointer value, a local copy of the
    string is made and stored in allocation's pUserData. The string is automatically
    freed together with the allocation. It is also used in vmaBuildStatsString().
    */
    VMA_ALLOCATION_CREATE_USER_DATA_COPY_STRING_BIT = 0x00000020,

    VMA_ALLOCATION_CREATE_FLAG_BITS_MAX_ENUM = 0x7FFFFFFF
} VmaAllocationCreateFlagBits;
typedef VkFlags VmaAllocationCreateFlags;

typedef struct VmaAllocationCreateInfo
{
    /// Use VmaAllocationCreateFlagBits enum.
    VmaAllocationCreateFlags flags;
    /** \brief Intended usage of memory.
    
    You can leave `VMA_MEMORY_USAGE_UNKNOWN` if you specify memory requirements in other way. \n
    If `pool` is not null, this member is ignored.
    */
    VmaMemoryUsage usage;
    /** \brief Flags that must be set in a Memory Type chosen for an allocation.
    
    Leave 0 if you specify memory requirements in other way. \n
    If `pool` is not null, this member is ignored.*/
    VkMemoryPropertyFlags requiredFlags;
    /** \brief Flags that preferably should be set in a memory type chosen for an allocation.
    
    Set to 0 if no additional flags are prefered. \n
    If `pool` is not null, this member is ignored. */
    VkMemoryPropertyFlags preferredFlags;
    /** \brief Bitmask containing one bit set for every memory type acceptable for this allocation.

    Value 0 is equivalent to `UINT32_MAX` - it means any memory type is accepted if
    it meets other requirements specified by this structure, with no further
    restrictions on memory type index. \n
    If `pool` is not null, this member is ignored.
    */
    uint32_t memoryTypeBits;
    /** \brief Pool that this allocation should be created in.

    Leave `VK_NULL_HANDLE` to allocate from default pool. If not null, members:
    `usage`, `requiredFlags`, `preferredFlags`, `memoryTypeBits` are ignored.
    */
    VmaPool pool;
    /** \brief Custom general-purpose pointer that will be stored in VmaAllocation, can be read as VmaAllocationInfo::pUserData and changed using vmaSetAllocationUserData().
    
    If `VMA_ALLOCATION_CREATE_USER_DATA_COPY_STRING_BIT` is used, it must be either
    null or pointer to a null-terminated string. The string will be then copied to
    internal buffer, so it doesn't need to be valid after allocation call.
    */
    void* pUserData;
} VmaAllocationCreateInfo;

/**
This algorithm tries to find a memory type that:

- Is allowed by memoryTypeBits.
- Contains all the flags from pAllocationCreateInfo->requiredFlags.
- Matches intended usage.
- Has as many flags from pAllocationCreateInfo->preferredFlags as possible.

\return Returns VK_ERROR_FEATURE_NOT_PRESENT if not found. Receiving such result
from this function or any other allocating function probably means that your
device doesn't support any memory type with requested features for the specific
type of resource you want to use it for. Please check parameters of your
resource, like image layout (OPTIMAL versus LINEAR) or mip level count.
*/
VkResult vmaFindMemoryTypeIndex(
    VmaAllocator allocator,
    uint32_t memoryTypeBits,
    const VmaAllocationCreateInfo* pAllocationCreateInfo,
    uint32_t* pMemoryTypeIndex);

/// Flags to be passed as VmaPoolCreateInfo::flags.
typedef enum VmaPoolCreateFlagBits {
    /** \brief Use this flag if you always allocate only buffers and linear images or only optimal images out of this pool and so Buffer-Image Granularity can be ignored.

    This is na optional optimization flag.

    If you always allocate using vmaCreateBuffer(), vmaCreateImage(),
    vmaAllocateMemoryForBuffer(), then you don't need to use it because allocator
    knows exact type of your allocations so it can handle Buffer-Image Granularity
    in the optimal way.

    If you also allocate using vmaAllocateMemoryForImage() or vmaAllocateMemory(),
    exact type of such allocations is not known, so allocator must be conservative
    in handling Buffer-Image Granularity, which can lead to suboptimal allocation
    (wasted memory). In that case, if you can make sure you always allocate only
    buffers and linear images or only optimal images out of this pool, use this flag
    to make allocator disregard Buffer-Image Granularity and so make allocations
    more optimal.
    */
    VMA_POOL_CREATE_IGNORE_BUFFER_IMAGE_GRANULARITY_BIT = 0x00000002,

    VMA_POOL_CREATE_FLAG_BITS_MAX_ENUM = 0x7FFFFFFF
} VmaPoolCreateFlagBits;
typedef VkFlags VmaPoolCreateFlags;

/** \brief Describes parameter of created `VmaPool`.
*/
typedef struct VmaPoolCreateInfo {
    /** \brief Vulkan memory type index to allocate this pool from.
    */
    uint32_t memoryTypeIndex;
    /** \brief Use combination of `VmaPoolCreateFlagBits`.
    */
    VmaPoolCreateFlags flags;
    /** \brief Size of a single `VkDeviceMemory` block to be allocated as part of this pool, in bytes.

    Optional. Leave 0 to use default.
    */
    VkDeviceSize blockSize;
    /** \brief Minimum number of blocks to be always allocated in this pool, even if they stay empty.

    Set to 0 to have no preallocated blocks and let the pool be completely empty.
    */
    size_t minBlockCount;
    /** \brief Maximum number of blocks that can be allocated in this pool.

    Optional. Set to 0 to use `SIZE_MAX`, which means no limit.
    
    Set to same value as minBlockCount to have fixed amount of memory allocated
    throuout whole lifetime of this pool.
    */
    size_t maxBlockCount;
    /** \brief Maximum number of additional frames that are in use at the same time as current frame.

    This value is used only when you make allocations with
    `VMA_ALLOCATION_CREATE_CAN_BECOME_LOST_BIT` flag. Such allocation cannot become
    lost if allocation.lastUseFrameIndex >= allocator.currentFrameIndex - frameInUseCount.

    For example, if you double-buffer your command buffers, so resources used for
    rendering in previous frame may still be in use by the GPU at the moment you
    allocate resources needed for the current frame, set this value to 1.

    If you want to allow any allocations other than used in the current frame to
    become lost, set this value to 0.
    */
    uint32_t frameInUseCount;
} VmaPoolCreateInfo;

/** \brief Describes parameter of existing `VmaPool`.
*/
typedef struct VmaPoolStats {
    /** \brief Total amount of `VkDeviceMemory` allocated from Vulkan for this pool, in bytes.
    */
    VkDeviceSize size;
    /** \brief Total number of bytes in the pool not used by any `VmaAllocation`.
    */
    VkDeviceSize unusedSize;
    /** \brief Number of VmaAllocation objects created from this pool that were not destroyed or lost.
    */
    size_t allocationCount;
    /** \brief Number of continuous memory ranges in the pool not used by any `VmaAllocation`.
    */
    size_t unusedRangeCount;
    /** \brief Size of the largest continuous free memory region.

    Making a new allocation of that size is not guaranteed to succeed because of
    possible additional margin required to respect alignment and buffer/image
    granularity.
    */
    VkDeviceSize unusedRangeSizeMax;
} VmaPoolStats;

/** \brief Allocates Vulkan device memory and creates `VmaPool` object.

@param allocator Allocator object.
@param pCreateInfo Parameters of pool to create.
@param[out] pPool Handle to created pool.
*/
VkResult vmaCreatePool(
	VmaAllocator allocator,
	const VmaPoolCreateInfo* pCreateInfo,
	VmaPool* pPool);

/** \brief Destroys VmaPool object and frees Vulkan device memory.
*/
void vmaDestroyPool(
    VmaAllocator allocator,
    VmaPool pool);

/** \brief Retrieves statistics of existing VmaPool object.

@param allocator Allocator object.
@param pool Pool object.
@param[out] pPoolStats Statistics of specified pool.
*/
void vmaGetPoolStats(
    VmaAllocator allocator,
    VmaPool pool,
    VmaPoolStats* pPoolStats);

/** \brief Marks all allocations in given pool as lost if they are not used in current frame or VmaPoolCreateInfo::frameInUseCount back from now.

@param allocator Allocator object.
@param pool Pool.
@param[out] pLostAllocationCount Number of allocations marked as lost. Optional - pass null if you don't need this information.
*/
void vmaMakePoolAllocationsLost(
    VmaAllocator allocator,
    VmaPool pool,
    size_t* pLostAllocationCount);

VK_DEFINE_HANDLE(VmaAllocation)

/** \brief Parameters of `VmaAllocation` objects, that can be retrieved using function vmaGetAllocationInfo().
*/
typedef struct VmaAllocationInfo {
    /** \brief Memory type index that this allocation was allocated from.
    
    It never changes.
    */
    uint32_t memoryType;
    /** \brief Handle to Vulkan memory object.

    Same memory object can be shared by multiple allocations.
    
    It can change after call to vmaDefragment() if this allocation is passed to the function, or if allocation is lost.

    If the allocation is lost, it is equal to `VK_NULL_HANDLE`.
    */
    VkDeviceMemory deviceMemory;
    /** \brief Offset into deviceMemory object to the beginning of this allocation, in bytes. (deviceMemory, offset) pair is unique to this allocation.

    It can change after call to vmaDefragment() if this allocation is passed to the function, or if allocation is lost.
    */
    VkDeviceSize offset;
    /** \brief Size of this allocation, in bytes.

    It never changes, unless allocation is lost.
    */
    VkDeviceSize size;
    /** \brief Pointer to the beginning of this allocation as mapped data.

    If the allocation hasn't been mapped using vmaMapMemory() and hasn't been
    created with `VMA_ALLOCATION_CREATE_MAPPED_BIT` flag, this value null.

    It can change after call to vmaMapMemory(), vmaUnmapMemory().
    It can also change after call to vmaDefragment() if this allocation is passed to the function.
    */
    void* pMappedData;
    /** \brief Custom general-purpose pointer that was passed as VmaAllocationCreateInfo::pUserData or set using vmaSetAllocationUserData().

    It can change after call to vmaSetAllocationUserData() for this allocation.
    */
    void* pUserData;
} VmaAllocationInfo;

/** \brief General purpose memory allocation.

@param[out] pAllocation Handle to allocated memory.
@param[out] pAllocationInfo Optional. Information about allocated memory. It can be later fetched using function vmaGetAllocationInfo().

You should free the memory using vmaFreeMemory().

It is recommended to use vmaAllocateMemoryForBuffer(), vmaAllocateMemoryForImage(),
vmaCreateBuffer(), vmaCreateImage() instead whenever possible.
*/
VkResult vmaAllocateMemory(
    VmaAllocator allocator,
    const VkMemoryRequirements* pVkMemoryRequirements,
    const VmaAllocationCreateInfo* pCreateInfo,
    VmaAllocation* pAllocation,
    VmaAllocationInfo* pAllocationInfo);

/**
@param[out] pAllocation Handle to allocated memory.
@param[out] pAllocationInfo Optional. Information about allocated memory. It can be later fetched using function vmaGetAllocationInfo().

You should free the memory using vmaFreeMemory().
*/
VkResult vmaAllocateMemoryForBuffer(
    VmaAllocator allocator,
    VkBuffer buffer,
    const VmaAllocationCreateInfo* pCreateInfo,
    VmaAllocation* pAllocation,
    VmaAllocationInfo* pAllocationInfo);

/// Function similar to vmaAllocateMemoryForBuffer().
VkResult vmaAllocateMemoryForImage(
    VmaAllocator allocator,
    VkImage image,
    const VmaAllocationCreateInfo* pCreateInfo,
    VmaAllocation* pAllocation,
    VmaAllocationInfo* pAllocationInfo);

/// Frees memory previously allocated using vmaAllocateMemory(), vmaAllocateMemoryForBuffer(), or vmaAllocateMemoryForImage().
void vmaFreeMemory(
    VmaAllocator allocator,
    VmaAllocation allocation);

/// Returns current information about specified allocation.
void vmaGetAllocationInfo(
    VmaAllocator allocator,
    VmaAllocation allocation,
    VmaAllocationInfo* pAllocationInfo);

/** \brief Sets pUserData in given allocation to new value.

If the allocation was created with VMA_ALLOCATION_CREATE_USER_DATA_COPY_STRING_BIT,
pUserData must be either null, or pointer to a null-terminated string. The function
makes local copy of the string and sets it as allocation's pUserData. String
passed as pUserData doesn't need to be valid for whole lifetime of the allocation -
you can free it after this call. String previously pointed by allocation's
pUserData is freed from memory.

If the flag was not used, the value of pointer pUserData is just copied to
allocation's pUserData. It is opaque, so you can use it however you want - e.g.
as a pointer, ordinal number or some handle to you own data.
*/
void vmaSetAllocationUserData(
    VmaAllocator allocator,
    VmaAllocation allocation,
    void* pUserData);

/** \brief Creates new allocation that is in lost state from the beginning.

It can be useful if you need a dummy, non-null allocation.

You still need to destroy created object using vmaFreeMemory().

Returned allocation is not tied to any specific memory pool or memory type and
not bound to any image or buffer. It has size = 0. It cannot be turned into
a real, non-empty allocation.
*/
void vmaCreateLostAllocation(
    VmaAllocator allocator,
    VmaAllocation* pAllocation);

/** \brief Maps memory represented by given allocation and returns pointer to it.

Maps memory represented by given allocation to make it accessible to CPU code.
When succeeded, `*ppData` contains pointer to first byte of this memory.
If the allocation is part of bigger `VkDeviceMemory` block, the pointer is
correctly offseted to the beginning of region assigned to this particular
allocation.

Mapping is internally reference-counted and synchronized, so despite raw Vulkan
function `vkMapMemory()` cannot be used to map same block of `VkDeviceMemory`
multiple times simultaneously, it is safe to call this function on allocations
assigned to the same memory block. Actual Vulkan memory will be mapped on first
mapping and unmapped on last unmapping.

If the function succeeded, you must call vmaUnmapMemory() to unmap the
allocation when mapping is no longer needed or before freeing the allocation, at
the latest.

It also safe to call this function multiple times on the same allocation. You
must call vmaUnmapMemory() same number of times as you called vmaMapMemory().

It is also safe to call this function on allocation created with
`VMA_ALLOCATION_CREATE_MAPPED_BIT` flag. Its memory stays mapped all the time.
You must still call vmaUnmapMemory() same number of times as you called
vmaMapMemory(). You must not call vmaUnmapMemory() additional time to free the
"0-th" mapping made automatically due to `VMA_ALLOCATION_CREATE_MAPPED_BIT` flag.

This function fails when used on allocation made in memory type that is not
`HOST_VISIBLE`.

This function always fails when called for allocation that was created with
`VMA_ALLOCATION_CREATE_CAN_BECOME_LOST_BIT` flag. Such allocations cannot be
mapped.
*/
VkResult vmaMapMemory(
    VmaAllocator allocator,
    VmaAllocation allocation,
    void** ppData);

/** \brief Unmaps memory represented by given allocation, mapped previously using vmaMapMemory().

For details, see description of vmaMapMemory().
*/
void vmaUnmapMemory(
    VmaAllocator allocator,
    VmaAllocation allocation);

/** \brief Optional configuration parameters to be passed to function vmaDefragment(). */
typedef struct VmaDefragmentationInfo {
    /** \brief Maximum total numbers of bytes that can be copied while moving allocations to different places.
    
    Default is `VK_WHOLE_SIZE`, which means no limit.
    */
    VkDeviceSize maxBytesToMove;
    /** \brief Maximum number of allocations that can be moved to different place.

    Default is `UINT32_MAX`, which means no limit.
    */
    uint32_t maxAllocationsToMove;
} VmaDefragmentationInfo;

/** \brief Statistics returned by function vmaDefragment(). */
typedef struct VmaDefragmentationStats {
    /// Total number of bytes that have been copied while moving allocations to different places.
    VkDeviceSize bytesMoved;
    /// Total number of bytes that have been released to the system by freeing empty `VkDeviceMemory` objects.
    VkDeviceSize bytesFreed;
    /// Number of allocations that have been moved to different places.
    uint32_t allocationsMoved;
    /// Number of empty `VkDeviceMemory` objects that have been released to the system.
    uint32_t deviceMemoryBlocksFreed;
} VmaDefragmentationStats;

/** \brief Compacts memory by moving allocations.

@param pAllocations Array of allocations that can be moved during this compation.
@param allocationCount Number of elements in pAllocations and pAllocationsChanged arrays.
@param[out] pAllocationsChanged Array of boolean values that will indicate whether matching allocation in pAllocations array has been moved. This parameter is optional. Pass null if you don't need this information.
@param pDefragmentationInfo Configuration parameters. Optional - pass null to use default values.
@param[out] pDefragmentationStats Statistics returned by the function. Optional - pass null if you don't need this information.
@return VK_SUCCESS if completed, VK_INCOMPLETE if succeeded but didn't make all possible optimizations because limits specified in pDefragmentationInfo have been reached, negative error code in case of error.

This function works by moving allocations to different places (different
`VkDeviceMemory` objects and/or different offsets) in order to optimize memory
usage. Only allocations that are in pAllocations array can be moved. All other
allocations are considered nonmovable in this call. Basic rules:

- Only allocations made in memory types that have
  `VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT` flag can be compacted. You may pass other
  allocations but it makes no sense - these will never be moved.
- You may pass allocations made with `VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT` but
  it makes no sense - they will never be moved.
- Both allocations made with or without `VMA_ALLOCATION_CREATE_MAPPED_BIT`
  flag can be compacted. If not persistently mapped, memory will be mapped
  temporarily inside this function if needed.
- You must not pass same `VmaAllocation` object multiple times in pAllocations array.

The function also frees empty `VkDeviceMemory` blocks.

After allocation has been moved, its VmaAllocationInfo::deviceMemory and/or
VmaAllocationInfo::offset changes. You must query them again using
vmaGetAllocationInfo() if you need them.

If an allocation has been moved, data in memory is copied to new place
automatically, but if it was bound to a buffer or an image, you must destroy
that object yourself, create new one and bind it to the new memory pointed by
the allocation. You must use `vkDestroyBuffer()`, `vkDestroyImage()`,
`vkCreateBuffer()`, `vkCreateImage()` for that purpose and NOT vmaDestroyBuffer(),
vmaDestroyImage(), vmaCreateBuffer(), vmaCreateImage()! Example:

\code
VkDevice device = ...;
VmaAllocator allocator = ...;
std::vector<VkBuffer> buffers = ...;
std::vector<VmaAllocation> allocations = ...;

std::vector<VkBool32> allocationsChanged(allocations.size());
vmaDefragment(allocator, allocations.data(), allocations.size(), allocationsChanged.data(), nullptr, nullptr);

for(size_t i = 0; i < allocations.size(); ++i)
{
    if(allocationsChanged[i])
    {
        VmaAllocationInfo allocInfo;
        vmaGetAllocationInfo(allocator, allocations[i], &allocInfo);

        vkDestroyBuffer(device, buffers[i], nullptr);

        VkBufferCreateInfo bufferInfo = ...;
        vkCreateBuffer(device, &bufferInfo, nullptr, &buffers[i]);
            
        // You can make dummy call to vkGetBufferMemoryRequirements here to silence validation layer warning.
            
        vkBindBufferMemory(device, buffers[i], allocInfo.deviceMemory, allocInfo.offset);
    }
}
\endcode

Warning! This function is not correct according to Vulkan specification. Use it
at your own risk. That's becuase Vulkan doesn't guarantee that memory
requirements (size and alignment) for a new buffer or image are consistent. They
may be different even for subsequent calls with the same parameters. It really
does happen on some platforms, especially with images.

This function may be time-consuming, so you shouldn't call it too often (like
every frame or after every resource creation/destruction), but rater you can
call it on special occasions (like when reloading a game level, when you just
destroyed a lot of objects).
*/
VkResult vmaDefragment(
    VmaAllocator allocator,
    VmaAllocation* pAllocations,
    size_t allocationCount,
    VkBool32* pAllocationsChanged,
    const VmaDefragmentationInfo *pDefragmentationInfo,
    VmaDefragmentationStats* pDefragmentationStats);

/**
@param[out] pBuffer Buffer that was created.
@param[out] pAllocation Allocation that was created.
@param[out] pAllocationInfo Optional. Information about allocated memory. It can be later fetched using function vmaGetAllocationInfo().

This function automatically:

-# Creates buffer.
-# Allocates appropriate memory for it.
-# Binds the buffer with the memory.

If any of these operations fail, buffer and allocation are not created,
returned value is negative error code, *pBuffer and *pAllocation are null.

If the function succeeded, you must destroy both buffer and allocation when you
no longer need them using either convenience function vmaDestroyBuffer() or
separately, using `vkDestroyBuffer()` and vmaFreeMemory().

If VMA_ALLOCATOR_CREATE_KHR_DEDICATED_ALLOCATION_BIT flag was used,
VK_KHR_dedicated_allocation extension is used internally to query driver whether
it requires or prefers the new buffer to have dedicated allocation. If yes,
and if dedicated allocation is possible (VmaAllocationCreateInfo::pool is null
and VMA_ALLOCATION_CREATE_NEVER_ALLOCATE_BIT is not used), it creates dedicated
allocation for this buffer, just like when using
VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT.
*/
VkResult vmaCreateBuffer(
    VmaAllocator allocator,
    const VkBufferCreateInfo* pBufferCreateInfo,
    const VmaAllocationCreateInfo* pAllocationCreateInfo,
    VkBuffer* pBuffer,
    VmaAllocation* pAllocation,
    VmaAllocationInfo* pAllocationInfo);

/** \brief Destroys Vulkan buffer and frees allocated memory.

This is just a convenience function equivalent to:

\code
vkDestroyBuffer(device, buffer, allocationCallbacks);
vmaFreeMemory(allocator, allocation);
\endcode

It it safe to pass null as buffer and/or allocation.
*/
void vmaDestroyBuffer(
    VmaAllocator allocator,
    VkBuffer buffer,
    VmaAllocation allocation);

/// Function similar to vmaCreateBuffer().
VkResult vmaCreateImage(
    VmaAllocator allocator,
    const VkImageCreateInfo* pImageCreateInfo,
    const VmaAllocationCreateInfo* pAllocationCreateInfo,
    VkImage* pImage,
    VmaAllocation* pAllocation,
    VmaAllocationInfo* pAllocationInfo);

/** \brief Destroys Vulkan image and frees allocated memory.

This is just a convenience function equivalent to:

\code
vkDestroyImage(device, image, allocationCallbacks);
vmaFreeMemory(allocator, allocation);
\endcode

It it safe to pass null as image and/or allocation.
*/
void vmaDestroyImage(
    VmaAllocator allocator,
    VkImage image,
    VmaAllocation allocation);

#ifdef __cplusplus
}
#endif

#endif // AMD_VULKAN_MEMORY_ALLOCATOR_H
