#include "Vulkan_Utils.h"

#include <fstream>
#include <stdexcept>
#include <vector>



namespace {

// Reads an entire binary file into a vector of char
std::vector<char> readBinaryFile(std::string& filepath) {
    std::ifstream file(filepath, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open shader file");
    }

    size_t fileSize = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    return buffer;
}

}



void addPipelineImageBarrier(VkCommandBuffer commandBuffer, VkImageMemoryBarrier2 barrier) {
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



VkShaderModule createShaderModule(VkDevice device, std::string filepath) {
    std::vector<char> shaderCode = readBinaryFile(filepath);

    VkShaderModuleCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext{},
        .flags{},
        .codeSize = shaderCode.size(),
        .pCode = reinterpret_cast<const uint32_t*>(shaderCode.data())
    };

    VkShaderModule shader;
    if (vkCreateShaderModule(device, &createInfo, nullptr, &shader) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create shader module");
    }
    return shader;
}
