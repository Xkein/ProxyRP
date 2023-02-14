
if(NOT TARGET Vulkan)
    set(vulkan_dir $ENV{VULKAN_SDK} CACHE STRING "")
    if (NOT vulkan_dir)
        message(FATAL_ERROR "please install Vulkan SDK.")
    endif()
    set(vulkan_include ${vulkan_dir}/Include CACHE STRING "")
    set(vulkan_lib_dir ${vulkan_dir}/Lib CACHE STRING "")
    set(vulkan_bin_dir ${vulkan_dir}/Bin CACHE STRING "")

    find_file(_vulkan_hpp "vulkan.hpp" REQUIRED PATHS ${vulkan_include}/vulkan)
    
    if(WIN32)
        set(vulkan_libs
            ${vulkan_lib_dir}/vulkan-1.lib CACHE STRING "")
    else()
    endif()
    
    add_library(Vulkan INTERFACE)
    target_include_directories(Vulkan INTERFACE ${vulkan_include})
    target_compile_definitions(Vulkan INTERFACE
        "VK_LAYER_PATH=$<BUILD_INTERFACE:${vulkan_bin_dir}>"
        VULKAN_HPP_NO_CONSTRUCTORS)
    target_link_libraries(Vulkan INTERFACE ${vulkan_libs})
endif()