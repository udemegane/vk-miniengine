#include <VkEngine.h>
#include <slang-com-ptr.h>
#include <slang.h>

int main()
{
    VkEngine engine;
    engine.init();
    engine.run();
    engine.cleanup();
    return 0;
}