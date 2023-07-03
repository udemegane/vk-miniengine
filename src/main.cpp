#include <VkEngine.h>
#include <shaderCompiler.h>

int main()
{
    SlangShaderCompiler compiler("glsl_460");
    VkEngine engine(compiler, 1600, 900);
    engine.init();
    engine.run();
    engine.cleanup();
    return 0;
}