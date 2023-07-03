# Add source to this project's executable.
file(GLOB SOURCE_FILES *.cpp *.hpp *.h *.c)
add_executable(vulkan-pathtracer
    ${SOURCE_FILES}
    shaders/pathtracer.slang
)

set_property(TARGET ${PROJNAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:vulkan-pathtracer>)

target_include_directories(${PROJNAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

# ###################################
find_package(Vulkan REQUIRED)

find_package(spdlog CONFIG REQUIRED)

find_package(tinyobjloader CONFIG REQUIRED)

find_package(vk-bootstrap CONFIG REQUIRED)

find_package(Stb REQUIRED)

find_package(glm CONFIG REQUIRED)

find_package(fastgltf CONFIG REQUIRED)

find_package(magic_enum CONFIG REQUIRED)

target_link_libraries(${PROJNAME}
    vk-bootstrap::vk-bootstrap
    tinyobjloader::tinyobjloader
    spdlog::spdlog_header_only
    glm::glm
    fastgltf::fastgltf
    magic_enum::magic_enum
    ${Stb_INCULDE_DIR}
)

configure_third_party_libs(${PROJNAME})

target_link_libraries(vulkan-pathtracer Vulkan::Vulkan slang)

target_compile_definitions(
    vulkan-pathtracer PUBLIC
    $<$<CONFIG:Release>: NDEBUG> # Release
    $<$<CONFIG:Debug>: _DEBUG> # Debug
)

# ###########################
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

finalize_slang_dependency(${THIRDPARTY_DIR})