#include <iostream>
#define VK_CHECK(x)                                                                                                    \
    do                                                                                                                 \
    {                                                                                                                  \
        VkResult err = x;                                                                                              \
        if (err)                                                                                                       \
        {                                                                                                              \
            std::cout << "Detected Critical Vulkan error: " << err << "\n"                                             \
        }                                                                                                              \
    } while (0)