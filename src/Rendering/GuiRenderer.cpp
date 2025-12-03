#include "GuiRenderer.h"
#include <algorithm>
#include <stdexcept>

#include "Vulkan_Utils.h"



namespace {

struct Vertex {
    float x;
    float y;
    uint16_t texture;

    static std::array<VkVertexInputBindingDescription, 1> getBindingDescriptions() {
        return {
            VkVertexInputBindingDescription{
                .binding = 0,
                .stride = sizeof(Vertex),
                .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
            }
        };
    }



    static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
        return {
            VkVertexInputAttributeDescription{
                .location = 0,
                .binding = 0,
                .format = VK_FORMAT_R32G32_SFLOAT,
                .offset = 0
            },
            VkVertexInputAttributeDescription{
                .location = 1,
                .binding = 0,
                .format = VK_FORMAT_R16_USCALED,
                .offset = offsetof(Vertex, texture)
            }
        };
    }
};

}



void GuiRenderer::createLayout(VkDescriptorSetLayout setLayout) {
    VkPipelineLayoutCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext{},
        .flags{},
        .setLayoutCount = 1,
        .pSetLayouts = &setLayout,
        .pushConstantRangeCount{},
        .pPushConstantRanges{}
    };
    if (vkCreatePipelineLayout(device, &createInfo, nullptr, &pipelineLayout)) {
        throw std::runtime_error("Failed to create GUI rnderer pipeline layout");
    }
}



void GuiRenderer::createPipeline(const RenderTarget& renderTarget) {
    VkFormat swapchainFormat = renderTarget.getColourFormat();
    VkPipelineRenderingCreateInfo renderingInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
        .pNext{},
        .viewMask{},
        .colorAttachmentCount = 1,
        .pColorAttachmentFormats = &swapchainFormat,
        .depthAttachmentFormat = renderTarget.getDepthFormat(),
        .stencilAttachmentFormat{}
    };

    VkShaderModule shaderModule = createShaderModule(device, "res/shaders/gui.spv");
    std::array<VkPipelineShaderStageCreateInfo, 2> shaderStageInfos{
        VkPipelineShaderStageCreateInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .pNext{},
            .flags{},
            .stage = VK_SHADER_STAGE_VERTEX_BIT,
            .module = shaderModule,
            .pName = "vertMain",
            .pSpecializationInfo{}
        },
        VkPipelineShaderStageCreateInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .pNext{},
            .flags{},
            .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
            .module = shaderModule,
            .pName = "fragMain",
            .pSpecializationInfo{}
        }
    };

    auto bindingDescriptions = Vertex::getBindingDescriptions();
    auto attributeDescriptions = Vertex::getAttributeDescriptions();
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .pNext{},
        .flags{},
        .vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size()),
        .pVertexBindingDescriptions = bindingDescriptions.data(),
        .vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size()),
        .pVertexAttributeDescriptions = attributeDescriptions.data()
    };

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .pNext{},
        .flags{},
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable{}
    };

    // Viewport and scissor specify counts with null pointers because we are using dynamic states for both
    VkPipelineViewportStateCreateInfo viewportStateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .pNext{},
        .flags{},
        .viewportCount = 1,
        .pViewports{},
        .scissorCount = 1,
        .pScissors{}
    };

    VkPipelineRasterizationStateCreateInfo rasterizationStateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .pNext{},
        .flags{},
        .depthClampEnable{},
        .rasterizerDiscardEnable{},
        .polygonMode{},
        .cullMode{},
        .frontFace{},
        .depthBiasEnable{},
        .depthBiasConstantFactor{},
        .depthBiasClamp{},
        .depthBiasSlopeFactor{},
        .lineWidth = 1.0f
    };

    VkPipelineMultisampleStateCreateInfo multisampleStateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .pNext{},
        .flags{},
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
        .sampleShadingEnable{},
        .minSampleShading{},
        .pSampleMask{},
        .alphaToCoverageEnable{},
        .alphaToOneEnable{}
    };
    
    VkPipelineDepthStencilStateCreateInfo depthStencilInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .pNext{},
        .flags{},
        .depthTestEnable{},
        .depthWriteEnable{},
        .depthCompareOp{},
        .depthBoundsTestEnable{},
        .stencilTestEnable{},
        .front{},
        .back{},
        .minDepthBounds{},
        .maxDepthBounds{}
    };

    VkPipelineColorBlendAttachmentState blendAttachmentInfo{
        .blendEnable{},
        .srcColorBlendFactor{},
        .dstColorBlendFactor{},
        .colorBlendOp{},
        .srcAlphaBlendFactor{},
        .dstAlphaBlendFactor{},
        .alphaBlendOp{},
        .colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT |
            VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT |
            VK_COLOR_COMPONENT_A_BIT
    };
    VkPipelineColorBlendStateCreateInfo colourBlendStateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .pNext{},
        .flags{},
        .logicOpEnable{},
        .logicOp{},
        .attachmentCount = 1,
        .pAttachments = &blendAttachmentInfo,
        .blendConstants{}
    };

    std::array dynamicStates{
        VK_DYNAMIC_STATE_SCISSOR,
        VK_DYNAMIC_STATE_VIEWPORT
    };
    VkPipelineDynamicStateCreateInfo dynamicStateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .pNext{},
        .flags{},
        .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
        .pDynamicStates = dynamicStates.data()
    };

    VkGraphicsPipelineCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext = &renderingInfo,
        .flags{},
        .stageCount = static_cast<uint32_t>(shaderStageInfos.size()),
        .pStages = shaderStageInfos.data(),
        .pVertexInputState = &vertexInputInfo,
        .pInputAssemblyState = &inputAssemblyInfo,
        .pTessellationState{},
        .pViewportState = &viewportStateInfo,
        .pRasterizationState = &rasterizationStateInfo,
        .pMultisampleState = &multisampleStateInfo,
        .pDepthStencilState = &depthStencilInfo,
        .pColorBlendState = &colourBlendStateInfo,
        .pDynamicState = &dynamicStateInfo,
        .layout = pipelineLayout,
        .renderPass{},
        .subpass{},
        .basePipelineHandle{},
        .basePipelineIndex{}
    };
    
    VkResult result = vkCreateGraphicsPipelines(device, {}, 1, &createInfo, nullptr, &pipeline);
    // Destroy the shader module regardless of whether the pipeline was successfully created
    vkDestroyShaderModule(device, shaderModule, nullptr);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create graphics pipeline");
    }
}



