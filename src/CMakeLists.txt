# Add source to this project's executable.
file(GLOB SOURCE_FILES *.cpp *.hpp *.h *.c)
add_executable(vulkan-pathtracer
    ${SOURCE_FILES}
    shaders/pathtracer.slang
)

set_property(TARGET vulkan-pathtracer PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:vulkan-pathtracer>)

target_include_directories(vulkan-pathtracer PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(vulkan-pathtracer vk-bootstrap VulkanMemoryAllocator glm tinyobjloader stb slang)

target_link_libraries(vulkan-pathtracer Vulkan::Vulkan sdl2)
set(SHADER_EXTENSION_REGEX "\.(slang|slangh|hlsli|hlsl)$")

macro(_copy_shaders shaders_dir)
    file(GLOB_RECURSE SLANG_FILES "${shaders_dir}/*")

    foreach(file ${SLANG_FILES})
        if(${file} MATCHES ${SHADER_EXTENSION_REGEX})
            add_custom_command(
                TARGET ${PROJNAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} ARGS -E copy_if_different ${file} $<TARGET_FILE_DIR:${PROJNAME}>
                COMMENT "Copying shader to ${file}"
            )
        endif()
    endforeach()
endmacro()

_copy_shaders(shaders)