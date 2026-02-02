#include "FrameRenderer.h"

#include <array>
#include <stdexcept>
#include <utility>

#include <glm/gtc/matrix_transform.hpp>

#include "Vulkan_Utils.h"



namespace {

VkSemaphore createSemaphore(VkDevice device) {
    VkSemaphoreCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext{},
        .flags{}
    };

    VkSemaphore semaphore;
    if (vkCreateSemaphore(device, &createInfo, nullptr, &semaphore) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create semaphore");
    }
    return semaphore;
}

}



/*
This function prepares all objects to be ready to render the next frame. It does the following:
 - Waits for the previous frame using these resources to complete
 - Acquires a new image from the swapchain
 - Resets and begins the command buffer
 - Transitions the image into a renderable state
*/
uint32_t FrameRenderer::beginFrame(
    std::queue<std::unique_ptr<MeshChunk::Data>> loadMeshes,
    std::unordered_map<ChunkPos, std::unique_ptr<MeshChunk>>& chunkMeshes
) {
    // Wait until the previous frame using these resources has completed
    VkFence fence = fenceBegin.get();
    if (vkWaitForFences(device, 1, &fence, {}, UINT64_MAX) != VK_SUCCESS) {
        throw std::runtime_error("Failed to wait for fences");
    }

    uint32_t imageIndex{};
    if (vkAcquireNextImageKHR(
        device,
        renderTarget.getSwapchain(),
        UINT64_MAX,
        semaphoreImageAvailable,
        {},
        &imageIndex
    ) != VK_SUCCESS) {
        throw std::runtime_error("Failed to acquire next swapchain image");
    }

    if (vkResetFences(device, 1, &fence) != VK_SUCCESS) {
        throw std::runtime_error("Failed to reset fence");
    }

    // Reset and start new command buffer
    commandBuffer.reset();

    std::vector<VkBufferMemoryBarrier2> bufferBarriers;
    std::vector<VkImageMemoryBarrier2> imageBarriers;

    // Transition the acquired image into a colour attachment
    imageBarriers.push_back(VkImageMemoryBarrier2{
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .pNext{},
            .srcStageMask = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT,
            .srcAccessMask{},
            .dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
            .dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
            .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .srcQueueFamilyIndex{},
            .dstQueueFamilyIndex{},
            .image = renderTarget.getSwapchainImage(imageIndex),
            .subresourceRange{
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1
            }
        }
    );
    // Transition the depth attachment into the right format
    imageBarriers.push_back(VkImageMemoryBarrier2{
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .pNext{},
            .srcStageMask = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT,
            .srcAccessMask = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT,
            .dstAccessMask = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
            .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .newLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
            .srcQueueFamilyIndex{},
            .dstQueueFamilyIndex{},
            .image = renderTarget.getDepthImage(),
            .subresourceRange{
                .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1
            }
        }
    );

    // Upload new meshes
    uploadMeshes(bufferBarriers, std::move(loadMeshes), chunkMeshes);

    // Barrier for image transitions and mesh uploading
    VkDependencyInfo dependencyInfo{
        .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
        .pNext{},
        .dependencyFlags{},
        .memoryBarrierCount{},
        .pMemoryBarriers{},
        .bufferMemoryBarrierCount = static_cast<uint32_t>(bufferBarriers.size()),
        .pBufferMemoryBarriers = bufferBarriers.data(),
        .imageMemoryBarrierCount = static_cast<uint32_t>(imageBarriers.size()),
        .pImageMemoryBarriers = imageBarriers.data()
    };
    vkCmdPipelineBarrier2(commandBuffer.getBuffer(), &dependencyInfo);

    VkRenderingAttachmentInfo attachmentColour{
        .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
        .pNext{},
        .imageView = renderTarget.getSwapchainImageView(imageIndex),
        .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .resolveMode{},
        .resolveImageView{},
        .resolveImageLayout{},
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .clearValue{
            .color{0.52f, 0.70f, 0.89f, 1.0f}
        }
    };
    VkRenderingAttachmentInfo attachmentDepth{
        .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
        .pNext{},
        .imageView = renderTarget.getDepthImageView(),
        .imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
        .resolveMode{},
        .resolveImageView{},
        .resolveImageLayout{},
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .clearValue = {
            .depthStencil{
                .depth = 1.0f,
                .stencil{}
            }
        }
    };
    VkRenderingInfo renderingInfo{
        .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
        .pNext{},
        .flags{},
        .renderArea{
            .offset{0, 0},
            .extent = renderTarget.getExtext()
        },
        .layerCount = 1,
        .viewMask{},
        .colorAttachmentCount = 1,
        .pColorAttachments = &attachmentColour,
        .pDepthAttachment = &attachmentDepth,
        .pStencilAttachment{}
    };
    vkCmdBeginRendering(commandBuffer.getBuffer(), &renderingInfo);

    return imageIndex;
}



