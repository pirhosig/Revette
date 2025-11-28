#pragma once
#include "Vulkan_Headers.h"



class VulkanContext {
private:
    VkInstance instance{};
    VkDebugUtilsMessengerEXT debugMessenger{};
    VkSurfaceKHR surface{};
    VkPhysicalDevice physicalDevice{};
    VkDevice device{};
    uint32_t queueGraphicsIndex{};
    VkQueue queueGraphics{};
    VmaAllocator allocator{};

private:
    VulkanContext() = default;

    void createInstance(bool debugEnabled);
    void createDebugMessenger();
    void createSurface(struct GLFWwindow* window);
    void selectPhysicalDevice();
    void createDevice();
    void createAllocator();

public:
    VulkanContext(struct GLFWwindow* window, bool debugEnabled);
    ~VulkanContext();

    VulkanContext(VulkanContext&&) = delete;
    VulkanContext(const VulkanContext&) = delete;
    VulkanContext operator=(VulkanContext&&) = delete;
    VulkanContext operator=(const VulkanContext&) = delete;

    void waitDeviceIdle() noexcept;

    VkPhysicalDevice getPhysicalDevice() const;
    VkSurfaceKHR getSurface() const;
    VkDevice getDevice() const;
    uint32_t getQueueGraphicsFamily() const;
    VkQueue  getQueueGraphics() const;
    VmaAllocator getAllocator() const;
};
