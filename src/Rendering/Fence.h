#pragma once
#include "Vulkan_Utils.h"



class Fence {
private:
    VkDevice device{};
    VkFence fence{};

public:
    Fence(VkDevice _device, VkFenceCreateFlags flags);
    Fence(Fence&&);
    ~Fence();

    Fence(const Fence&) = delete;
    Fence operator=(Fence&&) = delete;
    Fence operator=(const Fence&) = delete;

    VkFence get() const;
};
