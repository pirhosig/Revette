#include "RenderResources.h"

#include <array>
#include <stdexcept>

#include <lodepng/lodepng.h>

#include "Fence.h"
#include "LinearBufferSuballocator.h"
#include "Vulkan_Utils.h"
#include "SingleCommandBuffer.h"



namespace {

struct TextureLoadInfo{
    const char* filepath;
    uint32_t cellWidth;
    uint32_t cellHeight;
};
constexpr std::array TEXTURE_INFOS{
    TextureLoadInfo{"res/textures/texture_atlas.png", 16u, 16u},
    TextureLoadInfo{"res/textures/character_set.png", 6u, 8u}
};

}



// The fence that this returns must be waited on
void RenderResources::createTextures(VkQueue queue, uint32_t queueIndex) {
    SingleCommandBuffer commandBuffer(device, queueIndex);
    // TODO don't use a buffer suballocator for single allocation buffers
    std::vector<LinearBufferSuballocator> uploadBuffers;

    Fence fenceUploadsComplete(device, {});
 
    for (auto [path, cellWidth, cellHeight] : TEXTURE_INFOS) {
        std::vector<unsigned char> imageData;
        unsigned width;
        unsigned height;
        if (lodepng::decode(imageData, width, height, path)) {
            throw std::runtime_error("Failed to read texture data");
        }

        // Error if the texture isn't a grid of TEXTURE_SIZE sized squares
        if (width % cellWidth != 0 || height % cellHeight != 0) {
            throw std::runtime_error("Invalid texture size");
        }
        uint32_t textureGridWidth = width / cellWidth;
        uint32_t textureGridHeight = height / cellHeight;
        uint32_t textureCount = textureGridWidth * textureGridHeight;
    
        VkDeviceSize imageSize = width * height * 4;
        uploadBuffers.emplace_back(allocator, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, false);
        auto& uploadBuffer = uploadBuffers.back();
        static_cast<void>(uploadBuffer.writeData(imageData.data(), imageSize));
    
        textures.push_back({});
        auto& texture = textures.back();
    
        VkImageCreateInfo imageInfo{
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .pNext{},
            .flags{},
            .imageType = VK_IMAGE_TYPE_2D,
            .format = VK_FORMAT_R8G8B8A8_SRGB,
            .extent{
                .width = cellWidth,
                .height = cellHeight,
                .depth = 1
            },
            .mipLevels = 1,
            .arrayLayers = textureCount,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .tiling = VK_IMAGE_TILING_OPTIMAL,
            .usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            .sharingMode{},
            .queueFamilyIndexCount{},
            .pQueueFamilyIndices{},
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
        };
        VmaAllocationCreateInfo allocInfo{
            .flags{},
            .usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
            .requiredFlags{},
            .preferredFlags{},
            .memoryTypeBits{},
            .pool{},
            .pUserData{},
            .priority{}
        };
    
        if (vmaCreateImage(
            allocator,
            &imageInfo,
            &allocInfo,
            &texture.image,
            &texture.allocation,
            {}
        ) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate texture image");
        }
    
        // Transition the image into an optimal transfer destination
        transitionImageLayout(
            commandBuffer.getBuffer(),
            texture.image,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            {},
            {},
            VK_PIPELINE_STAGE_2_TRANSFER_BIT,
            VK_ACCESS_2_TRANSFER_WRITE_BIT,
            VkImageSubresourceRange{
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = textureCount
            }
        );

        std::vector<VkBufferImageCopy> copyRegions;
        copyRegions.reserve(textureCount);
        for (uint32_t row = 0; row < textureGridHeight; ++row) {
            for (uint32_t col = 0; col < textureGridWidth; ++col) {
                uint32_t bufferOffset = (row * textureGridWidth * cellWidth * cellHeight + col * cellWidth) * 4;
                uint32_t arrayIndex = row * textureGridWidth + col;

                copyRegions.push_back(VkBufferImageCopy{
                    .bufferOffset = bufferOffset,
                    .bufferRowLength = width,
                    .bufferImageHeight = height,
                    .imageSubresource{
                        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                        .mipLevel = 0,
                        .baseArrayLayer = arrayIndex,
                        .layerCount = 1
                    },
                    .imageOffset{},
                    .imageExtent{
                        .width = cellWidth,
                        .height = cellHeight,
                        .depth = 1
                    }
                });
            }
        }

        vkCmdCopyBufferToImage(
            commandBuffer.getBuffer(),
            uploadBuffer.getHandle(),
            texture.image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            static_cast<uint32_t>(copyRegions.size()),
            copyRegions.data()
        );
    
        // Transition the image into an optimal texture image
        transitionImageLayout(
            commandBuffer.getBuffer(),
            texture.image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            VK_PIPELINE_STAGE_2_TRANSFER_BIT,
            VK_ACCESS_2_TRANSFER_WRITE_BIT,
            VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
            VK_ACCESS_2_SHADER_READ_BIT,
            VkImageSubresourceRange{
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = textureCount
            }
        );

        VkImageViewCreateInfo viewCreateInfo{
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext{},
            .flags{},
            .image = texture.image,
            .viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY,
            .format = VK_FORMAT_R8G8B8A8_SRGB,
            .components{},
            .subresourceRange{
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = VK_REMAINING_ARRAY_LAYERS
            }
        };
        if (vkCreateImageView(device, &viewCreateInfo, nullptr, &texture.imageView)) {
            throw std::runtime_error("Failed to create image view");
        }
        
        VkSamplerCreateInfo samplerCreateInfo{
            .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
            .pNext{},
            .flags{},
            .magFilter = VK_FILTER_NEAREST,
            .minFilter = VK_FILTER_NEAREST,
            .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
            .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
            .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
            .addressModeW{},
            .mipLodBias{},
            .anisotropyEnable{},
            .maxAnisotropy{},
            .compareEnable{},
            .compareOp{},
            .minLod{},
            .maxLod{},
            .borderColor{},
            .unnormalizedCoordinates{}
        };
        if (vkCreateSampler(device, &samplerCreateInfo, nullptr, &texture.sampler) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create texture sampler");
        }
    }
    
    
    if (vkEndCommandBuffer(commandBuffer.getBuffer()) != VK_SUCCESS) {
        throw std::runtime_error("Failed to end command buffer");
    }
    
    VkCommandBuffer buffer = commandBuffer.getBuffer();
    VkSubmitInfo submitInfo{
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext{},
        .waitSemaphoreCount{},
        .pWaitSemaphores{},
        .pWaitDstStageMask{},
        .commandBufferCount = 1,
        .pCommandBuffers = &buffer,
        .signalSemaphoreCount{},
        .pSignalSemaphores{}
    };
    if (vkQueueSubmit(queue, 1, &submitInfo, fenceUploadsComplete.get()) != VK_SUCCESS) {
        throw std::runtime_error("Failed to submit draw commands to queue");
    }

    VkFence fence = fenceUploadsComplete.get();
    if (vkWaitForFences(device, 1, &fence, {}, UINT64_MAX)) {
        throw std::runtime_error("Failed to wait for fence");
    }
}



