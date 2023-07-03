#include <SDL.h>
#include <SDL_vulkan.h>

#include <VkEngine.h>
#include <VkInitializers.h>
#include <VkTypes.h>
#include <utils.h>

#include "VkBootstrap.h"
#include <slang-com-helper.h>
#include <spdlog/spdlog.h>

#include <array>
#include <filesystem>

VkEngine::VkEngine(SlangShaderCompiler compiler, uint32_t width, uint32_t height)
{
    this->compiler = compiler;
    windowExtent = VkExtent2D{width, height};
}

void VkEngine::init(bool useOffscreen)
{
    _isOffscreen = useOffscreen;

    if (_isOffscreen)
    {
        window = nullptr;
    }
    else
    {
        spdlog::info("Create a window");
        SDL_Init(SDL_INIT_VIDEO);

        SDL_WindowFlags windowFlags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN);

        window = SDL_CreateWindow("Vulkan Engine", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowExtent.width,
                                  windowExtent.height, windowFlags);
    }

    initVulkan();
    initSwapchain();
    initDefaultRenderPass();
    initFrameBuffers();
    initCommands();
    initSyncStructures();

    isInitialized = true;
}

void VkEngine::initVulkan()
{
    vkb::InstanceBuilder builder;

    bool useValidationLayers = false;
#ifdef _DEBUG
    useValidationLayers = true;
    spdlog::info("Activate validation layers (Debug build)");
#endif //_DEBUG
    auto inst_ret = builder.set_app_name("Example Vulkan Pathtracer")
                        .request_validation_layers(useValidationLayers)
                        .require_api_version(1, 3, 0)
                        .build();
    vkb::Instance vkbInstance = inst_ret.value();
    _instance = vkbInstance.instance;
    _debugMessenger = vkbInstance.debug_messenger;

    if (!_isOffscreen)
        SDL_Vulkan_CreateSurface(window, _instance, &_surface);

    VkPhysicalDeviceVulkan13Features features{};

    vkb::PhysicalDeviceSelector selector{vkbInstance};
    vkb::PhysicalDevice physicalDevice;

    if (!_isOffscreen)
        physicalDevice = selector.set_minimum_version(1, 3).set_surface(_surface).select().value();
    else
        physicalDevice = selector.set_minimum_version(1, 3).select().value();
    spdlog::info("Set {0} as a physical device", physicalDevice.name);
    vkb::DeviceBuilder deviceBuilder{physicalDevice};
    vkb::Device vkbDevice = deviceBuilder.build().value();

    _device = vkbDevice.device;
    _gpu = physicalDevice.physical_device;

    _graphicsQueue = vkbDevice.get_queue(vkb::QueueType::graphics).value();
    _graphicsQueueFamily = vkbDevice.get_queue_index(vkb::QueueType::graphics).value();
}

void VkEngine::initSwapchain()
{
    vkb::SwapchainBuilder swapchainBuilder{_gpu, _device, _surface};
    vkb::Swapchain vkbSwapchain = swapchainBuilder.use_default_format_selection()
                                      .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
                                      .set_desired_extent(windowExtent.width, windowExtent.height)
                                      .build()
                                      .value();
    _swapchain = vkbSwapchain.swapchain;
    _swapchainImages = vkbSwapchain.get_images().value();
    _swapchainImageViews = vkbSwapchain.get_image_views().value();
    _swapchainImageFormat = vkbSwapchain.image_format;
}

