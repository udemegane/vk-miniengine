#pragma once
#include <VkTypes.h>
#include <vector>
class VkEngine
{
  public:
    bool isInitialized{false};
    int frameNumber{0};
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

    void initVulkan();
    void initSwapchain();
    void initCommands();
};