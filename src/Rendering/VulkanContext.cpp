#include "VulkanContext.h"

#include <format>
#include <iostream>
#include <optional>
#include <vector>

#include <boost/container/small_vector.hpp>

#include <GlobalLog.h>



constexpr auto REVETTE_VULKAN_VERSION = VK_API_VERSION_1_3;



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



void getRequiredGLFWInstanceExtensions(boost::container::small_vector_base<const char*>& extensions) {
    u32 extensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&extensionCount);
    if (glfwExtensions == nullptr) {
        throw std::runtime_error("Failed to fetch required extensions for GLFW");
    }

    for (u32 i = 0; i < extensionCount; ++i) {
        extensions.push_back(*(glfwExtensions + i));
    }
}



VkInstance createInstance(bool debugEnabled) {
    boost::container::small_vector<const char*, 16> requiredLayers;
    boost::container::small_vector<const char*, 16> requiredExtensions;
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
        .apiVersion = REVETTE_VULKAN_VERSION
    };
    VkInstanceCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext{},
        .flags{},
        .pApplicationInfo = &appInfo,
        .enabledLayerCount = static_cast<u32>(requiredLayers.size()),
        .ppEnabledLayerNames = requiredLayers.data(),
        .enabledExtensionCount = static_cast<u32>(requiredExtensions.size()),
        .ppEnabledExtensionNames = requiredExtensions.data()
    };

    if (debugEnabled) {
        createInfo.pNext = &debugMessengerCreateInfo;
    }

    VkInstance instance;
    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create instance");
    }
    return instance;
}



VkDebugUtilsMessengerEXT createDebugMessenger(VkInstance instance) {
    auto createFunction = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
        vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT")
    );
    if (!createFunction) {
        throw std::runtime_error("Failed to fetch function to create debug messenger");
    }

    VkDebugUtilsMessengerEXT debugMessenger;
    if (createFunction(instance, &debugMessengerCreateInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create debug messenger");
    }
    return debugMessenger;
}



VkSurfaceKHR createSurface(VkInstance instance, GLFWwindow* window) {
    VkSurfaceKHR surface;
    if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create surface");
    }
    return surface;
}



// We want to select the discrete GPU if there is one.
u32 calculatePhysicalDeviceRankScore(VkPhysicalDevice physicalDevice) {
    VkPhysicalDeviceProperties2 properties{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2,
        .pNext{},
        .properties{}
    };
    vkGetPhysicalDeviceProperties2(physicalDevice, &properties);

    u32 score = 0;
    if (properties.properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        score |= (1u << 31);
    }
    else if (properties.properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
        score |= (1u << 30);
    }

    return score;
}



void logPhysicalDeviceInfo(VkPhysicalDevice device) {
    VkPhysicalDeviceProperties2 properties{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2,
        .pNext{},
        .properties{}
    };
    vkGetPhysicalDeviceProperties2(device, &properties);
    GlobalLog.Write(std::format(
        "GPU specifications:\n"
        "  device name: {}\n"
        "  vulkan API version: {}.{}.{}\n",
        properties.properties.deviceName,
        VK_VERSION_MAJOR(properties.properties.apiVersion),
        VK_VERSION_MINOR(properties.properties.apiVersion),
        VK_VERSION_PATCH(properties.properties.apiVersion)
    ));
}



VkPhysicalDevice selectPhysicalDevice(VkInstance instance) {
    u32 deviceCount = 0;
    if (vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr) != VK_SUCCESS) {
        throw std::runtime_error("Failed to count physical devices");
    }

    if (deviceCount == 0) throw std::runtime_error("Failed to find physical device");

    boost::container::small_vector<VkPhysicalDevice, 8> devices(deviceCount);
    if (vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data()) != VK_SUCCESS) {
        throw std::runtime_error("Failed to retrieve physical devices");
    }

    std::pair<VkPhysicalDevice, u32> bestDevice{
        devices[0],
        calculatePhysicalDeviceRankScore(devices[0])
    };

    for (u32 i = 1; i < deviceCount; ++i) {
        u32 score = calculatePhysicalDeviceRankScore(devices[i]);
        if (bestDevice.second < score) {
            bestDevice = {devices[i], score};
        }
    }

    logPhysicalDeviceInfo(bestDevice.first);

    return bestDevice.first;
}

}



