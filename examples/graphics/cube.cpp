#include <levikno/levikno.h>
#include <levikno/lvn_graphics.h>

#define LVN_GMATH_WHITELIST_INCLUDES
#define LVN_GMATH_INCLUDE_GRAPHICS_ESSENTIAL
#define LVN_GMATH_IMPL
#include <levikno/lvn_gmath.h>

#include <vector>

#include "utils.h"

#define WIN_WIDTH (800)
#define WIN_HEIGHT (600)

typedef struct UniformData
{
    LvnMat4 matrix;
} UniformData;

static float s_Vertices[] = {
    /*    pos (x,y,z)    |      UV   */
     0.5f, -0.5f, -0.5f,    1.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,
     0.5f,  0.5f, -0.5f,    1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,    0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,    1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,    1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,    1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,    1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,    0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,    1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,    1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,    0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,    0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,    1.0f, 0.0f,

     0.5f,  0.5f, -0.5f,    1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,    1.0f, 0.0f,
     0.5f, -0.5f, -0.5f,    0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,    0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,    0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,    1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,    0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,    1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,    1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,    1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,    0.0f, 1.0f,

     0.5f,  0.5f, -0.5f,    1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,    0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,    1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,    0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,    1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,    0.0f, 1.0f
};

void resizeFramebuffers(const LvnGraphicsContext* graphicsctx,
                        LvnSwapchain* swapchain,
                        LvnRenderPass* renderPass,
                        std::vector<LvnFramebuffer*>& swapchainFramebuffers,
                        uint32_t width,
                        uint32_t height)
{
    for (uint32_t i = 0; i < swapchainFramebuffers.size(); i++)
        lvnDestroyFramebuffer(swapchainFramebuffers[i]);

    uint32_t imageCount = lvnSwapchainGetImageCount(swapchain);

    swapchainFramebuffers.resize(imageCount);

    for (uint32_t i = 0; i < imageCount; i++)
    {
        LvnTexture* swapchainImage = lvnSwapchainGetImage(swapchain, i);
        LvnFramebufferCreateInfo framebufferCreateInfo{};
        framebufferCreateInfo.renderPass = renderPass;
        framebufferCreateInfo.colorAttachmentCount = 1;
        framebufferCreateInfo.pColorAttachments = &swapchainImage;
        framebufferCreateInfo.depthStencilAttachment = lvnSwapchainGetDepthImage(swapchain);
        framebufferCreateInfo.width = width;
        framebufferCreateInfo.height = height;

        lvnCreateFramebuffer(graphicsctx, &swapchainFramebuffers[i], &framebufferCreateInfo);
    }
}

void resizeSemaphores(const LvnGraphicsContext* graphicsctx,
                      LvnSwapchain* swapchain,
                      std::vector<LvnSemaphore*>& pSemaphores)
{
    for (uint32_t i = 0; i < pSemaphores.size(); i++)
        lvnDestroySemaphore(pSemaphores[i]);

    uint32_t imageCount = lvnSwapchainGetImageCount(swapchain);
    pSemaphores.resize(imageCount);
    for (uint32_t i = 0; i < imageCount; i++)
        lvnCreateSemaphore(graphicsctx, &pSemaphores[i]);
}

