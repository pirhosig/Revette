#pragma once
#include "LinearBufferSuballocator.h"
#include "RenderTarget.h"
#include "../World/Entities/EntityPosition.h"



class GuiRenderer {
private:
    VkDevice device{};

    VkPipelineLayout pipelineLayout{};
    VkPipeline pipeline{};

private:
    GuiRenderer() = default;

    void createLayout(VkDescriptorSetLayout setLayout);
    void createPipeline(const RenderTarget& renderTarget);
    
public:
    GuiRenderer(
        VkDevice _device,
        const RenderTarget& renderTarget,
        VkDescriptorSetLayout setLayout
    );
    ~GuiRenderer();

    GuiRenderer(GuiRenderer&&) = delete;
    GuiRenderer(const GuiRenderer&) = delete;
    GuiRenderer operator=(GuiRenderer&&) = delete;
    GuiRenderer operator=(const GuiRenderer&) = delete;

    void draw(
        VkCommandBuffer commandBuffer,
        VkExtent2D screenSize,
        LinearBufferSuballocator& transientBuffer,
        EntityPosition playerPosition
    );

    VkPipelineLayout getLayout() const;
};