void VkEngine::initCommands()
{
    VkCommandPoolCreateInfo commandPoolInfo =
        vkinit::commandPoolCreateInfo(_graphicsQueueFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

    abortOnFail(vkCreateCommandPool(_device, &commandPoolInfo, nullptr, &_commandPool));

    VkCommandBufferAllocateInfo cmdAllocInfo = vkinit::commandBufferAllocateInfo(_commandPool, 1);

    abortOnFail(vkAllocateCommandBuffers(_device, &cmdAllocInfo, &_mainCommandBuffer));
}

void VkEngine::initDefaultRenderPass()
{
    VkAttachmentDescription colorAttachment = {};
    colorAttachment.format = _swapchainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentReference = {};
    colorAttachmentReference.attachment = 0;
    colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentReference;

    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;

    abortOnFail(vkCreateRenderPass(_device, &renderPassInfo, nullptr, &_renderPass));
}

void VkEngine::initFrameBuffers()
{
    VkFramebufferCreateInfo fbCreateInfo = {};
    fbCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    fbCreateInfo.pNext = nullptr;

    fbCreateInfo.renderPass = _renderPass;
    fbCreateInfo.attachmentCount = 1;
    fbCreateInfo.width = windowExtent.width;
    fbCreateInfo.height = windowExtent.height;
    fbCreateInfo.layers = 1;

    const uint32_t swapchainImageCount = _swapchainImages.size();
    _framebuffers = std::vector<VkFramebuffer>(swapchainImageCount);
    auto swapchainViewsTmp = &_swapchainImageViews;

    for (int i = 0; i < swapchainImageCount; i++)
    {
        fbCreateInfo.pAttachments = &_swapchainImageViews[i];
        abortOnFail(vkCreateFramebuffer(_device, &fbCreateInfo, nullptr, &_framebuffers[i]));
    }
}

void VkEngine::initSyncStructures()
{
    VkFenceCreateInfo fenceCreateInfo = {};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.pNext = nullptr;

    fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    abortOnFail(vkCreateFence(_device, &fenceCreateInfo, nullptr, &_renderFence));

    VkSemaphoreCreateInfo semaphoreCreateInfo = {};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphoreCreateInfo.pNext = nullptr;
    semaphoreCreateInfo.flags = 0;

    abortOnFail(vkCreateSemaphore(_device, &semaphoreCreateInfo, nullptr, &_presentSemaphore));
    abortOnFail(vkCreateSemaphore(_device, &semaphoreCreateInfo, nullptr, &_renderSemaphore));
}

void VkEngine::initPipelines()
{
    VkShaderModule triangleFragShader = loadShaderModule("triangle", "fsMain");
    VkShaderModule triangleVertShader = loadShaderModule("triangle", "vsMain");
}

void VkEngine::cleanup()
{
    if (isInitialized)
    {
        spdlog::info("Cleanup the Engine...");
        vkDeviceWaitIdle(_device);
        vkDestroyCommandPool(_device, _commandPool, nullptr);
        if (!_isOffscreen)
        {
            vkDestroyFence(_device, _renderFence, nullptr);
            vkDestroySemaphore(_device, _renderSemaphore, nullptr);
            vkDestroySemaphore(_device, _presentSemaphore, nullptr);

            vkDestroySwapchainKHR(_device, _swapchain, nullptr);
            vkDestroyRenderPass(_device, _renderPass, nullptr);
            for (int i = 0; i < _swapchainImageViews.size(); i++)
            {
                vkDestroyFramebuffer(_device, _framebuffers[i], nullptr);
                vkDestroyImageView(_device, _swapchainImageViews[i], nullptr);
            }
            vkDestroySurfaceKHR(_instance, _surface, nullptr);
            vkDestroyDevice(_device, nullptr);
            vkb::destroy_debug_utils_messenger(_instance, _debugMessenger);
            vkDestroyInstance(_instance, nullptr);
            SDL_DestroyWindow(window);
        }
        else
        {
            vkDestroyDevice(_device, nullptr);
            vkb::destroy_debug_utils_messenger(_instance, _debugMessenger);
            vkDestroyInstance(_instance, nullptr);
        }
    }
}

void VkEngine::run()
{
    SDL_Event e;
    bool isQuit = false;
    while (!isQuit)
    {
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
                isQuit = true;
        }
        draw();
    }
}

void VkEngine::draw()
{
    abortOnFail(vkWaitForFences(_device, 1, &_renderFence, true, 1e9));
    abortOnFail(vkResetFences(_device, 1, &_renderFence));

    uint32_t swapchainImageIndex;
    abortOnFail(vkAcquireNextImageKHR(_device, _swapchain, 1e9, _presentSemaphore, nullptr, &swapchainImageIndex));

    abortOnFail(vkResetCommandBuffer(_mainCommandBuffer, 0));

    VkCommandBuffer cmd = _mainCommandBuffer;
    VkCommandBufferBeginInfo cmdBeginInfo = {};
    cmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cmdBeginInfo.pNext = nullptr;

    cmdBeginInfo.pInheritanceInfo = nullptr;
    cmdBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    abortOnFail(vkBeginCommandBuffer(cmd, &cmdBeginInfo));
    VkClearValue clearValue;
    float flash = abs(sin(frameNumber / 120.f));
    clearValue = {{0.f, 0.f, flash, 1.0f}};

    VkRenderPassBeginInfo rpInfo = {};
    rpInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rpInfo.pNext = nullptr;
    rpInfo.renderPass = _renderPass;
    rpInfo.renderArea.offset.x = 0;
    rpInfo.renderArea.offset.y = 0;
    rpInfo.renderArea.extent = windowExtent;
    rpInfo.framebuffer = _framebuffers[swapchainImageIndex];

    rpInfo.clearValueCount = 1;
    rpInfo.pClearValues = &clearValue;
    vkCmdBeginRenderPass(cmd, &rpInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdEndRenderPass(cmd);
    abortOnFail(vkEndCommandBuffer(cmd));

    VkSubmitInfo submit = {};
    submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit.pNext = nullptr;

    VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    submit.pWaitDstStageMask = &waitStage;
    submit.waitSemaphoreCount = 1;
    submit.pWaitSemaphores = &_presentSemaphore;
    submit.signalSemaphoreCount = 1;
    submit.pSignalSemaphores = &_renderSemaphore;

    submit.commandBufferCount = 1;
    submit.pCommandBuffers = &cmd;

    abortOnFail(vkQueueSubmit(_graphicsQueue, 1, &submit, _renderFence));

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.pNext = nullptr;
    presentInfo.pSwapchains = &_swapchain;
    presentInfo.swapchainCount = 1;
    presentInfo.pWaitSemaphores = &_renderSemaphore;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pImageIndices = &swapchainImageIndex;
    abortOnFail(vkQueuePresentKHR(_graphicsQueue, &presentInfo));
    frameNumber++;
}

inline VkShaderModule VkEngine::loadShaderModule(std::string moduleName, std::string entryName)
{
    auto [binaryCode, codeSize] = compiler.getCodeData(moduleName, entryName);
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = codeSize;
    createInfo.pCode = binaryCode;

    VkShaderModule shaderModule = VK_NULL_HANDLE;
    if (vkCreateShaderModule(_device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
    {
        assert(0 && "failed to create shader module!");
    }

    return shaderModule;
}

void VkEngine::render()
{
}