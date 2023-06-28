#include <SDL.h>
#include <SDL_vulkan.h>

#include <VkEngine.h>
#include <VkInitializers.h>
#include <VkTypes.h>

#include "VkBootstrap.h"
#include <array>
#include <quill/Quill.h>

#define VK_CHECK(x)                                                                                                    \
    do                                                                                                                 \
    {                                                                                                                  \
        VkResult err = x;                                                                                              \
        if (err)                                                                                                       \
        {                                                                                                              \
        }                                                                                                              \
    } while (0)

void VkEngine::init()
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_WindowFlags windowFlags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN);

    window = SDL_CreateWindow("Vulkan Engine", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowExtent.width,
                              windowExtent.height, windowFlags);
    isInitialized = true;
}

void VkEngine::cleanup()
{
    if (isInitialized)
    {
        SDL_DestroyWindow(window);
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