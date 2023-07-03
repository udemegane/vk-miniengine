#include <filesystem>
#include <shaderCompiler.h>
#include <spdlog/spdlog.h>
#include <utils.h>

SlangShaderCompiler::SlangShaderCompiler(const char *profile)
{
    abortOnFail(slang::createGlobalSession(_slangGlobalSession.writeRef()));

    slang::TargetDesc targetDesc = {};
    targetDesc.format = SLANG_SPIRV;
    targetDesc.profile = _slangGlobalSession->findProfile(profile);
    targetDesc.forceGLSLScalarBufferLayout = true;
    spdlog::info("Initialize slang global session...");
    spdlog::info(" - format: SPIR-V");
    spdlog::info(" - glsl profile: {}", profile);
    spdlog::info(" - GLSL Scalar buffer layout: ", targetDesc.forceGLSLScalarBufferLayout ? "enable" : "disable");
    slang::SessionDesc sessionDesc = {};
    sessionDesc.targets = &targetDesc;
    sessionDesc.targetCount = 1;

    auto currentDir = std::filesystem::current_path();
    std::vector<std::filesystem::path> shaderDirectories = {
        currentDir,
        currentDir / "shaders",
    };
    std::vector<const char *> slangSearchPaths;
    for (auto &path : shaderDirectories)
    {
        slangSearchPaths.push_back(path.string().data());
    }
    sessionDesc.searchPaths = slangSearchPaths.data();
    sessionDesc.searchPathCount = (SlangInt)slangSearchPaths.size();
    abortOnFail(_slangGlobalSession->createSession(sessionDesc, _mainSession.writeRef()));
}

std::tuple<const uint32_t *, const size_t> SlangShaderCompiler::getCodeData(std::string moduleName,
                                                                            std::string entryName)
{

    slang::IModule *slangModule = nullptr;
    {
        Slang::ComPtr<slang::IBlob> diagnosticBlob;
        slangModule = _mainSession->loadModule(moduleName.c_str(), diagnosticBlob.writeRef());
        if (diagnosticBlob != nullptr)
        {
        }
        if (!slangModule)
            return std::tuple<const uint32_t *, const size_t>(nullptr, 0);
    }
    Slang::ComPtr<slang::IEntryPoint> entryPoint;
    slangModule->findEntryPointByName(entryName.c_str(), entryPoint.writeRef());

    std::vector<slang::IComponentType *> componentTypes;
    componentTypes.push_back(slangModule);
    componentTypes.push_back(entryPoint);

    Slang::ComPtr<slang::IComponentType> composedProgram;
    {
        Slang::ComPtr<slang::IBlob> diagnosticsBlob;
        SlangResult result =
            _mainSession->createCompositeComponentType(&componentTypes[0], (SlangInt)componentTypes.size(),
                                                       composedProgram.writeRef(), diagnosticsBlob.writeRef());
        abortOnFail(result);
    }

    Slang::ComPtr<slang::IBlob> spirvCode;
    {
        Slang::ComPtr<slang::IBlob> diagnositcsBlob;
        SlangResult result = composedProgram->getEntryPointCode(0, 0, spirvCode.writeRef(), diagnositcsBlob.writeRef());
        abortOnFail(result);
    }
    return std::tuple<const uint32_t *, const size_t>(static_cast<const uint32_t *>(spirvCode->getBufferPointer()),
                                                      spirvCode->getBufferSize());
}