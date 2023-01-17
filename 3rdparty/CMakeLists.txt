
set(THIRD_PARTY_DIR "${ENGINE_ROOT_DIR}/3rdparty")

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

if(NOT WIN32)
    message(FATAL_ERROR "not support other platform except for Win32 yet.")
endif()

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

if(NOT TARGET stb)
    add_library(stb INTERFACE)
    target_include_directories(stb INTERFACE ${THIRD_PARTY_DIR}/stb)
endif()

if(NOT TARGET assimp)
    option(ASSIMP_BUILD_TESTS "" OFF)
    option(ASSIMP_INSTALL "" OFF)
    option(ASSIMP_BUILD_ASSIMP_VIEW "" OFF)
    add_subdirectory(assimp)
    set_target_properties(assimp PROPERTIES FOLDER 3rdparty/assimp)
    set_target_properties(UpdateAssimpLibsDebugSymbolsAndDLLs PROPERTIES FOLDER 3rdparty/assimp)
endif()

if(NOT TARGET glm)
    find_file(_glm_hpp "glm.hpp" REQUIRED PATHS ${vulkan_include}/glm)

    add_library(glm INTERFACE)
    target_compile_definitions(glm INTERFACE
        GLM_FORCE_DEPTH_ZERO_TO_ONE)
    target_include_directories(glm INTERFACE ${vulkan_include})
    # set_target_properties(glm PROPERTIES FOLDER 3rdparty)
endif()

if(NOT TARGET imgui)
    set(imgui_SOURCE_DIR_ ${CMAKE_CURRENT_SOURCE_DIR}/imgui)

    file(GLOB imgui_sources CONFIGURE_DEPENDS "${imgui_SOURCE_DIR_}/*.cpp")
    file(GLOB imgui_impl CONFIGURE_DEPENDS  
        "${imgui_SOURCE_DIR_}/backends/imgui_impl_glfw.cpp" 
        "${imgui_SOURCE_DIR_}/backends/imgui_impl_glfw.h"
        "${imgui_SOURCE_DIR_}/backends/imgui_impl_vulkan.cpp" 
        "${imgui_SOURCE_DIR_}/backends/imgui_impl_vulkan.h")
    add_library(imgui STATIC ${imgui_sources} ${imgui_impl})
    target_include_directories(imgui PUBLIC $<BUILD_INTERFACE:${imgui_SOURCE_DIR_}>)
    target_include_directories(imgui PUBLIC $<BUILD_INTERFACE:${vulkan_include}>)
    target_link_libraries(imgui PUBLIC glfw ${vulkan_lib})

    set_target_properties(imgui PROPERTIES FOLDER 3rdparty)
endif()

if(NOT TARGET spdlog)
    option(SPDLOG_BUILD_EXAMPLE "" OFF)
    option(SPDLOG_INSTALL "" OFF)
    add_subdirectory(spdlog)
    set_target_properties(spdlog PROPERTIES FOLDER 3rdparty)
endif()

if(NOT TARGET eigen)
    add_subdirectory(eigen)
endif()

if(NOT TARGET json)
    set(JSON_Install OFF CACHE INTERNAL "")
    set(JSON_BuildTests OFF CACHE INTERNAL "")
    add_subdirectory(json)
    add_library (json ALIAS nlohmann_json)
endif()

if(NOT TARGET glfw)
    option(GLFW_BUILD_EXAMPLES "" OFF)
    option(GLFW_BUILD_TESTS "" OFF)
    option(GLFW_BUILD_DOCS "" OFF)
    option(GLFW_INSTALL "" OFF)
    add_subdirectory(glfw)
    set_target_properties(glfw PROPERTIES FOLDER 3rdparty/glfw)
    set_target_properties(update_mappings PROPERTIES FOLDER 3rdparty/glfw)
endif()

if(NOT TARGET boost)
    add_subdirectory(boost)

    set_target_properties(boost_atomic                     PROPERTIES FOLDER 3rdparty/boost)
    set_target_properties(boost_chrono                     PROPERTIES FOLDER 3rdparty/boost)
    set_target_properties(boost_container                  PROPERTIES FOLDER 3rdparty/boost)
    set_target_properties(boost_context                    PROPERTIES FOLDER 3rdparty/boost)
    set_target_properties(boost_contract                   PROPERTIES FOLDER 3rdparty/boost)
    set_target_properties(boost_coroutine                  PROPERTIES FOLDER 3rdparty/boost)
    set_target_properties(boost_date_time                  PROPERTIES FOLDER 3rdparty/boost)
    set_target_properties(boost_exception                  PROPERTIES FOLDER 3rdparty/boost)
    set_target_properties(boost_fiber                      PROPERTIES FOLDER 3rdparty/boost)
    set_target_properties(boost_fiber_numa                 PROPERTIES FOLDER 3rdparty/boost)
    set_target_properties(boost_filesystem                 PROPERTIES FOLDER 3rdparty/boost)
    set_target_properties(boost_graph                      PROPERTIES FOLDER 3rdparty/boost)
    set_target_properties(boost_iostreams                  PROPERTIES FOLDER 3rdparty/boost)
    set_target_properties(boost_json                       PROPERTIES FOLDER 3rdparty/boost)
    set_target_properties(boost_locale                     PROPERTIES FOLDER 3rdparty/boost)
    set_target_properties(boost_log                        PROPERTIES FOLDER 3rdparty/boost)
    set_target_properties(boost_log_setup                  PROPERTIES FOLDER 3rdparty/boost)
    set_target_properties(boost_nowide                     PROPERTIES FOLDER 3rdparty/boost)
    set_target_properties(boost_prg_exec_monitor           PROPERTIES FOLDER 3rdparty/boost)
    set_target_properties(boost_program_options            PROPERTIES FOLDER 3rdparty/boost)
    set_target_properties(boost_random                     PROPERTIES FOLDER 3rdparty/boost)
    set_target_properties(boost_serialization              PROPERTIES FOLDER 3rdparty/boost)
    set_target_properties(boost_stacktrace_basic           PROPERTIES FOLDER 3rdparty/boost)
    set_target_properties(boost_stacktrace_noop            PROPERTIES FOLDER 3rdparty/boost)
    set_target_properties(boost_stacktrace_windbg          PROPERTIES FOLDER 3rdparty/boost)
    set_target_properties(boost_stacktrace_windbg_cached   PROPERTIES FOLDER 3rdparty/boost)
    set_target_properties(boost_test_exec_monitor          PROPERTIES FOLDER 3rdparty/boost)
    set_target_properties(boost_thread                     PROPERTIES FOLDER 3rdparty/boost)
    set_target_properties(boost_timer                      PROPERTIES FOLDER 3rdparty/boost)
    set_target_properties(boost_type_erasure               PROPERTIES FOLDER 3rdparty/boost)
    set_target_properties(boost_unit_test_framework        PROPERTIES FOLDER 3rdparty/boost)
    set_target_properties(boost_url                        PROPERTIES FOLDER 3rdparty/boost)
    set_target_properties(boost_wave                       PROPERTIES FOLDER 3rdparty/boost)
    set_target_properties(boost_wserialization             PROPERTIES FOLDER 3rdparty/boost)