GuiRenderer::GuiRenderer(
    VkDevice _device,
    const RenderTarget& renderTarget,
    VkDescriptorSetLayout setLayout
) : GuiRenderer() {
    device = _device;
    createLayout(setLayout);
    createPipeline(renderTarget);
}



GuiRenderer::~GuiRenderer() {
    vkDestroyPipeline(device, pipeline, nullptr);
    vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
}



void GuiRenderer::draw(
    VkCommandBuffer commandBuffer,
    VkExtent2D screenSize,
    LinearBufferSuballocator& transientBuffer,
    EntityPosition playerPosition
) {

	// Update coordinates
    char coordinateString[40]{};
    int _length = snprintf(
        &coordinateString[0],
        40,
        "%8.2lf %8.2lf %8.2lf",
        playerPosition.pos.x,
        playerPosition.pos.y,
        playerPosition.pos.z
    );
    _length = std::min(_length, 40 - 1);

    float charWidth = 24.0f / static_cast<float>(screenSize.width);
    float charHeight = 32.0f / static_cast<float>(screenSize.height);
    std::vector<Vertex> vertices;
    std::vector<uint16_t> indices;
    for (int i = 0; i < _length; ++i) {
        char c = coordinateString[i];
        uint16_t tex = 0;
        if (c == ' ') {
            continue;
        }
        if ('0' <= c && c <= '9') {
            tex = c - '0';
        }
        else if (c == '.') {
            tex = 10;
        }
        else if (c == '-') {
            tex = 11;
        }

        float xl = -1.0f + charWidth * static_cast<float>(i);
        float xr = -1.0f + charWidth * static_cast<float>(i + 1);
        float yl = -1.0f;
        float yu = -1.0f + charHeight;
        uint16_t baseIndex = static_cast<uint16_t>(vertices.size());
        vertices.push_back(Vertex{.x = xl, .y = yl, .texture = tex});
        vertices.push_back(Vertex{.x = xr, .y = yl, .texture = tex});
        vertices.push_back(Vertex{.x = xr, .y = yu, .texture = tex});
        vertices.push_back(Vertex{.x = xl, .y = yu, .texture = tex});

		indices.push_back(baseIndex);
		indices.push_back(baseIndex + 2);
		indices.push_back(baseIndex + 1);
		indices.push_back(baseIndex + 2);
		indices.push_back(baseIndex);
		indices.push_back(baseIndex + 3);
    }

    VkDeviceSize offsetVertices = transientBuffer.writeData(
        vertices.data(),
        sizeof(decltype(vertices)::value_type) * vertices.size()
    );
    VkDeviceSize offsetIndices = transientBuffer.writeData(
        indices.data(),
        sizeof(decltype(indices)::value_type) * indices.size()
    );

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
    VkBuffer _buffer = transientBuffer.getHandle();
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, &_buffer, &offsetVertices);
    vkCmdBindIndexBuffer(commandBuffer, _buffer, offsetIndices, VK_INDEX_TYPE_UINT16);
    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
}



VkPipelineLayout GuiRenderer::getLayout() const { return pipelineLayout; }
