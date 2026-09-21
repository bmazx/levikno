#include <levikno/levikno.h>
#include <levikno/lvn_graphics.h>

#include <vector>

#include "utils.h"

#define WIN_WIDTH (800)
#define WIN_HEIGHT (600)

static float s_Vertices[] = {
    /*    pos (x,y,z)   |        color      */
     0.0f, -0.5f, 0.0f,    1.0f, 0.0f, 0.0f,
    -0.5f,  0.5f, 0.0f,    0.0f, 1.0f, 0.0f,
     0.5f,  0.5f, 0.0f,    0.0f, 0.0f, 1.0f,
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
    LvnFile vertfile = lvnLoadFileBin("res/shaders/simpleTriangleVert.spv");
    LvnFile fragfile = lvnLoadFileBin("res/shaders/simpleTriangleFrag.spv");

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

    // create pipeline
    LvnPipelineFixedFunctions pipelineFixedFuncs = lvnConfigPipelineFixedFunctionsInit();

    LvnVertexAttribute attributes[] =
    {
        { 0, 0, Lvn_Format_R32G32B32_FLOAT, 0 },
        { 0, 2, Lvn_Format_R32G32B32_FLOAT, (3 * sizeof(float)) },
    };

    LvnVertexBindingDescription vertexBindingDescription{};
    vertexBindingDescription.binding = 0;
    vertexBindingDescription.stride = 6 * sizeof(float);

    LvnShader* shaderStages[] = { vertShader, fragShader, };

    LvnPipelineCreateInfo pipelineCreateInfo{};
    pipelineCreateInfo.pipelineFixedFunctions = &pipelineFixedFuncs;
    pipelineCreateInfo.pVertexAttributes = attributes;
    pipelineCreateInfo.vertexAttributeCount = LVN_ARRAY_LEN(attributes);
    pipelineCreateInfo.pVertexBindingDescriptions = &vertexBindingDescription;
    pipelineCreateInfo.vertexBindingDescriptionCount = 1;
    pipelineCreateInfo.pDescriptorLayouts = NULL;
    pipelineCreateInfo.descriptorLayoutCount = 0;
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
            fprintf(stderr, "failed to get image\n");
            continue;
        }

        lvnBeginCommandBuffer(cmdBuff);

        LvnClearColorValue clearValues[] = {{{ 0.0f, 0.0f, 0.0f, 1.0f }}};
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
    lvnDestroyCommandBuffer(cmdBuff);
    lvnDestroyBuffer(vertexBuffer);
    lvnDestroyFence(fence);
    lvnDestroySemaphore(imageWaitSemaphore);
    for (uint32_t i = 0; i < renderFinishedSemaphores.size(); i++)
        lvnDestroySemaphore(renderFinishedSemaphores[i]);
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
