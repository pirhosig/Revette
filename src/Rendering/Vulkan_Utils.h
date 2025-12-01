#pragma once
#include <string>

#include "Vulkan_Headers.h"



void transitionImageLayout(
    VkCommandBuffer commandBuffer,
    VkImage image,
    VkImageLayout oldLayout,
    VkImageLayout newLayout,
    VkPipelineStageFlags2 srcStageMask,
    VkAccessFlags2 srcAccessMask,
    VkPipelineStageFlags2 dstStageMask,
    VkAccessFlags2 dstAccessMask,
    VkImageSubresourceRange subresourceRange
);

VkImageView createImageView(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspects);

VkShaderModule createShaderModule(VkDevice device, std::string filepath);
