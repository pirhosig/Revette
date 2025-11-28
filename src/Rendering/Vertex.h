#pragma once
#include <array>

#include "Vulkan_Headers.h"



struct alignas(8) Vertex {
	uint32_t x: 10;
	uint32_t y: 10;
	uint32_t z: 10;
	uint32_t u: 1;
	uint32_t v: 1;
	uint16_t texture;
	uint8_t light;

    static std::array<VkVertexInputBindingDescription, 1> getBindingDescriptions();
    static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions();
};
