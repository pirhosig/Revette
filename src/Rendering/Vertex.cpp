#include "Vertex.h"



std::array<VkVertexInputBindingDescription, 1> Vertex::getBindingDescriptions() {
    return {
        VkVertexInputBindingDescription{
            .binding = 0,
            .stride = sizeof(Vertex),
            .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
        }
    };
}



std::array<VkVertexInputAttributeDescription, 3> Vertex::getAttributeDescriptions() {
    return {
        VkVertexInputAttributeDescription{
            .location = 0,
            .binding = 0,
            .format = VK_FORMAT_A2B10G10R10_UINT_PACK32,
            .offset = 0
        },
        VkVertexInputAttributeDescription{
            .location = 1,
            .binding = 0,
            .format = VK_FORMAT_R16_USCALED,
            .offset = offsetof(Vertex, texture)
        },
        VkVertexInputAttributeDescription{
            .location = 2,
            .binding = 0,
            .format = VK_FORMAT_R8_UNORM,
            .offset = offsetof(Vertex, light)
        },
    };
}
