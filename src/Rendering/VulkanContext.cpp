#include "VulkanContext.h"

#include <iostream>
#include <vector>



constexpr auto VULKAN_VERSION = VK_API_VERSION_1_3;



namespace {

VKAPI_ATTR VkBool32 VKAPI_CALL debugMessengerCallback(
    [[maybe_unused]] VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    [[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    [[maybe_unused]] void* pUserData
) {
    std::cerr << "Validation Layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}



constexpr VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo{
    .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
    .pNext{},
    .flags{},
    .messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
    .messageType = 
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
    .pfnUserCallback = debugMessengerCallback,
    .pUserData{}
};



void getRequiredGLFWInstanceExtensions(std::vector<const char*>& extensions) {
    uint32_t extensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&extensionCount);
    if (glfwExtensions == nullptr) {
        throw std::runtime_error("Failed to fetch required extensions for GLFW");
    }

    for (uint32_t i = 0; i < extensionCount; ++i) {
        extensions.push_back(*(glfwExtensions + i));
    }
    
}

}



void VulkanContext::createInstance(bool debugEnabled) {
    std::vector<const char*> requiredLayers;
    std::vector<const char*> requiredExtensions;
    getRequiredGLFWInstanceExtensions(requiredExtensions);
    if (debugEnabled) {
        requiredLayers.push_back("VK_LAYER_KHRONOS_validation");
        requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    VkApplicationInfo appInfo{
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext{},
        .pApplicationName = "Revette",
        .applicationVersion = VK_MAKE_API_VERSION(0, 0, 0, 1),
        .pEngineName = "No Engine",
        .engineVersion{},
        .apiVersion = VULKAN_VERSION
    };
    VkInstanceCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext{},
        .flags{},
        .pApplicationInfo = &appInfo,
        .enabledLayerCount = static_cast<uint32_t>(requiredLayers.size()),
        .ppEnabledLayerNames = requiredLayers.data(),
        .enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size()),
        .ppEnabledExtensionNames = requiredExtensions.data()
    };

    if (debugEnabled) {
        createInfo.pNext = &debugMessengerCreateInfo;
    }

    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create instance");
    }
}



void VulkanContext::createDebugMessenger() {
    auto createFunction = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
        vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT")
    );
    if (!createFunction) {
        throw std::runtime_error("Failed to fetch function to create debug messenger");
    }

    if (createFunction(instance, &debugMessengerCreateInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create debug messenger");
    }
}



void VulkanContext::createSurface(GLFWwindow* window) {
    if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create surface");
    }
}



void VulkanContext::selectPhysicalDevice() {
    uint32_t deviceCount = 0;
    if (vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr) != VK_SUCCESS) {
        throw std::runtime_error("Failed to count physical devices");
    }

    if (!deviceCount) throw std::runtime_error("Failed to find physical device");

    std::vector<VkPhysicalDevice> devices(deviceCount);
    if (vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data()) != VK_SUCCESS) {
        throw std::runtime_error("Failed to retrieve physical devices");
    }

    // TODO of unbelievable proportions, this currently just picks the first device
    // and hopes for the best. This is atrocious and should certainly be improved in
    // due time.
    physicalDevice = devices[0];
}



uint32_t getQueueIndexGraphics(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

    constexpr VkQueueFlags REQUIRED_FLAGS = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_TRANSFER_BIT;

    for (uint32_t i = 0; i < queueFamilyCount; ++i) {
        // Skip if not all required usages are possible
        if ((queueFamilies[i].queueFlags & REQUIRED_FLAGS) != REQUIRED_FLAGS) continue;

        // Skip if the physical device cannot present to the window surface
        VkBool32 surfaceSupported = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &surfaceSupported);
        if (!surfaceSupported) continue;

        return i;
    }

    throw std::runtime_error("Failed to find suitable graphics queue");
}



void VulkanContext::createDevice() {
    queueGraphicsIndex = getQueueIndexGraphics(physicalDevice, surface);

    constexpr float priority = 1.0;
    VkDeviceQueueCreateInfo queueGraphicsCreateInfo{
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .pNext{},
        .flags{},
        .queueFamilyIndex = queueGraphicsIndex,
        .queueCount = 1,
        .pQueuePriorities = &priority
    };

    VkPhysicalDeviceVulkan13Features features13{};
    features13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    features13.dynamicRendering = true;
    features13.synchronization2 = true;

    std::vector<const char*> deviceExtensions{
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    VkDeviceCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = &features13,
        .flags{},
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &queueGraphicsCreateInfo,
        .enabledLayerCount{},
        .ppEnabledLayerNames{},
        .enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size()),
        .ppEnabledExtensionNames = deviceExtensions.data(),
        .pEnabledFeatures{}
    };

    if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create device");
    }

    vkGetDeviceQueue(device, queueGraphicsIndex, 0, &queueGraphics);
}



void VulkanContext::createAllocator() {
    VmaVulkanFunctions vulkanFunctions{};
    vulkanFunctions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
    vulkanFunctions.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;
    VmaAllocatorCreateInfo createInfo{
        .flags{},
        .physicalDevice = physicalDevice,
        .device = device,
        .preferredLargeHeapBlockSize{},
        .pAllocationCallbacks{},
        .pDeviceMemoryCallbacks{},
        .pHeapSizeLimit{},
        .pVulkanFunctions = &vulkanFunctions,
        .instance = instance,
        .vulkanApiVersion = VULKAN_VERSION,
        .pTypeExternalMemoryHandleTypes{}
    };

    if (vmaCreateAllocator(&createInfo, &allocator) != VK_SUCCESS)  {
        throw std::runtime_error("Failed to create allocator");
    }
}



VulkanContext::VulkanContext(GLFWwindow* window, bool debugEnabled) {
    createInstance(debugEnabled);
    createDebugMessenger();
    createSurface(window);
    selectPhysicalDevice();
    createDevice();
    createAllocator();
}



VulkanContext::~VulkanContext() {
    vmaDestroyAllocator(allocator);
    vkDestroyDevice(device, nullptr);
    vkDestroySurfaceKHR(instance, surface, nullptr);
    if (debugMessenger) {
        auto destroyFunction = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
            vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT")
        );
        if (destroyFunction) {
            destroyFunction(instance, debugMessenger, nullptr);
        }
    }
    vkDestroyInstance(instance, nullptr);
}



void VulkanContext::waitDeviceIdle() noexcept {
    // This does not handle VK_DEVICE_LOST so that the function can be noexcept,
    // and VK_DEVICE_LOST isn't really something that can be dealt with
    static_cast<void>(vkDeviceWaitIdle(device));
}



VkPhysicalDevice VulkanContext::getPhysicalDevice() const { return physicalDevice; }
VkSurfaceKHR VulkanContext::getSurface() const { return surface; }
VkDevice VulkanContext::getDevice() const { return device; }
uint32_t VulkanContext::getQueueGraphicsFamily() const { return queueGraphicsIndex; }
VkQueue VulkanContext::getQueueGraphics() const { return queueGraphics; }
VmaAllocator VulkanContext::getAllocator() const { return allocator; }