class VulkanContext::Private {
public:
    static void selectQueueIndices(VulkanContext& context);
    static void createDevice(VulkanContext& context);
    static void createAllocator(VulkanContext& context);
};



void VulkanContext::Private::selectQueueIndices(VulkanContext& context) {
    u32 queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(context.physicalDevice, &queueFamilyCount, nullptr);
    boost::container::small_vector<VkQueueFamilyProperties, 8> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(context.physicalDevice, &queueFamilyCount, queueFamilies.data());

    constexpr VkQueueFlags REQUIRED_FLAGS = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_TRANSFER_BIT;

    std::optional<u32> graphicsQueueFamily;
    for (u32 i = 0; i < queueFamilyCount; ++i) {
        // Skip if not all required usages are possible
        if ((queueFamilies[i].queueFlags & REQUIRED_FLAGS) != REQUIRED_FLAGS) continue;

        // Skip if the queue family cannot present to the window surface
        VkBool32 surfaceSupported = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(context.physicalDevice, i, context.surface, &surfaceSupported);
        if (!surfaceSupported) continue;

        graphicsQueueFamily = i;
    }

    if (!graphicsQueueFamily.has_value()) {
        throw std::runtime_error("Failed to find suitable graphics queue family");
    }
    context.queueGraphicsIndex = graphicsQueueFamily.value();
}



void VulkanContext::Private::createDevice(VulkanContext& context) {
    constexpr float priority = 1.0;
    VkDeviceQueueCreateInfo queueGraphicsCreateInfo{
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .pNext{},
        .flags{},
        .queueFamilyIndex = context.queueGraphicsIndex,
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
        .enabledExtensionCount = static_cast<u32>(deviceExtensions.size()),
        .ppEnabledExtensionNames = deviceExtensions.data(),
        .pEnabledFeatures{}
    };

    if (vkCreateDevice(context.physicalDevice, &createInfo, nullptr, &context.device) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create device");
    }
}



void VulkanContext::Private::createAllocator(VulkanContext& context) {
    VmaVulkanFunctions vulkanFunctions{};
    VmaAllocatorCreateInfo createInfo{
        .flags{},
        .physicalDevice = context.physicalDevice,
        .device = context.device,
        .preferredLargeHeapBlockSize{},
        .pAllocationCallbacks{},
        .pDeviceMemoryCallbacks{},
        .pHeapSizeLimit{},
        .pVulkanFunctions = &vulkanFunctions,
        .instance = context.instance,
        .vulkanApiVersion = REVETTE_VULKAN_VERSION,
        .pTypeExternalMemoryHandleTypes{}
    };
    vmaImportVulkanFunctionsFromVolk(&createInfo, &vulkanFunctions);

    if (vmaCreateAllocator(&createInfo, &context.allocator) != VK_SUCCESS)  {
        throw std::runtime_error("Failed to create allocator");
    }
}



VulkanContext::VulkanContext(bool debugEnabled) {
    if (volkInitialize() != VK_SUCCESS) {
        throw std::runtime_error("Failed to initialize volk.");
    }
    instance = createInstance(debugEnabled);
    volkLoadInstance(instance);
}



// Delegates to a constructor to create the instance, so that the destructor will be called if the instance was successfully
// created regardless of whether all other objects were successfully created.
VulkanContext::VulkanContext(GLFWwindow* window, bool debugEnabled) : VulkanContext(debugEnabled) {
    if (debugEnabled) {
        debugMessenger = createDebugMessenger(instance);
    }
    surface = createSurface(instance, window);
    physicalDevice = selectPhysicalDevice(instance);
    Private::selectQueueIndices(*this);

    Private::createDevice(*this);
    vkGetDeviceQueue(device, queueGraphicsIndex, 0, &queueGraphics);
    volkLoadDevice(device);

    Private::createAllocator(*this);
}



VulkanContext::~VulkanContext() {
    if (device != nullptr) {
        vmaDestroyAllocator(allocator);
        vkDestroyDevice(device, nullptr);
    }

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
u32 VulkanContext::getQueueGraphicsFamily() const { return queueGraphicsIndex; }
VkQueue VulkanContext::getQueueGraphics() const { return queueGraphics; }
VmaAllocator VulkanContext::getAllocator() const { return allocator; }
