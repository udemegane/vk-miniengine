set(SHADER_EXTENSION_REGEX "\.(slang|slangh|hlsli|hlsl)$")

function(target_copy_shaders target output_root output_dir)
    get_target_property(target_source_dir ${target} SOURCE_DIR)

    get_target_property(target_sources_ ${target} SOURCES)

    foreach(file ${target_sources_})
        if(${file} MATCHES ${SHADER_EXTENSION_REGEX})
            if(IS_ABSOLUTE ${file})
                file(RELATIVE_PATH file ${target_source_dir} ${file})
            endif()

            set(src_file ${target_source_dir}/${file})
            set(dst_file ${output_root}/${output_dir}/${file})

            add_custom_command(
                OUTPUT ${dst_file}
                COMMAND ${CMAKE_COMMAND} ARGS -E copy_if_different ${src_file} ${dst_file}
                MAIN_DEPENDENCY ${src_file}
                COMMENT "${target}: Copying shader ${file}"
            )
        endif()
    endforeach()
endfunction()

function(target_copy_media_directory target src_dir output_path)
    add_custom_command(
        TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} ARGS -E copy_directory ${src_dir} ${output_path}
        COMMENT "${target}: Copying Media directory ${source_dir}"
    )
endfunction()