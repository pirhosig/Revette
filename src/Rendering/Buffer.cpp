#include "Buffer.h"

#include <stdexcept>
#include <utility>



Buffer::Buffer(
    VmaAllocator _allocator,
    VkDeviceSize size,
    VkBufferUsageFlags usage,
    VmaAllocationCreateFlags allocationFlags,
    VmaMemoryUsage allocationUsage,
    VkMemoryPropertyFlags memoryFlags
) : allocator{_allocator} {
    if (size == 0) {
        throw std::runtime_error("Cannot create zero sized VkBuffer");
    }

    VkBufferCreateInfo bufferInfo{
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext{},
        .flags{},
        .size = size,
        .usage = usage,
        .sharingMode{},
        .queueFamilyIndexCount{},
        .pQueueFamilyIndices{}
    };

    VmaAllocationCreateInfo allocCreateInfo{
        .flags = allocationFlags,
        .usage = allocationUsage,
        .requiredFlags = memoryFlags,
        .preferredFlags{},
        .memoryTypeBits{},
        .pool{},
        .pUserData{},
        .priority{}
    };
    VmaAllocationInfo allocInfo{};
    if (vmaCreateBuffer(allocator, &bufferInfo, &allocCreateInfo, &buffer, &allocation, &allocInfo) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate vertex buffer");
    }
    mapping = allocInfo.pMappedData;
}



Buffer::Buffer(Buffer&& other) :
    allocator{std::exchange(other.allocator, nullptr)},
    allocation{other.allocation},
    buffer{other.buffer},
    mapping{other.mapping}
{}



Buffer::~Buffer() {
    if (!allocator) return;

    vmaDestroyBuffer(allocator, buffer, allocation);
}



// This function must only be called if the buffer was created as mapped
void* Buffer::getMappedPointer() const {
    if (!mapping) {
        throw std::runtime_error("Buffer mapping does not exist");
    }
    return mapping;
}



VkBuffer Buffer::getHandle() const {
    return buffer;
}