endif()

if(NOT TARGET rttr)
    option(BUILD_STATIC "" ON)
    option(BUILD_WITH_STATIC_RUNTIME_LIBS "" OFF)
    option(BUILD_UNIT_TESTS "" OFF)
    option(BUILD_BENCHMARKS "" OFF)
    option(BUILD_EXAMPLES "" OFF)
    option(BUILD_DOCUMENTATION "" OFF)
    option(BUILD_INSTALLER "" OFF)
    option(BUILD_PACKAGE "" OFF)
    option(USE_PCH "" OFF)
    add_subdirectory(rttr)
    set_target_properties(rttr_core PROPERTIES FOLDER 3rdparty/rttr)
    set_target_properties(rttr_core_lib PROPERTIES FOLDER 3rdparty/rttr)
endif()

if(NOT TARGET taskflow)
    add_library(taskflow INTERFACE)
    target_include_directories(taskflow INTERFACE ${THIRD_PARTY_DIR}/taskflow/)
endif()

if(NOT TARGET mustache)
    add_library(mustache INTERFACE)
    target_include_directories(mustache INTERFACE ${THIRD_PARTY_DIR}/Mustache/)
endif()

if(NOT TARGET llvm)
    add_library(llvm INTERFACE)
    target_include_directories(llvm INTERFACE
        ${THIRD_PARTY_DIR}/llvm-project/clang/include
        ${THIRD_PARTY_DIR}/llvm-project/llvm/include)
    if (COMPILE_LLVM)
        ExternalProject_Add(llvm-project
            PREFIX llvm-project
            SOURCE_DIR ${THIRD_PARTY_DIR}/llvm-project/llvm
            BINARY_DIR ${THIRD_PARTY_DIR}/llvm-project/build
            CMAKE_ARGS
            -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
            -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
            -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
            -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
            -DCMAKE_INSTALL_PREFIX=${STAGING_DIR}
            -DLLVM_ENABLE_PROJECTS=clang
            -DLLVM_TARGETS_TO_BUILD=host
            -DLLVM_BUILD_BENCHMARKS=OFF
            -DLLVM_BUILD_EXAMPLES=OFF
            -DLLVM_BUILD_INSTRUMENTED_COVERAGE=OFF
            -DLLVM_BUILD_TESTS=OFF
            -DLLVM_BUILD_TOOLS=OFF
            -DLLVM_INCLUDE_BENCHMARKS=OFF
            -DLLVM_INCLUDE_EXAMPLES=OFF
            -DLLVM_INCLUDE_TESTS=OFF
            -DCLANG_BUILD_TOOLS=OFF
            #BUILD_IN_SOURCE ON
        )
        add_dependencies(llvm llvm-project)
    endif()
endif()

if(NOT TARGET shader_conductor)
    add_library(shader_conductor INTERFACE)
    target_include_directories(shader_conductor INTERFACE ${THIRD_PARTY_DIR}/ShaderConductor/Include/)
    if (COMPILE_SHADER_CONDUCTOR)
        ExternalProject_Add(ShaderConductor
            PREFIX ShaderConductor
            SOURCE_DIR ${THIRD_PARTY_DIR}/ShaderConductor
            BINARY_DIR ${THIRD_PARTY_DIR}/ShaderConductor/build
            CMAKE_ARGS
            -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
            -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
            -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
            -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
            -DCMAKE_INSTALL_PREFIX=${STAGING_DIR}
            -DLLVM_BUILD_BENCHMARKS=OFF
            -DLLVM_BUILD_EXAMPLES=OFF
            -DLLVM_BUILD_INSTRUMENTED_COVERAGE=OFF
            -DLLVM_BUILD_TESTS=OFF
            -DLLVM_BUILD_TOOLS=OFF
            -DLLVM_INCLUDE_BENCHMARKS=OFF
            -DLLVM_INCLUDE_EXAMPLES=OFF
            -DLLVM_INCLUDE_TESTS=OFF
            -DCMAKE_CXX_FLAGS=/wd4189
            #BUILD_IN_SOURCE ON
        )
        add_dependencies(shader_conductor ShaderConductor)
    endif()
endif()