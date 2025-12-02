#pragma once
#include <string>

#include "Vulkan_Headers.h"



void addPipelineImageBarrier(VkCommandBuffer commandBuffer, VkImageMemoryBarrier2 barrier);

VkImageView createImageView(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspects);

VkShaderModule createShaderModule(VkDevice device, std::string filepath);
