#include <SDL.h>
#include <SDL_vulkan.h>

#include <VkEngine.h>
#include <VkInitializers.h>
#include <VkTypes.h>
#include <utils.h>

#include "VkBootstrap.h"
#include <array>
#include <spdlog/spdlog.h>

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
    isInitialized = true;
}

void VkEngine::initVulkan()
{
    vkb::InstanceBuilder builder;

    bool useValidationLayers = false;
#ifdef _DEBUG
    useValidationLayers = true;
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
}

void VkEngine::cleanup()
{
    if (isInitialized)
    {
        spdlog::info("Cleanup the Engine...");

        if (!_isOffscreen)
        {
            vkDestroySwapchainKHR(_device, _swapchain, nullptr);
            for (int i = 0; i < _swapchainImageViews.size(); i++)
            {
                vkDestroyImageView(_device, _swapchainImageViews[i], nullptr);
            }
            vkDestroyDevice(_device, nullptr);
            vkDestroySurfaceKHR(_instance, _surface, nullptr);
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
    while (isQuit)
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
}

void VkEngine::render()
{
}