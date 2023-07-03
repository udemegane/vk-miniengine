#pragma once
#include <slang-com-helper.h>
#include <slang-com-ptr.h>
#include <slang.h>
#include <tuple>
#include <vector>
// class IShaderCompiler
// {
//   public:
//     virtual std::tuple<uint32_t *, size_t> getCodeData() = 0;
// };

class SlangShaderCompiler
{
  public:
    SlangShaderCompiler(const char *profile = "glsl_460");
    std::tuple<const uint32_t *, const size_t> getCodeData(std::string moduleName, std::string entryName);

  private:
    Slang::ComPtr<slang::IGlobalSession> _slangGlobalSession;
    Slang::ComPtr<slang::ISession> _mainSession;
};
