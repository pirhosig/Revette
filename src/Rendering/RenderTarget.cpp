#include "RenderTarget.h"

#include <stdexcept>

#include "Vulkan_Utils.h"



void RenderTarget::createSwapchainObjects() {
    // Create the swapchain object itself

    VkSurfaceCapabilitiesKHR capabilities;
    if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities) != VK_SUCCESS) {
        throw std::runtime_error("Failed to get surface capabilities");
    }

    // This should be changed to actually query the available formats, as this isn't even guaranteed to exist
    colourFormat = VK_FORMAT_B8G8R8A8_SRGB;
    VkColorSpaceKHR colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

    // This should probably be a setting rather than hardcoded
    uint32_t imageCount = std::max(3U, capabilities.minImageCount + 1);
    // A value of zero in maxImageCount indicates that it is unlimited
    if (capabilities.maxImageCount && imageCount > capabilities.maxImageCount) {
        imageCount = capabilities.maxImageCount;
    }

    extent = capabilities.currentExtent;
    // Maximum value indicates that the extent needs to be set manually
    if (extent.width == 0xFFFFFFFF) {
        int width;
        int height;
        glfwGetFramebufferSize(window, &width, &height);

        extent.width = std::clamp(
            static_cast<uint32_t>(width), capabilities.minImageExtent.width, capabilities.maxImageExtent.width
        );
        extent.height = std::clamp(
            static_cast<uint32_t>(height), capabilities.minImageExtent.height, capabilities.maxImageExtent.height
        );
    }

    VkSwapchainCreateInfoKHR createInfo{
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext{},
        .flags{},
        .surface = surface,
        .minImageCount = imageCount,
        .imageFormat = colourFormat,
        .imageColorSpace = colorSpace,
        .imageExtent = extent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode{},
        .queueFamilyIndexCount{},
        .pQueueFamilyIndices{},
        .preTransform = capabilities.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = VK_PRESENT_MODE_FIFO_KHR,
        .clipped = true,
        .oldSwapchain = nullptr
    };

    if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapchain) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create swapchain");
    }



    // Get the swapchain images (created automatically along with the swapchain)

    uint32_t realImageCount;
    if (vkGetSwapchainImagesKHR(device, swapchain, &realImageCount, nullptr) != VK_SUCCESS) {
        throw std::runtime_error("Failed to get swapchain image count");
    }
    swapchainImages.resize(realImageCount);
    if (vkGetSwapchainImagesKHR(device, swapchain, &realImageCount, swapchainImages.data()) != VK_SUCCESS) {
        throw std::runtime_error("Failed to get swapchain images");
    }



    // Create image views for each of the swapchain images
    swapchainImageViews.resize(realImageCount);
    for (size_t i = 0; i < realImageCount; ++i) {
        swapchainImageViews[i] = createImageView(
            device,
            swapchainImages[i],
            colourFormat,
            VK_IMAGE_ASPECT_COLOR_BIT
        );
    }
}



void RenderTarget::createDepthObjects() {
    depthFormat = VK_FORMAT_D32_SFLOAT;

    VkImageCreateInfo imageInfo{
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext{},
        .flags{},
        .imageType = VK_IMAGE_TYPE_2D,
        .format = depthFormat,
        .extent{
            .width = extent.width,
            .height = extent.height,
            .depth = 1
        },
        .mipLevels = 1,
        .arrayLayers = 1,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
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
    if (vmaCreateImage(allocator, &imageInfo, &allocInfo, &depthImage, &depthImageAllocation, {}) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate depth image");
    }

    depthImageView = createImageView(device, depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
}



// Delegates to an empty constructor so that the constructor will be called even
// if this function throws an exception
RenderTarget::RenderTarget(
    GLFWwindow* _window,
    VkPhysicalDevice _physicalDevice,
    VkSurfaceKHR _surface,
    VkDevice _device,
    VmaAllocator _allocator
) : RenderTarget()
{
    window = _window;
    physicalDevice = _physicalDevice;
    surface = _surface;
    device = _device;
    allocator = _allocator;

    createSwapchainObjects();
    createDepthObjects();
}



RenderTarget::~RenderTarget() {
    if (!device) return;

    vkDestroyImageView(device, depthImageView, nullptr);
    vmaDestroyImage(allocator, depthImage, depthImageAllocation);

    for (auto view : swapchainImageViews) {
        vkDestroyImageView(device, view, nullptr);
    }
    vkDestroySwapchainKHR(device, swapchain, nullptr);

    device = {};
}



// Surely the language could like, not require me to do this
VkFormat RenderTarget::getColourFormat() const { return colourFormat; }
VkExtent2D RenderTarget::getExtext() const { return extent; }
VkSwapchainKHR RenderTarget::getSwapchain() const { return swapchain; }
VkImage RenderTarget::getSwapchainImage(size_t index) const { return swapchainImages[index]; }
VkImageView RenderTarget::getSwapchainImageView(size_t index) const {return swapchainImageViews[index]; } 
VkFormat RenderTarget::getDepthFormat() const { return depthFormat; }
VkImage RenderTarget::getDepthImage() const { return depthImage; }
VkImageView RenderTarget::getDepthImageView() const { return depthImageView; }
