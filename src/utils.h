#pragma once
#include <VkTypes.h>
#include <magic_enum.hpp>
#include <optional>
#include <slang-com-helper.h>
#include <slang.h>
#include <spdlog/spdlog.h>

template <typename T> void abortOnFail(T res)
{
    if (res)
    {
        spdlog::error("Detected Critical Error, abort program.\n{}", res);
        abort();
    }
}
template <> inline void abortOnFail(VkResult res)
{
    if (res)
    {
        auto str = magic_enum::enum_name(res);
        spdlog::error("Detected Critical Vulkan Error, abort program.\n{}", str);
        abort();
    }
}
template <> inline void abortOnFail(SlangResult res)
{
    if (SLANG_FAILED(res))
    {
        spdlog::error("Detected Critical Slang Error, abort program.\n{}", res);
        abort();
    }
}