macro(project_includes PATH)
    include_directories(${PROJECT_SRC}/${PATH})
endmacro()

# project_module(<module_path> [module])
# include submodule by module_path only is this module not included yet
macro(project_module)
    set(PATH ${ARGV0})

    if (${ARGC} STREQUAL 1)
        get_filename_component(MODULE ${PATH} NAME)
    else()
        set(MODULE ${ARGV1})
    endif()

    if (NOT TARGET ${MODULE})
        add_subdirectory(${PROJECT_SRC}/${PATH} ${MODULE})
    endif()
endmacro()

macro(kmpx_project_image)
    add_custom_target(image
        COMMAND cmake -DCMAKE_INSTALL_PREFIX=`pwd`/image ${CMAKE_HOME_DIRECTORY} && make install
    )
endmacro()

macro(kmpx_project_add_library)
    add_library(${ARGV})
    set_target_properties(${ARGV0}
        PROPERTIES
        OUTPUT_NAME ${ARGV0}${DEBUG_SUFFIX}
    )
endmacro()

macro(kmpx_project_add_executable)
    add_executable(${ARGV})
    set_target_properties(${ARGV0}
        PROPERTIES
        OUTPUT_NAME ${ASU_NAME}-${ARGV0}${DEBUG_SUFFIX}
    )
endmacro()

macro(set_if_not)
    if(NOT ${ARGV0})
        set(${ARGV0} ${ARGV1} ${ARGV2} ${ARGV3} ${ARGV3} ${ARGV4})
    endif()
endmacro()

macro(smart_copy)
    set(source_name ${ARGV0})
    set(destination_name ${ARGV1})
    set(copy_flags ${ARGV2})
    get_filename_component(destination_directory_name ${destination_name} PATH)
    install(CODE "
        execute_process(COMMAND mkdir -p ${destination_directory_name})
    ")
    install(CODE "
        execute_process(COMMAND cp ${copy_flags} ${source_name} ${destination_name})
    ")
endmacro()

macro(kmpx_project_install)
    get_target_property(target_location ${ARGV0} LOCATION)
    get_target_property(target_type ${ARGV0} TYPE)
    get_filename_component(target_filename ${target_location} NAME)

    set(binary_install_flags "-v --remove-destination")
    if (INSTALL_SYMLINKS)
        set(binary_install_flags "-sfv")
    endif()

    if(target_type STREQUAL "EXECUTABLE")
        smart_copy(${target_location} ${ASU_INSTALL_ROOT}/bin/${target_filename} ${binary_install_flags})
    endif()
    if(target_type STREQUAL "STATIC_LIBRARY")
        smart_copy(${target_location} ${ASU_INSTALL_ROOT}/lib/${ASU_NAME}/${target_filename} ${binary_install_flags})
    endif()
    if(target_type STREQUAL "SHARED_LIBRARY")
        smart_copy(${target_location} ${ASU_INSTALL_ROOT}/lib/${ASU_NAME}/${target_filename} ${binary_install_flags})
    endif()

    if (${ARGC} GREATER 2)
        set (headers_sub_dir ${ARGV1})
        set (instaled_headers ${${ARGV2}})
        foreach (f ${instaled_headers})
            set(destination_name ${ASU_INSTALL_ROOT}/include/${ASU_NAME}/${headers_sub_dir}/${f})
            smart_copy(${CMAKE_CURRENT_SOURCE_DIR}/${f} ${destination_name} ${binary_install_flags})
        endforeach(f)
     endif()

endmacro()

macro(kmpx_project_install_shares)
    if (${ARGC} GREATER 1)
        file(GLOB_RECURSE shares RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/${ARGV0} ${CMAKE_CURRENT_SOURCE_DIR}/${ARGV0}/*)
        set (shares_dest_sub_dir ${ARGV1})

        foreach (f ${shares})
            get_filename_component(share_name ${f} NAME)
            get_filename_component(share_path ${f} PATH)
            set(destination_name ${KMPX_PROJECT_INSTALL_ROOT}/share/${KMPX_PROJECT_NAME}/${shares_dest_sub_dir}/${share_name})

            if (INSTALL_SYMLINKS)
                smart_copy(${CMAKE_CURRENT_SOURCE_DIR}/${ARGV0}/${f} ${destination_name} "-sfvr")
            else()
                smart_copy(${CMAKE_CURRENT_SOURCE_DIR}/${ARGV0}/${f} ${destination_name} "-vr --remove-destination")
            endif()
        endforeach(f)
     endif()

endmacro()