void FrameRenderer::uploadMeshes(
    std::vector<VkBufferMemoryBarrier2>& bufferBarriers,
    std::queue<std::unique_ptr<MeshChunk::Data>> loadMeshes,
    std::unordered_map<ChunkPos, std::unique_ptr<MeshChunk>>& chunkMeshes
) {
    stagingBuffer.reset();

    while (loadMeshes.size()) {
        ChunkPos pos = loadMeshes.front()->getPosition();
        bufferBarriers.push_back({});
        chunkMeshes.insert({
            pos,
            std::make_unique<MeshChunk>(
                bufferBarriers.back(),
                std::move(loadMeshes.front()),
                allocator,
                commandBuffer.getBuffer(),
                stagingBuffer
            )
        });
        loadMeshes.pop();
    }
}



void FrameRenderer::drawChunks(
    EntityPosition playerPos,
    std::unordered_map<ChunkPos, std::unique_ptr<MeshChunk>>& chunkMeshes
) {
    double rotationY = glm::radians(std::clamp(playerPos.yRotation, -89.9, 89.9));
    double rotationX = glm::radians(playerPos.xRotation);
    glm::mat4 projection = glm::perspective(glm::radians(45.0), 1920.0 / 1080.0, 0.25, 1024.0);
    projection[1][1] *= -1.0f;
    ChunkPos _playerChunk(playerPos);
    const glm::vec3 cameraPos = glm::vec3(
        playerPos.pos.x - _playerChunk.x * CHUNK_SIZE,
        playerPos.pos.y - _playerChunk.y * CHUNK_SIZE + 3.0,
        playerPos.pos.z - _playerChunk.z * CHUNK_SIZE
    ) * 0.5f;
    const glm::vec3 front = glm::normalize(glm::vec3(
        cos(rotationX) * cos(rotationY),
        sin(rotationY),
        sin(rotationX) * cos(rotationY)
    ));
    const glm::mat4 view = glm::lookAt(cameraPos, cameraPos + front, glm::vec3(0.0, 1.0, 0.0));
    const glm::mat4 matrixProjectionView = projection * view;

    chunkRenderer.draw(
        commandBuffer.getBuffer(),
        matrixProjectionView,
        _playerChunk,
        chunkMeshes
    );
}



/*
This function submits all the rendering commands to the GPU, and submits the frame for presentation to the screen.
*/
void FrameRenderer::endFrame(uint32_t imageIndex) {
    vkCmdEndRendering(commandBuffer.getBuffer());

    // Transition the image to the presentation format (mostly likely a no-op)
    addPipelineImageBarrier(
        commandBuffer.getBuffer(),
        VkImageMemoryBarrier2{
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .pNext{},
            .srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
            .srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT,
            .dstAccessMask{},
            .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
            .srcQueueFamilyIndex{},
            .dstQueueFamilyIndex{},
            .image = renderTarget.getSwapchainImage(imageIndex),
            .subresourceRange{
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1
            }
        }
    );

    if (vkEndCommandBuffer(commandBuffer.getBuffer()) != VK_SUCCESS) {
        throw std::runtime_error("Failed to end command buffer");
    }

    // Submit the command buffer
    VkPipelineStageFlags waitStages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkCommandBuffer buffer = commandBuffer.getBuffer();
    VkSubmitInfo submitInfo{
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext{},
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &semaphoreImageAvailable,
        .pWaitDstStageMask = &waitStages,
        .commandBufferCount = 1,
        .pCommandBuffers = &buffer,
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = &semaphorePresent
    };
    if (vkQueueSubmit(queue, 1, &submitInfo, fenceBegin.get()) != VK_SUCCESS) {
        throw std::runtime_error("Failed to submit draw commands to queue");
    }

    VkResult presentResult{};
    VkSwapchainKHR swapchain = renderTarget.getSwapchain();
    VkPresentInfoKHR presentInfo{
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pNext{},
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &semaphorePresent,
        .swapchainCount = 1,
        .pSwapchains = &swapchain,
        .pImageIndices = &imageIndex,
        .pResults = &presentResult
    };
    if (vkQueuePresentKHR(queue, &presentInfo) != VK_SUCCESS) {
        throw std::runtime_error("Failed to submit presentation to queue");
    }
    if (presentResult != VK_SUCCESS) {
        throw std::runtime_error("Failed to present to swapchain");
    }
}



