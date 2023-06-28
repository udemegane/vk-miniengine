

message("configure vulkan-memory-allocator...")

# vulkanmemoryallocator
add_library(VulkanMemoryAllocator INTERFACE)
target_include_directories(VulkanMemoryAllocator INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/VulkanMemoryAllocator/include)
message("set include directories to ${CMAKE_CURRENT_SOURCE_DIR}/VulkanMemoryAllocator/include ")

# sdl2
message("configure sdl2...")
add_library(sdl2 INTERFACE)
target_include_directories(sdl2 INTERFACE $ENV{VULKAN_SDK}/Include/SDL2)
target_link_directories(sdl2 INTERFACE $ENV{VULKAN_SDK}/Lib)
target_link_libraries(sdl2 INTERFACE SDL2 SDL2main)
message("set include directories to $ENV{VULKAN_SDK}/Include/SDL2")

message("link libraries to ${target}")

# slang
set(SLANG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/slang)
add_library(slang SHARED IMPORTED GLOBAL)
set_target_properties(slang PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${SLANG_DIR}
    IMPORTED_IMPLIB ${SLANG_DIR}/bin/windows-x64/release/slang.lib
    IMPORTED_LOCATION ${SLANG_DIR}/bin/windows-x64/release/slang.dll
)
file(GLOB SLANG_BIN LIST_DIRECTORIES false "${SLANG_DIR}/bin/windows-x64/release/*.dll" "${SLANG_DIR}/bin/windows-x64/release/*.lib")

function(configure_third_party_libs target)
    target_link_libraries(${target} sdl2 VulkanMemoryAllocator slang)
endfunction()

function(finalize_slang_dependency thirdparty_root)
    message("-- copy slang dependencies")
    set(SLANG_DIR ${thirdparty_root}/slang)
    file(GLOB SLANG_BIN LIST_DIRECTORIES false "${SLANG_DIR}/bin/windows-x64/release/*.dll" "${SLANG_DIR}/bin/windows-x64/release/*.lib")

    foreach(bin ${SLANG_BIN})
        add_custom_command(
            TARGET ${PROJNAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} ARGS -E copy ${bin} $<TARGET_FILE_DIR:${PROJNAME}>
            COMMENT "Copy binary to $<TARGET_FILE_DIR:${PROJNAME}>"
        )
    endforeach()
endfunction()