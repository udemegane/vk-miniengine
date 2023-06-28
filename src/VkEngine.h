#pragma once
#include <VkTypes.h>

class VkEngine
{
  public:
    bool isInitialized{false};
    int frameNumber{0};
    VkExtent2D windowExtent{1600, 900};
    struct SDL_Window *window{nullptr};
    void init();
    void cleanup();
    void draw();
    void run();
};