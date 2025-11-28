#include "SingleCommandBuffer.h"

#include <stdexcept>



namespace {

void beginCommandBuffer(VkCommandBuffer commandBuffer) {
    VkCommandBufferBeginInfo beginInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext{},
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        .pInheritanceInfo{}
    };
    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("Failed to begin command buffer");
    }
}

}



SingleCommandBuffer::SingleCommandBuffer(VkDevice _device, uint32_t queueIndex) : SingleCommandBuffer() {
    device = _device;

    VkCommandPoolCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext{},
        .flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
        .queueFamilyIndex = queueIndex
    };
    if (vkCreateCommandPool(device, &createInfo, nullptr, &commandPool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create command pool");
    }

    VkCommandBufferAllocateInfo allocInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext{},
        .commandPool = commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1
    };
    if (vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate command buffer");
    }

    beginCommandBuffer(commandBuffer);
}



SingleCommandBuffer::SingleCommandBuffer(SingleCommandBuffer&& old) :
    device{old.device},
    commandPool{old.commandPool},
    commandBuffer{old.commandBuffer}
{
    old.device = {};
}



SingleCommandBuffer::~SingleCommandBuffer() {
    if (!device) return;

    vkDestroyCommandPool(device, commandPool, nullptr);
}



// Reset commandPool (implicitly resets commandBuffer) and start a new command buffer
void SingleCommandBuffer::reset() {
    if (vkResetCommandPool(device, commandPool, {}) != VK_SUCCESS) {
        throw std::runtime_error("Failed to reset command pool");
    }
    beginCommandBuffer(commandBuffer);
}



VkCommandBuffer SingleCommandBuffer::getBuffer() { return commandBuffer; }
