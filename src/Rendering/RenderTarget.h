#pragma once
#include <vector>

#include "Vulkan_Headers.h"



class RenderTarget {
private:
    struct GLFWwindow* window;
    VkPhysicalDevice physicalDevice;
    VkSurfaceKHR surface;
    VkDevice device;
    VmaAllocator allocator;

    VkFormat colourFormat{};
    VkExtent2D extent{};
    VkSwapchainKHR swapchain{};
    std::vector<VkImage> swapchainImages;
    std::vector<VkImageView> swapchainImageViews;

    VkFormat depthFormat{};
    VkImage depthImage{};
    VmaAllocation depthImageAllocation{};
    VkImageView depthImageView{};

private:
    RenderTarget() = default;

    void createSwapchainObjects();
    void createDepthObjects();

public:
    RenderTarget(
        struct GLFWwindow* _window,
        VkPhysicalDevice _physicalDevice,
        VkSurfaceKHR _surface,
        VkDevice _device,
        VmaAllocator _allocator
    );
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