FrameRenderer::FrameRenderer(
    VkDevice _device,
    RenderTarget& _renderTarget,
    RenderResources& _renderResources,
    ChunkRenderer& _chunkRenderer,
    GuiRenderer& _guiRenderer,
    uint32_t queueFamilyIndex,
    VmaAllocator _allocator
) :
    device{_device},
    allocator{_allocator},
    renderTarget{_renderTarget},
    renderResources{_renderResources},
    chunkRenderer{_chunkRenderer},
    guiRenderer{_guiRenderer},
    // 8MB should be good, probably?
    stagingBuffer(
        allocator,
        (1u << 23),
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        false
    ),
    commandBuffer(device, queueFamilyIndex),
    fenceBegin(device, VK_FENCE_CREATE_SIGNALED_BIT)
{}



FrameRenderer::FrameRenderer(
    VkDevice _device,
    VkQueue _queue,
    RenderTarget& _renderTarget,
    RenderResources& _renderResources,
    ChunkRenderer& _chunkRenderer,
    GuiRenderer& _guiRenderer,
    uint32_t queueFamilyIndex,
    VmaAllocator allocator
) : FrameRenderer(
    _device,
    _renderTarget,
    _renderResources,
    _chunkRenderer,
    _guiRenderer,
    queueFamilyIndex,
    allocator
) {
    queue = _queue;

    semaphoreImageAvailable = createSemaphore(device);
    semaphorePresent = createSemaphore(device);
}



FrameRenderer::~FrameRenderer() {
    // Handle zombie state in order to support move construction
    // (I fucking hate this language)
    if (!device) return;

    vkDestroySemaphore(device, semaphorePresent, nullptr);
    vkDestroySemaphore(device, semaphoreImageAvailable, nullptr);
}



FrameRenderer::FrameRenderer(FrameRenderer&& old) :
    device{std::exchange(old.device, nullptr)},
    queue{old.queue},
    allocator{old.allocator},
    renderTarget{old.renderTarget},
    renderResources{old.renderResources},
    chunkRenderer{old.chunkRenderer},
    guiRenderer{old.guiRenderer},
    stagingBuffer{std::move(old.stagingBuffer)},
    commandBuffer{std::move(old.commandBuffer)},
    fenceBegin{std::move(old.fenceBegin)},
    semaphoreImageAvailable{old.semaphoreImageAvailable},
    semaphorePresent{old.semaphorePresent}
{}



void FrameRenderer::drawFrame(
    std::queue<std::unique_ptr<MeshChunk::Data>> loadMeshes,
    EntityPosition playerPosition,
    std::unordered_map<ChunkPos, std::unique_ptr<MeshChunk>>& chunkMeshes
) {
    uint32_t imageIndex = beginFrame(std::move(loadMeshes), chunkMeshes);
    
    // Delete the whole queue
    meshDeletionQueue = std::queue<std::unique_ptr<MeshChunk>>();

    VkViewport viewport{
        .x = 0.0f,
        .y = 0.0f,
        .width = static_cast<float>(renderTarget.getExtext().width),
        .height = static_cast<float>(renderTarget.getExtext().height),
        .minDepth = 0.0f,
        .maxDepth = 1.0f
    };
    vkCmdSetViewport(commandBuffer.getBuffer(), 0, 1, &viewport);

    VkRect2D scissor{
        .offset{},
        .extent = renderTarget.getExtext()
    };
    vkCmdSetScissor(commandBuffer.getBuffer(), 0, 1, &scissor);

    VkDescriptorSet descriptorSet = renderResources.getDescriptorSet();
    vkCmdBindDescriptorSets(
        commandBuffer.getBuffer(),
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        chunkRenderer.getLayout(),
        0,
        1,
        &descriptorSet,
        0,
        {}
    );

    drawChunks(playerPosition, chunkMeshes);

    vkCmdBindDescriptorSets(
        commandBuffer.getBuffer(),
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        guiRenderer.getLayout(),
        0,
        1,
        &descriptorSet,
        0,
        {}
    );
    guiRenderer.draw(
        commandBuffer.getBuffer(),
        renderTarget.getExtext(),
        stagingBuffer,
        playerPosition
    );

    endFrame(imageIndex);
}



void FrameRenderer::queueMeshForDeletion(std::unique_ptr<MeshChunk> mesh) {
    meshDeletionQueue.push(std::move(mesh));
}
