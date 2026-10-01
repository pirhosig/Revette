#pragma once
#include "Util/efficient_vector.h"
#include "Vulkan_Headers.h"
#include "VulkanContext.h"



class RenderTarget {
private:
    struct GLFWwindow* window;
    const VulkanContext& vulkanContext;

    VkFormat colourFormat{};
    VkExtent2D extent{};
    VkSwapchainKHR swapchain{};
    rvl::efficient_vector<VkImage> swapchainImages;
    rvl::efficient_vector<VkImageView> swapchainImageViews;

    VkFormat depthFormat{};
    VkImage depthImage{};
    VmaAllocation depthImageAllocation{};
    VkImageView depthImageView{};

private:
    explicit RenderTarget(const VulkanContext& _vulkanContext);

    void createSwapchainObjects();
    void createDepthObjects();

public:
    RenderTarget(struct GLFWwindow* _window, const VulkanContext& _vulkanContext);
    ~RenderTarget();

    RenderTarget(RenderTarget&&) = delete;
    RenderTarget(const RenderTarget&) = delete;
    RenderTarget operator=(RenderTarget&&) = delete;
    RenderTarget operator=(const RenderTarget&) = delete;

    VkFormat getColourFormat() const;
    VkExtent2D getExtext() const;
    VkSwapchainKHR getSwapchain() const;
    VkImage getSwapchainImage(size_t index) const;
    VkImageView getSwapchainImageView(size_t index) const;
    VkFormat getDepthFormat() const;
    VkImage getDepthImage() const;
    VkImageView getDepthImageView() const;
};
