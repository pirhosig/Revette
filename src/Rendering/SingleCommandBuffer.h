#pragma once

#include "Vulkan_Headers.h"
#include "Core/RevetteCore.h"



class SingleCommandBuffer {
private:
    VkDevice device;
    VkCommandPool commandPool{};
    VkCommandBuffer commandBuffer{};

private:
    SingleCommandBuffer() = default;

public:
    SingleCommandBuffer(VkDevice _device, u32 queueIndex);
    SingleCommandBuffer(SingleCommandBuffer&&);
    ~SingleCommandBuffer();


    SingleCommandBuffer(const SingleCommandBuffer&) = delete;
    SingleCommandBuffer operator=(SingleCommandBuffer&&) = delete;
    SingleCommandBuffer operator=(const SingleCommandBuffer&) = delete;

    void reset();
    VkCommandBuffer getBuffer();
};
