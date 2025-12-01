#include "LinearBufferSuballocator.h"

#include <cstring>
#include <stdexcept>
#include <utility>



LinearBufferSuballocator::LinearBufferSuballocator(
    VmaAllocator allocator,
    VkDeviceSize size,
    VkBufferUsageFlags usage,
    bool preferDevice
) : buffer(
        allocator,
        size,
        usage,
        VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
        (preferDevice ? VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE : VMA_MEMORY_USAGE_AUTO),
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    ),
    allocationSize{size},
    currentOffset{0}
{}



VkDeviceSize LinearBufferSuballocator::writeData(const void* src, VkDeviceSize size) {
    if (currentOffset + size > allocationSize) {
        throw std::runtime_error("Cannot write to linear buffer suballocator, budget exceeded.");
    }
    std::memcpy(
        static_cast<char*>(buffer.getMappedPointer()) + currentOffset,
        src,
        size
    );

    return std::exchange(currentOffset, currentOffset + size);
}



void LinearBufferSuballocator::reset() {
    currentOffset = 0;
}



VkBuffer LinearBufferSuballocator::getHandle() const {
    return buffer.getHandle();
}

