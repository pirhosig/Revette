#pragma once
#include "Vulkan_Headers.h"



class Buffer {
private:
    VmaAllocator allocator;
    VmaAllocation allocation{};
    VkBuffer buffer{};
    void* mapping{};

public:
    Buffer(
        VmaAllocator _allocator,
        VkDeviceSize size,
        VkBufferUsageFlags usage,
        VmaAllocationCreateFlags allocationFlags,
        VmaMemoryUsage allocationUsage,
        VkMemoryPropertyFlags memoryFlags
    );
    Buffer(Buffer&&);
    ~Buffer();


    Buffer(const Buffer&) = delete;
    Buffer operator=(Buffer&&) = delete;
    Buffer operator=(const Buffer&) = delete;

    void* getMappedPointer() const;
    VkBuffer getHandle() const;
};
