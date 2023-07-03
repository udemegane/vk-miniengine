#pragma once
#include <VkTypes.h>
#include <shaderCompiler.h>
#include <slang-com-helper.h>
#include <slang-com-ptr.h>
#include <slang.h>
#include <string>
#include <vector>

class VkEngine
{
  public:
    VkEngine(SlangShaderCompiler compiler, uint32_t width, uint32_t height);
    bool isInitialized{false};
    int frameNumber{0};
    SlangShaderCompiler compiler;
    VkExtent2D windowExtent{1600, 900};
    struct SDL_Window *window{nullptr};
    void init(bool useOffscreen = false);
    void cleanup();
    void draw();
    void run();
    void render();

  private:
    bool _isOffscreen = false;
    VkInstance _instance;
    VkDebugUtilsMessengerEXT _debugMessenger;
    VkPhysicalDevice _gpu;
    VkDevice _device;
    VkSurfaceKHR _surface;

    VkSwapchainKHR _swapchain;
    VkFormat _swapchainImageFormat;
    std::vector<VkImage> _swapchainImages;
    std::vector<VkImageView> _swapchainImageViews;

    VkQueue _graphicsQueue;
    uint32_t _graphicsQueueFamily;

    VkCommandPool _commandPool;
    VkCommandBuffer _mainCommandBuffer;

    VkRenderPass _renderPass;
    std::vector<VkFramebuffer> _framebuffers;

    VkSemaphore _presentSemaphore, _renderSemaphore;
    VkFence _renderFence;

    void initVulkan();
    void initSwapchain();
    void initCommands();
    void initDefaultRenderPass();
    void initFrameBuffers();
    void initSyncStructures();
    void initPipelines();
    VkShaderModule loadShaderModule(std::string moduleName, std::string entryName);
};

class PipelineBuilder
{
  public:
    std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
    VkPipelineVertexInputStateCreateInfo vertexInputInfo;
};