void RenderResources::createDescriptorLayout() {
    std::array bindings{
        VkDescriptorSetLayoutBinding{
            .binding = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
            .pImmutableSamplers{}
        },
        VkDescriptorSetLayoutBinding{
            .binding = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
            .pImmutableSamplers{}
        }
    };
    VkDescriptorSetLayoutCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext{},
        .flags{},
        .bindingCount = static_cast<uint32_t>(bindings.size()),
        .pBindings = bindings.data()
    };

    if (vkCreateDescriptorSetLayout(device, &createInfo, nullptr, &descriptorLayout) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create descriptor set layout");
    }
}



void RenderResources::createDescriptorSet() {
    std::array<VkDescriptorPoolSize, 2> poolSizes{
        VkDescriptorPoolSize{
            .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = 1
        },
        VkDescriptorPoolSize{
            .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = 2
        }
    };
    VkDescriptorPoolCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .pNext{},
        .flags{},
        .maxSets = 1,
        .poolSizeCount = static_cast<uint32_t>(poolSizes.size()),
        .pPoolSizes = poolSizes.data()
    };

    if (vkCreateDescriptorPool(device, &createInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create descriptor pool");
    }

    VkDescriptorSetAllocateInfo allocInfo{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext{},
        .descriptorPool = descriptorPool,
        .descriptorSetCount = 1,
        .pSetLayouts = &descriptorLayout
    };

    if (vkAllocateDescriptorSets(device, &allocInfo, &descriptorSet) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate descriptor sets");
    }

    std::vector<VkDescriptorImageInfo> imageInfos;
    std::vector<VkWriteDescriptorSet> descriptorWrites;
    imageInfos.reserve(textures.size());
    descriptorWrites.reserve(textures.size());
    for (auto& texture : textures) {
        imageInfos.push_back(
            VkDescriptorImageInfo{
                .sampler = texture.sampler,
                .imageView = texture.imageView,
                .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
            }
        );
        descriptorWrites.push_back(
            VkWriteDescriptorSet{
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .pNext{},
                .dstSet = descriptorSet,
                .dstBinding = static_cast<uint32_t>(imageInfos.size() - 1),
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .pImageInfo = &imageInfos.back(),
                .pBufferInfo{},
                .pTexelBufferView{}
            }
        );
    }

    vkUpdateDescriptorSets(
        device,
        static_cast<uint32_t>(descriptorWrites.size()),
        descriptorWrites.data(),
        {},
        {}
    );
}



RenderResources::RenderResources(VkDevice _device, VkQueue queue, uint32_t queueIndex, VmaAllocator _allocator) :
    RenderResources()
{
    device = _device;
    allocator = _allocator;

    createTextures(queue, queueIndex);
    createDescriptorLayout();
    createDescriptorSet();
}



RenderResources::~RenderResources() {
    vkDestroyDescriptorPool(device, descriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(device, descriptorLayout, nullptr);

    for (auto& texture : textures) {
        vkDestroySampler(device, texture.sampler, nullptr);
        vkDestroyImageView(device, texture.imageView, nullptr);
        vmaDestroyImage(allocator, texture.image, texture.allocation);
    }
}



VkDescriptorSetLayout RenderResources::getDescriptorLayout() const { return descriptorLayout; }
VkDescriptorSet RenderResources::getDescriptorSet() const { return descriptorSet; }
