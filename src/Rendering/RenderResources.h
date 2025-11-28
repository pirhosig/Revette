#pragma once
#include <vector>

#include "Fence.h"
#include "Vulkan_Headers.h"



class RenderResources{
private:
    struct TextureArray{
        VkImage image;
        VmaAllocation allocation;
        VkImageView imageView;
        VkSampler sampler;
    };

private:
    VkDevice device;
    VmaAllocator allocator;

    std::vector<TextureArray> textures;

    VkDescriptorSetLayout descriptorLayout{};
    VkDescriptorPool descriptorPool{};
    VkDescriptorSet descriptorSet{};

private:
    RenderResources() = default;

    void createTextures(VkQueue queue, uint32_t queueIndex);
    void createDescriptorLayout();
    void createDescriptorSet();

public:
    RenderResources(VkDevice _device, VkQueue queue, uint32_t queueIndex, VmaAllocator _allocator);
    ~RenderResources();

    RenderResources(RenderResources&&) = delete;
    RenderResources(const RenderResources&) = delete;
    RenderResources operator=(RenderResources&&) = delete;
    RenderResources operator=(const RenderResources&) = delete;

    VkDescriptorSetLayout getDescriptorLayout() const;
    VkDescriptorSet getDescriptorSet() const;
};
