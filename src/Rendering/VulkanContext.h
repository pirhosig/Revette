#pragma once
#include "Vulkan_Headers.h"

#include "Core/RevetteCore.h"



class VulkanContext {
private:
    VkInstance instance{};
    VkDebugUtilsMessengerEXT debugMessenger{};
    VkSurfaceKHR surface{};
    VkPhysicalDevice physicalDevice{};
    VkDevice device{};
    u32 queueGraphicsIndex{};
    VkQueue queueGraphics{};
    VmaAllocator allocator{};

private:
    class Private;
    VulkanContext(bool);

public:
    VulkanContext(struct GLFWwindow* window, bool debugEnabled);
    ~VulkanContext();

    VulkanContext(VulkanContext&&) = delete;
    VulkanContext(const VulkanContext&) = delete;
    VulkanContext& operator=(VulkanContext&&) = delete;
    VulkanContext& operator=(const VulkanContext&) = delete;

    void waitDeviceIdle() noexcept;

    VkPhysicalDevice getPhysicalDevice() const;
    VkSurfaceKHR getSurface() const;
    VkDevice getDevice() const;
    u32 getQueueGraphicsFamily() const;
    VkQueue  getQueueGraphics() const;
    VmaAllocator getAllocator() const;
};
