#include "Vulkan_Utils.h"

#include <stdexcept>



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
) {
    VkImageMemoryBarrier2 barrier{
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
        .pNext{},
        .srcStageMask = srcStageMask,
        .srcAccessMask = srcAccessMask,
        .dstStageMask = dstStageMask,
        .dstAccessMask = dstAccessMask,
        .oldLayout = oldLayout,
        .newLayout = newLayout,
        .srcQueueFamilyIndex{},
        .dstQueueFamilyIndex{},
        .image = image,
        .subresourceRange = subresourceRange
    };

    VkDependencyInfo dependencyInfo{
        .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
        .pNext{},
        .dependencyFlags{},
        .memoryBarrierCount{},
        .pMemoryBarriers{},
        .bufferMemoryBarrierCount{},
        .pBufferMemoryBarriers{},
        .imageMemoryBarrierCount = 1,
        .pImageMemoryBarriers = &barrier
    };

    vkCmdPipelineBarrier2(commandBuffer, &dependencyInfo);
}



VkImageView createImageView(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspects) {
    VkImageViewCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext{},
        .flags{},
        .image = image,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = format,
        .components{},
        .subresourceRange{
            .aspectMask = aspects,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1
        }
    };

    VkImageView view;
    if (vkCreateImageView(device, &createInfo, nullptr, &view) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create image view");
    }
    return view;
}