int main(int argc, char** argv)
{
    lvnSetMemAllocCallbacks(customMalloc, customFree, customRealloc, NULL);
    printf("s_MemAllocCount BEGIN: %zu\n", s_MemAllocCount);

    // create context
    LvnContextCreateInfo ctxCreateInfo{};
    ctxCreateInfo.logging.enableLogging = true;

    LvnContext* ctx;
    lvnCreateContext(&ctx, &ctxCreateInfo);

    // create window
    GLFWwindow* window = initWindow(WIN_WIDTH, WIN_HEIGHT, "simpleTriangle");

    NativeWindowData nwd = getNativeWindowData(window);

    LvnPlatformData pd{};
    pd.ndh = nwd.display;
    pd.nwh = nwd.window;

    // create graphics context
    LvnGraphicsContextCreateInfo graphicsCreateInfo{};
    graphicsCreateInfo.graphicsapi = Lvn_GraphicsApi_Vulkan;
    graphicsCreateInfo.presentationModeFlags = Lvn_PresentationModeFlag_Headless | Lvn_PresentationModeFlag_Surface;
    graphicsCreateInfo.platformData = &pd;
    graphicsCreateInfo.enableGraphicsApiDebugLogging = true;

    LvnGraphicsContext* graphicsctx;
    lvnCreateGraphicsContext(ctx, &graphicsctx, &graphicsCreateInfo);

    // get main surface from graphicsctx
    const LvnSurface* surface = lvnGetSurface(graphicsctx);

    // get supported surface format and present mode
    uint32_t formatCount;
    lvnSurfaceGetSupportedFormats(surface, &formatCount, NULL);

    std::vector<LvnFormat> formats(formatCount);
    lvnSurfaceGetSupportedFormats(surface, &formatCount, formats.data());

    LvnFormat selFormat = formats[0];
    for (uint32_t i = 0; i < formatCount; i++)
    {
        if (formats[i] == Lvn_Format_B8G8R8A8_SRGB)
        {
            selFormat = formats[i];
            break;
        }
    }

    uint32_t presentModeCount;
    lvnSurfaceGetSupportedPresentModes(surface, &presentModeCount, NULL);

    std::vector<LvnPresentMode> presentModes(presentModeCount);
    lvnSurfaceGetSupportedPresentModes(surface, &presentModeCount, presentModes.data());

    LvnPresentMode selPresentMode = Lvn_PresentMode_FIFO;
    for (uint32_t i = 0; i < presentModeCount; i++)
    {
        if (presentModes[i] == Lvn_PresentMode_FIFO)
        {
            selPresentMode = presentModes[i];
        }
    }

    // find supported depth format
    LvnFormat depthFormats[] = { Lvn_Format_D24_UNORM_S8_UINT, Lvn_Format_D32_FLOAT_S8_UINT, Lvn_Format_D32_FLOAT, Lvn_Format_D16_UNORM, };
    LvnFormat supportedDepthFormat = lvnFindSupportedDepthFormats(graphicsctx, LVN_ARRAY_LEN(depthFormats), depthFormats);

    // swapchain
    LvnSwapchainCreateInfo swapchainCreateInfo{};
    swapchainCreateInfo.surface = surface;
    swapchainCreateInfo.width = WIN_WIDTH;
    swapchainCreateInfo.height = WIN_HEIGHT;
    swapchainCreateInfo.surfaceFormat = selFormat;
    swapchainCreateInfo.depthFormat = supportedDepthFormat;
    swapchainCreateInfo.presentMode = selPresentMode;
    swapchainCreateInfo.minImageCount = 3;

    LvnSwapchain* swapchain;
    lvnCreateSwapchain(graphicsctx, &swapchain, &swapchainCreateInfo);

    // create render pass
    LvnColorAttachment colorAttachment{};
    colorAttachment.usage = Lvn_AttachmentUsage_PresentSrc;
    colorAttachment.format = selFormat;
    colorAttachment.samples = Lvn_SampleCountFlag_1_Bit;
    colorAttachment.loadOp = Lvn_AttachmentLoadOp_Clear;
    colorAttachment.storeOp = Lvn_AttachmentStoreOp_Store;

    LvnDepthStencilAttachment depthAttachment{};
    depthAttachment.samples = Lvn_SampleCountFlag_1_Bit;
    depthAttachment.format = supportedDepthFormat;
    depthAttachment.loadOp = Lvn_AttachmentLoadOp_Clear;
    depthAttachment.storeOp = Lvn_AttachmentStoreOp_Store;
    depthAttachment.stencilLoadOp = Lvn_AttachmentLoadOp_Clear;
    depthAttachment.stencilStoreOp = Lvn_AttachmentStoreOp_Store;

    LvnRenderPassCreateInfo renderPassCreateInfo{};
    renderPassCreateInfo.pColorAttachments = &colorAttachment;
    renderPassCreateInfo.colorAttachmentCount = 1;
    renderPassCreateInfo.depthStencilAttachment = &depthAttachment;

    LvnRenderPass* renderPass;
    lvnCreateRenderPass(graphicsctx, &renderPass, &renderPassCreateInfo);

    // create framebuffer
    uint32_t imageCount = lvnSwapchainGetImageCount(swapchain);
    LvnExtent2D extent = lvnSwapchainGetExtent(swapchain);

    std::vector<LvnFramebuffer*> swapchainFramebuffers(imageCount);

    for (uint32_t i = 0; i < swapchainFramebuffers.size(); i++)
    {
        LvnTexture* swapchainImage = lvnSwapchainGetImage(swapchain, i);
        LvnFramebufferCreateInfo framebufferCreateInfo{};
        framebufferCreateInfo.renderPass = renderPass;
        framebufferCreateInfo.colorAttachmentCount = 1;
        framebufferCreateInfo.pColorAttachments = &swapchainImage;
        framebufferCreateInfo.depthStencilAttachment = lvnSwapchainGetDepthImage(swapchain);
        framebufferCreateInfo.width = extent.width;
        framebufferCreateInfo.height = extent.height;

        lvnCreateFramebuffer(graphicsctx, &swapchainFramebuffers[i], &framebufferCreateInfo);
    }

    // create shaders
    LvnFile vertfile = lvnLoadFileBin("res/shaders/cubeVert.spv");
    LvnFile fragfile = lvnLoadFileBin("res/shaders/cubeFrag.spv");

    LvnShaderCreateInfo vertShCreateInfo{};
    vertShCreateInfo.pCode = vertfile.data;
    vertShCreateInfo.codeSize = vertfile.size;
    vertShCreateInfo.stage = Lvn_ShaderStageFlag_Vertex;
    vertShCreateInfo.entryPoint = "main";

    LvnShader* vertShader;
    lvnCreateShader(graphicsctx, &vertShader, &vertShCreateInfo);

    LvnShaderCreateInfo fragShCreateInfo{};
    fragShCreateInfo.pCode = fragfile.data;
    fragShCreateInfo.codeSize = fragfile.size;
    fragShCreateInfo.stage = Lvn_ShaderStageFlag_Fragment;
    fragShCreateInfo.entryPoint = "main";

    LvnShader* fragShader;
    lvnCreateShader(graphicsctx, &fragShader, &fragShCreateInfo);

    // create descriptor layout
    LvnDescriptorBinding descriptorBindings[] = {
        { 0, Lvn_DescriptorType_UniformBuffer, Lvn_ShaderStageFlag_Vertex },
        { 1, Lvn_DescriptorType_CombinedImageSampler, Lvn_ShaderStageFlag_Fragment },
    };

    LvnDescriptorLayoutCreateInfo descriptorLayoutCreateInfo{};
    descriptorLayoutCreateInfo.descriptorBindingCount = LVN_ARRAY_LEN(descriptorBindings);
    descriptorLayoutCreateInfo.pDescriptorBindings = descriptorBindings;

    LvnDescriptorLayout* descriptorLayout;
    lvnCreateDescriptorLayout(graphicsctx, &descriptorLayout, &descriptorLayoutCreateInfo);

    // create descriptor pool
    LvnDescriptorPoolSize descriptorPoolSizes[] = {
        { Lvn_DescriptorType_UniformBuffer, 1, },
        { Lvn_DescriptorType_CombinedImageSampler, 1, },
    };

    LvnDescriptorPoolCreateInfo descriptorPoolCreateInfo{};
    descriptorPoolCreateInfo.poolSizeCount = LVN_ARRAY_LEN(descriptorPoolSizes);
    descriptorPoolCreateInfo.pPoolSizes = descriptorPoolSizes;
    descriptorPoolCreateInfo.maxSets = 1;

    LvnDescriptorPool* descriptorPool;
    lvnCreateDescriptorPool(graphicsctx, &descriptorPool, &descriptorPoolCreateInfo);

    // allocate descriptor set
    LvnDescriptorSetAllocateInfo descriptorSetAllocInfo{};
    descriptorSetAllocInfo.descriptorPool = descriptorPool;
    descriptorSetAllocInfo.descriptorSetCount = 1;
    descriptorSetAllocInfo.pDescriptorLayouts = descriptorLayout;

    LvnDescriptorSet* descriptorSet;
    lvnAllocateDescriptorSets(graphicsctx, &descriptorSet, &descriptorSetAllocInfo);
    // create pipeline
    LvnPipelineFixedFunctions pipelineFixedFuncs = lvnConfigPipelineFixedFunctionsInit();
    pipelineFixedFuncs.depthstencil.depthTestEnable = true;
    pipelineFixedFuncs.depthstencil.depthWriteEnable = true;
    pipelineFixedFuncs.depthstencil.depthOpCompare = Lvn_CompareOp_LessOrEqual;

    LvnVertexAttribute attributes[] =
    {
        { 0, 0, Lvn_Format_R32G32B32_FLOAT, 0 },
        { 0, 2, Lvn_Format_R32G32_FLOAT, (3 * sizeof(float)) },
    };

    LvnVertexBindingDescription vertexBindingDescription{};
    vertexBindingDescription.binding = 0;
    vertexBindingDescription.stride = 5 * sizeof(float);

    LvnShader* shaderStages[] = { vertShader, fragShader, };

    LvnPipelineCreateInfo pipelineCreateInfo{};
    pipelineCreateInfo.pipelineFixedFunctions = &pipelineFixedFuncs;
    pipelineCreateInfo.pVertexAttributes = attributes;
    pipelineCreateInfo.vertexAttributeCount = LVN_ARRAY_LEN(attributes);
    pipelineCreateInfo.pVertexBindingDescriptions = &vertexBindingDescription;
    pipelineCreateInfo.vertexBindingDescriptionCount = 1;
    pipelineCreateInfo.pDescriptorLayouts = &descriptorLayout;
    pipelineCreateInfo.descriptorLayoutCount = 1;
    pipelineCreateInfo.pShaderStages = shaderStages;
    pipelineCreateInfo.stageCount = LVN_ARRAY_LEN(shaderStages);
    pipelineCreateInfo.renderPass = renderPass;

    LvnPipeline* pipeline;
    lvnCreatePipeline(graphicsctx, &pipeline, &pipelineCreateInfo);

    lvnDestroyShader(vertShader);
    lvnDestroyShader(fragShader);
    lvnUnloadFile(&vertfile);
    lvnUnloadFile(&fragfile);

    // create command buffer and sync objects
    LvnCommandBuffer* cmdBuff;
    lvnCreateCommandBuffer(graphicsctx, &cmdBuff);

    LvnFence* fence;
    lvnCreateFence(graphicsctx, &fence, true);

    LvnSemaphore* imageWaitSemaphore;
    lvnCreateSemaphore(graphicsctx, &imageWaitSemaphore);

    std::vector<LvnSemaphore*> renderFinishedSemaphores(imageCount);
    for (uint32_t i = 0; i < imageCount; i++)
        lvnCreateSemaphore(graphicsctx, &renderFinishedSemaphores[i]);

    // create vertex buffer
    LvnBufferCreateInfo bufferCreateInfo{};
    bufferCreateInfo.type = Lvn_BufferTypeFlag_Vertex;
    bufferCreateInfo.usage = Lvn_BufferMemoryUsage_CpuToGpu;
    bufferCreateInfo.data = s_Vertices;
    bufferCreateInfo.size = sizeof(s_Vertices);

    LvnBuffer* vertexBuffer;
    lvnCreateBuffer(graphicsctx, &vertexBuffer, &bufferCreateInfo);

    // create uniform buffer
    bufferCreateInfo.type = Lvn_BufferTypeFlag_Uniform;
    bufferCreateInfo.usage = Lvn_BufferMemoryUsage_CpuToGpu;
    bufferCreateInfo.size = sizeof(UniformData);
    bufferCreateInfo.data = NULL;

    LvnBuffer* uniformBuffer;
    lvnCreateBuffer(graphicsctx, &uniformBuffer, &bufferCreateInfo);

    // create sampler
    LvnSamplerCreateInfo samplerCreateInfo{};
    samplerCreateInfo.magFilter = Lvn_TextureFilter_Nearest;
    samplerCreateInfo.minFilter = Lvn_TextureFilter_Nearest;
    samplerCreateInfo.wrapR = Lvn_TextureMode_Repeat;
    samplerCreateInfo.wrapS = Lvn_TextureMode_Repeat;
    samplerCreateInfo.wrapT = Lvn_TextureMode_Repeat;

    LvnSampler* sampler;
    lvnCreateSampler(graphicsctx, &sampler, &samplerCreateInfo);

    // load image
    LvnLoadImageInfo loadImageInfo{};
    loadImageInfo.filepath = "res/images/debug.png";
    loadImageInfo.forceChannels = 4;
    loadImageInfo.flipVertically = true;

    LvnImage image = lvnLoadImage(&loadImageInfo);

    // create texture
    LvnTextureCreateInfo textureCreateInfo{};
    textureCreateInfo.format = Lvn_Format_R8G8B8A8_SRGB;
    textureCreateInfo.samples = Lvn_SampleCountFlag_1_Bit;
    textureCreateInfo.image = image.data;
    textureCreateInfo.width = image.width;
    textureCreateInfo.height = image.height;

    LvnTexture* texture;
    lvnCreateTexture(graphicsctx, &texture, &textureCreateInfo);

    LvnDescriptorBufferInfo descriptorBufferInfo{};
    descriptorBufferInfo.buffer = uniformBuffer;
    descriptorBufferInfo.range = sizeof(UniformData);
    descriptorBufferInfo.offset = 0;

    LvnDescriptorImageInfo descriptorImageInfo{};
    descriptorImageInfo.texture = texture;
    descriptorImageInfo.sampler = sampler;

    LvnDescriptorSetWriteInfo descriptorWriteInfos[2];
    descriptorWriteInfos[0].descriptorSet = descriptorSet,
    descriptorWriteInfos[0].descriptorType = Lvn_DescriptorType_UniformBuffer,
    descriptorWriteInfos[0].binding = 0,
    descriptorWriteInfos[0].bufferInfo = &descriptorBufferInfo,
    descriptorWriteInfos[1].descriptorSet = descriptorSet,
    descriptorWriteInfos[1].descriptorType = Lvn_DescriptorType_CombinedImageSampler,
    descriptorWriteInfos[1].binding = 1,
    descriptorWriteInfos[1].imageInfo = &descriptorImageInfo,

    lvnUpdateDescriptorSets(graphicsctx, LVN_ARRAY_LEN(descriptorWriteInfos), descriptorWriteInfos, 0, NULL);

    UniformData uboData{};
    lvn_mat4_identity(uboData.matrix);

    // render loop
    LvnResult result;
    uint32_t imageIndex = 0;
    int fps = 0;
    double prevTime = 0.0;

    while (!glfwWindowShouldClose(window))
    {
        double currTime = glfwGetTime();
        double delta = currTime - prevTime;
        fps++;
        if (delta >= 1.0)
        {
            lvnLogMessageInfo(lvnCtxGetCoreLogger(ctx), "fps: %d", fps);
            prevTime = currTime;
            fps = 0;
        }

        int width, height;
        glfwGetWindowSize(window, &width, &height);

        float aspect = (float)width / height;

        // uniform buffer
        LvnMat4 proj;
        lvn_perspectiveRHZO(proj, lvn_rad(60.0f), aspect, 0.01f, 1000.0f);
        proj[1][1] *= -1;

        LvnVec3 eye = {0.0f, 2.0f, 2.0f};
        LvnVec3 center = {0.0f, 0.0f, 0.0f};
        LvnVec3 up = {0.0f, 1.0f, 0.0f};
        LvnMat4 view;
        lvn_lookAtRH(view, eye, center, up);

        LvnMat4 model;
        lvn_mat4_identity(model);

        LvnVec3 axis = {0.0f, 1.0f, 0.0f};
        lvn_rotate(model, lvn_rad((float)currTime * 200), axis);

        LvnMat4 camera;
        LvnMat4* mpv[] = { &proj, &view, &model };
        lvn_mat4_mulN(mpv, LVN_ARRAY_LEN(mpv), camera);

        lvn_mat4_copy(camera, uboData.matrix);
        lvnBufferUpdate(uniformBuffer, &uboData, sizeof(UniformData), 0);

        lvnFenceWait(fence, UINT64_MAX);
        lvnFenceReset(fence);

        result = lvnSwapchainAcquireNextImage(swapchain, imageWaitSemaphore, NULL, &imageIndex);

        if (result == Lvn_Result_OutOfDate)
        {
            int width, height;
            glfwGetFramebufferSize(window, &width, &height);
            lvnSwapchainResize(swapchain, width, height);
            extent = lvnSwapchainGetExtent(swapchain);
            resizeFramebuffers(graphicsctx, swapchain, renderPass, swapchainFramebuffers, extent.width, extent.height);
            resizeSemaphores(graphicsctx, swapchain, renderFinishedSemaphores);
            imageCount = lvnSwapchainGetImageCount(swapchain);
            continue;
        }
        else if (result != Lvn_Result_Success)
        {
            lvnLogMessageError(lvnCtxGetCoreLogger(ctx), "failed to get image");
            continue;
        }

        lvnBeginCommandBuffer(cmdBuff);

        LvnClearColorValue clearValues[] = {{{ 0.1f, 0.1f, 0.1f, 1.0f }}};
        LvnClearDepthStencilValue depthValue = { 1.0f, 0 };

        LvnRenderPassBeginInfo beginInfo{};
        beginInfo.renderPass = renderPass;
        beginInfo.framebuffer = swapchainFramebuffers[imageIndex];
        beginInfo.renderArea = {extent, {0, 0}};
        beginInfo.clearColorValueCount = 1;
        beginInfo.pClearColorValues = clearValues;
        beginInfo.clearDepthStencilValue = depthValue;

        lvnCmdBeginRenderPass(cmdBuff, &beginInfo);

        lvnCmdBindPipeline(cmdBuff, pipeline);

        LvnViewport viewport{};
        viewport.width = (float)extent.width;
        viewport.height = (float)extent.height;
        viewport.x = 0;
        viewport.y = 0;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        lvnCmdSetViewport(cmdBuff, &viewport);

        LvnRenderArea renderArea{};
        renderArea.extent = extent;
        renderArea.offset = { 0, 0 };

        lvnCmdSetScissor(cmdBuff, &renderArea);

        uint64_t offsets[] = {0};
        lvnCmdBindVertexBuffer(cmdBuff, 0, 1, &vertexBuffer, offsets);

        lvnCmdBindDescriptorSets(cmdBuff, pipeline, 0, 1, &descriptorSet, 0, NULL);

        lvnCmdDraw(cmdBuff, LVN_ARRAY_LEN(s_Vertices), 1, 0, 0);

        lvnCmdEndRenderPass(cmdBuff);
        lvnEndCommandBuffer(cmdBuff);

        LvnSubmitInfo submitInfo{};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &imageWaitSemaphore;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &renderFinishedSemaphores[imageIndex];
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &cmdBuff;

        lvnRenderSubmit(graphicsctx, &submitInfo, 1, fence);

        LvnPresentInfo presentInfo{};
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &renderFinishedSemaphores[imageIndex];
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &swapchain;
        presentInfo.pImageIndices = &imageIndex;

        result = lvnRenderPresent(graphicsctx, &presentInfo);

        if (result == Lvn_Result_OutOfDate || s_WindowData.framebufferResized)
        {
            int width, height;
            glfwGetFramebufferSize(window, &width, &height);
            lvnSwapchainResize(swapchain, width, height);
            extent = lvnSwapchainGetExtent(swapchain);
            resizeFramebuffers(graphicsctx, swapchain, renderPass, swapchainFramebuffers, extent.width, extent.height);
            resizeSemaphores(graphicsctx, swapchain, renderFinishedSemaphores);
            imageCount = lvnSwapchainGetImageCount(swapchain);
            s_WindowData.framebufferResized = false;
        }
        glfwPollEvents();
    }

    // cleanup resources
    lvnUnloadImage(&image);
    lvnDestroyCommandBuffer(cmdBuff);
    lvnDestroyTexture(texture);
    lvnDestroySampler(sampler);
    lvnDestroyBuffer(uniformBuffer);
    lvnDestroyBuffer(vertexBuffer);
    lvnDestroyFence(fence);
    lvnDestroySemaphore(imageWaitSemaphore);
    for (uint32_t i = 0; i < renderFinishedSemaphores.size(); i++)
        lvnDestroySemaphore(renderFinishedSemaphores[i]);
    lvnDestroyDescriptorPool(descriptorPool);
    lvnDestroyDescriptorLayout(descriptorLayout);
    lvnDestroyPipeline(pipeline);

    for (uint32_t i = 0; i < imageCount; i++)
        lvnDestroyFramebuffer(swapchainFramebuffers[i]);
    lvnDestroyRenderPass(renderPass);
    lvnDestroySwapchain(swapchain);

    lvnDestroyGraphicsContext(graphicsctx);
    lvnDestroyContext(ctx);

    terminateWindow(window);

    printf("s_MemAllocCount END: %zu\n", s_MemAllocCount);
    return 0;
}
