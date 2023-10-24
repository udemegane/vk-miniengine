# Add source to this project's executable.
file(GLOB SOURCE_FILES *.cpp *.hpp *.h *.c)
add_executable(${PROJNAME}
    ${SOURCE_FILES}
)

file(GLOB SHADER_FILES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.slang
    ${CMAKE_CURRENT_SOURCE_DIR}/*.slangh)

target_sources(${PROJNAME} PRIVATE ${SHADER_FILES})

set_property(TARGET ${PROJNAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:vk-miniengine>)

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

target_link_libraries(vk-miniengine Vulkan::Vulkan slang)

target_compile_definitions(
    vk-miniengine PUBLIC
    $<$<CONFIG:Release>: NDEBUG> # Release
    $<$<CONFIG:Debug>: _DEBUG> # Debug
)

# ###########################
target_copy_shaders(${PROJNAME} ${SHADER_OUTPUT_DIRECTORY} core)

finalize_slang_dependency(${THIRDPARTY_DIR})