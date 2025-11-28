#pragma once
#include "Buffer.h"



class LinearBufferSuballocator {
private:
    Buffer buffer;
    
    VkDeviceSize allocationSize;
    VkDeviceSize currentOffset = 0;

public:
    LinearBufferSuballocator(
        VmaAllocator _allocator,
        VkDeviceSize size,
        VkBufferUsageFlags usage,
        bool preferDevice
    );
    ~LinearBufferSuballocator() = default;
    
    LinearBufferSuballocator(LinearBufferSuballocator&&) = default;
    
    LinearBufferSuballocator(const LinearBufferSuballocator&) = delete;
    LinearBufferSuballocator operator=(LinearBufferSuballocator&&) = delete;
    LinearBufferSuballocator operator=(const LinearBufferSuballocator&) = delete;

    VkDeviceSize writeData(const void* src, VkDeviceSize size);
    void reset();
    VkBuffer getHandle() const;
};
