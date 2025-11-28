#include "Fence.h"

#include <stdexcept>



Fence::Fence(VkDevice _device, VkFenceCreateFlags flags) : device{_device} {
    VkFenceCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext{},
        .flags = flags
    };
    if (vkCreateFence(device, &createInfo, nullptr, &fence) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create fence");
    }
}



Fence::Fence(Fence&& old) :
    device{old.device},
    fence{old.fence}
{
    old.device = {};
}



Fence::~Fence() {
    if (!device) return;

    vkDestroyFence(device, fence, nullptr);
}



VkFence Fence::get() const { return fence; }
